#include "calculation.h"

// 返回拥有所有权的Token*
// 输入拥有所有权的Token*
// 消耗t
// return NULL when t == NULL
Token *transfrom_token_and_free(Token *f(const Token *const t), Token *t)
{
    if (t == NULL)
        return NULL;
    Token *temp = f(t);
    free(t);
    return temp;
}

// 返回拥有所有权的AST_Node*
// 输入拥有所有权的AST_Node*
// 消耗node
// return NULL when node == NULL
AST_Node *transfrom_ast_and_free(AST_Node *f(const AST_Node *const node), AST_Node *node)
{
    if (node == NULL)
        return NULL;
    AST_Node *temp = f(node);
    recu_free_ast(node);
    return temp;
}

// 返回拥有所有权的Token*
// never return NULL
Token *cast_int2float(const Token *const t)
{
    if (t->type == Float)
        return dump_token(t);
    assert(t->type == Int);
    Token *res = malloc(sizeof(Token));
    res->v.f = (float)t->v.i;
    res->type = Float;
    return res;
}

// 返回拥有所有权的Token*
// return NULL when error
Token *powii(const Token *const l_int, const Token *const r_int)
{
    assert(l_int->type == Int && r_int->type == Int);
    int li, ri, i, acc;
    bool neg = false;
    li = l_int->v.i;
    ri = r_int->v.i;
    if (li == 0 && ri <= 0)
        // 0^0
        // 0^(-x) (x>0)
        return NULL;
    if (li == 1)
    {
        return dump_token(l_int);
    }
    if (ri < 0)
    {
        ri = -ri;
        neg = true;
    }
    // assert(ri > 0);
    acc = 1;
    for (i = 0; i < ri; i++)
    {
        acc *= li;
    }
    Token *const res = malloc(sizeof(Token));
    if (neg)
    {
        res->type = Float;
        res->v.f = 1. / (float)acc;
    }
    else
    {
        res->type = Int;
        res->v.i = acc;
    }
    return res;
}

// 返回拥有所有权的Token*
// return NULL when r_int->v.i == 0
Token *divii(const Token *const l_int, const Token *const r_int)
{
    if (r_int->v.i == 0)
        return NULL;
    const int l = l_int->v.i;
    const int r = r_int->v.i;
    Token *res = malloc(sizeof(Token));
    if (l % r == 0)
    {
        // l整除r
        res->type = Int;
        res->v.i = l / r;
    }
    else
    {
        res->type = Float;
        res->v.f = (float)l / (float)r;
    }
    return res;
}

