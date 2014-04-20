/*
 * Simple Dalvik Virtual Machine Implementation
 *
 * Copyright (C) 2013 Chun-Yu Wang <wicanr2@gmail.com>
 */

#include "java_lib.h"

int java_lang_math_random(DexFileFormat *dex, simple_dalvik_vm *vm, char *type)
{
    double r = 0.0f;
    double test = 0.0f;
    int i = 0;
    int times = 0;
    srand(time(0));
    times = rand() % 100;
    for (i = 0; i < times; i++)
        r = ((double) rand() / (double) RAND_MAX);

    if (is_verbose() > 3) printf("get random number = %f \n", r);
    store_double_to_result(vm, (unsigned char *) &r);
    load_result_to_double(vm, (unsigned char *) &test);

    return 0;
}

/* java.io.PrintStream.println */
static char buf[1024];
static int buf_ptr = 0;
static int use_buf = 0;
int java_io_print_stream_println(DexFileFormat *dex, simple_dalvik_vm *vm, char *type)
{
    invoke_parameters *p = &vm->p;
    int i = 0;
    int string_id = 0;
    if (is_verbose())
        printf("call java.io.PrintStream.println\n");

    load_reg_to(vm, p->reg_idx[1], (unsigned char *) &string_id);
    if (use_buf == 1) {
        printf("%s\n", buf);
        use_buf = 0;
        memset(buf, 0, 1024);
        buf_ptr = 0;
    } else {
        printf("%s\n", get_string_data(dex, string_id));
    }
    return 0;
}

/* java.lang.StringBuilder.<init> */
int java_lang_string_builder_init(DexFileFormat *dex, simple_dalvik_vm *vm, char *type)
{
    invoke_parameters *p = &vm->p;
    if (is_verbose())
        printf("call java.lang.StringBuilder.<init>\n");
    memset(buf, 0, 1024);
    buf_ptr = 0;
    return 0;
}

int java_lang_string_builder_append(DexFileFormat *dex, simple_dalvik_vm *vm, char *type)
{
    invoke_parameters *p = &vm->p;
    int string_id = 0;
    if (is_verbose())
        printf("call java.lang.StringBuilder.append\n");
    load_reg_to(vm, p->reg_idx[1], (unsigned char *) &string_id);
    if (type != 0) {
        if (strcmp(type, "Ljava/lang/String;") == 0) {
            buf_ptr += snprintf(buf + buf_ptr, 1024, "%s", get_string_data(dex, string_id));
        } else if (strcmp(type, "I") == 0) {
            buf_ptr += snprintf(buf + buf_ptr, 1024, "%d", string_id);
        }
    }
    return 0;
}

int java_lang_string_builder_to_string(DexFileFormat *dex, simple_dalvik_vm *vm, char *type)
{
    invoke_parameters *p = &vm->p;
    if (is_verbose())
        printf("call java.lang.StringBuilder.toString\n");
    use_buf = 1;
    return 0;
}

int java_lang_system_currenttimemillis(DexFileFormat *dex, simple_dalvik_vm *vm, char *type)
{
    invoke_parameters *p = &vm->p;
    if (is_verbose())
        printf("call java.lang.System.currentTimeMillis\n");
    struct timeval t;
    gettimeofday(&t, NULL);
    unsigned long mtime = (t.tv_sec%1000000L)*1000 + t.tv_usec / 1000;
    store_long_to_result(vm, (unsigned char *) &mtime);
    return 0;
}

static int java_lang_reflect_Array_newInstance(DexFileFormat *dex, simple_dalvik_vm *vm, char *type){
    /*Using new_instance*/
    int idx_p1 = vm->p.reg_idx[0];
    int idx_p2 = vm->p.reg_idx[1];
    int type_obj,ref,size;
    java_array* array_info = NULL;
    load_reg_to(vm,idx_p1,(unsigned char*)&type_obj);
    load_reg_to(vm,idx_p2,(unsigned char*)&ref);
    if(ref>=8192){
        ref -= 8192;
        array_info = vm->array[ref];
    }
    //create out index of array [[I
    char buffer[256]="[";
    strcpy(buffer+1,dex->string_data_item[dex->type_id_item[array_info->type_idx].descriptor_idx].data);
    int outer_idx = create_array(dex,vm,get_type_item_by_name(dex,buffer),array_info->size);
    field_value* list = vm->array[outer_idx-8192]->list;
    int i = 0;
    for(;i<array_info->size;i++){
         //should according to the type create array or instance
         // write as no type checking
         int inner_idx = create_array(dex,vm,array_info->type_idx,array_info->size);
         memcpy(list[i].data, &inner_idx, sizeof(int));
         list[i].type_idx = array_info->type_idx;
         //printf("list[i].data = %d\n", *((int*)list[i].data) );
    }
    move_int_to_bottom_result(vm,outer_idx);
    return 0;
}

static int java_lang_obj_init(DexFileFormat *dex, simple_dalvik_vm *vm, char *type){

}

static int compareTo(){

}

static java_lang_method method_table[] = {
    {"Ljava/lang/Math;",          "random",   java_lang_math_random},
    {"Ljava/io/PrintStream;",     "println",  java_io_print_stream_println},
    {"Ljava/lang/Object;",        "<init>",   java_lang_obj_init},
    {"Ljava/lang/StringBuilder;", "<init>",   java_lang_string_builder_init},
    {"Ljava/lang/StringBuilder;", "append",   java_lang_string_builder_append},
    {"Ljava/lang/StringBuilder;", "toString", java_lang_string_builder_to_string},
    {"Ljava/lang/System;",        "currentTimeMillis", java_lang_system_currenttimemillis},
    {"Ljava/lang/reflect/Array;", "newInstance", java_lang_reflect_Array_newInstance}
};

static int java_lang_method_size = sizeof(method_table) / sizeof(java_lang_method);

java_lang_method *find_java_lang_method(char *cls_name, char *method_name)
{
    int i = 0;
    for (i = 0; i < java_lang_method_size; i++)
        if (strcmp(cls_name, method_table[i].clzname) == 0 &&
            strcmp(method_name, method_table[i].methodname) == 0)
            return &method_table[i];
    return 0;
}

int invoke_java_lang_library(DexFileFormat *dex, simple_dalvik_vm *vm,
                             char *cls_name, char *method_name, char *type)
{
    java_lang_method *method = find_java_lang_method(cls_name, method_name);
    if (method != 0) {
        if (is_verbose())
            printf("invoke %s/%s %s\n", method->clzname, method->methodname, type);
        method->method_runtime(dex, vm, type);
        return 1;
    }
    //printf("static method %s in class %s type:%s not found\n", method_name, cls_name, type);
    return 0;
}
