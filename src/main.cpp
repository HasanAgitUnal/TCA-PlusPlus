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

#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <string>
#include <nlohmann/json.hpp>

#include <cstdint>

#include "../include/functions.hpp"
#include "../include/ArgParser.hpp"

using json = nlohmann::json;


const std::string DEFAULT_OUTPUT = "program";
const std::string DEFAULT_OTYPE = "binary";
const std::string DEFAULT_ARCH_FILE = getConfigPath();

int main (int argc, char* argv[]) {
        
        // handle cli args
        // TODO: Delete the ArgParser.hpp file (dont use AI!!!!)
        ArgParser args(argc, argv);

        if (args.hasFlag("--help")) {
                std::cout    << "Turing Complete Assempler++ - v0.2\n\n"
                        << "Usage: tca++ FILES [OPTIONS]\n\n"
                        << "OPTIONS:\n"
                        << "\t--help, -h  :\tShow this message\n"
                        << "\t-o FILES    :\tOutput destination\n"
                        << "\t-t OTYPE    :\tOutput type\n"
                        << "\t-a ARCH     :\tThe architecture json file\n\n"
                        << "OTYPE:\n"
                        << "\tbinary      :\tDefault output type\n"
                        << "\ttext-binary :\tBinary as text 0s and 1s\n"
                        << "\thex         :\tHexadecimal text\n\n"
                        << "ARCH:\n"
                        << "\tLoad architecture from given json file if not specified default values will be used for os:\n"
                        << "\tWindows: C:\\Users\\<USER>\\AppData\\Roaming\\TCAPP\\architecture.json\n"
                        << "\tLinux  : /home/USER/.config/TCAPP/architecture.json\n\n"
                        << "NOTE:\n"
                        << "	If you get an error caused by architecture. You should fix error in Architecture.hpp in source code and rebuild compiler.\n";

                return 0;
        }

        // FILES
        std::vector<std::string> inputs = args.getPositional();

        if (inputs.size() == 0) {
                std::cout << ERROR << "no input files given.\n";
                return 1;
        }

        // -o OUTPUT
        std::string output;
        std::string opt = args.getOption("-o");
        if (!opt.empty()) { output = opt; } else { output = DEFAULT_OUTPUT; };

        // -t OTYPE
        opt = args.getOption("-t");
        std::string otype;
        if (!opt.empty()) { otype = opt; } else { otype = DEFAULT_OTYPE; };
        
        if (otype != "binary" && otype != "text-binary" && otype != "hex") {
                std::cout << ERROR << "Invalid output type: " << otype << std::endl;
                return 1;
        }

        // -a ARCH
        std::string arch_path;
        opt = args.getOption("-a");
        if (!opt.empty()) { arch_path = opt; } else { arch_path = DEFAULT_ARCH_FILE; }
        
        std::string arch_content = read_file(arch_path);
        
        json arch;
        int INSTRUCTION_SIZE;
        int MAX_INT_SIZE;
        json KEYWORDS;
        json ARGS;
        try {
                arch = json::parse(arch_content);
                INSTRUCTION_SIZE = arch.at("instruction_size").get<int>();
                MAX_INT_SIZE = arch.at("max_int_size").get<int>();
                KEYWORDS = arch.at("keywords");
                ARGS = arch.at("arg_sets");

        } catch (const json::exception& e) {
                std::cerr << ERROR << "Failed to parse or required keys are not found in json file: " << arch_path << "\n"
                          << "\tReason: " << e.what() << std::endl;
                return 1;
        }

        if (!KEYWORDS.is_array() || !ARGS.is_array()) {
                std::cerr << ERROR << "Error in config file: \"keywords\" or \"arg_sets\" not an json array\n";
                return 1;
        }

        if (MAX_INT_SIZE > 64 || MAX_INT_SIZE < 1) {
                std::cerr << ERROR << "FATAL ERROR: Error in architecture MAX_INT_SIZE should be in range: 1-64.\n";
                return 1;
        }

        if (INSTRUCTION_SIZE % 8 != 0) {
                std::cerr << ERROR << "FATAL ERROR: Error in architecture INSTRUCTION_SIZE should be multiple of 8.\n";
                return 1;
        }

        if (INSTRUCTION_SIZE > 64 && otype == "hex") {
                std::cerr << ERROR << "Hex output type not supported for architectures over 64-bit instructions.\n";
                return 1;
        }

        // Read and add files end to end
        std::string fileContents = read_files(inputs);

        // Split lines and remove comments and empty lines
        std::vector<std::string> codelines = split_and_clean(fileContents);

        // Parse code
        std::vector<std::string> binaryCommands = parse_code(codelines, ARGS, KEYWORDS, INSTRUCTION_SIZE, MAX_INT_SIZE);

        // Prepare the output
        std::vector<uint8_t> output_data;

        if (otype == "text-binary") {
                std::string text_binary = sep_bin(binaryCommands);
                output_data.assign(text_binary.begin(), text_binary.end());

        } else if (otype == "binary") {
                output_data = convert_bin(binaryCommands);

        } else { // Hexadecimal
                std::string hexadecimal_text = convert_hex(binaryCommands, INSTRUCTION_SIZE);
                output_data.assign(hexadecimal_text.begin(), hexadecimal_text.end());
        }

        // Write output file
        std::ofstream out(output, std::ios::binary);
        out.write(reinterpret_cast<const char*>(output_data.data()), output_data.size());
        out.close();

        std::cout << SUCCESS << "Build completed. output file generated at: \"" << std::filesystem::absolute(output).string() << "\"\n";

        return 0;
}
