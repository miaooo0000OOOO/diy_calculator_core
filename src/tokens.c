#include "tokens.h"

bool char_in_num(const char c)
{
    return '0' <= c && c <= '9';
}

bool char_is_symbol(const char c)
{
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '^' || c == '%' || c == '(' || c == ')' || c == '=';
}

bool char_in_alphabet(const char c)
{
    return 'a' <= c && c <= 'z' || 'A' <= c && c <= 'Z';
}

bool char_in_alphabet_and_underline(const char c)
{
    return char_in_alphabet(c) || c == '_';
}

bool char_in_hex(const char c)
{
    return char_in_num(c) || 'a' <= c && c <= 'f' || 'A' <= c && c <= 'F';
}

// 返回拥有所有权的Token*
Token *new_token_from_float(float x)
{
    Token *res = malloc(sizeof(Token));
    res->type = Float;
    res->v.f = x;
    return res;
}

// 返回拥有所有权的Token*
Token *new_token_from_int(int x)
{
    Token *res = malloc(sizeof(Token));
    res->type = Int;
    res->v.i = x;
    return res;
}

// 返回Token的不可变引用
// return NULL when out of index
const Token *const peek_token()
{
    if (gtoken_ind >= tokens_len)
        return NULL;
    return &token_list[gtoken_ind];
}

// 返回Token的不可变引用
// return NULL when no next
Token *next_token()
{
    if (gtoken_ind >= tokens_len)
        return NULL;
    Token *temp = &token_list[gtoken_ind];
    gtoken_ind++;
    return temp;
}

// 返回拥有所有权的Token*
// return NULL when t == NULL
Token *dump_token(const Token *const t)
{
    if (t == NULL)
        return NULL;
    Token *new_token = malloc(sizeof(Token));
    new_token->type = t->type;
    new_token->v = t->v;
    return new_token;
}

void copy_token(const Token *const from, Token *const to)
{
    to->type = from->type;
    to->v = from->v;
}

Token token_list[1024] = {0};
int tokens_len = 0;
int gtoken_ind = 0;

