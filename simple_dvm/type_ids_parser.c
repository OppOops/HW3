/*
 * Simple Dalvik Virtual Machine Implementation
 *
 * Copyright (C) 2013 Chun-Yu Wang <wicanr2@gmail.com>
 */

#include "simple_dvm.h"
#include <string.h>

static char *type[9] = {"V", // void
                 "Z", // boolean
                 "B", // byte
                 "S", // short
                 "C", // char
                 "I", // int
                 "J", // long
                 "F", // float
                 "D", // double
                 };
static int type_s[9] = { 0, // void
                  1, // boolean
                  1, // byte
                  2, // short
                  1, // char
                  4, // int
                  8, // long
                  4, // float
                  8  // double
                 };

int check_primitive(char *str){
    int i = 0;
    for(;i<9;i++){
        if(strcmp(type[i],str)==0)
            return type_s[i];
    }
    return -1;
}


void init_instance_field(DexFileFormat *dex, field_value* f, encoded_field* e, int size){
   int i = 0;
   int acc = 0;
   for(i=0;i<size;i++){
       acc = acc + e[i].field_idx_diff;
       f[i].type_idx = get_field_type(dex, acc);
   } 
}

int create_instance(DexFileFormat *dex, simple_dalvik_vm *vm, int type_id){
    int* length = &(vm->object_length);
    java_object* new_object = (java_object*) malloc(sizeof(java_object)); 
    class_data_item *class_info = get_class_data_item(dex, type_id);
    if(class_info==NULL)
        return -1;
    int ins_size = class_info->instance_fields_size;

    vm->object[*length] = new_object;
    *length = *length + 1;
    new_object->class_idx       = type_id;
    new_object->class_info      = class_info;
    new_object->instance_fields = (field_value*) malloc(sizeof(field_value)*ins_size);
    init_instance_field(dex, new_object->instance_fields, new_object->class_info->instance_fields, ins_size);
    return (*length) - 1;
}

int create_array(DexFileFormat *dex, simple_dalvik_vm *vm, int type_id, int size){
    //printf("create array, size = %d\n",size);
    int i = 0;
    int* length = &(vm->array_number);
    java_array* new_array = (java_array*) malloc(sizeof(java_array));
    field_value* list = (field_value*) malloc(sizeof(field_value)*size);
    for(i=0;i<size;i++)
        list[i].type_idx = type_id;
    //printf("create arr, length = %d, size = %d\n",*length,size);
    vm->array[*length] = new_array;
    *length = *length + 1;
    new_array->type_idx       = type_id;
    new_array->size           = size;
    new_array->list           = list;
    return (*length) - 1 + 8192;
}




int create_array_filled(DexFileFormat *dex, simple_dalvik_vm *vm, int type_id, int size, int content){
    int idx = create_array(dex,  vm,  type_id,  size) - 8192;
    java_array* new_array = vm->array[idx];
    for(int i=0;i<size;i++)
	memcpy(new_array->list[i].data ,&content, sizeof(int));
    return idx + 8192;
}


void parse_type_ids(DexFileFormat *dex, unsigned char *buf, int offset)
{
    int i = 0;
    if (is_verbose() > 3)
        printf("parse type ids offset = %04lx\n", offset + sizeof(DexHeader));
    dex->type_id_item = malloc(
                            sizeof(type_id_item) * dex->header.typeIdsSize);

    for (i = 0; i < dex->header.typeIdsSize; i++) {
        memcpy(&dex->type_id_item[i],
               buf + i * sizeof(type_id_item) + offset,
               sizeof(type_id_item));

        if (is_verbose() > 3)
            printf(" type_ids [%d], = %s\n", i,
                   dex->string_data_item[
                       dex->type_id_item[i].descriptor_idx].data);
    }
}

type_id_item *get_type_item(DexFileFormat *dex, int type_id)
{
    if (type_id >= 0 && type_id < dex->header.typeIdsSize)
        return &dex->type_id_item[type_id];
    return 0;
}
int get_type_item_by_name(DexFileFormat *dex, char* str){
    int i = 0;
    for(;i<dex->header.typeIdsSize;i++){
        char* t = dex->string_data_item[dex->type_id_item[i].descriptor_idx].data;
        if(strcmp(str,t)==0) 
            return i;
    }
    return -1;
}


char *get_type_item_name(DexFileFormat *dex, int type_id)
{
    type_id_item *type_item = get_type_item(dex, type_id);
    if (type_item != 0)
        return get_string_data(dex, type_item->descriptor_idx);
    return 0;
}

