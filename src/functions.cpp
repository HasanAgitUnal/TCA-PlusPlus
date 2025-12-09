// TCA++ Assembler
// Copyright (C) 2025  Hasan Agit Ünal
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
// 
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifdef _WIN32
#include <windows.h>
#include <shlobj.h>
#endif

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <string>
#include <bitset>

#include <cstdint>
#include <cstdlib>

#include "nlohmann/json.hpp"

#include "../include/functions.hpp" // get message mods

using json = nlohmann::json;

std::string getConfigPath() {

#ifdef _WIN32
    // Windows
    char path[MAX_PATH];
    if (SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, path) != S_OK) {
        return "";
    }
    return std::string(path) + "\\tcapp";
#else
    // Linux
    return std::string(getenv("HOME")) + "/.config/tcapp";
#endif

}

// splits str by delim seperator
std::vector<std::string> split(const std::string& str, char delim) {
        std::vector<std::string> items;
        std::stringstream ss(str);
        std::string item;

        while (std::getline(ss, item, delim))
                if (!item.empty()) {
                        items.push_back(item);
                }

        return items;
}

// cleans string from whitespace
std::string strip(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\r\f\v");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\n\r\f\v");
    return str.substr(start, end - start + 1);
}

// Read single file
std::string read_file(std::string &f) {
        std::ifstream file(f);

        if (!file.is_open() && getConfigPath() != f) {
                std::cerr << msg::error << "Cant read file: " << f << std::endl;
                exit(1);

        } else if (!file.is_open()) { // Reading default config file
                std::cerr << msg::error << "No config file found or cant read it. Use tca++ --help to see config location";
                exit(1);
        }

        std::stringstream buffer;
        buffer << file.rdbuf();

        return buffer.str();
}

// Read given files, add end to and and return as a std::string
std::string read_files(std::vector<std::string> &files) {
        std::string readed;

        for (std::string& filename : files) {
                readed += "\n" + read_file(filename);
        }

        return readed;
}


// split str by newlines and remove comments which starts with ';' for every line. skip empty lines 
std::vector<std::string> split_and_clean(std::string &str) {
        std::vector<std::string> lines = split(str, '\n');
        std::vector<std::string> result;

        for (std::string& line : lines) {
                line = strip(line.substr(0, line.find(';')));

                if (line != "" ){
                        result.push_back(line);
                }
        }

        return result;
}

// make int arg (but its a std::string) binary (uint64_t)s
uint64_t mk_int_arg(std::string &arg, int &MAX_INT_SIZE) {
        long long intarg;
        try {
                intarg = std::stoll(arg);
        } catch(const std::exception& e) {
                std::cerr << msg::error << "Invalid argument: " << arg << std::endl;
                exit(1);
        }

        // TODO: Add support for signed binary
        if (intarg < 0) {
                std::cerr << msg::error << "Signed binary(negative int) not supported (for now)\n";
                exit(1);
        }

        if (MAX_INT_SIZE > 64) {
                std::cerr << msg::error << "Error in Archtitecture. More than 64 bit integer not supported";
                exit(1);
        }

        uint64_t uarg = static_cast<uint64_t>(intarg);
        uint64_t maxInt = (MAX_INT_SIZE == 64) ? UINT64_MAX : (1ULL << MAX_INT_SIZE) - 1;

        if (uarg > maxInt) {
                std::cerr << msg::error << "Integer is too big: " << intarg << " .Max integer for this architecture: " << maxInt << std::endl;
                exit(1);
        }

        return uarg;
}