// private
// adja_char_stynax[i][j] 是否合法
// 字符映射i，上一个字符映射j
// 合法 0 非法 1
const u8 adja_char_stynax[8][8] = {
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

// private
int adja_char_map(const char c)
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

// private
bool check_adja_char_stynax(const char c, const char prev_c)
{
    return adja_char_stynax[adja_char_map(c)][adja_char_map(prev_c)] == 0;
}

// private
// 0 Err 1 十六进制整数 2 十进制整数 3 八进制整数 4 二进制整数 5 浮点数
u8 str_is_int_or_float(const char str[])
{
    const usize len = strlen(str);
    if (str[0] == '0') // 0开头的16进制或8进制数或2进制数
    {
        if (len <= 1)
        {
            return 0; // Err
        }
        switch (str[1])
        {
        case '.':
            return 5; // 浮点数
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

// private
unsigned long strbintol(const char str[], const usize len)
{
    assert(len > 2);
    assert(str[0] == '0' && str[1] == 'b' || str[1] == 'B');
    unsigned long n = 0;
    for (usize i = 2; i < len; i++)
    {
        if (str[i] != '0' && str[i] != '1')
            break;
        n *= 2;
        if (str[i] == '1')
            n += 1;
    }
    return n;
}

// return -1 when error
// return len(tokens)
int parse_to_token_list(const char str[])
{
    const usize len = strlen(str);
    usize i;
    usize j;
    char c;
    char prev_c = '\0';
    int parenthesis_layer = 0;
    for (i = j = 0; i < len; i++, j++)
    {
        c = str[i];
        if (c == ' ' || c == '\n' || c == '\r')
        {
            j--;
            continue;
        }
        if (!check_adja_char_stynax(c, prev_c))
        {
            // 语法错误
            return -1;
        }
        switch (c)
        {
        case '+':
            token_list[j].type = Add;
            token_list[j].v.i = 0;
            break;
        case '-':
            if (char_in_num(prev_c) || prev_c == ')' || char_in_alphabet(prev_c)) // 是减号的情况
            {
                token_list[j].type = Sub;
                token_list[j].v.i = 0;
            }
            else // 是负号的情况
            {
                token_list[j].type = Neg;
                token_list[j].v.i = 0;
            }
            break;

        case '*':
            token_list[j].type = Mul;
            token_list[j].v.i = 0;
            break;
        case '/':
            token_list[j].type = Div;
            token_list[j].v.i = 0;
            break;
        case '^':
            token_list[j].type = Pow;
            token_list[j].v.i = 0;
            break;
        case '%':
            token_list[j].type = Mod;
            token_list[j].v.i = 0;
            break;
        case '(':
            token_list[j].type = LeftParenthesis;
            token_list[j].v.i = parenthesis_layer;
            parenthesis_layer++;
            break;
        case ')':
            if (parenthesis_layer == 0)
            {
                // 括号不匹配
                return -1;
            }
            parenthesis_layer--;
            token_list[j].type = RightParenthesis;
            token_list[j].v.i = parenthesis_layer;
            break;
        case '=':
            if (parenthesis_layer != 0)
            {
                return -1;
                // 括号不匹配
            }
            token_list[j].type = Eq;
            token_list[j].v.i = 0;
            break;
        default:
            if (char_in_num(c))
            {
                // 数字
                switch (str_is_int_or_float(&str[i]))
                {
                case 0: // Err
                    return -1;
                    break;
                case 4:
                    token_list[j].type = Int;
                    token_list[j].v.i = strbintol(&str[i], len - i);
                    break;
                case 5: // 浮点数
                    token_list[j].type = Float;
                    token_list[j].v.f = strtof(&str[i], NULL);
                    break;
                default:
                    token_list[j].type = Int;
                    token_list[j].v.i = strtoul(&str[i], NULL, 0);
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
                for (k = i; k < len + 1; k++)
                {
                    if (!char_in_alphabet_and_underline(str[k]))
                    {
                        name[k - i] = '\0';
                        if (str[k] == '(') // 函数
                        {
                            token_list[j].type = Func;
                        }
                        else
                        {
                            token_list[j].type = Var;
                            symbol_table[symbol_table_len].type = Real;
                            symbol_table[symbol_table_len].name = name;
                            symbol_table[symbol_table_len].data = NULL;
                            symbol_table_len++;
                        }
                        token_list[j].v.p = name;
                        // printf("name:%s\n", name);
                        break;
                    }
                    else
                    {
                        if (k - i >= 15)
                        {
                            // 变量名或函数名过长
                            return -1;
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
        return -1;
    }
    if (parenthesis_layer != 0)
    {
        // 括号不匹配
        return -1;
    }
    tokens_len = j;
    return j;
}

void print_terimal_token(const Token *const t, const bool newline)
{
    switch (t->type)
    {
    case Int:
        printf("Int(%d)", t->v.i);
        break;
    case Float:
        printf("Float(%f)", t->v.f);
        break;
    case Var:
        printf("Var(%s)", (char *)t->v.p);
    default:
        break;
    }
    if (newline)
        printf("\n");
}

void print_token_list()
{
    int i;
    Token this_token;
    for (i = 0; i < tokens_len; i++)
    {
        switch (token_list[i].type)
        {
        case Int:
            printf("Int(%d)", token_list[i].v.i);
            break;
        case Float:
            printf("Float(%f)", token_list[i].v.f);
            break;
        case Add:
            printf("+");
            break;
        case Neg:
            printf("Neg");
            break;
        case Sub:
            printf("Sub");
            break;
        case Mul:
            printf("*");
            break;
        case Div:
            printf("/");
            break;
        case Mod:
            printf("%%");
            break;
        case Pow:
            printf("^");
            break;
        case Func:
            printf("Func{name: %s}", (char *)token_list[i].v.p);
            break;
        case Var:
            printf("Var{name: %s}", (char *)token_list[i].v.p);
            break;
        case LeftParenthesis:
            printf("( Layer: %d", token_list[i].v.i);
            break;
        case RightParenthesis:
            printf(") Layer: %d", token_list[i].v.i);
            break;
        case Eq:
            printf("=");
            break;
        default:
            printf("还没实现");
            break;
        }
        printf("\n");
    }
}