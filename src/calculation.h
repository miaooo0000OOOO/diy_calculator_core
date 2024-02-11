#ifndef __CALCULATION_H__
#define __CALCULATION_H__

#include <stdbool.h>
#include <math.h>

#include "tokens.h"
#include "ast.h"

#define EPSILON 0.0005

Token *calc(const AST_Node *const node);
float calc_x_expr(const AST_Node *const node, float x);
AST_Node *solve(const AST_Node *const node, const Token *const initial_x);
AST_Node *solve_dichotomy(const AST_Node *const node, const Token *const left_x, const Token *const right_x);
float get_delta(const AST_Node *const node, float x);
Token *get_var_value(const char *name);
#endif