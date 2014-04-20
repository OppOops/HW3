/*
 * Simple Dalvik Virtual Machine Implementation
 *
 * Copyright (C) 2013 Chun-Yu Wang <wicanr2@gmail.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "simple_dvm.h"

#ifdef debug
static int verbose_flag = 4;

int is_verbose()
{
    return verbose_flag;
}
int enable_verbose()
{
    verbose_flag = 1;
    return 0;
}

int disable_verbose()
{
    verbose_flag = 0;
    return 0;
}

int set_verbose(int l)
{
    verbose_flag = l;
    return 0;
}
#endif
/*
#define load_reg_to(vm, id, ptr)       \
{                                      \
    int *r = (int*)vm->regs[id].data;  \
    int *p = (int*)ptr;                \
    *p = *r;                           \
}                                      \
*/
void load_reg_to_double(simple_dalvik_vm *vm, int id, unsigned char *ptr)
{
    int *r = (int*)vm->regs[id].data;
    int *p = (int*)ptr;
    *p = *r;
}
void load_result_to_double(simple_dalvik_vm *vm, unsigned char *ptr)
{
    double *r = (double*)vm->result;
    double *p = (double*)ptr;
    *p = *r;
}

void load_reg_to_long(simple_dalvik_vm *vm, int id, unsigned char *ptr)
{
    int *r = (int*)vm->regs[id].data;
    int *p = (int*)ptr;
    *p = *r;
}

void store_double_to_result(simple_dalvik_vm *vm, unsigned char *ptr)
{
    double *r = (double*)vm->result;
    double *p = (double*)ptr;
    *r = *p;
}

void store_double_to_reg(simple_dalvik_vm *vm, int id, unsigned char *ptr)
{
    int *r = (int*)vm->regs[id].data;
    int *p = (int*)ptr;
    *r = *p;
}
/*
#define store_to_reg(vm, id, ptr)         \
{                                         \
    int *r = (int*)vm->regs[id].data;     \
    int *p = (int*)ptr;                   \
    *r = *p;                              \
}                                         \
*/
void store_long_to_reg(simple_dalvik_vm *vm, int id, unsigned char *ptr)
{
    int *r = (int*)vm->regs[id].data;
    int *p = (int*)ptr;
    *r = *p;
}

void store_long_to_result(simple_dalvik_vm *vm, unsigned char *ptr)
{
    double *r = (double*)vm->result;
    double *p = (double*)ptr;
    *r = *p;
}

void move_top_half_result_to_reg(simple_dalvik_vm *vm, int id)
{
    int *r = (int*)vm->result;
    int *p = (int*)vm->regs[id].data;
    *p = *r;
}

void move_bottom_half_result_to_reg(simple_dalvik_vm *vm, int id)
{
    int *r = ((int*)vm->result) + 1;
    int *p = (int*)vm->regs[id].data;
    *p = *r;
}

void move_reg_to_bottom_result(simple_dalvik_vm *vm, int id){
    int *r = ((int*)vm->result) + 1;
    int *p = (int*)vm->regs[id].data;
    *r = *p;
}
void move_int_to_bottom_result(simple_dalvik_vm *vm, int result){
    int *r = ((int*)vm->result) + 1;
    *r = result;
}