// make command binary
std::string mk_binary(std::vector<std::string> &splitted, json &keyword, std::string &code, json &ARGS, json &KEYWORDS, int &MAX_INT_SIZE, int &INSTRUCTION_SIZE) {
        std::string binary_code = keyword.at("binary").get<std::string>();

        // if arg_count is bigger than zero add arg bytes. otherwise just command opcode will be used as binary_code
        if ( keyword.at("arg_count").get<int>() > 0 ) {

                // Get valid args from "valid_args" value 
                
                std::vector<std::string> arg_types = split(keyword.at("arg_sets").get<std::string>(), '|');
                
                json valid_args = {{"int", "0"}};
                
                for (std::string &type : arg_types) {
                        if (type != "int") {
                                if (!ARGS.contains(type)) {
                                        std::cerr << msg::error << "Unknown arg set in architecture: " << type << std::endl;
                                        exit(1);
                                }

                                valid_args.update(ARGS.at(type));
                        } else {
                                valid_args["int"] = "1";
                        }
                }

                // Add argument bytes end to end
                for (std::string &arg : splitted) {
                        // skip command name
                        if (arg == splitted[0]) continue;


                        uint64_t byte_int;
                        if (!valid_args.contains(arg)){
                                if (valid_args["int"] == "0") {
                                        std::cerr << msg::error << "Invalid argument: " << arg << std::endl;
                                        exit(1);
                                } else {

                                        byte_int = mk_int_arg(arg, MAX_INT_SIZE);
                                }
                        }

                        if (valid_args["int"] == "0") {
                                binary_code += valid_args.at(arg);
                        } else {
                                binary_code += std::bitset<64>(byte_int).to_string().substr(64 - MAX_INT_SIZE);
                        }
                }
        }

        if (binary_code.size() != INSTRUCTION_SIZE) {
                std::cerr << msg::error << "Error in architecture or because of args. Binary command is too long. Command: " << code << std::endl;
                exit(1);
        }
        
        return binary_code;
}

// Parse given codelines and return a binary sring array
std::vector<std::string> parse_code(std::vector<std::string> &codelines, json &KEYWORDS, json &ARGS, int &INSTRUCTION_SIZE, int &MAX_INT_SIZE) {
        std::vector<std::string> binary_commands;

        for (std::string& code : codelines) {
                std::vector<std::string> splitted = split(code, ' ');
                if (splitted.empty()) continue; // if im stupid maybe splitted be empty

                if (!KEYWORDS.contains(splitted[0])){
                        std::cerr << msg::error << "Invalid keyword: \"" << splitted[0] << "\"\n";
                        exit(1);
                }

                json &keyword = KEYWORDS.at(splitted[0]);

                const std::string &binary = keyword.at("binary").get<std::string>();
                int arg_count =  keyword.at("arg_count").get<int>();

                if (splitted.size()-1 > arg_count || splitted.size()-1 < arg_count) {
                        std::cerr << msg::error << "Expected " << arg_count << " arguments but " << splitted.size()-1 << " given.\n";
                        exit(1);
                }

                // If doesnt takes arguments just add command itself
                if (arg_count == 0) {
                        if (binary.size() != INSTRUCTION_SIZE) {
                                std::cerr << msg::error << "Error in command architecture. Command \"" << splitted[0] << "\" has invalid binary.\n";
                                exit(1);
                        }

                        binary_commands.push_back(binary);
                        continue;
                }

                // Realy parse command
                binary_commands.push_back(mk_binary(splitted, keyword, code, ARGS, KEYWORDS, MAX_INT_SIZE, INSTRUCTION_SIZE));
        }

        return binary_commands;
}

// for example make "010" -> 2
std::vector<uint8_t> convert_bin(std::vector<std::string> &binarytext) {
        std::vector<uint8_t> output;
        for (std::string& bin : binarytext) {
                for (size_t i = 0; i + 8 <= bin.size(); i += 8)
                        output.push_back(static_cast<uint8_t>(std::stoul(bin.substr(i, 8), nullptr, 2)));

                size_t rem = bin.size() % 8;
                if (rem) {
                        output.push_back(static_cast<uint8_t>(std::stoul(bin.substr(bin.size() - rem), nullptr, 2)));
                }
        }

        return output;
}

// for example make "010" -> 0x02  
std::string convert_hex(std::vector<std::string> &binarytext, int &INSTRUCTION_SIZE) {
        std::string hexadecimal_text;

        for (std::string bin : binarytext) {
                unsigned long long val = std::stoull(bin, nullptr, 2);
                std::stringstream ss;
                ss << "0x" << std::hex << std::setw(INSTRUCTION_SIZE / 4) << std::setfill('0') << val;
                hexadecimal_text += ss.str() + "\n";
        }

        return hexadecimal_text;
}

// dont look at name just makes list a std::string seperated with \n
std::string sep_bin(std::vector<std::string> &binarytext) {
        std::string output;
        for (std::string bin : binarytext) {
                output += bin + "\n";
        }

        return output;
}
