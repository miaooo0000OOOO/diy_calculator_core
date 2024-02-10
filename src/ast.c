#include "ast.h"

/// 返回拥有所有权的AST_Node*
// return NULL when t == NULL
AST_Node *alloc_node(Token *const t, AST_Node *const left, AST_Node *const right)
{
    if (t == NULL)
        return NULL;
    AST_Node *const node = malloc(sizeof(AST_Node));
    node->token = t;
    node->left = left;
    node->right = right;
    return node;
}

// private
BindingPower prefix_binding_power(const TokenType op)
{
    BindingPower bp;
    if (op == Neg)
    {
        bp.left = -1;
        bp.right = 7;
    }
    else
    {
        bp.left = bp.right = -1;
    }
    return bp;
}

// private
BindingPower infix_binding_power(const TokenType op)
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
        bp.right = bp.left = -1;
    }
    return bp;
}

// private
// 返回拥有所有权的AST_Node*
// never return NULL
// bp binding power
AST_Node *expr_bp(const int min_bp)
{
    AST_Node *rhs;
    AST_Node *lhs = malloc(sizeof(AST_Node));
    Token *temp_token = dump_token(next_token());
    if (temp_token->type == Int || temp_token->type == Float || temp_token->type == Var)
    {
        // Atom
        lhs->token = temp_token;
        lhs->left = lhs->right = NULL;
    }
    else if (temp_token->type == LeftParenthesis)
    {
        // 括号
        free(lhs);
        free(temp_token);
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
        lhs->token = temp_token;
        lhs->left = rhs;
        lhs->right = NULL;
    }
    else if (temp_token->type == Neg)
    {
        // 一元前缀运算符
        const BindingPower temp_bp = prefix_binding_power(temp_token->type);
        rhs = expr_bp(temp_bp.right);
        // lhs = &AST_Node{token: op, left: rhs, right: NULL}
        lhs->token = temp_token;
        lhs->left = rhs;
        lhs->right = NULL;
    }

    const Token *op;
    for (;;)
    {
        op = peek_token();
        if (op == NULL)
            break;

        const BindingPower bp = infix_binding_power(op->type);
        if (bp.left != -1 && bp.right != -1)
        {
            if (bp.left < min_bp)
                break;
            next_token();
            rhs = expr_bp(bp.right);

            // lhs = &AST_Node{token: op, left: lhs, right: rhs}
            lhs = alloc_node(dump_token(op), lhs, rhs);
            continue;
        }
        break;
    }
    return lhs;
}

// 返回拥有所有权的AST_Node*
// never return NULL
AST_Node *parse_to_ast()
{
    gtoken_ind = 0;
    AST_Node *node;
    node = expr_bp(0);
    gtoken_ind = 0;
    return node;
}

// private
void print_tree_branch(const int layer)
{
    if (layer == 0)
        return;
    int i;
    for (i = 0; i < layer - 1; i++)
        printf(TAB);
    printf(FOLD_TAB);
}

void print_ast(const AST_Node *const node, const int layer)
{
    if (node == NULL)
        return;

    print_tree_branch(layer);
    // Atom
    if (node->token->type == Int)
    {
        printf("Int(%d)\n", node->token->v.i);
        return; // ！
    }
    else if (node->token->type == Float)
    {
        printf("Float(%f)\n", node->token->v.f);
        return;
    }
    else if (node->token->type == Var)
    {
        printf("Var(%s)\n", (char *)node->token->v.p);
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
        printf("Func(%s)", (char *)node->token->v.p);
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

AST_Node *copy_node(const AST_Node *const node)
{
    AST_Node *new_node = malloc(sizeof(AST_Node));
    new_node->left = node->left;
    new_node->right = node->right;
    new_node->token = dump_token(node->token);
    return new_node;
}

AST_Node *deep_copy_node(const AST_Node *const node)
{
    AST_Node *new_node;
    new_node = copy_node(node);
    if (node->token->type == Int || node->token->type == Float || node->token->type == Var)
    {
    }
    else if (node->token->type == Func || node->token->type == Neg)
    {
        new_node->left = deep_copy_node(node->left);
    }
    else
    {
        new_node->left = deep_copy_node(node->left);
        new_node->right = deep_copy_node(node->right);
    }
    return new_node;
}

void recu_free_ast(const AST_Node *const node)
{
    if (node == NULL)
        return;
    if (node->token != NULL)
        free(node->token);
    recu_free_ast(node->right);
    recu_free_ast(node->left);
}