void parse_proto_ids(DexFileFormat *dex, unsigned char *buf, int offset)
{
    volatile int i = 0, j = 0;
    int idx = 0;
    if (is_verbose() > 3)
        printf("parse proto ids offset = %04lx\n", offset + sizeof(DexHeader));
    dex->proto_id_item = malloc(
                             sizeof(proto_id_item) * dex->header.protoIdsSize);

    dex->proto_type_list = malloc(
                               sizeof(type_list) * dex->header.protoIdsSize);
    for (i = 0 ; i < dex->header.protoIdsSize; i++) {
        memcpy(&dex->proto_id_item[i],
               buf + i * sizeof(proto_id_item) + offset,
               sizeof(proto_id_item));
        memset(&dex->proto_type_list[i], 0, sizeof(type_list));
        idx = dex->proto_id_item[i].return_type_idx;
        if (is_verbose() > 3)
            printf(" proto_id_item [%d], %s, type_id = %d %s, parameters_off = %08x\n", i,
                   dex->string_data_item[dex->proto_id_item[i].shorty_idx].data,
                   idx, get_type_item_name(dex, idx),
                   dex->proto_id_item[i].parameters_off);
        if (dex->proto_id_item[i].parameters_off == 0)
            continue;
        if (is_verbose() > 3)
            printf(" proto_typ_list[%d] offset %p ", i,
                   buf + dex->proto_id_item[i].parameters_off - sizeof(DexHeader));
        memcpy(&dex->proto_type_list[i].size,
               buf + dex->proto_id_item[i].parameters_off - sizeof(DexHeader),
               sizeof(int));

        if (is_verbose() > 3)
            printf("proto_type_list[%d].size = %d\n", i,
                   dex->proto_type_list[i].size);
        if (dex->proto_type_list[i].size > 0) {
            dex->proto_type_list[i].type_item = (type_item *)
                                                malloc(sizeof(type_item) * dex->proto_type_list[i].size);

            for (j = 0 ; j < dex->proto_type_list[i].size ; j++) {
                memset(&dex->proto_type_list[i].type_item[j], 0, sizeof(type_item));
                type_item *item = &dex->proto_type_list[i].type_item[j];
                memcpy(item,
                       buf
                       + dex->proto_id_item[i].parameters_off
                       - sizeof(DexHeader)
                       + 4
                       + (sizeof(type_item) * j),
                       sizeof(type_item));

                if (is_verbose() > 3)
                    printf("item[%d], type_idx = %d, type = %s\n",
                           j, item->type_idx,
                           get_type_item_name(dex, item->type_idx));
            }
        }
    }
}

proto_id_item *get_proto_item(DexFileFormat *dex, int proto_id)
{
    if (proto_id >= 0 && proto_id < dex->header.protoIdsSize)
        return &dex->proto_id_item[proto_id];
    return 0;
}

type_list *get_proto_type_list(DexFileFormat *dex, int proto_id)
{
    if (proto_id >= 0 && proto_id < dex->header.protoIdsSize)
        return &dex->proto_type_list[proto_id];
    return 0;
}

void parse_field_ids(DexFileFormat *dex, unsigned char *buf, int offset)
{
    int i;
    if (is_verbose() > 3)
        printf("parse feild ids offset = %04lx\n", offset + sizeof(DexHeader));
    dex->field_id_item = malloc(sizeof(field_id_item) * dex->header.fieldIdsSize);

    if (is_verbose() > 3)
        printf("dex->header.fieldIdsSize = %d\n", dex->header.fieldIdsSize);
    for (i = 0; i < dex->header.fieldIdsSize; i++) {
        memcpy(&dex->field_id_item[i],
               buf + i * sizeof(field_id_item) + offset,
               sizeof(field_id_item));
        if (is_verbose() > 3) {
            printf(" field_id_item [%d], class_id = %d %s, type_id = %d %s, name_idx=%d %s\n",
                   i, dex->field_id_item[i].class_idx,
                   dex->string_data_item[
                       dex->type_id_item[
                           dex->field_id_item[i].class_idx].descriptor_idx].data,

                   dex->field_id_item[i].type_idx,
                   dex->string_data_item[
                       dex->type_id_item[
                           dex->field_id_item[i].type_idx].descriptor_idx].data,
                   dex->field_id_item[i].name_idx,
                   dex->string_data_item[dex->field_id_item[i].name_idx].data);
        }
    }
}

field_id_item *get_field_item(DexFileFormat *dex, int field_id)
{
    if (field_id >= 0 && field_id < dex->header.fieldIdsSize)
        return &dex->field_id_item[field_id];
    return 0;
}

int get_field_type(DexFileFormat *dex, int field_id)
{
    if (field_id >= 0 && field_id < dex->header.fieldIdsSize)
        return dex->field_id_item[field_id].type_idx;
    return 0;
}
