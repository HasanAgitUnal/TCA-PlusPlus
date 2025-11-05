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