#pragma once
#include <map>
#include <string>

using namespace std;

//
// CPU ARCHITECTURE
//

// Define everything about your architecture here

// Instruction size
inline const int INSTRUCTION_SIZE = 8;

// Maximum integer bit wide
// For example 6 means an integer should be 6 bits like 011010 and maximum value is 63
// WARNING: Dont make it more than 64
inline const int MAX_INT_SIZE = 6;

/*

	Command Opcodes

RULES:

1)	Yes opcode is maybe first 1-4 or 8 bytes but this assembler needs full version without arguments.
	for example if your command is:

	JMP Addr	|	10 -- AAAA (- means no matter 1 or 0)

	AAAA is your arg and 10 is your opcode but assembler needs -- part.
	So your opcode in this config file should be 1000

	if your commands has no arguments just enter that binary



2)	You can see format at below but if you dont understand whats that:

	{
		"MVI", // Command name in assembly
		{
			{"binary", "00"},	// Binary opcode of command (remember rule 1!)
			{"arg_count", "1"},	// Arguments count
			{"valid_args", "int"}	// Valid argument types (read rule 3)
		}
	}



3)	Rules for args

	ARGS is defined under keywords. format for a arg set:
	
	{ 
		"reg", 			// Name of arg set
		{
			{"R0", "000"},	// Argument and its binary representation
			{"R1", "001"},
			{"R2", "010"},
			{"R3", "011"},
			{"R4", "100"},
			{"R5", "101"},
			{"IN", "110"},
			{"OUT", "110"},
			{"RAM", "111"}
		}
	}
	
	For using args in commands you should set "arg_count" and "valid_args".

	"arg_count":  Argument count if command doesnt takes argumnets it must be zero (DONT REMOVE THAT)
	"valid_args": Argument sets for command if just one set just give the sets name. if more than one split set names with ";" sign
		Examples:
			"reg"
			"reg|addr"
			"int|condr|reg"
		
		"int" is not an arg set it means command excepts integer values as argument. you can set max integer size in bits at start of the file
		if "arg_count" is zero you can remove that.
*/

inline const map<string, map<string, string>> KEYWORDS = {
    {
	"MVI",
	{
	    {"binary", "00"},
	    {"arg_count", "1"},
	    {"valid_args", "int"}
	}
    },
    {
	"OPPR",
	{
	    {"binary", "01000"},
	    {"arg_count", "1"},
	    {"valid_args", "oppr"}
	}
    },
    {
	"MOV",
	{
	    {"binary", "10"},
	    {"arg_count", "2"},
	    {"valid_args", "reg"}
	}
    },
    {
	"JMP",
	{
	    {"binary", "11000100"},
	    {"arg_count", "0"}
	}
    },
    {
	"JUMPIF",
	{
	    {"binary", "11000"},
	    {"arg_count", "1"},
	    {"valid_args", "cond"}
	}
    }
};

/*

	Arguments for commands.

*/
inline const map<string, map<string, string>> ARGS = {
	{ 
		"reg",
		{
			{"R0", "000"},
			{"R1", "001"},
			{"R2", "010"},
			{"R3", "011"},
			{"R4", "100"},
			{"R5", "101"},
			{"IN", "110"},
			{"OUT", "110"},
			{"RAM", "111"}
		}
	},

	{
		"oppr",
		{
			{"OR", "000"},
			{"AND", "001"},
			{"XOR", "010"},
			{"NOR", "011"},
			{"ADD", "100"},
			{"SUB", "101"}
		}
	},

	{
		"cond",
		{
			{"N", "000"},
			{"=", "001"},
			{"<", "010"},
			{"<=", "011"},
			{"A", "100"},
			{"!=", "101"},
			{">=", "110"},
			{">", "111"}
		}
	}
};
