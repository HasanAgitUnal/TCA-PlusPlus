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

#include <cstdint>

#include "../include/functions.hpp"
#include "../include/ArgParser.hpp"

// TODO: Remove this header file and load architecture from json files and with this compiler will doesnt need recompiling on architecture errors
#include "../include/Architecture.hpp"

using namespace std;

const string DEFAULT_OUTPUT = "program";
const string DEFAULT_OTYPE = "binary";

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
                cout << "Turing Complete Assempler++ - v0.1\n\n"
                     << "Usage: tca++ FILE [OPTIONS]\n\n"
                     << "OPTIONS:\n"
                     << "       --help, -h :     Show this message\n"
                     << "       -o FILES    :     Output destination\n"
                     << "       -t OTYPE   :     Output type\n\n"
                     << "OTYPE:\n"
                     << "       binary     :     Default output type\n"
                     << "       text-binary:     Binary as text 0s and 1s\n"
                     << "       hex        :     Hexadecimal text\n\n"
                     << "NOTE:\n"
                     << "	If you get an error caused by architecture. You should fix error in Architecture.hpp in source code and rebuild compiler.\n";

                return 0;
        }

        string output;
        string opt = args.getOption("-o");
        if (!opt.empty()) { output = opt; } else { output = DEFAULT_OUTPUT; };

        vector<string> inputs = args.getPositional();

        if (inputs.size() == 0) {
                cout << ERROR << "no input files given.\n";
                return 1;
        }


        opt = args.getOption("-t");
        string otype;
        if (!opt.empty()) { otype = opt; } else { otype = DEFAULT_OTYPE; };

        if (otype != "binary" && otype != "text-binary" && otype != "hex") {
                cout << ERROR << "Invalid output type: " << otype << endl;
                return 1;
        }

        if (INSTRUCTION_SIZE > 64 && otype == "hex") {
                cerr << ERROR << "Hex output type not supported for architectures over 64-bit instructions.\n";
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

        if (otype == "text-binary") {
                string text_binary = sep_bin(binaryCommands);
                output_data.assign(text_binary.begin(), text_binary.end());

        } else if (otype == "binary") {
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
