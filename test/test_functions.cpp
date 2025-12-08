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

#include <gtest/gtest.h>

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

#include "../include/functions.hpp"

int MAX_INT = 6;
int INSTRUCTION = 8;
json KEYWORDS = R"(
{
    "MVI": {
        "binary": "00",
        "arg_count": 1,
        "arg_sets": "int"
    },
    "OPPR": {
        "binary": "01000",
        "arg_count": 1,
        "arg_sets": "oppr"
    },
    "MOV": {
        "binary": "10",
        "arg_count": 2,
        "arg_sets": "reg"
    },
    "JMP": {
        "binary": "11000100",
        "arg_count": 0
    },
    "JUMPIF": {
        "binary": "11000",
        "arg_count": 1,
        "arg_sets": "cond"
    }
}
)"_json;

json ARGS = R"(
{
    "reg": {
        "R0": "000",
        "R1": "001",
        "R2": "010",
        "R3": "011",
        "R4": "100",
        "R5": "101",
        "IN": "110",
        "OUT": "110",
        "RAM": "111"
    },
    "oppr": {
        "OR": "000",
        "AND": "001",
        "XOR": "010",
        "NOR": "011",
        "ADD": "100",
        "SUB": "101"
    },
    "cond": {
        "N": "000",
        "=": "001",
        "<": "010",
        "<=": "011",
        "A": "100",
        "!=": "101",
        ">=": "110",
        ">": "111"
    }
}
)"_json;

TEST(SplitFunc, TestJob) {
        const std::vector<std::string> HELLO = {"hello", "world"};
        const std::vector<std::string> FOO = {"foo", "bar", "baz"};
        const std::vector<std::string> TEST = {"test", "example"};

        EXPECT_EQ(split("hello world", ' '), HELLO);
        EXPECT_EQ(split("foo\nbar\nbaz", '\n'), FOO);
        EXPECT_EQ(split("testAexample", 'A'), TEST);
}

TEST(StripFunc, TestJob) {
        EXPECT_EQ(strip("    test1        "), "test1");
        EXPECT_EQ(strip("    left"), "left");
        EXPECT_EQ(strip("   at    middle    "), "at    middle");
        EXPECT_EQ(strip("right     "), "right");
        EXPECT_EQ(strip("\ttab\t"), "tab");
        EXPECT_EQ(strip("\nnew\nline\n"), "new\nline");
}

TEST(SplitAndCleanFunc, TestJob) {
        std::string T1 = "\ntest1\n\n";
        std::string T2 = "\ntest2\nand a bonus!\n";
        std::string T3 = "\nunder there\n; a comment!\nit should go";
        std::string T4 = "code and comment in same line; its a comment \nand another line \t;iam also comment";
        std::string T5 = "same line ; comment here";

        std::vector<std::string> TEST1 = { "test1" };
        std::vector<std::string> TEST2 = { "test2", "and a bonus!" };
        std::vector<std::string> TEST3 = { "under there", "it should go"};
        std::vector<std::string> TEST4 = { "code and comment in same line", "and another line" };
        std::vector<std::string> TEST5 = { "same line" };

        EXPECT_EQ(split_and_clean(T1), TEST1);
        EXPECT_EQ(split_and_clean(T2), TEST2);
        EXPECT_EQ(split_and_clean(T3), TEST3);
        EXPECT_EQ(split_and_clean(T4), TEST4);
        EXPECT_EQ(split_and_clean(T5), TEST5);
}

TEST(MkIntArgFunc, TestJob) {
        std::string s = "5";
        uint64_t r = 5;

        EXPECT_EQ(mk_int_arg(s, MAX_INT), r);
}

TEST(MkIntArgFunc, TestError) {
        std::string S5 = "938629806372";
        std::string S6 = "-5";
        std::string S7 = "kjsdgbcj";

        EXPECT_EXIT(
                mk_int_arg(S5, MAX_INT),
                ::testing::ExitedWithCode(1),
                "Integer is too big"
        );

        EXPECT_EXIT(
                mk_int_arg(S6, MAX_INT),
                ::testing::ExitedWithCode(1),
                "Signed binary"
        );

        EXPECT_EXIT(
                mk_int_arg(S7, MAX_INT),
                ::testing::ExitedWithCode(1),
                "Invalid argument:"
        );
}

