#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <math.h>

#include "type.h"
#include "tokens.h"
#include "symbol_table.h"
#include "ast.h"
#include "calculation.h"
#include "arraylist.h"

int main()
{
    char str[100] = {0};
    AST_Node *node, *temp;
    // for (;;)
    // {
    //     printf(">>> ");
    //     setbuf(stdin, NULL);
    //     char *a = fgets(str, 100, stdin);
    //     if (a == NULL)
    //     {
    //         break;
    //     }
    //     int state_code = parse_to_token_list(str);
    //     if (state_code == -1)
    //         continue;
    //     node = parse_to_ast();
    //     if (node == NULL)
    //         continue;
    //     float x = 0;
    //     for (int i = 0; i < 10; i++)
    //     {
    //         x += EPSILON;
    //         temp = deep_copy_node(node);
    //         printf("x = %f\nf(x) = %f\n\n", x, calc_x_expr(temp, x));
    //         recu_free_ast(temp);
    //     }
    // }

    for (;;)
    {
        printf(">>> ");
        setbuf(stdin, NULL);
        char *a = fgets(str, 100, stdin);
        if (a == NULL)
        {
            break;
        }
        int state_code = parse_to_token_list(str);
        if (state_code == -1)
            continue;
        // printf("Tokens:\n");
        // print_token_list();
        AST_Node *node = parse_to_ast();
        // printf("\nAST:\n");
        // print_ast(node, 0);
        if (node->token->type == Eq)
        {
            // 牛顿迭代法
            // float x;
            // printf("initial x:");
            // scanf("%f", &x);
            // Token *temp_x_token = new_token_from_float(x);
            // AST_Node *solution = solve(node, temp_x_token);
            // free(temp_x_token);
            // if (solution == NULL)
            // {
            //     recu_free_ast(node);
            //     continue;
            // }
            // print_ast(solution, 0);
            // recu_free_ast(solution);
            // Token *var_x = get_var_value("x");
            // printf("\ndelta: %f\n", fabs(get_delta(node, var_x->v.f)));

            // 二分法
            float xl, xr;
            printf("请输入\n");
            printf("left x:");
            scanf("%f", &xl);
            printf("right x:");
            scanf("%f", &xr);
            Token *temp_xl_token = new_token_from_float(xl);
            Token *temp_xr_token = new_token_from_float(xr);
            AST_Node *solution = solve_dichotomy(node, temp_xl_token, temp_xr_token);
            free(temp_xl_token);
            free(temp_xr_token);
            if (solution == NULL)
            {
                recu_free_ast(node);
                continue;
            }
            printf("有一解为\n");
            print_ast(solution, 0);
            recu_free_ast(solution);
            Token *var_x = get_var_value("x");
            printf("误差为\n");
            printf("abs(left-right): %f\n", fabs(get_delta(node, var_x->v.f)));
        }
        else
        {
            Token *res_token = calc(node);
            if (res_token == NULL)
            {
                recu_free_ast(node);
                continue;
            } // printf("\ncalc result:\n");
            print_terimal_token(res_token, true);
            free(res_token);
        }
        recu_free_ast(node);
    }
    printf("\nexited\n");
    return 0;
}