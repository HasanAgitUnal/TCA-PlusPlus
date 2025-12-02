#include <iostream>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <cstdint>
#include <iomanip>
#include <vector>
#include <string>
#include <map>
#include <bitset>

#include "ArgParser.hpp"

// TODO: Remove this header file and load architecture from json files and with this compiler will doesnt need recompiling on architecture errors
#include "Architecture.hpp"

using namespace std;

bool skip = false;
bool continue_files = false;

string output = "program";
string output_type = "binary";

const string ERROR = "\033[0;21mE:\033[0m ";
const string WARNING = "\033[0;93mW:\033[0m ";
const string SUCCESS = "\033[0;32mS:\033[0m ";
const string INFO = "I: ";

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

template <typename K, typename V>
map<K, V> mergeMap(const map<K, V> &dest, const map<K, V> &src) {
    map<K, V> result = dest;
    result.insert(src.begin(), src.end()); 
    return result;
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

	ArgParser args(argc, argv);

	if (args.hasFlag("--help")) {
		cout << "Turing Complete Assempler++ - v0.1\n\n"
		     << "Usage: tca++ code.asm OPTIONS\n\n"
		     << "OPTIONS:\n"
		     << "       --help, -h :     Show this message\n"
		     << "       -o FILE    :     Output destination\n"
		     << "       -t OTYPE   :     Output type\n"
		     << "       --skip     :     Skip the multiple files warning\n"
		     << "       --continue :     If an error happen while reading a file still continue with others\n\n"
		     << "OTYPE:\n"
		     << "       binary     :     Default output type\n"
		     << "       text-binary:     Binary as text 0s and 1s\n"
		     << "       hex        :     Hexadecimal text\n\n"
		     << "NOTE:\n"
		     << "	If you get an error caused by architecture. You should fix error in Architecture.hpp in source code and rebuild compiler.";

		return 0;
	} 

	if (args.hasFlag("--skip")) {
		skip = true;
	}
	
	if (args.hasFlag("--continue")) {
		continue_files = true;
	}

	string opt = args.getOption("-o");
	if (!opt.empty()) output = opt;

	vector<string> inputs = args.getPositional();

	if (inputs.size() == 0) {
		cout << ERROR << "no input files given.\n";
		return 1;
	} else if (inputs.size() > 1 && !skip) {
		string confirm;
		cout << WARNING << "You entered more than 1 file they will be added to end to end.\nWill you continue? [Y/n]: ";
		cin >> confirm;

		if (confirm == "N" || confirm == "n") {
			return 0;
		}
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


	//
	// ASSEMBLING STARTS HERE
	//

	// Read and add files end to end

	string fileContents;

	for (string& filename : inputs) {
		ifstream file(filename);

		if (!file.is_open() && !continue_files) {
			cout << ERROR << "Cant read file: " << filename << endl;
			return 1;
		} else if (!file.is_open()) {
			cout << WARNING << "File skipped because of an error: " << filename << endl;
			continue;
		}

		stringstream buffer;
		buffer << file.rdbuf();
		fileContents += "\n" + buffer.str();
	}


	// Split lines and remove comments and empty lines
	vector<string> lines = split(fileContents, '\n');
	vector<string> codelines;

	for (string& line : lines) {
		line =strip(line.substr(0, line.find(';')));

		if (line != "" ){
			codelines.push_back(line);
		}
	}

	//
	// Parser
	//
	
	vector<string> binaryCommands;

	for (string& code : codelines) {
		vector<string> splitted = split(code, ' ');
		if (splitted.empty()) continue; // if im stupid maybe splitted is empty 

		if (KEYWORDS.find(splitted[0]) == KEYWORDS.end()){
			cout << ERROR << "Invalid keyword: " << splitted[0] << endl;
			return 1;
		}

		const map<string, string> &keyword = KEYWORDS.at(splitted[0]);

		const string &binary = keyword.at("binary");
		const int arg_count =  stoi(keyword.at("arg_count"));

		if (splitted.size()-1 > arg_count || splitted.size()-1 < arg_count) {
			cout << ERROR << "Expected " << arg_count << " arguments but " << splitted.size()-1 << " given.\n";
			return 1;
		}

		// If doesnt takes arguments just add command itself
		if (arg_count == 0) {
			if (binary.size() != INSTRUCTION_SIZE) {
				cout << ERROR << "Error in command architecture. Command \"" << splitted[0] << "\" has invalid binary.\n";
				return 1;
			}

			binaryCommands.push_back(binary);
			continue;
		}
		
		// Get valid args from "valid_args" value 
		vector<string> arg_types = split(keyword.at("valid_args"), '|');
		map<string, string> valid_args = {{"int", "0"}};
		for (string &type : arg_types) {
			if (type != "int") {
				if (ARGS.find(type) == ARGS.end()) {
					cout << ERROR << "Unknown arg set in architecture: " << type << endl;
					return 1;
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
					return 1;
				} else {
					long long intarg;
					try {
						intarg = stoll(arg);
					} catch(const exception& e) {
						cout << ERROR << "Invalid argument: " << arg << endl;
						return 1;
					}

					// TODO: Add support for signed binary
					if (intarg < 0) {
						cout << ERROR << "Signed binary(negative int) not supported (for now)";
						return 1;
					}

					uint64_t uarg = static_cast<uint64_t>(intarg);
					uint64_t maxInt = (MAX_INT_SIZE == 64) ? UINT64_MAX : (1ULL << MAX_INT_SIZE) - 1;
					
					if (uarg > maxInt) {
						cout << ERROR << "Integer is too big: " << intarg << " .Max integer for this architecture: " << maxInt << endl;
						return 1;
					}

					byte_int = uarg;
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
			return 1;
		}

		binaryCommands.push_back(binary_code);
	}

	// Prepare the output 
	vector<uint8_t> output_data;

	if (output_type == "text-binary") {
		string text_binary;
		for (string bin : binaryCommands) {
			text_binary += bin + "\n";
		}

		output_data.assign(text_binary.begin(), text_binary.end());

	} else if (output_type == "binary") {
		// Most readable C++ code:)
		for (string& bin : binaryCommands) {
			for (size_t i = 0; i + 8 <= bin.size(); i += 8)
				output_data.push_back(static_cast<uint8_t>(stoul(bin.substr(i, 8), nullptr, 2)));

			size_t rem = bin.size() % 8;
			if (rem) {
				output_data.push_back(static_cast<uint8_t>(stoul(bin.substr(bin.size() - rem), nullptr, 2)));
			}
		}


	} else { // Hexadecimal
		string hexadecimal_text;
		for (string bin : binaryCommands) {
			unsigned long long val = stoull(bin, nullptr, 2);
			stringstream ss;
			ss << "0x" << hex << setw(MAX_INSTRUCTION_SIZE / 4) << setfill('0') << val;
			hexadecimal_text += ss.str() + "\n";
		}

		output_data.assign(hexadecimal_text.begin(), hexadecimal_text.end());
	}

	// Write output file
	ofstream out(output, ios::binary);
	out.write(reinterpret_cast<const char*>(output_data.data()), output_data.size());
	out.close();

	cout << SUCCESS << "Build completed. output file generated at: \"" << filesystem::absolute(output).string() << "\"\n";

	return 0;
}