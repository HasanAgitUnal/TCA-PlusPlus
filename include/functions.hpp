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

#pragma once

#include <vector>
#include <cstdint>
#include <string>
#include "nlohmann/json.hpp"

using json = nlohmann::json;

#ifdef _WIN32
namespace msg { // ANSII codes doesnt working on windows. so define them without codes
        inline const std::string success = "S: ";
        inline const std::string error = "E: ";
        inline const std::string warning = "W: ";
        inline const std::string info = "I: ";
        inline const std::string debug = "D: ";
}

#else
namespace msg {
	inline const std::string success = "\033[0;32mS:\033[0m ";
	inline const std::string error = "\033[0;31mE:\033[0m ";
	inline const std::string warning = "\033[0;33mW:\033[0m ";
	inline const std::string info = "I: ";
	inline const std::string debug = "\033[0;34mD:\033[0m ";
}

#endif


// get config path
std::string getConfigPath();

// splits str by delim seperator
std::vector<std::string> split(const std::string& str, char delim);

// cleans string from whitespace
std::string strip(const std::string& str);

// Read single file
std::string read_file(std::string &f);

// Read given files, add end to and and return as a string
std::string read_files(std::vector<std::string> &files);

// split str by newlines and remove comments which starts with ';' for every line. and strip it. skip empty lines 
std::vector<std::string> split_and_clean(std::string &str);

// make int arg (string actualy) binary (uint64_t)
uint64_t mk_int_arg(std::string &arg, int &MAX_INT_SIZE);

// make command binary
std::string mk_binary(std::vector<std::string> &splitted, json &keyword, std::string &code, json &ARGS, json &KEYWORDS, int &MAX_INT_SIZE, int &INSTRUCTION_SIZE);

// Parse given codelines and return a binary sring array
std::vector<std::string> parse_code(std::vector<std::string> &codelines, json &KEYWORDS, json &ARGS, int &INSTRUCTION_SIZE, int &MAX_INT_SIZE);

// for example make "010" -> 2
std::vector<uint8_t> convert_bin(std::vector<std::string> &binarytext);

// for example make "010" -> 0x02  
std::string convert_hex(std::vector<std::string> &binarytext, int &INSTRUCTION_SIZE);

// dont look at name just makes list a string seperated with \n
std::string sep_bin(std::vector<std::string> &binarytext);
