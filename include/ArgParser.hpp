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
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

class ArgParser
{
        std::unordered_map<std::string, std::string> options;
        std::unordered_set<std::string> flags;
        std::vector<std::string> positional;

public:
        ArgParser(int argc, char *argv[])
        {
                for (int i = 1; i < argc; ++i)
                {
                        std::string arg = argv[i];

                        if (arg[0] == '-')
                        {
                                if (arg[1] == '-')
                                {
                                        if (i + 1 < argc && argv[i + 1][0] != '-')
                                        {
                                                options[arg] = argv[++i];
                                        }
                                        else
                                        {
                                                flags.insert(arg);
                                        }
                                }
                                else
                                {
                                        if (i + 1 < argc && argv[i + 1][0] != '-')
                                        {
                                                options[arg] = argv[++i];
                                        }
                                        else
                                        {
                                                flags.insert(arg);
                                        }
                                }
                        }
                        else
                        {
                                positional.push_back(arg);
                        }
                }
        }

        std::string getOption(const std::string &option, const std::string &defaultValue = "") const
        {
                auto it = options.find(option);
                return it != options.end() ? it->second : defaultValue;
        }

        bool hasFlag(const std::string &flag) const
        {
                return flags.find(flag) != flags.end();
        }

        bool hasOption(const std::string &option) const
        {
                return options.find(option) != options.end();
        }

        const std::vector<std::string> &getPositional() const
        {
                return positional;
        }
};