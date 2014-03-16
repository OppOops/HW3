/*
 * Simple Java Virtual Machine Implementation
 *
 * Copyright (C) 2013 Chun-Yu Wang <wicanr2@gmail.com>
 */

#include "java_lib.h"

int java_lang_math_random(StackFrame *stack, SimpleConstantPool *p, char *type)
{
    double r = 0.0f;
    int i;
    int times = 0;
    srand(time(0));
    times = rand() % 100;
    for (i = 0; i < times; i++)
        r = ((double) rand() / (double) RAND_MAX);
#if SIMPLE_JVM_DEBUG
    printf("rand r = %f\n", r);
#endif
    pushDouble(stack, r);
    return 0;
}

int java_lang_system_currenttimemillis(StackFrame *stack, SimpleConstantPool *p){
    struct timeval t;
    gettimeofday(&t, NULL);
    unsigned long mtime = (t.tv_sec%1000000L)*1000 + t.tv_usec / 1000;
#if SIMPLE_JVM_DEBUG
    printf("currentTime t = %lu\n", mtime);
#endif
    pushLong(stack,mtime); 
}

static java_lang_method method_table[] = {
    {"java/lang/Math", "random", java_lang_math_random},
    {"java/lang/System", "currentTimeMillis", java_lang_system_currenttimemillis}
};

static int java_lang_method_size = sizeof(method_table) / sizeof(java_lang_method);

java_lang_method *find_java_lang_method(char *cls_name, char *method_name)
{
    int i;
    for (i = 0; i < java_lang_method_size; i++)
        if (strcmp(cls_name, method_table[i].clzname) == 0 &&
            strcmp(method_name, method_table[i].methodname) == 0)
            return &method_table[i];
    return 0;
}

int invoke_java_lang_library(StackFrame *stack, SimpleConstantPool *p,
                             char *cls_name, char *method_name, char *type)
{
    java_lang_method *method = find_java_lang_method(cls_name, method_name);
    if (method != 0) {
#if SIMPLE_JVM_DEBUG
        printf("invoke %s/%s %s\n", method->clzname, method->methodname, type);
#endif
        method->method_runtime(stack, p, type);
        return 1;
    }
    return 0;
}
