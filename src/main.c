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
        Token *res_token = calc(node);
        if (res_token == NULL)
        {
            AST_Node *solution = solve(node);
            if (solution == NULL)
            {
                recu_free_ast(node);
                continue;
            }
            print_ast(solution, 0);
        }
        else
        {
            // printf("\ncalc result:\n");
            print_terimal_token(res_token, true);
            free(res_token);
        }
        recu_free_ast(node);
    }
    printf("\nexited\n");
    return 0;
}