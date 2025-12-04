#include <gtest/gtest.h>

#include <map>

#include "../include/functions.hpp"

TEST(MergeMapFunc, TestJob) {
        const map<string, string> MAP1 = {
                {"test", "other"},
                {"hello", "world"}
        };

        const map<string, string> MAP2 = {
                {"foo", "bar"}
        };

        const map<string, string> MAP3 = {
                {"test", "other"},
                {"hello", "world"},
                {"foo", "bar"}
        };

        const map<string, string> MAP4;

        EXPECT_EQ(mergeMap(MAP1, MAP2), MAP3);
        EXPECT_EQ(mergeMap(MAP1, MAP3), MAP3);
        EXPECT_EQ(mergeMap(MAP2, MAP3), MAP3);
        EXPECT_EQ(mergeMap(MAP1, MAP4), MAP1);
}

TEST(SplitFunc, TestJob) {
        const vector<string> HELLO = {"hello", "world"};
        const vector<string> FOO = {"foo", "bar", "baz"};
        const vector<string> TEST = {"test", "example"};

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
        string T1 = "\ntest1\n\n";
        string T2 = "\ntest2\nand a bonus!\n";
        string T3 = "\nunder there\n; a comment!\nit should go";
        string T4 = "code and comment in same line; its a comment \nand another line \t;iam also comment";
        string T5 = "same line ; comment here";

        vector<string> TEST1 = { "test1" };
        vector<string> TEST2 = { "test2", "and a bonus!" };
        vector<string> TEST3 = { "under there", "it should go"};
        vector<string> TEST4 = { "code and comment in same line", "and another line" };
        vector<string> TEST5 = { "same line" };

        EXPECT_EQ(split_and_clean(T1), TEST1);
        EXPECT_EQ(split_and_clean(T2), TEST2);
        EXPECT_EQ(split_and_clean(T3), TEST3);
        EXPECT_EQ(split_and_clean(T4), TEST4);
        EXPECT_EQ(split_and_clean(T5), TEST5);
}

TEST(MkIntArgFunc, TestJob) {
        string s = "5";
        uint64_t r = 5;

        EXPECT_EQ(mk_int_arg(s), r);
}

TEST(MkIntArgFunc, TestError) {
        string S5 = "938629806372";
        string S6 = "-5";
        string S7 = "kjsdgbcj";

        EXPECT_EXIT(
                mk_int_arg(S5),
                ::testing::ExitedWithCode(1),
                "Integer is too big"
        );

        EXPECT_EXIT(
                mk_int_arg(S6),
                ::testing::ExitedWithCode(1),
                "Signed binary"
        );

        EXPECT_EXIT(
                mk_int_arg(S7),
                ::testing::ExitedWithCode(1),
                "Invalid argument:"
        );
}

TEST(MkBinaryFunc, TestJob) {
        vector<string> S1 = {"MOV", "R1", "R2"};
        map<string, string> K1 = {
                {"binary", "01"},
                {"arg_count", "2"},
                {"valid_args", "reg"}
        };
        string C1 = "MOV R1 R2";
        string E1 = "01001010";

        vector<string> S2 = {"MVI", "20"};
        map<string, string> K2 = {
                {"binary", "00"},
                {"arg_count", "1"},
                {"valid_args", "int"}
        };
        string C2 = "MVI 20";
        string E2 = "00010100";

        vector<string> S3 =  {"JMP"};
        map<string, string> K3 = {	
                {"binary", "11000100"},
                {"arg_count", "0"}
        };
        string C3 = "JMP";
        string E3 = "11000100";

        EXPECT_EQ(
                mk_binary( S1, K1, C1),
                E1 
        );

        EXPECT_EQ(
                mk_binary(S2, K2, C2),
                E2
        );

        EXPECT_EQ(
                mk_binary(S3, K2, C2),
                E2
        );


}

TEST(MkBinaryFunc, TestError) {
        vector<string> S1 = {"MOV", "R1", "R2"};
        map<string, string> K1 = {
                {"binary", "01"},
                {"arg_count", "2"},
                {"valid_args", "guluk"}
        };
        string C1 = "MOV R1 R2";

        vector<string> S2 = {"MOV", "R1", "R2", "R3"};
        map<string, string> K2 = {
                {"binary", "01"},
                {"arg_count", "3"},
                {"valid_args", "reg"}
        };
        string C2 = "MOV R1 R2 R3";

        EXPECT_EXIT(
                mk_binary(S1, K1, C1),
                ::testing::ExitedWithCode(1),
                "Unknown arg set in architecture"
        );

        EXPECT_EXIT(
                mk_binary(S2, K2, C2),
                ::testing::ExitedWithCode(1),
                "Error in architecture or because of args. Binary command is too long. Command:"
        );
}

TEST(ParseCodeFunc, TestJob) {
        vector<string> code = {"MOV R1 R2", "JMP", "OPPR OR", "MVI 36"};
        vector<string> result = {"10001010", "11000100", "01000000", "00100100"};

        // This simple test should be enought
        EXPECT_EQ(parse_code(code), result);
}

TEST(ParseCodeFunc, TestError) {
        vector<string> invalidkey = {"GULUK"};
        vector<string> muchargs = {"MOV R1 R2 R3 R4"};
        vector<string> lessargs = {"MOV R1"};

        EXPECT_EXIT(
                parse_code(invalidkey),
                ::testing::ExitedWithCode(1),
                "Invalid keyword:"
        );

        EXPECT_EXIT(
                parse_code(muchargs),
                ::testing::ExitedWithCode(1),
                "Expected"
        );

        EXPECT_EXIT(
                parse_code(lessargs),
                ::testing::ExitedWithCode(1),
                "Expected"
        );
}

TEST(ConvertBinFunc, TestJob) {
        vector<string> b = {"10110101", "00000010"};
        vector<uint8_t> r = {181, 2};

        EXPECT_EQ(convert_bin(b), r);
}

TEST(ConvertHexFunc, TestJob) {
        vector<string> b = {"10110101", "00000010"};
        string r = "0xb5\n0x02\n";

        EXPECT_EQ(convert_hex(b), r);
}