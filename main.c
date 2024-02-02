#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <math.h>

#define TAB "│ "
#define FOLD_TAB "├─"

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
    return char_in_alphabet(c) || c == '_';
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
    Var,              // 变量
    Fraction          // 分数
} TokenType;

typedef struct Token
{
    TokenType type;
    usize value;
} Token;

// a/b +-a
typedef struct FractionData
{
    int a; // symbol
    int b; // b > 0
} FractionData;

Token token_list[1024] = {0};
int tokens_len = 0;
int gtoken_ind = 0;

Token *peek_token()
{
    if (gtoken_ind >= tokens_len)
        return NULL;
    return &token_list[gtoken_ind];
}

Token *next_token()
{
    if (gtoken_ind >= tokens_len)
        return NULL;
    Token *temp = &token_list[gtoken_ind];
    gtoken_ind++;
    return temp;
}

Token *dump_token(Token *t)
{
    Token *new_token = malloc(sizeof(Token));
    new_token->type = t->type;
    new_token->value = t->value;
}

void copy_token(Token *from, Token *to)
{
    to->type = from->type;
    to->value = from->value;
}

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

int parse_to_token_list(char *str)
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
            token_list[j].value = 0;
            break;
        case '-':
            if (char_in_num(prev_c) || prev_c == ')' || char_in_alphabet(prev_c)) // 是减号的情况
            {
                token_list[j].type = Sub;
                token_list[j].value = 0;
            }
            else // 是负号的情况
            {
                token_list[j].type = Neg;
                token_list[j].value = 0;
            }
            break;

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
                return -1;
            }
            parenthesis_layer--;
            token_list[j].type = RightParenthesis;
            token_list[j].value = parenthesis_layer;
            break;
        case '=':
            if (parenthesis_layer != 0)
            {
                return -1;
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
                    return -1;
                    break;
                case 5: // 浮点数
                    token_list[j].type = Float;
                    float *p = (float *)&token_list[j].value;
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
                        }
                        token_list[j].value = (usize)name;
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

