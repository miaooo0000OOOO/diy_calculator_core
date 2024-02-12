#ifndef __AST_H__
#define __AST_H__

#include "tokens.h"

#define TAB "│ "
#define FOLD_TAB "├─"

// a/b +-a
typedef struct FractionData
{
    int a; // symbol
    int b; // b > 0
} FractionData;

typedef struct AST_Node
{
    Token *token;
    struct AST_Node *left;
    struct AST_Node *right;
} AST_Node; // size 12B

typedef struct BindingPower
{
    float left;
    float right;
} BindingPower; // size 8B

AST_Node *parse_to_ast();
void print_ast(const AST_Node *const node, const int layer);
AST_Node *copy_node(const AST_Node *const node);
AST_Node *deep_copy_node(const AST_Node *const node);
void recu_free_ast(const AST_Node *const node);
AST_Node *alloc_node(Token *const t, AST_Node *const left, AST_Node *const right);
#endif