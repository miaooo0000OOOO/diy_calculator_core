#ifndef __SYMBOL_TABLE_H__
#define __SYMBOL_TABLE_H__

#include "type.h"

typedef enum VarType
{
    Real = 0, // 实数
    Complex,  // 复数
    Tensor,   // 张量
} VarType;

typedef struct VarTableLine
{
    char *name;
    VarType type;
    void *data;
} VarTableLine;

extern VarTableLine symbol_table[100];
extern int symbol_table_len;

#endif