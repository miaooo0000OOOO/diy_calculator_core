#include <stdio.h>
#include <stdlib.h>

int main()
{
    char str[30] = "123456789 This is test";
    char *ptr;
    unsigned long ret;

    // ret = strtod(str, NULL);
    ret = strtoul(&str[2], NULL, 10);
    printf("数字是 %d\n", ret);
    // printf("字符串部分是 |%s|", ptr);

    return (0);
}