// 返回持有所有权的AST_Node*
// return NULL when error
AST_Node *op_polymorphism(const AST_Node *const base_l, const AST_Node *const base_r, const TokenType op)
{
    assert(base_l != NULL && base_r != NULL);
    AST_Node *res, *ret;
    res = malloc(sizeof(AST_Node));
    res->left = res->right = NULL;
    res->token = NULL;
    Token *l, *r;
    l = dump_token(base_l->token);
    r = dump_token(base_r->token);
    // 类型转换
    if (l->type == Float && r->type == Int)
    {
        r = transfrom_token_and_free(cast_int2float, r);
    }
    else if (l->type == Int && r->type == Float)
    {
        l = transfrom_token_and_free(cast_int2float, l);
    }

    // 计算
    if (l->type == Float && r->type == Float)
    {
        float *const lp = &l->v.f;
        const float *const rp = &r->v.f;
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
            {
                free(l);
                free(r);
                recu_free_ast(res);
                return NULL;
            }
            *lp /= *rp;
            break;
        case Mod:
            // error x%0
            if (*rp == 0.)
            {
                free(l);
                free(r);
                recu_free_ast(res);
                return NULL;
            }
            *lp = fmod(*lp, *rp);
            break;
        case Pow:
            // 实数幂运算不封闭
            *lp = powf(*lp, *rp);
            break;
        default:
            free(l);
            free(r);
            recu_free_ast(res);
            return NULL;
        }

        free(r);
        res->token = l;
        return res;
    }
    else if (l->type == Int && r->type == Int)
    {
        int *p = &l->v.i;
        switch (op)
        {
        case Add:
            *p = l->v.i + r->v.i;
            break;
        case Sub:
            *p = l->v.i - r->v.i;
            break;
        case Mul:
            *p = l->v.i * r->v.i;
            break;
        case Div:
            // 整数除法不封闭
            // error x/0
            ret = alloc_node(divii(l, r), NULL, NULL);
            free(l);
            free(r);
            recu_free_ast(res);
            return ret;
        case Mod:
            // error x%0
            if (r->v.i == 0)
            {
                free(l);
                free(r);
                recu_free_ast(res);
                return NULL;
            }
            *p = l->v.i % r->v.i;
            break;
        case Pow:
            // 整数非负指数幂运算封闭
            // 整数负指数幂运算不封闭
            // error 0^0 0^(-x) (x<0 in Z)
            ret = alloc_node(powii(l, r), NULL, NULL);
            free(l);
            free(r);
            recu_free_ast(res);
            return ret;
        default:
            free(l);
            free(r);
            recu_free_ast(res);
            return NULL;
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

// 返回拥有所有权的AST_Node*
// never return NULL
AST_Node *neg(const AST_Node *const node)
{
    assert(node->token->type == Int || node->token->type == Float);
    AST_Node *const res = alloc_node(malloc(sizeof(Token)), NULL, NULL);
    if (node->token->type == Int)
    {
        res->token->type = Int;
        res->token->v.i = -node->token->v.i;
    }
    else if (node->token->type == Float)
    {
        res->token->type = Float;
        res->token->v.f = -node->token->v.f;
    }
    return res;
}

// -1 no
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

// 返回拥有所有权的Token*
// return NULL when error
Token *sqrt_token(const Token *const x)
{
    assert(x->type == Int || x->type == Float);
    Token *t = dump_token(x);
    if (t->type == Int)
    {
        if (t->v.i < 0)
        {
            free(t);
            return NULL;
        }
        int temp = try_sqrti(t->v.i);
        if (temp != -1)
        {
            t->v.i = temp;
            return t;
        }
        else
        {
            t->v.f = sqrtf(t->v.i);
            t->type = Float;
            if (t->v.f == NAN)
            {
                free(t);
                return NULL;
            }
            return t;
        }
    }
    t->v.f = sqrtf(t->v.f);
    if (t->v.f == NAN)
    {
        free(t);
        return NULL;
    }
    return t;
}

// 返回拥有所有权的Token*
// return NULL when error
Token *fx_float_token(const Token *const x, float f(float))
{
    assert(x->type == Int || x->type == Float);
    Token *res;
    if (x->type == Int)
    {
        res = cast_int2float(x);
    }
    else if (x->type == Float)
    {
        res = dump_token(x);
    }
    res->v.f = f(res->v.f);
    if (!isfinite(res->v.f) && !isnan(res->v.f))
    {
        free(res);
        return NULL;
    }
    return res;
}

float cotf(float x)
{
    return 1. / tanf(x);
}

// 返回拥有所有权的AST_Node*
// return NULL when error
AST_Node *func_call(const AST_Node *const fn_node)
{
    if (fn_node == NULL)
        return NULL;
    assert(fn_node->token->type == Func);
    assert(fn_node->left->token->type == Int || fn_node->left->token->type == Float);
    const float *const fp = &fn_node->left->token->v.f;
    const int *const ip = &fn_node->left->token->v.i;
    const char *fn_name = (char *)fn_node->token->v.p;
    const Token *const fn_inner = fn_node->left->token;
    assert(fn_inner->type == Int || fn_inner->type == Float);
    if (strcmp(fn_name, "sqrt") == 0)
    {
        return alloc_node(sqrt_token(fn_inner), NULL, NULL);
    }
    else if (strcmp(fn_name, "sin") == 0)
    {
        return alloc_node(fx_float_token(fn_inner, sinf), NULL, NULL);
    }
    else if (strcmp(fn_name, "cos") == 0)
    {
        return alloc_node(fx_float_token(fn_inner, cosf), NULL, NULL);
    }
    else if (strcmp(fn_name, "tan") == 0)
    {
        return alloc_node(fx_float_token(fn_inner, tanf), NULL, NULL);
    }
    else if (strcmp(fn_name, "cot") == 0)
    {
        return alloc_node(fx_float_token(fn_inner, cotf), NULL, NULL);
    }
    else if (strcmp((char *)fn_node->token->v.p, "ln") == 0)
    {
        // e为底
        return alloc_node(fx_float_token(fn_inner, logf), NULL, NULL);
    }
    else if (strcmp((char *)fn_node->token->v.p, "log") == 0)
    {
        // 10为底
        return alloc_node(fx_float_token(fn_inner, log10f), NULL, NULL);
    }
    else if (strcmp((char *)fn_node->token->v.p, "abs") == 0)
    {
        AST_Node *res = alloc_node(malloc(sizeof(Token)), NULL, NULL);
        if (fn_inner->type == Int)
        {
            res->token->type = Int;
            res->token->v.i = abs(*ip);
        }
        else if (fn_inner->type == Float)
        {
            res->token->type = Float;
            res->token->v.i = fabsf(*fp);
        }
        return res;
    }
    else
    {
        return NULL;
    }
}

// 返回拥有所有权的Token*
// return NULL when var not found
Token *get_var_value(const char *name)
{
    for (int i = 0; i < symbol_table_len; i++)
    {
        if (strcmp(symbol_table[i].name, name) == 0)
        {
            Token *res = malloc((sizeof(Token)));
            res->type = Float;
            res->v.p = symbol_table[i].data;
            return res;
        }
    }
    return NULL;
}

// 返回拥有所有权的AST_Node*
// return NULL when error
AST_Node *recu_calc(const AST_Node *const node)
{
    if (node == NULL)
        return NULL;
    if (node->token->type == Neg)
    {
        AST_Node *neg_inner = recu_calc(node->left);
        AST_Node *res = neg(neg_inner);
        recu_free_ast(neg_inner);
        return res;
    }
    if (node->token->type == Func)
    {
        AST_Node *cp = deep_copy_node(node);
        cp->left = transfrom_ast_and_free(recu_calc, cp->left);
        return transfrom_ast_and_free(func_call, cp);
    }
    if (node->token->type == Var)
    {
        return alloc_node(get_var_value((char *)node->token->v.p), NULL, NULL);
    }
    if (node->token->type == Int || node->token->type == Float)
        // 基本情况
        return copy_node(node);
    // 递归情况
    AST_Node *l_res = recu_calc(node->left);
    AST_Node *r_res = recu_calc(node->right);
    AST_Node *res = op_polymorphism(l_res, r_res, node->token->type);
    recu_free_ast(l_res);
    recu_free_ast(r_res);
    return res;
}

// 返回拥有所有权的Token*
// return NULL when error
Token *calc(const AST_Node *const node)
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
    Token *ret = dump_token(res->token);
    recu_free_ast(res);
    return ret;
}

bool assign_real_var(const char *name, const float value)
{
    for (int i = 0; i < symbol_table_len; i++)
    {
        if (strcmp(symbol_table[i].name, name) == 0)
        {
            float *fp = (float *)&symbol_table[i].data;
            *fp = value;
            return true;
        }
    }
    return false;
}

float get_delta(const AST_Node *const node, float x)
{
    assert(assign_real_var("x", x));
    Token *tl, *tr;
    tl = calc(node->left);
    tr = calc(node->right);
    tl = transfrom_token_and_free(cast_int2float, tl);
    tr = transfrom_token_and_free(cast_int2float, tr);
    return tl->v.f - tr->v.f;
}

float calc_x_expr(const AST_Node *const node, float x)
{
    assign_real_var("x", x);
    float *f;
    Token *t;
    t = calc(node);
    t = transfrom_token_and_free(cast_int2float, t);
    return t->v.f;
}

AST_Node *solve(const AST_Node *const node, const Token *const initial_x)
{
    assert(node != NULL);
    if (node->token->type != Eq)
    {
        return NULL;
    }
    // assign x
    float x, y, y_, dydx;
    Token *temp;
    if (initial_x == NULL)
    {
        temp = get_var_value("x");
        assert(temp != NULL);
        assert(temp->type == Float);
        x = temp->v.f;
        free(temp);
    }
    else
    {
        assert(initial_x->type == Float);
        x = initial_x->v.f;
    }
    for (int i = 0; i < 1000; i++)
    {
        y = get_delta(node, x - EPSILON);
        y_ = get_delta(node, x + EPSILON);
        dydx = (y_ - y) / EPSILON / 2;
        if (dydx == 0)
        {
            continue;
        }
        x = x - y / dydx;
    }
    AST_Node *eq = malloc(sizeof(AST_Node));
    AST_Node *x_node = malloc(sizeof(AST_Node));
    AST_Node *value_node = malloc(sizeof(AST_Node));

    eq->token = malloc(sizeof(Token));
    eq->token->type = Eq;
    eq->token->v.i = 0;
    eq->left = x_node;
    eq->right = value_node;
    x_node->token = malloc(sizeof(Token));
    x_node->token->type = Var;
    x_node->token->v.p = "x";
    x_node->left = x_node->right = NULL;
    value_node->token = malloc(sizeof(Token));
    value_node->token->type = Float;
    value_node->token->v.f = x;
    value_node->left = value_node->right = NULL;
    return eq;
}

float recu_solve_dichotomy_float(const AST_Node *const node, const Token *const left_x, const Token *const right_x)
{
}

// x^y
int simple_pow(const int x, const int y)
{
    int acc = 1;
    int i = 0;
    for (i = 0; i < y; i++)
    {
        acc *= x;
    }
    return acc;
}

AST_Node *ast_x_eq_float(float x)
{
    AST_Node *eq = malloc(sizeof(AST_Node));
    AST_Node *x_node = malloc(sizeof(AST_Node));
    AST_Node *value_node = malloc(sizeof(AST_Node));

    eq->token = malloc(sizeof(Token));
    eq->token->type = Eq;
    eq->token->v.i = 0;
    eq->left = x_node;
    eq->right = value_node;
    x_node->token = malloc(sizeof(Token));
    x_node->token->type = Var;
    x_node->token->v.p = "x";
    x_node->left = x_node->right = NULL;
    value_node->token = malloc(sizeof(Token));
    value_node->token->type = Float;
    value_node->token->v.f = x;
    value_node->left = value_node->right = NULL;
    return eq;
}

AST_Node *solve_dichotomy(const AST_Node *const node, const Token *const left_x, const Token *const right_x)
{
    assert(node != NULL && node->token->type == Eq);
    assert(left_x != NULL && left_x->type == Float);
    assert(right_x != NULL && right_x->type == Float);
    float xl, xr, xm;
    float step, l, r, m, res_x;
    int i, j;
    bool break_out = false;
    xl = left_x->v.f;
    xr = right_x->v.f;
    l = get_delta(node, xl);
    r = get_delta(node, xr);
    if (l * r > 0)
        for (i = 1; i < 8; i++)
        {
            step = (xr - xl) / simple_pow(2, i);
            for (j = 0; j < simple_pow(2, i); j += 2)
            {
                xr = xl + (step + step * j);
                r = get_delta(node, xr);
                if (l * r <= 0)
                {
                    break_out = true;
                    break;
                }
            }
            if (break_out)
                break;
        }
    if (l == 0.)
        return ast_x_eq_float(xl);
    else if (r == 0.)
    {
        return ast_x_eq_float(xr);
    }
    if (l * r > 0)
    {
        // solution not found
        return NULL;
    }
    for (i = 0; i < 100000; i++)
    {
        l = get_delta(node, xl);
        r = get_delta(node, xr);
        xm = (xl + xr) / 2.;
        m = get_delta(node, xm);
        if (xr - xl <= __FLT_MIN__ * 10 || m == 0.)
            return ast_x_eq_float(xm);
        if (l * m < 0)
        {
            xr = xm;
        }
        else
        {
            xl = xm;
        }
    }
    if (xr - xl <= EPSILON)
    {
        return ast_x_eq_float(xm);
    }
    else
    {
        return NULL;
    }
}
