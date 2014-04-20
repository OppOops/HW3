/*
 * Simple Dalvik Virtual Machine Implementation
 *
 * Copyright (C) 2013 Chun-Yu Wang <wicanr2@gmail.com>
 */

#include "simple_dvm.h"

static void parse_encoded_method(DexFileFormat *dex,
                                 unsigned char *buf, encoded_method *method)
{
    int i = 0;
    int offset = 0;
#ifdef debug
    if (is_verbose() > 3)
        printf("parse encoded method\n");
#endif
    offset = method->code_off - sizeof(DexHeader);

    memcpy(&method->code_item.registers_size, buf + offset, sizeof(ushort));
    offset += sizeof(ushort);
    memcpy(&method->code_item.ins_size, buf + offset, sizeof(ushort));
    offset += sizeof(ushort);
    memcpy(&method->code_item.outs_size, buf + offset, sizeof(ushort));
    offset += sizeof(ushort);
    memcpy(&method->code_item.tries_size, buf + offset, sizeof(ushort));
    offset += sizeof(ushort);
    memcpy(&method->code_item.debug_info_off, buf + offset, sizeof(uint));
    offset += sizeof(uint);
    memcpy(&method->code_item.insns_size, buf + offset, sizeof(uint));
    offset += sizeof(uint);

#ifdef debug
    if (is_verbose() > 3) {
        printf("registers_size = %d\n", method->code_item.registers_size);
        printf("insns_size = %d\n", method->code_item.insns_size);
    }
#endif
    method->code_item.insns = malloc(sizeof(ushort) * method->code_item.insns_size);
    memcpy(method->code_item.insns, buf + offset,
           sizeof(ushort) * method->code_item.insns_size);
    offset += sizeof(ushort) * method->code_item.insns_size;
}

