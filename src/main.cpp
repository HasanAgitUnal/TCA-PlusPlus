#include <iostream>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <iomanip>
#include <vector>
#include <string>
#include <map>
#include <bitset>

#include <cstdint>
#include <cstdlib>

#include "ArgParser.hpp"

// TODO: Remove this header file and load architecture from json files and with this compiler will doesnt need recompiling on architecture errors
#include "Architecture.hpp"

using namespace std;

string output = "program";
string output_type = "binary";

const string ERROR = "\033[0;31mE:\033[0m ";
const string WARNING = "\033[0;33mW:\033[0m ";
const string SUCCESS = "\033[0;32mS:\033[0m ";
const string INFO = "I: ";


template <typename K, typename V>
map<K, V> mergeMap(const map<K, V> &dest, const map<K, V> &src) {
    map<K, V> result = dest;
    result.insert(src.begin(), src.end());
    return result;
}

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

string strip(const string& str) {
    size_t start = str.find_first_not_of(" \t\n\r\f\v");
    if (start == string::npos) return "";
    size_t end = str.find_last_not_of(" \t\n\r\f\v");
    return str.substr(start, end - start + 1);
}


string read_files(vector<string> &files) {
        string readed;

        for (string& filename : files) {
                ifstream file(filename);

                if (!file.is_open()) {
                        cout << ERROR << "Cant read file: " << filename << endl;
                        exit(1);
                }

                stringstream buffer;
                buffer << file.rdbuf();
                readed += "\n" + buffer.str();
        }

        return readed;
}

vector<string> split_and_clean(string &str) {
        vector<string> lines = split(str, '\n');
        vector<string> result;

        for (string& line : lines) {
                line =strip(line.substr(0, line.find(';')));

                if (line != "" ){
                        result.push_back(line);
                }
        }

        return result;
}

uint64_t mk_int_arg(string &arg) {
        long long intarg;
        try {
                intarg = stoll(arg);
        } catch(const exception& e) {
                cout << ERROR << "Invalid argument: " << arg << endl;
                exit(1);
        }

        // TODO: Add support for signed binary
        if (intarg < 0) {
                cout << ERROR << "Signed binary(negative int) not supported (for now)";
                exit(1);
        }

        uint64_t uarg = static_cast<uint64_t>(intarg);
        uint64_t maxInt = (MAX_INT_SIZE == 64) ? UINT64_MAX : (1ULL << MAX_INT_SIZE) - 1;

        if (uarg > maxInt) {
                cout << ERROR << "Integer is too big: " << intarg << " .Max integer for this architecture: " << maxInt << endl;
                exit(1);
        }

        return uarg;
}

