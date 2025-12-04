#pragma once

#include <map>
#include <vector>
#include <cstdint>
#include <string>

using namespace std;

// Message mods
inline const string SUCCESS = "\033[0;32mS:\033[0m ";
inline const string ERROR = "\033[0;31mE:\033[0m ";
inline const string WARNING = "\033[0;33mW:\033[0m ";
inline const string INFO = "I: ";
inline const string DEBUG = "\033[0;34mD:\033[0m ";

// merges 2 map<K, V>
template <typename K, typename V>
map<K, V> mergeMap(const map<K, V> &dest, const map<K, V> &src);

// splits str by delim seperator
vector<string> split(const string& str, char delim);

// cleans string from whitespace
string strip(const string& str);

// Read given files, add end to and and return as a string
string read_files(vector<string> &files);

// split str by newlines and remove comments which starts with ';' for every line. and strip it. skip empty lines 
vector<string> split_and_clean(string &str);

// make int arg (string actualy) binary (uint64_t)
uint64_t mk_int_arg(string &arg);

// make command binary
string mk_binary(vector<string> &splitted, const map<string, string> &keyword, string &code);

// Parse given codelines and return a binary sring array
vector<string> parse_code(vector<string> &codelines);

// for example make "010" -> 2
vector<uint8_t> convert_bin(vector<string> &binarytext);

// for example make "010" -> 0x02  
string convert_hex(vector<string> &binarytext);

// dont look at name just makes list a string seperated with \n
string sep_bin(vector<string> &binarytext);