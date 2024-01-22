#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

bool char_in_num(char c)
{
    return '0' <= c && c <= '9';
}

bool char_is_symbol(char c)
{
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '^' || c == '%' || c == '(' || c == ')' || c == '=';
}

bool char_in_alphabet(char c)
{
    return 'a' <= c && c <= 'z' || 'A' <= c && c <= 'Z';
}

bool char_in_alphabet_and_underline(char c)
{
    char_in_alphabet(c) || c == '_';
}

bool char_in_hex(char c)
{
    return char_in_num(c) || 'a' <= c && c <= 'f' || 'A' <= c && c <= 'F';
}

typedef char u8;
typedef size_t usize;

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
    Var               // 变量
} TokenType;

typedef struct Token
{
    TokenType type;
    usize value;
} Token;

Token token_list[1024] = {0};

// adja_char_stynax[i][j] 是否合法
// 字符映射i，上一个字符映射j
// 合法 0 非法 1
u8 adja_char_stynax[8][8] = {
    // 数字 +*/^% - ( ) = 字母 空
    {0, 0, 0, 0, 1, 0, 0, 0}, // 数字
    {0, 1, 1, 0, 0, 1, 0, 1}, //+*/^%
    {0, 1, 1, 0, 0, 0, 0, 0}, //-
    {1, 0, 0, 0, 1, 0, 0, 0}, //(
    {0, 1, 1, 1, 0, 1, 0, 1}, //)
    {0, 1, 1, 1, 0, 1, 0, 1}, //=
    {0, 0, 0, 0, 1, 0, 0, 0}, // 字母
    {0, 1, 1, 1, 0, 1, 0, 0}  // 空
};

int adja_char_map(char c)
{
    if (char_in_num(c))
    {
        return 0;
    }
    else if (c == '+' || c == '*' || c == '/' || c == '^' || c == '%')
    {
        return 1;
    }
    else if (c == '-')
    {
        return 2;
    }
    else if (c == '(')
    {
        return 3;
    }
    else if (c == ')')
    {
        return 4;
    }
    else if (c == '=')
    {
        return 5;
    }
    else if (char_in_alphabet(c))
    {
        return 6;
    }
    else
    {
        // '\0'
        return 7;
    }
}

bool check_adja_char_stynax(char c, char prev_c)
{
    return adja_char_stynax[adja_char_map(c)][adja_char_map(prev_c)] == 0;
}

// 0 Err 1 十六进制整数 2 十进制整数 3 八进制整数 4 二进制整数 5 浮点数
u8 str_is_int_or_float(char *str)
{
    usize len = strlen(str);
    if (str[0] == '0') // 0开头的16进制或8进制数或2进制数
    {
        if (len <= 1)
        {
            return 0; // Err
        }
        switch (str[1])
        {
        case 'b':
            return 4; // 二进制整数
        case 'B':
            return 4;
        case 'x':
            return 1; // 十六进制整数
        case 'X':
            return 1;
        default:
            return 3; // 八进制整数
        }
    }
    usize i;
    for (i = 1; i < len; i++)
    {
        if (!char_in_num(str[i]))
        {
            if (str[i] == '.')
            {
                return 5; // 浮点数
            }
            else
            {
                return 2; // 十进制整数
            }
        }
    }
    return 2; // 十进制整数
}

long long parse_int(char *str)
{
}

double parse_float(char *str)
{
}

typedef enum ParseResult
{
    Ok,
    Err
} ParseResult;

ParseResult parse_to_token_list(char *str)
{
    usize len = strlen(str);
    usize i;
    usize j;
    char c;
    char prev_c = '\0';
    int parenthesis_layer = 0;
    for (i = j = 0; i < len; i++, j++)
    {
        c = str[i];
        if (c == ' ')
        {
            continue;
        }
        if (!check_adja_char_stynax(c, prev_c))
        {
            // 语法错误
            return Err;
        }
        switch (c)
        {
        case '+':
            token_list[j].type = Add;
            token_list[j].value = 0;
            break;
        case '-':
            if (char_in_num(prev_c) || prev_c == ')' || char_in_alphabet(prev_c)) // 是减号的情况
            {
                token_list[j].type = Sub;
                token_list[j].value = 0;
                break;
            }
            else // 是负号的情况
            {
                token_list[j].type = Neg;
                token_list[j].value = 0;
            }

        case '*':
            token_list[j].type = Mul;
            token_list[j].value = 0;
            break;
        case '/':
            token_list[j].type = Div;
            token_list[j].value = 0;
            break;
        case '^':
            token_list[j].type = Pow;
            token_list[j].value = 0;
            break;
        case '%':
            token_list[j].type = Mod;
            token_list[j].value = 0;
            break;
        case '(':
            token_list[j].type = LeftParenthesis;
            token_list[j].value = parenthesis_layer;
            parenthesis_layer++;
            break;
        case ')':
            if (parenthesis_layer == 0)
            {
                // 括号不匹配
                return Err;
            }
            parenthesis_layer--;
            token_list[j].type = RightParenthesis;
            token_list[j].value = parenthesis_layer;
            break;
        case '=':
            if (parenthesis_layer != 0)
            {
                return Err;
                // 括号不匹配
            }
            token_list[j].type = Eq;
            token_list[j].value = 0;
            break;
        default:
            if (char_in_num(c))
            {
                // 数字
                switch (str_is_int_or_float(&str[i]))
                {
                case 0: // Err
                    return Err;
                    break;
                case 5: // 浮点数
                    token_list[j].type = Float;
                    float *p = &token_list[j].value;
                    *p = strtof(&str[i], NULL);
                    break;
                default:
                    token_list[j].type = Int;
                    token_list[j].value = strtoul(&str[i], NULL, 0);
                }
                for (; i < len; i++)
                {
                    if (!(str[i] == '.' || char_in_hex(str[i]) || str[i] == 'x' || str[i] == 'X'))
                    {
                        break;
                    }
                }
                i--;
            }
            else if (char_in_alphabet(c))
            {
                // 函数或变量

                u8 *name = malloc(sizeof(u8) * 16);
                usize k;
                for (k = i; k < len; k++)
                {
                    if (!char_in_alphabet_and_underline(k))
                    {
                        name[k - i] = '\0';
                        if (str[k] == '(') // 函数
                        {
                            token_list[j].type = Func;
                        }
                        else
                        {
                            token_list[j].type = Var;
                        }
                        token_list[j].value = (usize)name;
                    }
                    else
                    {
                        if (k - i >= 15)
                        {
                            // 变量名或函数名过长
                            return Err;
                        }
                        name[k - i] = str[k];
                    }
                }
                i = k - 1;
            }
        }
        prev_c = str[i];
    }
    if (!check_adja_char_stynax('\0', c))
    {
        // 语法错误
        return Err;
    }
    if (parenthesis_layer != 0)
    {
        // 括号不匹配
        return Err;
    }
    return Ok;
}

int main()
{
    char str[] = "(123/3+456^2)*9.4";
    u8 res = parse_to_token_list(str);

    return 0;
}