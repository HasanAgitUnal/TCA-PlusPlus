#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <string>
#include <map>
#include <bitset>

#include <cstdint>
#include <cstdlib>

#include "../include/functions.hpp"

// TODO: Remove this header file and load architecture from json files and with this compiler will doesnt need recompiling on architecture errors
#include "../include/Architecture.hpp"

using namespace std;

string output = "program";
string output_type = "binary";

const string ERROR = "\033[0;31mE:\033[0m ";
const string WARNING = "\033[0;33mW:\033[0m ";
const string SUCCESS = "\033[0;32mS:\033[0m ";
const string INFO = "I: ";

// merges 2 map<K, V>
template <typename K, typename V>
map<K, V> mergeMap(const map<K, V> &dest, const map<K, V> &src) {
    map<K, V> result = dest;
    result.insert(src.begin(), src.end());
    return result;
}

// splits str by delim seperator
vector<string> split(const string& str, char delim) {
        vector<string> items;
        stringstream ss(str);
        string item;

        while (getline(ss, item, delim))
                if (!item.empty()) {
                        items.push_back(item);
                }

        return items;
}

// cleans string from whitespace
string strip(const string& str) {
    size_t start = str.find_first_not_of(" \t\n\r\f\v");
    if (start == string::npos) return "";
    size_t end = str.find_last_not_of(" \t\n\r\f\v");
    return str.substr(start, end - start + 1);
}

// Read given files, add end to and and return as a string
string read_files(vector<string> &files) {
        string readed;

        for (string& filename : files) {
                ifstream file(filename);

                if (!file.is_open()) {
                        cerr << ERROR << "Cant read file: " << filename << endl;
                        exit(1);
                }

                stringstream buffer;
                buffer << file.rdbuf();
                readed += "\n" + buffer.str();
        }

        return readed;
}

// split str by newlines and remove comments which starts with ';' for every line. skip empty lines 
vector<string> split_and_clean(string &str) {
        vector<string> lines = split(str, '\n');
        vector<string> result;

        for (string& line : lines) {
                line = strip(line.substr(0, line.find(';')));

                if (line != "" ){
                        result.push_back(line);
                }
        }

        return result;
}

// make int arg (but its a string) binary (uint64_t)
uint64_t mk_int_arg(string &arg) {
        long long intarg;
        try {
                intarg = stoll(arg);
        } catch(const exception& e) {
                cerr << ERROR << "Invalid argument: " << arg << endl;
                exit(1);
        }

        // TODO: Add support for signed binary
        if (intarg < 0) {
                cerr << ERROR << "Signed binary(negative int) not supported (for now)\n";
                exit(1);
        }

        uint64_t uarg = static_cast<uint64_t>(intarg);
        uint64_t maxInt = (MAX_INT_SIZE == 64) ? UINT64_MAX : (1ULL << MAX_INT_SIZE) - 1;

        if (uarg > maxInt) {
                cerr << ERROR << "Integer is too big: " << intarg << " .Max integer for this architecture: " << maxInt << endl;
                exit(1);
        }

        return uarg;
}

// make command binary
string mk_binary(vector<string> &splitted, const map<string, string> &keyword, string &code) {
        string binary_code = keyword.at("binary");

        // if arg_count is bigger than zero add arg bytes. otherwise just command opcode will be used as binary_code
        if ( stoi(keyword.at("arg_count")) > 0 ) {

                // Get valid args from "valid_args" value 
                vector<string> arg_types = split(keyword.at("valid_args"), '|');
                map<string, string> valid_args = {{"int", "0"}};
                for (string &type : arg_types) {
                        if (type != "int") {
                                if (ARGS.find(type) == ARGS.end()) {
                                        cerr << ERROR << "Unknown arg set in architecture: " << type << endl;
                                        exit(1);
                                }

                                valid_args = mergeMap(valid_args, ARGS.at(type));
                        } else {
                                valid_args["int"] = "1";
                        }
                }

                // Add argument bytes end to end
                for (string &arg : splitted) {
                        // skip command name
                        if (arg == splitted[0]) continue;


                        uint64_t byte_int;
                        if (valid_args.find(arg) == valid_args.end()){
                                if (valid_args["int"] == "0") {
                                        cerr << ERROR << "Invalid argument: " << arg << endl;
                                        exit(1);
                                } else {

                                        byte_int = mk_int_arg(arg);
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
                cerr << ERROR << "Error in architecture or because of args. Binary command is too long. Command: " << code << endl;
                exit(1);
        }
        
        return binary_code;
}

// Parse given codelines and return a binary sring array
vector<string> parse_code(vector<string> &codelines) {
        vector<string> binary_commands;

        for (string& code : codelines) {
                vector<string> splitted = split(code, ' ');
                if (splitted.empty()) continue; // if im stupid maybe splitted be empty

                if (KEYWORDS.find(splitted[0]) == KEYWORDS.end()){
                        cerr << ERROR << "Invalid keyword: " << splitted[0] << endl;
                        exit(1);
                }

                const map<string, string> &keyword = KEYWORDS.at(splitted[0]);

                const string &binary = keyword.at("binary");
                const int arg_count =  stoi(keyword.at("arg_count"));

                if (splitted.size()-1 > arg_count || splitted.size()-1 < arg_count) {
                        cerr << ERROR << "Expected " << arg_count << " arguments but " << splitted.size()-1 << " given.\n";
                        exit(1);
                }

                // If doesnt takes arguments just add command itself
                if (arg_count == 0) {
                        if (binary.size() != INSTRUCTION_SIZE) {
                                cerr << ERROR << "Error in command architecture. Command \"" << splitted[0] << "\" has invalid binary.\n";
                                exit(1);
                        }

                        binary_commands.push_back(binary);
                        continue;
                }

                // Realy parse command
                binary_commands.push_back(mk_binary(splitted, keyword, code));
        }

        return binary_commands;
}

// for example make "010" -> 2
vector<uint8_t> convert_bin(vector<string> &binarytext) {
        vector<uint8_t> output;
        for (string& bin : binarytext) {
                for (size_t i = 0; i + 8 <= bin.size(); i += 8)
                        output.push_back(static_cast<uint8_t>(stoul(bin.substr(i, 8), nullptr, 2)));

                size_t rem = bin.size() % 8;
                if (rem) {
                        output.push_back(static_cast<uint8_t>(stoul(bin.substr(bin.size() - rem), nullptr, 2)));
                }
        }

        return output;
}

// for example make "010" -> 0x02  
string convert_hex(vector<string> &binarytext) {
        string hexadecimal_text;

        for (string bin : binarytext) {
                unsigned long long val = stoull(bin, nullptr, 2);
                stringstream ss;
                ss << "0x" << hex << setw(INSTRUCTION_SIZE / 4) << setfill('0') << val;
                hexadecimal_text += ss.str() + "\n";
        }

        return hexadecimal_text;
}

// dont look at name just makes list a string seperated with \n
string sep_bin(vector<string> &binarytext) {
        string output;
        for (string bin : binarytext) {
                output += bin + "\n";
        }

        return output;
}