string mk_binary(vector<string> &splitted, const string &binary, const map<string, string> &keyword, string &code) {
        // Get valid args from "valid_args" value 
        vector<string> arg_types = split(keyword.at("valid_args"), '|');
        map<string, string> valid_args = {{"int", "0"}};
        for (string &type : arg_types) {
                if (type != "int") {
                        if (ARGS.find(type) == ARGS.end()) {
                                cout << ERROR << "Unknown arg set in architecture: " << type << endl;
                                exit(1);
                        }

                        valid_args = mergeMap(valid_args, ARGS.at(type));
                } else {
                        valid_args["int"] = "1";
                }
        }

        // Add argument bytes end to end
        string binary_code = binary;
        for (string &arg : splitted) {
                // skip command name
                if (arg == splitted[0]) continue;


                uint64_t byte_int;
                if (valid_args.find(arg) == valid_args.end()){
                        if (valid_args["int"] == "0") {
                                cout << ERROR << "Invalid argument: " << arg << endl;
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

        if (binary_code.size() != INSTRUCTION_SIZE) {
                cout << ERROR << "Error in architecture or args. Binary command is too long. Command: " << code << endl;
                exit(1);
        }
        
        return binary_code;
}

vector<string> parse_code(vector<string> &codelines) {
        vector<string> binary_commands;

        for (string& code : codelines) {
                vector<string> splitted = split(code, ' ');
                if (splitted.empty()) continue; // if im stupid maybe splitted be empty

                if (KEYWORDS.find(splitted[0]) == KEYWORDS.end()){
                        cout << ERROR << "Invalid keyword: " << splitted[0] << endl;
                        exit(1);
                }

                const map<string, string> &keyword = KEYWORDS.at(splitted[0]);

                const string &binary = keyword.at("binary");
                const int arg_count =  stoi(keyword.at("arg_count"));

                if (splitted.size()-1 > arg_count || splitted.size()-1 < arg_count) {
                        cout << ERROR << "Expected " << arg_count << " arguments but " << splitted.size()-1 << " given.\n";
                        exit(1);
                }

                // If doesnt takes arguments just add command itself
                if (arg_count == 0) {
                        if (binary.size() != INSTRUCTION_SIZE) {
                                cout << ERROR << "Error in command architecture. Command \"" << splitted[0] << "\" has invalid binary.\n";
                                exit(1);
                        }

                        binary_commands.push_back(binary);
                        continue;
                }

                // Realy parse command
                binary_commands.push_back(mk_binary(splitted, binary, keyword, code));
        }

        return binary_commands;
}

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

string clean_and_sep(vector<string> &binarytext) {
        string output;
        for (string bin : binarytext) {
                output += bin + "\n";
        }

        return output;
}

int main (int argc, char* argv[]) {
        if (MAX_INT_SIZE > 64 || MAX_INT_SIZE < 1) {
                cout << "\033[0;21mFATAL ERROR: Error in architecture MAX_INT_SIZE should be in range: 1-64.\n";
                return 1;
        }

        if (INSTRUCTION_SIZE % 8 != 0) {
                cout << "\033[0;21mFATAL ERROR: Error in architecture INSTRUCTION_SIZE should be multiple of 8.\n";
                return 1;
        }

        // handle cli args
        // TODO: Delete the ArgParser.hpp file and recode it (dont use AI!!!!)
        ArgParser args(argc, argv);

        if (args.hasFlag("--help")) {
                cout << "Turing Complete Assempler++ - v0.0.1.2\n\n"
                     << "Usage: tca++ code.asm OPTIONS\n\n"
                     << "OPTIONS:\n"
                     << "       --help, -h :     Show this message\n"
                     << "       -o FILE    :     Output destination\n"
                     << "       -t OTYPE   :     Output type\n\n"
                     << "OTYPE:\n"
                     << "       binary     :     Default output type\n"
                     << "       text-binary:     Binary as text 0s and 1s\n"
                     << "       hex        :     Hexadecimal text\n\n"
                     << "NOTE:\n"
                     << "	If you get an error caused by architecture. You should fix error in Architecture.hpp in source code and rebuild compiler.\n";

                return 0;
        }

        string opt = args.getOption("-o");
        if (!opt.empty()) output = opt;

        vector<string> inputs = args.getPositional();

        if (inputs.size() == 0) {
                cout << ERROR << "no input files given.\n";
                return 1;
        }

        output_type = args.getOption("-t");

        if (output_type != "binary" && output_type != "text-binary" && output_type != "hex") {
                cout << ERROR << "Invalid output type: " << output_type << endl;
                return 1;
        }

        if (INSTRUCTION_SIZE > 64 && output_type == "hex") {
                cout << ERROR << "Hex output type not supported for architectures over 64-bit instructions.\n";
                return 1;
        }

        // Read and add files end to end
        string fileContents = read_files(inputs);

        // Split lines and remove comments and empty lines
        vector<string> codelines = split_and_clean(fileContents);

        // Parse code
        vector<string> binaryCommands = parse_code(codelines);

        // Prepare the output
        vector<uint8_t> output_data;

        if (output_type == "text-binary") {
                string text_binary = clean_and_sep(binaryCommands);
                output_data.assign(text_binary.begin(), text_binary.end());

        } else if (output_type == "binary") {
                output_data = convert_bin(binaryCommands);

        } else { // Hexadecimal
                string hexadecimal_text = convert_hex(binaryCommands);
                output_data.assign(hexadecimal_text.begin(), hexadecimal_text.end());
        }

        // Write output file
        ofstream out(output, ios::binary);
        out.write(reinterpret_cast<const char*>(output_data.data()), output_data.size());
        out.close();

        cout << SUCCESS << "Build completed. output file generated at: \"" << filesystem::absolute(output).string() << "\"\n";

        return 0;
}
