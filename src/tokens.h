#ifndef __PARSE_TOKEN_H__
#define __PARSE_TOKEN_H__

#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

#include "type.h"
#include "symbol_table.h"

typedef enum TokenType
{
    Int = 0,          // 整数
    Float,            // 浮点数
    Add,              // 加
    Sub,              // 减
    Neg,              // 负
    Mul,              // 乘
    Div,              // 除
    Pow,              // 幂
    Mod,              // 模
    LeftParenthesis,  // 左括号
    RightParenthesis, // 有括号
    Eq,               // 等号
    Func,             // 函数
    Var,              // 变量
    Fraction          // 分数
} TokenType;          // size 1B

typedef union TokenValue
{
    int i;
    float f;
    void *p;
} TokenValue; // size 4B

typedef struct Token
{
    TokenType type;
    TokenValue v;
} Token; // size 8B

extern Token token_list[1024];
extern int tokens_len;
extern int gtoken_ind;

// bool char_in_num(char c);
// bool char_is_symbol(char c);
// bool char_in_alphabet(char c);
// bool char_in_alphabet_and_underline(char c);
// bool char_in_hex(char c);

const Token *peek_token();
Token *next_token();
Token *dump_token(const Token *const t);
void copy_token(const Token *const from, Token *const to);
Token *new_token_from_float(float x);
Token *new_token_from_int(int x);

int parse_to_token_list(const char str[]);
void print_terimal_token(const Token *const t, const bool newline);
void print_token_list();

#endif