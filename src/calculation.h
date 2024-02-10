#ifndef __CALCULATION_H__
#define __CALCULATION_H__

#include <stdbool.h>
#include <math.h>

#include "tokens.h"
#include "ast.h"

#define EPSILON 0.001

Token *calc(const AST_Node *const node);
float calc_x_expr(const AST_Node *const node, float x);
AST_Node *solve(const AST_Node *const node);
#endif