TEST(MkBinaryFunc, TestJob) {
        std::vector<std::string> S1 = {"MOV", "R1", "R2"};
        json K1 = R"({
                "binary": "01",
                "arg_count": 2,
                "arg_sets": "reg"
        })"_json;
        
        std::string C1 = "MOV R1 R2";
        std::string E1 = "01001010";

        std::vector<std::string> S2 = {"MVI", "20"};
        json K2 = R"({
                "binary": "00",
                "arg_count": 1,
                "arg_sets": "int"
        })"_json;
        std::string C2 = "MVI 20";
        std::string E2 = "00010100";

        std::vector<std::string> S3 =  {"JMP"};
        json K3 = R"({	
                "binary": "11000100",
                "arg_count": 0
        })"_json;
        std::string C3 = "JMP";
        std::string E3 = "11000100";

        EXPECT_EQ(
                mk_binary( S1, K1, C1, ARGS, KEYWORDS, MAX_INT, INSTRUCTION),
                E1 
        );

        EXPECT_EQ(
                mk_binary(S2, K2, C2, ARGS, KEYWORDS, MAX_INT, INSTRUCTION),
                E2
        );

        EXPECT_EQ(
                mk_binary(S3, K3, C3, ARGS, KEYWORDS, MAX_INT, INSTRUCTION),
                E3
        );


}

TEST(MkBinaryFunc, TestError) {
        std::vector<std::string> S1 = {"MOV", "R1", "R2"};
        json K1 = R"({
                "binary": "01",
                "arg_count": 2,
                "arg_sets": "guluk"
        })"_json;
        std::string C1 = "MOV R1 R2";

        std::vector<std::string> S2 = {"MOV", "R1", "R2", "R3"};
        json K2 = R"({
                "binary": "01",
                "arg_count": 3,
                "arg_sets": "reg"
        })"_json;
        std::string C2 = "MOV R1 R2 R3";

        EXPECT_EXIT(
                mk_binary(S1, K1, C1, ARGS, KEYWORDS, MAX_INT, INSTRUCTION),
                ::testing::ExitedWithCode(1),
                "Unknown arg set in architecture"
        );

        EXPECT_EXIT(
                mk_binary(S2, K2, C2, ARGS, KEYWORDS, MAX_INT, INSTRUCTION),
                ::testing::ExitedWithCode(1),
                "Error in architecture or because of args. Binary command is too long. Command:"
        );
}

TEST(ParseCodeFunc, TestJob) {
        std::vector<std::string> code = {"MOV R1 R2", "JMP", "OPPR OR", "MVI 36"};
        std::vector<std::string> result = {"10001010", "11000100", "01000000", "00100100"};

        // This simple test should be enought
        EXPECT_EQ(parse_code(code, KEYWORDS, ARGS, INSTRUCTION, MAX_INT), result);
}

TEST(ParseCodeFunc, TestError) {
        std::vector<std::string> invalidkey = {"GULUK"};
        std::vector<std::string> muchargs = {"MOV R1 R2 R3 R4"};
        std::vector<std::string> lessargs = {"MOV R1"};

        EXPECT_EXIT(
                parse_code(invalidkey, KEYWORDS, ARGS, INSTRUCTION, MAX_INT),
                ::testing::ExitedWithCode(1),
                "Invalid keyword:"
        );

        EXPECT_EXIT(
                parse_code(muchargs, KEYWORDS, ARGS, INSTRUCTION, MAX_INT),
                ::testing::ExitedWithCode(1),
                "Expected"
        );

        EXPECT_EXIT(
                parse_code(lessargs, KEYWORDS, ARGS, INSTRUCTION, MAX_INT),
                ::testing::ExitedWithCode(1),
                "Expected"
        );
}

TEST(ConvertBinFunc, TestJob) {
        std::vector<std::string> b = {"10110101", "00000010"};
        std::vector<uint8_t> r = {181, 2};

        EXPECT_EQ(convert_bin(b), r);
}

TEST(ConvertHexFunc, TestJob) {
        std::vector<std::string> b = {"10110101", "00000010"};
        std::string r = "0xb5\n0x02\n";

        EXPECT_EQ(convert_hex(b, INSTRUCTION), r);
}