void print_terimal_token(Token *t, bool newline)
{
    switch (t->type)
    {
    case Int:
        printf("Int(%d)", (int)t->value);
        break;
    case Float:
        float *p = (float *)&t->value;
        printf("Float(%.8f)", *p);
        break;
    case Var:
        printf("Var(%s)", (char *)t->value);
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
            printf("Int(%d)", (int)token_list[i].value);
            break;
        case Float:
            float *p = (float *)&token_list[i].value;
            printf("Float(%f)", *p);
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
            printf("Func{name: %s}", (char *)token_list[i].value);
            break;
        case Var:
            printf("Var{name: %s}", (char *)token_list[i].value);
            break;
        case LeftParenthesis:
            printf("( Layer: %d", (int)token_list[i].value);
            break;
        case RightParenthesis:
            printf(") Layer: %d", (int)token_list[i].value);
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

typedef struct AST_Node
{
    Token *token;
    struct AST_Node *left;
    struct AST_Node *right;
} AST_Node;

typedef struct BindingPower
{
    float left;
    float right;
} BindingPower;

BindingPower prefix_binding_power(TokenType op)
{
    BindingPower bp;
    if (op == Neg)
    {
        bp.left = -1;
        bp.right = 7;
    }
    return bp;
}

BindingPower infix_binding_power(TokenType op)
{
    BindingPower bp;
    if (op == Add || op == Sub)
    {
        bp.left = 3;
        bp.right = 4;
    }
    else if (op == Mul || op == Div || op == Mod)
    {
        bp.left = 5;
        bp.right = 6;
    }
    else if (op == Pow)
    {
        bp.left = 9;
        bp.right = 8;
    }
    else if (op == Eq)
    {
        bp.left = 1;
        bp.right = 2;
    }
    else
    {
        bp.left = -1;
        bp.right = -1;
    }
    return bp;
}

// bp binding power
AST_Node *expr_bp(int min_bp)
{
    AST_Node *rhs, *temp;
    BindingPower bp;
    AST_Node *lhs = malloc(sizeof(AST_Node));
    Token *temp_token = next_token();
    if (temp_token->type == Int || temp_token->type == Float || temp_token->type == Var)
    {
        // Atom
        lhs->token = dump_token(temp_token);
        lhs->left = lhs->right = NULL;
    }
    else if (temp_token->type == LeftParenthesis)
    {
        // 括号
        lhs = expr_bp(0);
        assert(next_token()->type == RightParenthesis);
    }
    else if (temp_token->type == Func)
    {
        // 函数
        assert(next_token()->type == LeftParenthesis);
        rhs = expr_bp(0);
        assert(next_token()->type == RightParenthesis);
        // lhs = &AST_Node{token: func, left: rhs, right: NULL}
        lhs->token = dump_token(temp_token);
        lhs->left = rhs;
        lhs->right = NULL;
    }
    else if (temp_token->type == Neg)
    {
        // 一元前缀运算符
        BindingPower temp_bp = prefix_binding_power(temp_token->type);
        rhs = expr_bp(temp_bp.right);
        // lhs = &AST_Node{token: op, left: rhs, right: NULL}
        lhs->token = dump_token(temp_token);
        lhs->left = rhs;
        lhs->right = NULL;
    }

    for (;;)
    {
        Token *op = peek_token();
        if (op == NULL)
            break;

        bp = infix_binding_power(op->type);
        if (bp.left != -1 && bp.right != -1)
        {
            if (bp.left < min_bp)
                break;
            next_token();
            rhs = expr_bp(bp.right);

            // lhs = &AST_Node{token: op, left: lhs, right: rhs}
            AST_Node *temp = malloc(sizeof(AST_Node));
            temp->token = dump_token(op);
            temp->left = lhs;
            temp->right = rhs;
            lhs = temp;
            continue;
        }
        break;
    }
    return lhs;
}

AST_Node *parse_to_ast()
{
    gtoken_ind = 0;
    AST_Node *node;
    node = expr_bp(0);
    gtoken_ind = 0;
    return node;
}

void print_tree_branch(int layer)
{
    if (layer == 0)
        return;
    int i;
    for (i = 0; i < layer - 1; i++)
        printf(TAB);
    printf(FOLD_TAB);
}

void print_ast(AST_Node *node, int layer)
{
    int i;
    if (node == NULL)
        return;

    print_tree_branch(layer);
    // Atom
    if (node->token->type == Int)
    {
        printf("Int(%d)\n", (int)node->token->value);
        return; // ！
    }
    else if (node->token->type == Float)
    {
        float *fp = (float *)&node->token->value;
        printf("Float(%f)\n", *fp);
        return;
    }
    else if (node->token->type == Var)
    {
        printf("Var(%s)\n", (char *)node->token->value);
        return;
    }

    // 递归
    printf("token: ");
    switch (node->token->type)
    {
    case Add:
        printf("+");
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
    case Neg:
        printf("Neg");
        break;
    case Pow:
        printf("^");
        break;
    case Mod:
        printf("%%");
        break;
    case Func:
        printf("Func(%s)", (char *)node->token->value);
        break;
    case Eq:
        printf("=");
        break;
    default:
        printf("还没实现");
        break;
    }
    printf("\n");
    // for (i = 0; i < layer; i++)
    //     printf(TAB);
    print_ast(node->left, layer + 1);
    // for (i = 0; i < layer; i++)
    //     printf(TAB);
    print_ast(node->right, layer + 1);
}

void cast_int2float_inplace(Token *t)
{
    assert(t->type == Int);
    float *p = (float *)&t->value;
    *p = (float)t->value;
    t->type = Float;
}

// left inplace return false when error
bool powii_inplace(Token *l_int, Token *r_int)
{
    int li, ri, i;
    bool neg = false;
    li = (int)l_int->value;
    ri = (int)r_int->value;
    if (li == 0 && ri <= 0)
        // 0^0
        // 0^(-x) (x>0)
        return false;
    if (li == 1)
    {
        return true;
    }
    if (ri < 0)
    {
        ri = -ri;
        neg = true;
    }
    // assert(ri > 0);
    int acc = 1;
    for (i = 0; i < ri; i++)
    {
        acc *= li;
    }
    li = acc;
    if (neg)
    {
        float *p = (float *)&l_int->value;
        *p = 1. / (float)li;
        l_int->type = Float;
    }
    else
    {
        int *p = (int *)&l_int->value;
        *p = li;
    }
    return true;
}

AST_Node *op_polymorphism(const AST_Node *base_l, const AST_Node *base_r, TokenType op)
{
    if (base_l == NULL || base_r == NULL)
        return NULL;
    AST_Node *res;
    res = malloc(sizeof(AST_Node));
    res->left = res->right = NULL;
    Token *l, *r;
    l = malloc(sizeof(Token));
    r = malloc(sizeof(Token));
    copy_token(base_l->token, l);
    copy_token(base_r->token, r);
    // 类型转换
    if (l->type == Float && r->type == Int)
    {
        cast_int2float_inplace(r);
    }
    else if (l->type == Int && r->type == Float)
    {
        cast_int2float_inplace(l);
    }

    // 计算
    if (l->type == Float && r->type == Float)
    {
        float *lp, *rp;
        lp = (float *)&l->value;
        rp = (float *)&r->value;
        switch (op)
        {
        case Add:
            *lp += *rp;
            break;
        case Sub:
            *lp -= *rp;
            break;
        case Mul:
            *lp *= *rp;
            break;
        case Div:
            // error x/0
            if (*rp == 0.)
                return NULL;
            *lp /= *rp;
            break;
        case Mod:
            // error x%0
            if (*rp == 0.)
                return 0;
            *lp = fmod(*lp, *rp);
            break;
        case Pow:
            // 实数幂运算不封闭
            *lp = powf(*lp, *rp);
            break;
        default:
            return NULL;
        }

        free(r);
        res->token = l;
        return res;
    }
    else if (l->type == Int && r->type == Int)
    {
        int *p = (int *)&l->value;
        switch (op)
        {
        case Add:
            *p = (int)l->value + (int)r->value;
            break;
        case Sub:
            *p = (int)l->value - (int)r->value;
            break;
        case Mul:
            *p = (int)l->value * (int)r->value;
            break;
        case Div:
            // 整数除法不封闭
            // error x/0
            if ((int)r->value == 0)
                return NULL;
            *p = (int)l->value / (int)r->value;
            break;
        case Mod:
            // error x%0
            if ((int)r->value == 0)
                return NULL;
            *p = (int)l->value % (int)r->value;
            break;
        case Pow:
            // 整数非负指数幂运算封闭
            // 整数负指数幂运算不封闭
            // error 0^0 0^(-x) (x<0 in Z)
            if (!powii_inplace(l, r))
                // pow error
                return NULL;
            break;
        default:
            break;
        }

        free(r);
        res->token = l;
        return res;
    }
    else
    {
        return NULL;
    }
}
void neg_inplace(AST_Node *node)
{
    // 从左子节点中取出记号的所有权移交到本节点上
    free(node->token);
    node->token = node->left->token;
    // 删除左子节点
    free(node->left);
    node->left = NULL;
    if (node->token->type == Float)
    {
        float *p = (float *)&node->token->value;
        *p = -*p;
    }
    else if (node->token->type == Int)
    {
        int *p = (int *)&node->token->value;
        *p = -*p;
    }
    else
        assert(false);
}

int try_sqrti(int x)
{
    int i;
    if (x == 0)
        return 0;
    for (i = 1;; i += 2)
    {
        x -= i;
        if (x < 0)
            return -1;
        else if (x == 0)
        {
            return (i + 1) / 2;
        }
    }
}

bool sqrt_inp(Token *x)
{
    if (x->type == Int)
    {
        if ((int)x->value < 0)
            return false;
        int temp = try_sqrti((int)x->value);
        if (temp == -1)
            cast_int2float_inplace(x);
        else
        {
            int *ip = (int *)&x->value;
            *ip = temp;
            return true;
        }
    }
    float *p = (float *)&x->value;
    *p = sqrtf(*p);
    if (*p == NAN)
        return false;
    return true;
}

bool func_call_inp(AST_Node *fn_node)
{
    assert(fn_node->left->token->type == Int || fn_node->left->token->type == Float || fn_node->left->token->type == Fraction);
    if (strcmp((char *)fn_node->token->value, "sqrt") == 0)
    {
        sqrt_inp(fn_node->left->token);
        free(fn_node->token);
        fn_node->token = fn_node->left->token;
        free(fn_node->left);
        fn_node->left = NULL;
        return true;
    }
    return false;
}

AST_Node *recu_calc(AST_Node *node)
{
    if (node == NULL)
        return NULL;
    if (node->token->type == Neg)
        neg_inplace(node);
    if (node->token->type == Func)
    {
        node->left = recu_calc(node->left);
        if (!func_call_inp(node))
            return NULL;
        return node;
    }
    if (node->token->type == Int || node->token->type == Float)
        // 基本情况
        return node;
    // 递归情况
    return op_polymorphism(recu_calc(node->left), recu_calc(node->right), node->token->type);
}

Token *calc(AST_Node *node)
{
    if (node == NULL)
        return NULL;
    if (node->token->type == Eq)
    {
        // 方程
        return NULL;
    }
    AST_Node *res = recu_calc(node);
    if (res == NULL)
        return NULL;
    return res->token;
}

int main()
{
    char str[] = "sqrt(0)-1";
    int state_code = parse_to_token_list(str);
    if (state_code == -1)
    {
        return -1;
    }
    printf("Tokens:\n");
    print_token_list();
    AST_Node *node = expr_bp(0);
    printf("\nAST:\n");
    print_ast(node, 0);
    Token *res_token = calc(node);
    printf("\ncalc result:\n");
    print_terimal_token(res_token, true);
    return 0;
}