static void parse_class_data_item(DexFileFormat *dex,
                                  unsigned char *buf, int offset, int index)
{
    int i = 0;
    int j = 0;
    int size = 0;
    int len = 0;
    i = offset;

    dex->class_data_item[index].static_fields_size =
        get_uleb128_len(buf, i, &size);
    i += size;

    dex->class_data_item[index].instance_fields_size =
        get_uleb128_len(buf, i, &size);
    i += size;

    dex->class_data_item[index].direct_methods_size =
        get_uleb128_len(buf, i, &size);
    i += size;

    dex->class_data_item[index].virtual_methods_size =
        get_uleb128_len(buf, i, &size);
    i += size;

#ifdef debug
    if (is_verbose() > 3) {
        printf("  class_data_item[%d]", index);
        printf("  , static_fields_size = %d\n",
               dex->class_data_item[index].static_fields_size);
        printf("  , instance_fields_size = %d\n",
               dex->class_data_item[index].instance_fields_size);
        printf("  , direct_method_size = %d\n",
               dex->class_data_item[index].direct_methods_size);
        printf("  , direct_method_size = %d\n",
               dex->class_data_item[index].virtual_methods_size);
        printf("i = %d\n", i);
    }
#endif
    if (dex->class_data_item[index].static_fields_size > 0) {
        dex->class_data_item[index].static_fields = (encoded_field *)
                malloc(sizeof(encoded_field) *
                       dex->class_data_item[index].static_fields_size);
        int acc = 0;
        for (j = 0; j < dex->class_data_item[index].static_fields_size; j++) {
#ifdef debug
	    if (is_verbose() > 3)
                printf("offset = %04lx ", i + sizeof(DexHeader));
#endif
            dex->class_data_item[index].static_fields[j].field_idx_diff =
                get_uleb128_len(buf, i, &size);
            i += size;
            dex->class_data_item[index].static_fields[j].access_flags =
                get_uleb128_len(buf, i, &size);
            i += size;
            acc = acc + dex->class_data_item[index].static_fields[j].field_idx_diff;
#ifdef debug
	    if (is_verbose() > 3)
                printf("static ecoded_field %d, field_id = %d, access_flag = %04x\n",j,
                       dex->class_data_item[index].static_fields[j].field_idx_diff,
                       dex->class_data_item[index].static_fields[j].access_flags);
#endif
	}
    }
    if (dex->class_data_item[index].instance_fields_size > 0) {
        dex->class_data_item[index].instance_fields = (encoded_field *)
                malloc(sizeof(encoded_field) *
                       dex->class_data_item[index].instance_fields_size);
        for (j = 0; j < dex->class_data_item[index].instance_fields_size; j++) {
#ifdef debug
            if (is_verbose() > 3)
                printf("offset = %04lx ", i + sizeof(DexHeader));
#endif
            dex->class_data_item[index].instance_fields[j].field_idx_diff =
                get_uleb128_len(buf, i, &size);
            i += size;
            dex->class_data_item[index].instance_fields[j].access_flags =
                get_uleb128_len(buf, i, &size);
            i += size;
#ifdef debug
            if (is_verbose() > 3)
                printf("instance ecoded_field, field_id = %d, access_flag = %04x\n",
                       dex->class_data_item[index].instance_fields[j].field_idx_diff,
                       dex->class_data_item[index].instance_fields[j].access_flags);
#endif
        }
    }
    if (dex->class_data_item[index].direct_methods_size > 0) {
        dex->class_data_item[index].direct_methods = (encoded_method *)
                malloc(sizeof(encoded_method) *
                       dex->class_data_item[index].direct_methods_size);
        for (j = 0; j < dex->class_data_item[index].direct_methods_size; j++) {
#ifdef debug
            if (is_verbose() > 3)
                printf("offset = %04lx ", i + sizeof(DexHeader));
#endif
            dex->class_data_item[index].direct_methods[j].method_idx_diff =
                get_uleb128_len(buf, i, &size);
            i += size;
            dex->class_data_item[index].direct_methods[j].access_flags =
                get_uleb128_len(buf, i, &size);
            i += size;
            dex->class_data_item[index].direct_methods[j].code_off =
                get_uleb128_len(buf, i, &size);
            i += size;

#ifdef debug
            if (is_verbose() > 3)
                printf("direct ecoded_method, method_id = %d, access_flag = %04x, code_off = %04x\n",
                       dex->class_data_item[index].direct_methods[j].method_idx_diff,
                       dex->class_data_item[index].direct_methods[j].access_flags,
                       dex->class_data_item[index].direct_methods[j].code_off);
#endif

            parse_encoded_method(dex, buf,
                                 &dex->class_data_item[index].direct_methods[j]);
        }

    }
    if (dex->class_data_item[index].virtual_methods_size > 0) {
        dex->class_data_item[index].virtual_methods = (encoded_method *)
                malloc(sizeof(encoded_method) *
                       dex->class_data_item[index].virtual_methods_size);
        for (j = 0; j < dex->class_data_item[index].virtual_methods_size; j++) {
#ifdef debug
            if (is_verbose() > 3)
                printf("offset = %04lx ", i + sizeof(DexHeader));
#endif
            dex->class_data_item[index].virtual_methods[j].method_idx_diff =
                get_uleb128_len(buf, i, &size);
            i += size;
            dex->class_data_item[index].virtual_methods[j].access_flags =
                get_uleb128_len(buf, i, &size);
            i += size;
            dex->class_data_item[index].virtual_methods[j].code_off =
                get_uleb128_len(buf, i, &size);
            i += size;

#ifdef debug
            if (is_verbose() > 3)
                printf("virtual ecoded_method, method_id = %d, access_flag = %04x, code_off = %04x\n",
                       dex->class_data_item[index].virtual_methods[j].method_idx_diff,
                       dex->class_data_item[index].virtual_methods[j].access_flags,
                       dex->class_data_item[index].virtual_methods[j].code_off);
#endif

            parse_encoded_method(dex, buf,
                                 &dex->class_data_item[index].virtual_methods[j]);
        }
    }
}

void parse_class_defs(DexFileFormat *dex, unsigned char *buf, int offset)
{
    int i = 0;
#ifdef debug
    if (is_verbose() > 3)
        printf("parse class defs offset = %04lx\n", offset + sizeof(DexHeader));
#endif
    if (dex->header.classDefsSize <= 0)
        return;
    dex->class_def_item = malloc(
                              sizeof(class_def_item) * dex->header.classDefsSize);
    dex->class_data_item = malloc(
                               sizeof(class_data_item) * dex->header.classDefsSize);

    for (i = 0 ; i < dex->header.classDefsSize; i++) {
        memcpy(&dex->class_def_item[i],
               buf + i * sizeof(class_def_item) + offset,
               sizeof(class_def_item));
#ifdef debug
        if (is_verbose() > 3) {
            printf(" class_defs[%d], cls_id = %d, data_off = 0x%04x, source_file_idx = %d\n",
                   i,
                   dex->class_def_item[i].class_idx,
                   dex->class_def_item[i].class_data_off,
                   dex->class_def_item[i].source_file_idx);
        }
#endif
        parse_class_data_item(dex, buf,
                              dex->class_def_item[i].class_data_off - sizeof(DexHeader), i);
    }
}

class_data_item* get_class_data_item(DexFileFormat *dex, int class_idx){
    int i = 0;
    for(i=0;i<dex->header.classDefsSize;i++){
        if(class_idx == dex->class_def_item[i].class_idx)
            return (dex->class_data_item) + i;
    }
    return NULL;
}
