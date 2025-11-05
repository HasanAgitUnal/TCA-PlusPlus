#include "iostream"
#include "fstream"
#include "filesystem"
#include "sstream"
#include "cstdint"
#include "vector"
#include "string"
#include "map"
#include "ArgParser.hpp"
#include "Architecture.hpp"

using namespace std;

bool skip = false;
bool continue_files = false;

string output = "program";
string output_type = "binary";

const string ERROR = "\033[0;21mE:\033[0m ";
const string WARNING = "\033[0;93;W:\033[0m ";
const string SUCCESS = "\033[0;32mS:\033[0m ";
const string INFO = "I: ";

vector<string> split(string str, char delim) {
	vector<string> items;
	stringstream ss(str);
	string item;

	while (getline(ss, item, delim))
		if (!item.empty()) {
			items.push_back(item);
		}

	return items;
}

string strip(string str) {
	str.erase(0, str.find_first_not_of(" \t\n\r\f\v"));
	str.erase(str.find_last_not_of(" \t\n\r\f\v") + 1);
	return str;
}

template <typename K, typename V>
map<K, V> mergeMap(const map<K, V> &dest, const map<K, V> &src) {
    map<K, V> result = dest;
    result.insert(src.begin(), src.end()); 
    return result;
}


int main (int argc, char* argv[]) {
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
		     << "       hex        :     Hexadecimal text\n";

		return 0;
	} 

	if (args.hasFlag("--skip")) {
		skip = true;
	}
	
	if (args.hasFlag("--continue")) {
		continue_files = true;
	}

	output = args.getOption("-o");

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

		if (KEYWORDS.find(splitted[0]) == KEYWORDS.end()){
			cout << ERROR << "Invalid keyword: " << splitted[0] << endl;
			return 1;
		}

		const map<string, string> &keyword = KEYWORDS.at(splitted[0]);

		const string &binary = keyword.at("binary");
		const int argc =  stoi(keyword.at("arg_count"));

		if (splitted.size()-1 > argc || splitted.size()-1 < argc) {
			cout << ERROR << "Expected " << argc << " arguments but " << splitted.size()-1 << " given.\n";
			return 1;
		}

		// If doesnt takes arguments just add command itself
		if (!(argc == 0)) {
			if (binary.size() != MAX_INSTRUCTION_SIZE) {
				cout << ERROR << "Error in command architecture. Command \"" << splitted[0] << "\" has invalid binary.\n";
				return 1;
			}

			binaryCommands.push_back(binary);
		}
		
		// Get valid args from "valid_args" value 
		vector<string> arg_types = split(keyword.at("valid_args"), '|');
		map<string, string> valid_args;
		for (string &type : arg_types) {
			valid_args = mergeMap(valid_args, ARGS.at(type));
		}

		// Add argument bytes end to end
		string binary_code = splitted[0];
		for (string &arg : splitted) {
			// skip command name
			if (arg == splitted[0]) continue;

			if (valid_args.find(arg) == valid_args.end()){
				cout << ERROR << "Invalid argument: " << arg << endl;
				return 1;
			}

			binary_code += valid_args.at(arg);
		}

		if (binary_code.size() != MAX_INSTRUCTION_SIZE) {
			cout << ERROR << "Error in architecture or args. Binary command is too long. Command: " << code << endl;
		
		}

		binaryCommands.push_back(binary_code);
	}

	// Prepare the output 
	vector<uint8_t> output_data;

	if (output_type == "text_binary") {
		string text_binary;
		for (string bin : binaryCommands) {
			text_binary += bin + "\n";
		}

		output_data.assign(text_binary.begin(), text_binary.end());

	} else if (output_type == "binary") {
		// Most readable C++ code:
		for (string bin : binaryCommands) {
			for (size_t i=0; i<bin.size(); i+=8) {
				output_data.push_back(stoi(bin.substr(i,8),nullptr,2));
			}
		}

	} else { // Hexadecimal
		string hexadecimal_text;
		for (string bin : binaryCommands) {
			unsigned int val = stoi(bin, nullptr, 2);
			stringstream ss;
			ss << "0x" << hex << setw(2) << setfill('0') << val;
			hexadecimal_text += ss.str() + "\n";
		}

		output_data.assign(hexadecimal_text.begin(), hexadecimal_text.end());
	}

	// Write output file
	ofstream out(output, ios::binary);
	out.write(reinterpret_cast<const char*>(output_data.data()), output_data.size());
	out.close();

	cout << SUCCESS << "Build completed. output file generated at: \"" << fs::absolute(output).string() << "\"\n";

	return 0;
}