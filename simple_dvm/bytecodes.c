/*
 * Simple Dalvik Virtual Machine Implementation
 *
 * Copyright (C) 2014 Jim Huang <jserv.tw@gmail.com>
 * Copyright (C) 2013 Chun-Yu Wang <wicanr2@gmail.com>
 */

#include "simple_dvm.h"
#include "java_lib.h"

static int op_move_result(DexFileFormat *dex, simple_dalvik_vm *vm, u1 *ptr, int *pc)
{
    int reg_idx_vx = 0;
    reg_idx_vx = ptr[*pc + 1];
    if (is_verbose())
        printf("move-result v%d\n", reg_idx_vx);
    move_bottom_half_result_to_reg(vm, reg_idx_vx);
    *pc = *pc + 2;
    return 0;
}


/* 0x0b, move-result-wide
 *
 * Move the long/double result value of the previous method invocation
 * into vx, vx+1.
 *
 * 0B02 - move-result-wide v2
 * Move the long/double result value of the previous method
 * invocation into v2,v3.
 */
static int op_move_result_wide(DexFileFormat *dex, simple_dalvik_vm *vm, u1 *ptr, int *pc)
{
    int reg_idx_vx = 0;
    int reg_idx_vy = 0;
    reg_idx_vx = ptr[*pc + 1];
    reg_idx_vy = reg_idx_vx + 1;
    if (is_verbose())
        printf("move-result-wide v%d,v%d\n", reg_idx_vx, reg_idx_vy);
    move_bottom_half_result_to_reg(vm, reg_idx_vx);
    move_top_half_result_to_reg(vm, reg_idx_vy);
    *pc = *pc + 2;
    return 0;
}

/* 0x0c, move-result-object vx
 *
 * Move the result object reference of
 * the previous method invocation into vx.
 *
 * 0C00 - move-result-object v0
 */

static int op_move(DexFileFormat *dex, simple_dalvik_vm *vm, u1 *ptr, int *pc)
{
    int reg_idx_vx = ptr[*pc + 1] & 0x0F;
    int reg_idx_vy = (ptr[*pc + 1]>>4) & 0x0F;
    if (is_verbose())
        printf("move v%d,v%d\n", reg_idx_vx, reg_idx_vy);
    simple_dvm_register *rx = &vm->regs[reg_idx_vx];
    simple_dvm_register *ry = &vm->regs[reg_idx_vy];
    rx->data[0] = ry->data[0]; 
    rx->data[1] = ry->data[1]; 
    rx->data[2] = ry->data[2]; 
    rx->data[3] = ry->data[3]; 

    *pc = *pc + 2;
    return 0;
}

static int op_move_result_object(DexFileFormat *dex, simple_dalvik_vm *vm, u1 *ptr, int *pc)
{
    int reg_idx_vx = 0;
    reg_idx_vx = ptr[*pc + 1];
    if (is_verbose())
        printf("move-result-object v%d\n", reg_idx_vx);
    move_bottom_half_result_to_reg(vm, reg_idx_vx);
    int test = -1; load_reg_to(vm, reg_idx_vx, (unsigned char*)&test);
    printf("@@test = %d\n",test);
    *pc = *pc + 2;
    return 0;
}

/* 0x0e , return-void
 * Return without a return value
 * 0E00 - return-void
 */
static int op_return_void(DexFileFormat *dex, simple_dalvik_vm *vm, u1 *ptr, int *pc)
{
    if (is_verbose())
        printf("return-void\n");
    *pc = *pc + 2;
    return 0;
}

/* 0x0f , return-vx
 * Return with vx return value
 * 0F00 - return-v0
 */
static int op_return(DexFileFormat *dex, simple_dalvik_vm *vm, u1 *ptr, int *pc)
{
    int reg_idx_vx = 0;
    reg_idx_vx = ptr[*pc + 1];

    if (is_verbose())
        printf("return v%d\n",reg_idx_vx);
    move_reg_to_bottom_result(vm, reg_idx_vx);
    *pc = *pc + 2;
    return 0;
}


/* 0x12, const/4 vx,lit4
 * Puts the 4 bit constant into vx
 * 1221 - const/4 v1, #int2
 * Moves literal 2 into v1.
 * The destination register is in the lower 4 bit
 * in the second byte, the literal 2 is in the higher 4 bit.
 */
static int op_const_4(DexFileFormat *dex, simple_dalvik_vm *vm, u1 *ptr, int *pc)
{
    int value = 0;
    int reg_idx_vx = 0;
    value = ptr[*pc + 1] >> 4;
    reg_idx_vx = ptr[*pc + 1] & 0x0F;
    if (value & 0x08) {
        value = 0x0F - value + 1;
        value = -value;
    }
    store_to_reg(vm, reg_idx_vx, (unsigned char *) &value);
    if (is_verbose())
        printf("const/4 v%d, #int%d\n", reg_idx_vx , value);
    *pc = *pc + 2;
    return 0;
}

/* 0x13, const/16 vx,lit16
 * Puts the 16 bit constant into vx
 * 1300 0A00 - const/16 v0, #int 10
 * Puts the literal constant of 10 into v0.
 */
static int op_const_16(DexFileFormat *dex, simple_dalvik_vm *vm, u1 *ptr, int *pc)
{
    int reg_idx_vx = 0;
    int value = 0;
    reg_idx_vx = ptr[*pc + 1];
    value = (ptr[*pc + 3] << 8 | ptr[*pc + 2]);

    store_to_reg(vm, reg_idx_vx, (unsigned char *) &value);
    if (is_verbose())
        printf("const/16 v%d, #int%d\n", reg_idx_vx, value);
    *pc = *pc + 4;
    return 0;
}

/* 0x19, const-wide/high16 vx,lit16
 * Puts the 16 bit constant into the highest 16 bit of vx
 * and vx+1 registers.
 * Used to initialize double values.
 * 1900 2440 - const-wide/high16 v0, #double 10.0 // #402400000
 * Puts the double constant of 10.0 into v0 register.
 */
static int op_const_wide_high16(DexFileFormat *dex, simple_dalvik_vm *vm, u1 *ptr, int *pc)
{
    long long value = 0L;
    unsigned char *ptr2 = (unsigned char *) &value;
    int reg_idx_vx = 0;
    reg_idx_vx = ptr[*pc + 1];
    ptr2[1] = ptr[*pc + 3];
    ptr2[0] = ptr[*pc + 2];
    if (is_verbose())
        printf("const-wide/hight16 v%d, #long %lld\n", reg_idx_vx, value);
    store_to_reg(vm, reg_idx_vx, ptr2+4);
    store_to_reg(vm, reg_idx_vx+1, ptr2);
    value = 1L;
    *pc = *pc + 4;
    return 0;
}

/* 0x17, const-wide/32 vx, lit32
 * Puts the 32 bit constant into vx and vx+1 registers, expanding the integer constant into a long constant.
 * 1702 4e61 bc00 - const-wide/32 v2, #long 12345678 // #00bc614e
 * Puts #12345678 into v2 and v3 registers.
*/
static int op_const_wide_32(DexFileFormat *dex, simple_dalvik_vm *vm, u1 *ptr, int *pc)
{
    long long value = 0L;
    unsigned char *ptr2 = (unsigned char *) &value;
    int reg_idx_vx = 0;
    reg_idx_vx = ptr[*pc + 1];
    ptr2[3] = ptr[*pc + 5];
    ptr2[2] = ptr[*pc + 4];
    ptr2[1] = ptr[*pc + 3];
    ptr2[0] = ptr[*pc + 2];

    if (is_verbose())
        printf("const-wide/32 v%d, #%02x%02x %02x%02x\n", reg_idx_vx, ptr2[3], ptr2[2], ptr2[1], ptr2[0]);
    store_double_to_reg(vm, reg_idx_vx  , ptr2+4);
    store_double_to_reg(vm, reg_idx_vx+1, ptr2);
    *pc = *pc + 6;
    return 0;
}

static int op_const_wide_16(DexFileFormat *dex, simple_dalvik_vm *vm, u1 *ptr, int *pc)
{
    long long value = 0L;
    unsigned char *ptr2 = (unsigned char *) &value;
    int reg_idx_vx = 0;
    reg_idx_vx = ptr[*pc + 1];
    ptr2[1] = ptr[*pc + 3];
    ptr2[0] = ptr[*pc + 2];

    if (is_verbose())
        printf("const-wide/16 v%d, #%02x%02x\n", reg_idx_vx, ptr2[1], ptr2[0]);
    store_double_to_reg(vm, reg_idx_vx  , ptr2+4);
    store_double_to_reg(vm, reg_idx_vx+1, ptr2);
    *pc = *pc + 4;
    return 0;
}

/* 0x1f, check-cast vx, type_id
 * Checks whether the object reference in vx can be cast to
 * an instance of a class referenced by type_id. Throws
 *  ClassCastException if the cast is not possible, continues
 * execution otherwise.
 *
 * 1F04 0100 - check-cast v4, Test3 // type@0001
 * Checks whether the object reference in
 * v4 can be cast to type@0001 (entry #1 in the type id table)
 */

static int op_check_cast(DexFileFormat *dex, simple_dalvik_vm *vm, u1 *ptr, int *pc)
{
    int reg_idx_vx = 0;
    int object_reference = 0;
    int type_id = 0;
    int s_type_idx = 0;
    reg_idx_vx = ptr[*pc + 1];
    type_id = ((ptr[*pc + 3] << 8) | ptr[*pc + 2]);

    if (is_verbose())
        printf("check-cast v%d, type_id 0x%04x\n", reg_idx_vx , type_id);
    load_reg_to(vm, reg_idx_vx, (unsigned char *) &object_reference);
    
    if(object_reference >= 8192){
        object_reference -= 8192;
        s_type_idx = (vm->array[object_reference])->type_idx;
    }
    
    //s = get_field_item(dex,source_field_id);
    if(s_type_idx == type_id) {
        *pc = *pc + 4;
        return 0;
    }
    printf("warning: cast failed from %s(%d) to %s(%d)\n",dex->string_data_item[
                                                         dex->type_id_item[
                                                         type_id].descriptor_idx].data, type_id,

                                                         dex->string_data_item[
                                                         dex->type_id_item[
                                                         s_type_idx].descriptor_idx].data, s_type_idx);
    exit(0);
}


/* 0x1a, const-string vx,string_id
 * Puts reference to a string constant identified by string_id into vx.
 * 1A08 0000 - const-string v8, "" // string@0000
 * Puts reference to string@0000 (entry #0 in the string table) into v8.
 */
static int op_const_string(DexFileFormat *dex, simple_dalvik_vm *vm, u1 *ptr, int *pc)
{
    int reg_idx_vx = 0;
    int string_id = 0;
    reg_idx_vx = ptr[*pc + 1];
    string_id = ((ptr[*pc + 3] << 8) | ptr[*pc + 2]);

    if (is_verbose())
        printf("const-string v%d, string_id 0x%04x\n",
               reg_idx_vx , string_id);
    store_to_reg(vm, reg_idx_vx, (unsigned char *) &string_id);
    *pc = *pc + 4;
    return 0;
}

/* 0x22 new-instance vx,type
 * Instantiates an object type and puts
 * the reference of the newly created instance into vx
 * 2200 1500 - new-instance v0, java.io.FileInputStream // type@0015
 * Instantiates type@0015 (entry #15H in the type table)
 * and puts its reference into v0.
 */
static int op_new_instance(DexFileFormat *dex, simple_dalvik_vm *vm, u1 *ptr, int *pc)
{
    int reg_idx_vx = 0;
    int type_id = 0;
    type_id_item *type_item = 0;

    reg_idx_vx = ptr[*pc + 1];
    type_id = ((ptr[*pc + 3] << 8) | ptr[*pc + 2]);

    type_item = get_type_item(dex, type_id);

    if (is_verbose()) {
        printf("new-instance v%d, type_id 0x%04x", reg_idx_vx, type_id);
        if (type_item != 0) {
            printf(" %s", get_string_data(dex, type_item->descriptor_idx));
        }
        printf("\n");
    }
    int obj_ref = create_instance(dex, vm, type_id);
    store_to_reg(vm, reg_idx_vx, (unsigned char*)&obj_ref);
    *pc = *pc + 4;
    return 0;
}

/* 0x23 new-array vy, vx,type_id
 * Generates a new array of type_id type and vy element
 * size and puts the reference to the array into vx.
 *
 * 2312 2500 - new-array v2, v1, char[] // type@0025
 * Generates a new array of type@0025 type and v1 size
 * and puts the reference to the new array into v2.
 */
static int op_new_array(DexFileFormat *dex, simple_dalvik_vm *vm, u1 *ptr, int *pc)
{
    int reg_idx_vx = 0;
    int reg_idx_vy = 0;
    int type_id = 0, size = 0;
    type_id_item *type_item = 0;
    reg_idx_vx = ptr[*pc + 1] & 0x0F ;
    reg_idx_vy = (ptr[*pc + 1] >> 4) & 0x0F ;

    type_id = ((ptr[*pc + 3] << 8) | ptr[*pc + 2]);

    type_item = get_type_item(dex, type_id);

    if (is_verbose()) {
        printf("new-array v%d, v%d ,type_id 0x%04x", reg_idx_vx, reg_idx_vy, type_id);
        if (type_item != 0) {
            printf(" %s", get_string_data(dex, type_item->descriptor_idx));
        }
        printf("\n");
    }
    load_reg_to(vm, reg_idx_vy, (unsigned char*)&size);
    int arr_ref = create_array(dex, vm, type_id, size);
    store_to_reg(vm, reg_idx_vx, (unsigned char*)&arr_ref);
    *pc = *pc + 4;
    return 0;
}

/*Not sure implementation is correct or not*/
static int op_filled_new_array(DexFileFormat *dex, simple_dalvik_vm *vm, u1 *ptr, int *pc)
{
    int size = 0;
    int p1 = 0;
    int p2 = 0;
    int p3 = 0;
    int p4 = 0;
    int p5 = 0;
    int type_id = 0, content = 0;
    type_id_item *type_item = 0;
    size = ptr[*pc + 1] >> 4;
    p1 = ptr[*pc + 4] & 0x0F ;
    p2 = ptr[*pc + 4] >> 4 ;
    p3 = ptr[*pc + 5] & 0x0f ;
    p4 = ptr[*pc + 5] >> 4 ;
    p5 = ptr[*pc + 1] & 0x0F ;

    type_id = ((ptr[*pc + 3] << 8) | ptr[*pc + 2]);
    type_item = get_type_item(dex, type_id);

    if (is_verbose()) {
        int i = 0;
        for(;i<3;i++){
            printf("%02x%02x ",(unsigned char)ptr[*pc + i*2],(unsigned char)ptr[*pc + i*2 +1]);
        }
        printf("filled-new-array %d v%d, v%d, type_id 0x%04x", size, p1, p2, type_id);
        if (type_item != 0) {
            printf(" %s", get_string_data(dex, type_item->descriptor_idx));
        }
        printf("\n");
    }
    load_reg_to(vm, p1, (unsigned char*)&size);
    int ref = create_array_filled(dex, vm, type_id, size, content);
    move_int_to_bottom_result(vm,ref);
    *pc = *pc + 6;
    return 0;
}



/* 35c format
 * A|G|op BBBB F|E|D|C
 * [A=5] op {vC, vD, vE, vF, vG}, meth@BBBB
 * [A=5] op {vC, vD, vE, vF, vG}, type@BBBB
 * [A=4] op {vC, vD, vE, vF}, kind@BBBB
 * [A=3] op {vC, vD, vE}, kind@BBBB
 * [A=2] op {vC, vD}, kind@BBBB
 * [A=1] op {vC}, kind@BBBB
 * [A=0] op {}, kind@BBBB
 * The unusual choice in lettering here reflects a desire to
 * make the count and the reference index have the same label as in format 3rc.
 */
static int op_utils_invoke_35c_parse(DexFileFormat *dex, u1 *ptr, int *pc,
                                     invoke_parameters *p)
{
    unsigned char tmp = 0;
    int i = 0;
    if (dex != 0 && ptr != 0 && p != 0) {
        memset(p, 0, sizeof(invoke_parameters));

        tmp = ptr[*pc + 1];
        p->reg_count = tmp >> 4;
        p->reg_idx[4] = tmp & 0x0F;

        p->method_id = ptr[*pc + 2];
        p->method_id |= (ptr[*pc + 3] << 4);

        tmp = ptr[*pc + 4];
        p->reg_idx[1] = tmp >> 4;
        p->reg_idx[0] = tmp & 0x0F;

        tmp = ptr[*pc + 5];
        p->reg_idx[3] = tmp >> 4;
        p->reg_idx[2] = tmp & 0x0F;
    }
    return 0;
}

static int op_utils_invoke(char *name, DexFileFormat *dex, simple_dalvik_vm *vm,
                           invoke_parameters *p)
{
    method_id_item *m = 0;
    type_id_item *type_class = 0;
    proto_id_item *proto_item = 0;
    type_list *proto_type_list = 0;
    if (p != 0) {
        m = get_method_item(dex, p->method_id);
        if (m != 0) {
            type_class = get_type_item(dex, m->class_idx);
            proto_item = get_proto_item(dex, m->proto_idx);
        }
        switch (p->reg_count) {
        case 0:
            if (is_verbose())
                printf("%s {} method_id 0x%04x", name, p->method_id);
            break;
        case 1:
            if (is_verbose())
                printf("%s, {v%d} method_id 0x%04x",
                       name, p->reg_idx[0], p->method_id);
            break;
        case 2:
            if (is_verbose())
                printf("%s {v%d, v%d} method_id 0x%04x",
                       name,
                       p->reg_idx[0], p->reg_idx[1],
                       p->method_id);
            break;
        case 3:
            if (is_verbose())
                printf("%s {v%d, v%d, v%d} method_id 0x%04x",
                       name,
                       p->reg_idx[0], p->reg_idx[1], p->reg_idx[2],
                       p->method_id);
            break;
        case 4:
            if (is_verbose())
                printf("%s {v%d, v%d, v%d, v%d} method_id 0x%04x",
                       name,
                       p->reg_idx[0], p->reg_idx[1],
                       p->reg_idx[2], p->reg_idx[3],
                       p->method_id);
            break;
        case 5:
            if (is_verbose())
                printf("%s {v%d, v%d, v%d, v%d, v%d} method_id 0x%04x",
                       name,
                       p->reg_idx[0], p->reg_idx[1], p->reg_idx[2],
                       p->reg_idx[3], p->reg_idx[4],
                       p->method_id);
            break;
        default:
            break;
        }

        if (m != 0 && type_class != 0 && p->reg_count <= 5) {
            if (proto_item != 0)
                proto_type_list = get_proto_type_list(dex, m->proto_idx);
            if (proto_type_list != 0 && proto_type_list->size > 0) {
                if (is_verbose())
                    printf(" %s,%s,(%s)%s \n",
                           get_string_data(dex, type_class->descriptor_idx),
                           get_string_data(dex, m->name_idx),
                           get_type_item_name(dex,
                                              proto_type_list->type_item[0].type_idx),
                           get_type_item_name(dex,
                                              proto_item->return_type_idx));
                return invoke_java_lang_library(dex, vm,
                                         get_string_data(dex, type_class->descriptor_idx),
                                         get_string_data(dex, m->name_idx),
                                         get_type_item_name(dex, proto_type_list->type_item[0].type_idx));
            } else {
                if (is_verbose())
                    printf(" %s,%s,()%s \n",
                           get_string_data(dex, type_class->descriptor_idx),
                           get_string_data(dex, m->name_idx),
                           get_type_item_name(dex,
                                              proto_item->return_type_idx));
                return invoke_java_lang_library(dex, vm,
                                         get_string_data(dex, type_class->descriptor_idx),
                                         get_string_data(dex, m->name_idx), 0);
            }

        } else {
            if (is_verbose())
                printf("\n");
        }
    }
    return 0;
}


/* invoke-virtual { parameters }, methodtocall */
/*
 * 6E53 0600 0421 - invoke-virtual { v4, v0, v1, v2, v3}, Test2.method5:(IIII)V // method@0006
 * 6e20 0200 3200   invoke-virtual {v2, v3}, Ljava/io/PrintStream;.println:(Ljava/lang/String;)V // method@0002
 */
static int op_invoke_virtual(DexFileFormat *dex, simple_dalvik_vm *vm, u1 *ptr, int *pc)
{
    int string_id = 0;

    op_utils_invoke_35c_parse(dex, ptr, pc, &vm->p);
    op_utils_invoke("invoke-virtual", dex, vm, &vm->p);
    /* TODO */
    *pc = *pc + 6;
    return 0;
}

/* invoke-direct
 * 7010 0400 0300  invoke-direct {v3}, Ljava/lang/StringBuilder;.<init>:()V // method@0004
 */
static int op_invoke_direct(DexFileFormat *dex, simple_dalvik_vm *vm, u1 *ptr, int *pc)
{
    int pc_current = *pc;
    op_utils_invoke_35c_parse(dex, ptr, pc, &vm->p);

    op_utils_invoke_35c_parse(dex, ptr, pc, &vm->p);
    method_id_item *m = get_method_item(dex, vm->p.method_id);
    if(!op_utils_invoke("invoke-direct", dex, vm, &vm->p))
        invoke_method_entry(dex,vm, get_string_data(dex, m->name_idx),1);
    //op_utils_invoke("invoke-direct", dex, vm, &vm->p);
    *pc = pc_current + 6;
    return 0;
}

/* 0x71 invoke-direct
 * 7100 0300 0000  invoke-static {}, Ljava/lang/Math;.random:()D // method@0003
 */
static int op_invoke_static(DexFileFormat *dex, simple_dalvik_vm *vm, u1 *ptr, int *pc)
{
    int pc_current = *pc;
    op_utils_invoke_35c_parse(dex, ptr, pc, &vm->p);

    method_id_item *m = get_method_item(dex, vm->p.method_id);
    if(!op_utils_invoke("invoke-static", dex, vm, &vm->p))
        invoke_method_entry(dex,vm, get_string_data(dex, m->name_idx),1);

    *pc = pc_current + 6;
    return 0;
}

/* 0x62 sget-object vx,field_id
 * Reads the object reference field identified by the field_id into vx.
 * 6201 0C00 - sget-object v1, Test3.os1:Ljava/lang/Object; // field@000c
 * Reads field@000c (entry #CH in the field id table) into v1.
 */
static int op_sget_object(DexFileFormat *dex, simple_dalvik_vm *vm, u1 *ptr, int *pc)
{
    int field_id = 0;
    int reg_idx_vx = 0;
    reg_idx_vx = ptr[*pc + 1];
    field_id = ((ptr[*pc + 3] << 8) | ptr[*pc + 2]);

    if (is_verbose()) {
        printf("sget-object v%d, field 0x%04x\n", reg_idx_vx, field_id);
    }
    store_to_reg(vm, reg_idx_vx, (unsigned char *) &field_id);
    /* TODO */
    *pc = *pc + 4;
    return 0;
}

/* 0x90 add-int vx,vy vz
 * Calculates vy+vz and puts the result into vx.
 * 9000 0203 - add-int v0, v2, v3
 * Adds v3 to v2 and puts the result into v0.
 */
static int op_add_int(DexFileFormat *dex, simple_dalvik_vm *vm, u1 *ptr, int *pc)
{
    int reg_idx_vx = 0;
    int reg_idx_vy = 0;
    int reg_idx_vz = 0;
    int x = 0, y = 0 , z = 0;
    reg_idx_vx = ptr[*pc + 1];
    reg_idx_vy = ptr[*pc + 2];
    reg_idx_vz = ptr[*pc + 3];

    if (is_verbose())
        printf("add-int v%d, v%d, v%d\n", reg_idx_vx, reg_idx_vy,
               reg_idx_vz);
    /* x = y + z */
    load_reg_to(vm, reg_idx_vy, (unsigned char *) &y);
    load_reg_to(vm, reg_idx_vz, (unsigned char *) &z);
    x = y + z;
    printf(" x = %d, y = %d, z = %d\n",x,y,z);
    store_to_reg(vm, reg_idx_vx, (unsigned char *) &x);
    *pc = *pc + 4;
    return 0;

}

/* 0x91 sub-int vx,vy,vz
 * Calculates vy-vz and puts the result into vx.
 * 9100 0203 - sub-int v0, v2, v3
 * Subtracts v3 from v2 and puts the result into v0.
 */
static int op_sub_int(DexFileFormat *dex, simple_dalvik_vm *vm, u1 *ptr, int *pc)
{
    int reg_idx_vx = 0;
    int reg_idx_vy = 0;
    int reg_idx_vz = 0;
    int x = 0, y = 0 , z = 0;
    reg_idx_vx = ptr[*pc + 1];
    reg_idx_vy = ptr[*pc + 2];
    reg_idx_vz = ptr[*pc + 3];

    if (is_verbose())
        printf("sub-int v%d, v%d, v%d\n", reg_idx_vx, reg_idx_vz,
               reg_idx_vy);
    /* x = y + z */
    load_reg_to(vm, reg_idx_vy, (unsigned char *) &y);
    load_reg_to(vm, reg_idx_vz, (unsigned char *) &z);
    x = y - z;
    store_to_reg(vm, reg_idx_vx, (unsigned char *) &x);
    *pc = *pc + 4;
    return 0;
}

/* 0x92 mul-int vx, vy, vz
 * Multiplies vz with wy and puts the result int vx.
 * 9200 0203 - mul-int v0,v2,v3
 * Multiplies v2 with w3 and puts the result into v0
 */
static int op_mul_int(DexFileFormat *dex, simple_dalvik_vm *vm, u1 *ptr, int *pc)
{
    int reg_idx_vx = 0;
    int reg_idx_vy = 0;
    int reg_idx_vz = 0;
    int x = 0, y = 0 , z = 0;
    reg_idx_vx = ptr[*pc + 1];
    reg_idx_vy = ptr[*pc + 2];
    reg_idx_vz = ptr[*pc + 3];

    if (is_verbose())
        printf("mul-int v%d, v%d, v%d\n", reg_idx_vx, reg_idx_vy, reg_idx_vz);
    /* x = y + z */
    load_reg_to(vm, reg_idx_vy, (unsigned char *) &y);
    load_reg_to(vm, reg_idx_vz, (unsigned char *) &z);
    x = y * z;
    store_to_reg(vm, reg_idx_vx, (unsigned char *) &x);
    *pc = *pc + 4;
    return 0;

}

/* 0x93 div-int vx,vy,vz
 * Divides vy with vz and puts the result into vx.
 * 9303 0001 - div-int v3, v0, v1
 * Divides v0 with v1 and puts the result into v3.
 */
static int op_div_int(DexFileFormat *dex, simple_dalvik_vm *vm, u1 *ptr, int *pc)
{
    int reg_idx_vx = 0;
    int reg_idx_vy = 0;
    int reg_idx_vz = 0;
    int x = 0, y = 0 , z = 0;
    reg_idx_vx = ptr[*pc + 1];
    reg_idx_vy = ptr[*pc + 2];
    reg_idx_vz = ptr[*pc + 3];

    if (is_verbose())
        printf("div-int v%d, v%d, v%d\n", reg_idx_vx, reg_idx_vy, reg_idx_vz);
    /* x = y + z */
    load_reg_to(vm, reg_idx_vy, (unsigned char *) &y);
    load_reg_to(vm, reg_idx_vz, (unsigned char *) &z);
    x = y % z;
    x = (y - x) / z;
    store_to_reg(vm, reg_idx_vx, (unsigned char *) &x);
    *pc = *pc + 4;
    return 0;

}

/* 0x8A double-to-int vx, vy
 * Converts the double value in vy,vy+1 into an integer value in vx.
 * 8A40  - double-to-int v0, v4
 * Converts the double value in v4,v5 into an integer value in v0.
 */
static int op_double_to_int(DexFileFormat *dex, simple_dalvik_vm *vm, u1 *ptr, int *pc)
{
    int reg_idx_vx = 0;
    int reg_idx_vy = 0;
    int reg_idx_vz = 0;
    double d = 0;
    unsigned char *ptr_d = (unsigned char *) &d;
    int i = 0;
    int i2 = 0 ;
    reg_idx_vx = ptr[*pc + 1] & 0x0F;
    reg_idx_vy = (ptr[*pc + 1] >> 4) & 0x0F;
    reg_idx_vz = reg_idx_vy + 1;

    load_reg_to_double(vm, reg_idx_vy , ptr_d + 4);
    load_reg_to_double(vm, reg_idx_vz , ptr_d);

    i = (int)d;
    if (is_verbose()) {
        printf("double-to-int v%d, v%d\n", reg_idx_vx, reg_idx_vy);
        printf("(%f) to (%d) \n", d , i);
    }

    store_to_reg(vm, reg_idx_vx, (unsigned char *) &i);
    *pc = *pc + 2;
    return 0;
}

/* 0xb0 add-int/2addr vx,vy
 * Adds vy to vx.
 * B010 - add-int/2addr v0,v1 Adds v1 to v0.
 */
static int op_add_int_2addr(DexFileFormat *dex, simple_dalvik_vm *vm, u1 *ptr, int *pc)
{
    int reg_idx_vx = 0;
    int reg_idx_vy = 0;
    int x = 0, y = 0;
    reg_idx_vx = ptr[*pc + 1] & 0x0F ;
    reg_idx_vy = (ptr[*pc + 1] >> 4) & 0x0F ;
    if (is_verbose())
        printf("add-int/2addr v%d, v%d\n", reg_idx_vx, reg_idx_vy);
    load_reg_to(vm, reg_idx_vx, (unsigned char *) &x);
    load_reg_to(vm, reg_idx_vy, (unsigned char *) &y);
    x = x + y;
    store_to_reg(vm, reg_idx_vx, (unsigned char *) &x);

    *pc = *pc + 2;
    return 0;
}

/* 0xb1 sub-int/2addr vx,vy
 * Subtracts vy to vx.
 * B110 - add-int/2addr v0,v1 subtracts v1 to v0.
 */
static int op_sub_int_2addr(DexFileFormat *dex, simple_dalvik_vm *vm, u1 *ptr, int *pc)
{
    int reg_idx_vx = 0;
    int reg_idx_vy = 0;
    int x = 0, y = 0;
    reg_idx_vx = ptr[*pc + 1] & 0x0F ;
    reg_idx_vy = (ptr[*pc + 1] >> 4) & 0x0F ;
    if (is_verbose())
        printf("add-int/2addr v%d, v%d\n", reg_idx_vx, reg_idx_vy);
    load_reg_to(vm, reg_idx_vx, (unsigned char *) &x);
    load_reg_to(vm, reg_idx_vy, (unsigned char *) &y);
    x = x - y;
    store_to_reg(vm, reg_idx_vx, (unsigned char *) &x);

    *pc = *pc + 2;
    return 0;
}


/* 0xcb , add-double/2addr
 * Adds vy to vx.
 * CB70 - add-double/2addr v0, v7
 * Adds v7 to v0.
 */
static int op_add_double_2addr(DexFileFormat *dex, simple_dalvik_vm *vm, u1 *ptr, int *pc)
{
    int reg_idx_vx = 0;
    int reg_idx_vy = 0;
    double x = 0.0, y = 0.0;
    unsigned char *ptr_x = (unsigned char *) &x;
    unsigned char *ptr_y = (unsigned char *) &y;
    reg_idx_vx = ptr[*pc + 1] & 0x0F;
    reg_idx_vy = (ptr[*pc + 1] >> 4) & 0x0F;

    load_reg_to_double(vm, reg_idx_vx, ptr_x + 4);
    load_reg_to_double(vm, reg_idx_vx + 1, ptr_x);
    load_reg_to_double(vm, reg_idx_vy, ptr_y + 4);
    load_reg_to_double(vm, reg_idx_vy + 1, ptr_y);


    if (is_verbose()) {
        printf("add-double/2addr v%d, v%d\n", reg_idx_vx, reg_idx_vy);
        printf("%f(%llx) + %f(%llx) = %f\n", x, (long long unsigned int)x, y,(long long unsigned int) y , y + x);
    }
    x = x + y;
    store_double_to_reg(vm, reg_idx_vx, ptr_x + 4);
    store_double_to_reg(vm, reg_idx_vx + 1, ptr_x);
    *pc = *pc + 2;
    return 0;
}

/* 0xcd , mul-double/2addr
 * Multiplies vx with vy
 * CD20 - mul-double/2addr v0, v2
 * Multiplies the double value in v0,v1 with the
 * double value in v2,v3 and puts the result into v0,v1.
 */
static int op_mul_double_2addr(DexFileFormat *dex, simple_dalvik_vm *vm, u1 *ptr, int *pc)
{
    int reg_idx_vx = 0;
    int reg_idx_vy = 0;
    int reg_idx_vz = 0;
    int reg_idx_vw = 0;
    double x = 0.0, y = 0.0;
    unsigned char *ptr_x = (unsigned char *) &x;
    unsigned char *ptr_y = (unsigned char *) &y;

    reg_idx_vx = ptr[*pc + 1] & 0x0F;
    reg_idx_vy = reg_idx_vx + 1;
    reg_idx_vz = (ptr[*pc + 1] >> 4) & 0x0F;
    reg_idx_vw = reg_idx_vz + 1 ;

    load_reg_to_double(vm, reg_idx_vx, ptr_x + 4);
    load_reg_to_double(vm, reg_idx_vy, ptr_x);

    load_reg_to_double(vm, reg_idx_vz, ptr_y + 4);
    load_reg_to_double(vm, reg_idx_vw, ptr_y);

    if (is_verbose()) {
        printf("mul-double/2addr v%d, v%d\n", reg_idx_vx, reg_idx_vz);
        printf(" %f * %f = %f\n", x, y, x * y);
    }

    x = x * y;

    store_double_to_reg(vm, reg_idx_vx, ptr_x + 4);
    store_double_to_reg(vm, reg_idx_vy, ptr_x);

    load_reg_to_double(vm, reg_idx_vx, ptr_y + 4);
    load_reg_to_double(vm, reg_idx_vy, ptr_y);

    *pc = *pc + 2;
    return 0;
}

static int op_goto(DexFileFormat *dex, simple_dalvik_vm *vm, u1 *ptr, int *pc){
    char offset = (char) ptr[*pc + 1];
    if(is_verbose())
        printf("goto %#x\n",(int)offset);
    *pc = *pc + offset * 2;
    return 0;
}

/* 0x31 cmp-long vx, vy, vz
 * Compares the long values in vy and vz and sets the integer value in vx accordingly.
 * 3100 0204 - cmp-long v0, v2, v4
 * Compares the long values in v2 and v4 then sets v0 accordingly.
 */
static int op_cmp_long(DexFileFormat *dex, simple_dalvik_vm *vm, u1 *ptr, int *pc)
{
  int reg_idx_vx = 0;
  int reg_idx_vy = 0;
  int reg_idx_vz = 0;
  reg_idx_vx = ptr[*pc + 1];
  reg_idx_vy = ptr[*pc + 2];
  reg_idx_vz = ptr[*pc + 3];

  long vx, vy, vz;
  unsigned char *ptr_x = (unsigned char *) &vx;
  unsigned char *ptr_y = (unsigned char *) &vy;
  unsigned char *ptr_z = (unsigned char *) &vz;
  load_reg_to_double(vm, reg_idx_vx, (unsigned char *) ptr_x+4);
  load_reg_to_double(vm, reg_idx_vx+1, (unsigned char *) ptr_x);
  load_reg_to_double(vm, reg_idx_vy, (unsigned char *) ptr_y+4);
  load_reg_to_double(vm, reg_idx_vy+1, (unsigned char *) ptr_y);

  if (is_verbose()) {
    printf("cmp-long v%d (%ld), v%d (%ld), v%d\n", reg_idx_vx, vx, reg_idx_vy, vy, reg_idx_vz);
  }

  if (vx == vy) {
    vz = 0;
  } else if (vx > vy){
    vz = 1;
  } else {
    vz = -1;
  }
  store_double_to_reg(vm, reg_idx_vz, ptr_z+4);
  store_double_to_reg(vm, reg_idx_vz+1, ptr_z);
  *pc = *pc + 4;
  return 0;
}

/* 0x32 if-eq
 * Jumps to target if vx==vy. vx and vy are integer values.
 * 32b3 6600 - if-eq v3, v11, 0080 // +0066
 * Jumps to the current position+66H words if v3==v11. 0080 is the label of the target instruction.
 */
static int op_if_eq(DexFileFormat *dex, simple_dalvik_vm *vm, u1 *ptr, int *pc)
{
  int reg_idx_vx = 0;
  int reg_idx_vy = 0;
  int jumpOffset = 0;

  reg_idx_vx = ptr[*pc + 1] & 0x0F;
  reg_idx_vy = ptr[*pc + 1] >> 4 & 0x0F;
  jumpOffset = (int)(ptr[*pc + 3] << 8 | ptr[*pc + 2]);

  int vx, vy;
  load_reg_to(vm, reg_idx_vy, (unsigned char *) &vy);
  load_reg_to(vm, reg_idx_vx, (unsigned char *) &vx);

  if (is_verbose()) {
    printf("if-get v%d (%d), v%d (%d), %d\n", reg_idx_vx, vx, reg_idx_vy, vy, jumpOffset);
  }

  if (vx == vy) {
    *pc = *pc + jumpOffset*2;
  } else {
    *pc = *pc + 4;
  }
  return 0;
}

/* 0x33 if-ne
 * Jumps to target if vx!=vy. vx and vy are integer values.
 * 33A3 1000 - if-ne v3, v10, 002c // +0010
 * Jumps to the current position+10H words if v3!=v10.
 * 002c is the label of the target instruction.
 */
static int op_if_ne(DexFileFormat *dex, simple_dalvik_vm *vm, u1 *ptr, int *pc)
{
  int reg_idx_vx = 0;
  int reg_idx_vy = 0;
  int jumpOffset = 0;

  reg_idx_vx = ptr[*pc + 1] & 0x0F;
  reg_idx_vy = ptr[*pc + 1] >> 4 & 0x0F;
  jumpOffset = (int)(ptr[*pc + 3] << 8 | ptr[*pc + 2]);

  int vx, vy;
  load_reg_to(vm, reg_idx_vy, (unsigned char *) &vy);
  load_reg_to(vm, reg_idx_vx, (unsigned char *) &vx);

  if (is_verbose()) {
    printf("if-get v%d (%d), v%d (%d), %d\n", reg_idx_vx, vx, reg_idx_vy, vy, jumpOffset);
  }

  if (vx != vy) {
    *pc = *pc + jumpOffset*2;
  } else {
    *pc = *pc + 4;
  }
  return 0;
}

/* 0x34 if-lt
 * Jumps to target is vx<vy. vx and vy are integer values.
 * 3432 CBFF - if-lt v2, v3, 0023 // -0035
 * Jumps to the current position-35H words if v2<v3.
 * 0023 is the label of the target instruction.
 */
static int op_if_lt(DexFileFormat *dex, simple_dalvik_vm *vm, u1 *ptr, int *pc)
{
  int reg_idx_vx = 0;
  int reg_idx_vy = 0;
  int jumpOffset = 0;

  reg_idx_vx = ptr[*pc + 1] & 0x0F;
  reg_idx_vy = ptr[*pc + 1] >> 4 & 0x0F;
  jumpOffset = (int)(ptr[*pc + 3] << 8 | ptr[*pc + 2]);

  int vx, vy;
  load_reg_to(vm, reg_idx_vy, (unsigned char *) &vy);
  load_reg_to(vm, reg_idx_vx, (unsigned char *) &vx);

  if (is_verbose()) {
    printf("if-get v%d (%d), v%d (%d), %d\n", reg_idx_vx, vx, reg_idx_vy, vy, jumpOffset);
  }

  if (vx < vy) {
    *pc = *pc + jumpOffset*2;
  } else {
    *pc = *pc + 4;
  }
  return 0;
}

static int op_if_ge(DexFileFormat *dex, simple_dalvik_vm *vm, u1 *ptr, int *pc){
    int reg_idx_vx = ptr[*pc + 1] & 0x0F;
    int reg_idx_vy = (ptr[*pc + 1]>>4) & 0x0F;
    int offset = (int) ptr[*pc + 2] + (int) ptr[*pc + 3] * (int) 256;
    int x,y;
    load_reg_to(vm, reg_idx_vy, (unsigned char*) &y);
    load_reg_to(vm, reg_idx_vx, (unsigned char*) &x);
    if(is_verbose()){
        printf("if-ge v[%d]:%d, v[%d]:%d, off:%#x\n",reg_idx_vy,y,reg_idx_vx,x,offset);
    }
    if(x >= y)
        *pc = *pc + offset * 2;
    else
        *pc = *pc + 4;
    return 0;
}

/* 0x36 if-gt
 * 3610 1B00 - if-ge v0, v1, 002b // +001b
 * Jumps to the current position+1BH words if v0>v1.
 * 002b is the label of the target instruction.
 */
static int op_if_gt(DexFileFormat *dex, simple_dalvik_vm *vm, u1 *ptr, int *pc)
{
  int reg_idx_vx = 0;
  int reg_idx_vy = 0;
  int jumpOffset = 0;

  reg_idx_vx = ptr[*pc + 1] & 0x0F;
  reg_idx_vx = ptr[*pc + 1] >> 4 & 0x0F;
  jumpOffset = (int)(ptr[*pc + 3] << 8 | ptr[*pc + 2]);

  int vx, vy;
  load_reg_to(vm, reg_idx_vy, (unsigned char *) &vy);
  load_reg_to(vm, reg_idx_vx, (unsigned char *) &vx); 

  if (is_verbose()) {
    printf("if-get v%d (%d), v%d (%d), %d\n", reg_idx_vx, vx, reg_idx_vy, vy, jumpOffset);
  }

  if (vx > vy) {
    *pc = *pc + jumpOffset*2;
  } else {
    *pc = *pc + 4;
  }
  return 0;
}

/* 0x37 if-le
 * Jumps to target is vx<=vy. vx and vy are integer values.
 * 3756 0B00 - if-le v6, v5, 0144 // +000b
 * Jumps to the current position+0BH words if v6<=v5.
 * 0144 is the label of the target instruction.
 */
static int 
op_if_le(DexFileFormat *dex, simple_dalvik_vm *vm, u1 *ptr, int *pc)
{
  int reg_idx_vx = 0;
  int reg_idx_vy = 0;
  int jumpOffset = 0;

  reg_idx_vx = ptr[*pc + 1] & 0x0F;
  reg_idx_vy = ptr[*pc + 1] >> 4 & 0x0F;
  jumpOffset = (int)(ptr[*pc + 3] << 8 | ptr[*pc + 2]);

  int vx, vy;
  load_reg_to(vm, reg_idx_vy, (unsigned char *) &vy);
  load_reg_to(vm, reg_idx_vx, (unsigned char *) &vx);

  if (is_verbose()) {
    printf("if-get v%d (%d), v%d (%d), %d\n", reg_idx_vx, vx, reg_idx_vy, vy, jumpOffset);
  }

  if (vx <= vy) {
    *pc = *pc + jumpOffset*2;
  } else {
    *pc = *pc + 4;
  }
  return 0;
}

/* 0x38 if-eqz
 * Jumps to target if vx==0, vx is an integer value.
 * 3802 1900 - if-eqz v2, 0038 // +0019
 * Jumps to the current position+19H words if v2==0.
 * 0038 is the label of the target instruction.
 */
static int 
op_if_eqz(DexFileFormat *dex, simple_dalvik_vm *vm, u1 *ptr, int *pc)
{
  int reg_idx_vx = 0;
  int jumpOffset = 0;

  reg_idx_vx = ptr[*pc + 1] & 0x0F;
  jumpOffset = (int)(ptr[*pc + 3] << 8 | ptr[*pc + 2]);

  int vx;
  load_reg_to(vm, reg_idx_vx, (unsigned char *) &vx);

  if (is_verbose()) {
    printf("if-eqz v%d (%d), %d\n", reg_idx_vx, vx, jumpOffset);
  }

  if (vx == 0) {
    *pc = *pc + jumpOffset*2;
  } else {
    *pc = *pc + 4;
  }
  return 0;
}

/* 0x39 if-nez
 * Checks vx and jumps if vx is nonzero2. 
 * 3902 1200 - if-nez v2, 0014 // +0012
 * Jumps to current position+18 words (hex 12) if v2 is nonzero.
 * 0014 is the label of the target instruction.
 */
static int op_if_nez(DexFileFormat *dex, simple_dalvik_vm *vm, u1 *ptr, int *pc)
{
  int reg_idx_vx = 0;
  int jumpOffset = 0;

  reg_idx_vx = ptr[*pc + 1] & 0x0F;
  jumpOffset = (int)(ptr[*pc + 3] << 8 | ptr[*pc + 2]);

  int vx;
  load_reg_to(vm, reg_idx_vx, (unsigned char *) &vx);

  if (is_verbose()) {
    printf("if-get v%d (%d), %d\n", reg_idx_vx, vx, jumpOffset);
  }

  if (vx != 0) {
    *pc = *pc + jumpOffset*2;
  } else {
    *pc = *pc + 4;
  }
  return 0;
}

/* 0x3a if-ltz
 * Checks vx and jumps if vx<0. 
 * 3A00 1600 - if-ltz v0, 002d // +0016
 * Jumps to the current position+16H words if v0<0.
 * 002d is the label of the target instruction.
 */
static int op_if_ltz(DexFileFormat *dex, simple_dalvik_vm *vm, u1 *ptr, int *pc)
{
  int reg_idx_vx = 0;
  int jumpOffset = 0;

  reg_idx_vx = ptr[*pc + 1] & 0x0F;
  jumpOffset = (int)(ptr[*pc + 3] << 8 | ptr[*pc + 2]);

  int vx;
  load_reg_to(vm, reg_idx_vx, (unsigned char *) &vx);

  if (is_verbose()) {
    printf("if-get v%d (%d), %d\n", reg_idx_vx, vx, jumpOffset);
  }

  if (vx < 0) {
    *pc = *pc + jumpOffset*2;
  } else {
    *pc = *pc + 4;
  }
  return 0;
}

/* 0x3b if-gez
 * Checks vx and jumps if vx>=0.
 * 3B00 1600 - if-gez v0, 002d // +0016
 * Jumps to the current position+16H words if v0 >=0.
 * 002d is the label of the target instruction.
 */
static int op_if_gez(DexFileFormat *dex, simple_dalvik_vm *vm, u1 *ptr, int *pc)
{
  int reg_idx_vx = 0;
  int jumpOffset = 0;

  reg_idx_vx = ptr[*pc + 1] & 0x0F;
  jumpOffset = (int)(ptr[*pc + 3] << 8 | ptr[*pc + 2]);

  int vx;
  load_reg_to(vm, reg_idx_vx, (unsigned char *) &vx);

  if (is_verbose()) {
    printf("if-get v%d (%d), %d\n", reg_idx_vx, vx, jumpOffset);
  }

  if (vx >= 0) {
    *pc = *pc + jumpOffset*2;
  } else {
    *pc = *pc + 4;
  }
  return 0;
}

/* 0x3c if-gtz
 * Checks vx and jumps if vx>02.
 * 3C00 1D00 - if-gtz v0, 004a // +001d
 * Jumps to the current position+1DH words if v0>0.
 * 004A is the label of the target instruction. 
 */
static int op_if_gtz(DexFileFormat *dex, simple_dalvik_vm *vm, u1 *ptr, int *pc)
{
  int reg_idx_vx = 0;
  int jumpOffset = 0;

  reg_idx_vx = ptr[*pc + 1] & 0x0F;
  reg_idx_vx = ptr[*pc + 1] >> 4 & 0x0F;
  jumpOffset = (int)(ptr[*pc + 3] << 8 | ptr[*pc + 2]);

  int vx;
  load_reg_to(vm, reg_idx_vx, (unsigned char *) &vx); 

  if (is_verbose()) {
    printf("if-get v%d (%d), %d\n", reg_idx_vx, vx, jumpOffset);
  }

  if (vx > 0) {
    *pc = *pc + jumpOffset*2;
  } else {
    *pc = *pc + 4;
  }
  return 0;
}

/* 0x3d if-lez
 * Checks vx and jumps if vx<=0. 
 * 3D00 1D00 - if-lez v0, 004a // +001d
 * Jumps to the current position+1DH words if v0<=0.
 * 004A is the label of the target instruction.
 */
static int op_if_lez(DexFileFormat *dex, simple_dalvik_vm *vm, u1 *ptr, int *pc)
{
  int reg_idx_vx = 0;
  int jumpOffset = 0;

  reg_idx_vx = ptr[*pc + 1] & 0x0F;
  jumpOffset = (int)(ptr[*pc + 3] << 8 | ptr[*pc + 2]);

  int vx;
  load_reg_to(vm, reg_idx_vx, (unsigned char *) &vx);

  if (is_verbose()) {
    printf("if-get v%d (%d), %d\n", reg_idx_vx, vx, jumpOffset);
  }

  if (vx <= 0) {
    *pc = *pc + jumpOffset*2;
  } else {
    *pc = *pc + 4;
  }
  return 0;
}


static int op_long_to_int(DexFileFormat *dex, simple_dalvik_vm *vm, u1 *ptr, int *pc){
    int reg_idx_vx = ptr[*pc + 1] & 0x0F;
    int reg_idx_vy = (ptr[*pc + 1]>>4) & 0x0F;
    int reg_idx_vz = reg_idx_vy + 1;
    long value = 0;
    unsigned char *ptr_value = (unsigned char*) &value;
    load_reg_to_long(vm, reg_idx_vy, ptr_value + 4);
    load_reg_to_long(vm, reg_idx_vz, ptr_value);
    int value_to = (int)value;
    store_to_reg(vm, reg_idx_vx, (unsigned char*)&value_to);
    if(is_verbose()){
        printf("long to int v%d v%d, %ld -> %d",reg_idx_vx,reg_idx_vy,value,value_to);
    }
    *pc = *pc + 2;
    return 0;
}

/* 0x81 int-to-long vx, vy
 * Converts the integer in vy into a long in vx,vx+1.
 * 8106 - int-to-long v6, v0
 * Converts an integer in v0 into a long in v6,v7.
 */
static int 
op_int_to_long(DexFileFormat *dex, simple_dalvik_vm *vm, u1 *ptr, int *pc)
{
  int reg_idx_vx = 0;
  int reg_idx_vy = 0;
  int i = 0;
  long l = 0;
  unsigned char *ptr_l = (unsigned char *) &l;
  reg_idx_vx = ptr[*pc + 1] & 0x0F;
  reg_idx_vy = (ptr[*pc + 1] >> 4) & 0x0F;

  load_reg_to(vm, reg_idx_vy, (unsigned char *) &i);

  l = (long)i;
  if (is_verbose()) {
    printf("int-to-long v%d, v%d\n", reg_idx_vy, reg_idx_vx);
    printf("(%d) to (%ld) \n", i , l);
  }

  store_double_to_reg(vm, reg_idx_vx, ptr_l+4);
  store_double_to_reg(vm, reg_idx_vx + 1, ptr_l);
  *pc = *pc + 2;
  return 0;
}

/* 0x8e int-to-char vx,vy
 * Converts the int value in vy to a char value and stores it in vx. 
 *
 * 8E33  - int-to-char v3, v3
 * Converts the integer in v3 into a char and puts the char value into v3.
 */
static int op_int_to_char(DexFileFormat *dex, simple_dalvik_vm *vm, u1 *ptr, int *pc){
    int reg_idx_vx = ptr[*pc + 1] & 0x0F;
    int reg_idx_vy = (ptr[*pc + 1]>>4) & 0x0F;
    int value1 = 0L, value2 = 0L;
    unsigned char* ptr2 = (unsigned char*) &value1;
    load_reg_to(vm, reg_idx_vx, ptr2);
    value2 = ptr2[0];
    if (is_verbose()) {
        printf("int-to-char v%d, v%d\n", reg_idx_vy, reg_idx_vx);
        printf("int %d to char %d\n", value1, value2);
    }
    store_long_to_reg(vm, reg_idx_vy, (unsigned char*)&value2);
    *pc = *pc + 2;
    return 0;
}



static int op_sub_long_2addr(DexFileFormat *dex, simple_dalvik_vm *vm, u1 *ptr, int *pc){
    int reg_idx_vx = ptr[*pc + 1] & 0x0F;
    int reg_idx_vy = (ptr[*pc + 1]>>4) & 0x0F;
    int reg_idx_vz = reg_idx_vy + 1;
    long value1 = 0L, value2 = 0L;
    unsigned char *ptr_value1 = (unsigned char*) &value1;
    unsigned char *ptr_value2 = (unsigned char*) &value2;
    load_reg_to_long(vm, reg_idx_vx, ptr_value1 + 4);
    load_reg_to_long(vm, reg_idx_vx+1, ptr_value1);
    load_reg_to_long(vm, reg_idx_vy, ptr_value2 + 4);
    load_reg_to_long(vm, reg_idx_vy+1, ptr_value2);
    value1 = value1 - value2;
    store_long_to_reg(vm, reg_idx_vx, ptr_value1+4);
    store_long_to_reg(vm, reg_idx_vx+1, ptr_value1);
    if(is_verbose()){
        printf("long to int v%d v%d, %ld - %ld",reg_idx_vx,reg_idx_vy,value1,value2);
    }
    *pc = *pc + 2;
    return 0;
}

/* 0xbd mul-long/2addr vx,vy
 * Calculates vx*vy and puts the result into vx
 * BD70 - mul-long/2addr v0, v7
 * Multiplies the long value in v7,v8 with the long value in v0,v1 and puts the result into v0,v1. 
 */
static int op_mul_long_2addr(DexFileFormat *dex, simple_dalvik_vm *vm, u1 *ptr, int *pc)
{
    int reg_idx_vx = 0;
    int reg_idx_vy = 0;
    long x = 0, y = 0;
    reg_idx_vx = ptr[*pc + 1] & 0x0F ;
    reg_idx_vy = (ptr[*pc + 1] >> 4) & 0x0F ;
    unsigned char *ptr_x = (unsigned char *) &x;
    unsigned char *ptr_y = (unsigned char *) &y;
    if (is_verbose())
        printf("mul-long/2addr v%d, v%d\n", reg_idx_vx, reg_idx_vy);
    load_reg_to_double(vm, reg_idx_vx, ptr_x + 4);
    load_reg_to_double(vm, reg_idx_vx + 1, ptr_x);
    load_reg_to_double(vm, reg_idx_vy, ptr_y + 4);
    load_reg_to_double(vm, reg_idx_vy + 1, ptr_y);
    x = x * y;
    store_double_to_reg(vm, reg_idx_vx, (unsigned char *) (&x)+4);
    store_double_to_reg(vm, reg_idx_vx, (unsigned char *) (&x)+0);

    *pc = *pc + 2;
    return 0;
}

static int op_add_long_2addr(DexFileFormat *dex, simple_dalvik_vm *vm, u1 *ptr, int *pc)
{
    int reg_idx_vx = 0;
    int reg_idx_vy = 0;
    long x = 0, y = 0;
    reg_idx_vx = ptr[*pc + 1] & 0x0F ;
    reg_idx_vy = (ptr[*pc + 1] >> 4) & 0x0F ;
    unsigned char *ptr_x = (unsigned char *) &x;
    unsigned char *ptr_y = (unsigned char *) &y;
    if (is_verbose())
        printf("add-long/2addr v%d, v%d\n", reg_idx_vx, reg_idx_vy);
    load_reg_to_double(vm, reg_idx_vx, ptr_x + 4);
    load_reg_to_double(vm, reg_idx_vx + 1, ptr_x);
    load_reg_to_double(vm, reg_idx_vy, ptr_y + 4);
    load_reg_to_double(vm, reg_idx_vy + 1, ptr_y);
    x = x + y;
    store_double_to_reg(vm, reg_idx_vx, (unsigned char *) (&x)+4);
    store_double_to_reg(vm, reg_idx_vx + 1, (unsigned char *) (&x)+0);

    *pc = *pc + 2;
    return 0;
}


/* 0x9b add-long vx,vy,vz
 * Adds vy to vz and puts the result into vx
 * 9B00 0305 - add-long v0, v3, v5
 * The long value in v3,v4 is added to the value in v5,v6 and the result is stored in v0,v1.
 */
static int 
op_add_long(DexFileFormat *dex, simple_dalvik_vm *vm, u1 *ptr, int *pc)
{
    int reg_idx_vx = 0;
    int reg_idx_vy = 0;
    int reg_idx_vz = 0;
    long x = 0, y = 0 , z = 0;
    unsigned char *ptr_x = (unsigned char *) &x;
    unsigned char *ptr_y = (unsigned char *) &y;
    unsigned char *ptr_z = (unsigned char *) &z;
    reg_idx_vx = ptr[*pc + 1];
    reg_idx_vy = ptr[*pc + 2];
    reg_idx_vz = ptr[*pc + 3];

    if (is_verbose()) {
        printf("add-long v%d, v%d, v%d\n", reg_idx_vx, reg_idx_vy, reg_idx_vz);
    }
    /* x = y + z */
    load_reg_to_double(vm, reg_idx_vy, ptr_y+4);
    load_reg_to_double(vm, reg_idx_vy+1, ptr_y);
    load_reg_to_double(vm, reg_idx_vz, ptr_z+4);
    load_reg_to_double(vm, reg_idx_vz+1, ptr_z);
    x = y + z;
    if (is_verbose()) {
        printf("add-long %ld, %ld, %ld\n", y, z, y+z);
    }
    store_double_to_reg(vm, reg_idx_vx, ptr_x+4);
    store_double_to_reg(vm, reg_idx_vx+1, ptr_x);
    *pc = *pc + 4;
    return 0;
}

static int op_sub_long(DexFileFormat *dex, simple_dalvik_vm *vm, u1 *ptr, int *pc)
{
    int reg_idx_vx = 0;
    int reg_idx_vy = 0;
    int reg_idx_vz = 0;
    long x = 0, y = 0 , z = 0;
    reg_idx_vx = ptr[*pc + 1];
    reg_idx_vy = ptr[*pc + 2];
    reg_idx_vz = ptr[*pc + 3];
    unsigned char *ptr_x = (unsigned char *) &x;
    unsigned char *ptr_y = (unsigned char *) &y;
    unsigned char *ptr_z = (unsigned char *) &z;

    if (is_verbose())
        printf("sub-long v%d, v%d, v%d\n", reg_idx_vx, reg_idx_vy, reg_idx_vz);
    /* x = y - z */
    load_reg_to_long(vm, reg_idx_vy, ptr_y+4);
    load_reg_to_long(vm, reg_idx_vy+1, ptr_y);
    load_reg_to_long(vm, reg_idx_vz, ptr_z+4);
    load_reg_to_long(vm, reg_idx_vz+1, ptr_z);
    x = y - z;
    store_long_to_reg(vm, reg_idx_vx, ptr_x+4);
    store_long_to_reg(vm, reg_idx_vx+1, ptr_x);
    *pc = *pc + 4;
    return 0;

}

/* 0x9d mul-long vx,vy,vz
 * Calculates vy*vz and puts the result into vx.
 * 9C00 0305 - sub-long v0, v3, v5
 * Subtracts the long value in v5,v6 from the long value in v3,v4 and puts the result into v0,v1.
 */
static int op_mul_long(DexFileFormat *dex, simple_dalvik_vm *vm, u1 *ptr, int *pc)
{
    int reg_idx_vx = 0;
    int reg_idx_vy = 0;
    int reg_idx_vz = 0;
    long x = 0, y = 0 , z = 0;
    unsigned char *ptr_x = (unsigned char *) &x;
    unsigned char *ptr_y = (unsigned char *) &y;
    unsigned char *ptr_z = (unsigned char *) &z;
    reg_idx_vx = ptr[*pc + 1];
    reg_idx_vy = ptr[*pc + 2];
    reg_idx_vz = ptr[*pc + 3];

    if (is_verbose()) {
        printf("mul-long v%d, v%d, v%d\n", reg_idx_vx, reg_idx_vy, reg_idx_vz);
    }
    /* x = y * z */
    load_reg_to_double(vm, reg_idx_vy, ptr_y+4);
    load_reg_to_double(vm, reg_idx_vy+1, ptr_y);
    load_reg_to_double(vm, reg_idx_vz, ptr_z+4);
    load_reg_to_double(vm, reg_idx_vz+1, ptr_z);
    x = y * z;
    if (is_verbose()) {
        printf("sub-long %ld, %ld, %ld\n", y, z, y*z);
    }
    store_double_to_reg(vm, reg_idx_vx, ptr_x+4);
    store_double_to_reg(vm, reg_idx_vx+1, ptr_x);
    *pc = *pc + 4;
    return 0;
}

/* 0x9e div-long vx,vy,vz
 * Calculates vy/vz and puts the result into vx.
 * 9E06 0002 - div-long v6, v0, v2
 * Divides the long value in v0,v1 with the long value in v2,v3 and pust the result into v6,v7.
 */
static int op_div_long(DexFileFormat *dex, simple_dalvik_vm *vm, u1 *ptr, int *pc)
{
    int reg_idx_vx = 0;
    int reg_idx_vy = 0;
    int reg_idx_vz = 0;
    long x = 0, y = 0 , z = 0;
    unsigned char *ptr_x = (unsigned char *) &x;
    unsigned char *ptr_y = (unsigned char *) &y;
    unsigned char *ptr_z = (unsigned char *) &z;
    reg_idx_vx = ptr[*pc + 1];
    reg_idx_vy = ptr[*pc + 2];
    reg_idx_vz = ptr[*pc + 3];

    if (is_verbose()) {
        printf("div-long v%d, v%d, v%d\n", reg_idx_vx, reg_idx_vy, reg_idx_vz);
    }
    /* x = y / z */
    load_reg_to_double(vm, reg_idx_vy, ptr_y+4);
    load_reg_to_double(vm, reg_idx_vy+1, ptr_y);
    load_reg_to_double(vm, reg_idx_vz, ptr_z+4);
    load_reg_to_double(vm, reg_idx_vz+1, ptr_z);
    x = y / z;
    if (is_verbose()) {
        printf("div-long %ld, %ld, %ld\n", y, z, y/z);
    }
    store_double_to_reg(vm, reg_idx_vx, ptr_x+4);
    store_double_to_reg(vm, reg_idx_vx+1, ptr_x);
    *pc = *pc + 4;
    return 0;
}


static int op_add_int_lit8(DexFileFormat *dex, simple_dalvik_vm *vm, u1 *ptr, int *pc)
{
    int reg_idx_vx = 0;
    int reg_idx_vy = 0;
    int x = 0, y = 0 ;
    int z = 0;
    reg_idx_vx = ptr[*pc + 1];
    reg_idx_vy = ptr[*pc + 2];
    z = ptr[*pc + 3];

    if (is_verbose())
        printf("add-int v%d, v%d, #int%d\n", reg_idx_vx, reg_idx_vy, z);
    /* x = y + z */
    load_reg_to(vm, reg_idx_vy, (unsigned char *) &y);
    x = y + z;
    store_to_reg(vm, reg_idx_vx, (unsigned char *) &x);

    *pc = *pc + 4;
    return 0;
}

/* 0xd9 sub-int/lit8 vx,vy,lit8
 * Calculates vy-lit8 and stores the result into vx.
 * D900 0201 - sub-int/lit8 v0,v2, #int1
 * Calculates v2-1 and stores the result into v0.
 */
static int 
op_sub_int_lit8(DexFileFormat *dex, simple_dalvik_vm *vm, u1 *ptr, int *pc)
{
    int reg_idx_vx = 0;
    int reg_idx_vy = 0;
    int x = 0, y = 0 ;
    int z = 0;
    reg_idx_vx = ptr[*pc + 1];
    reg_idx_vy = ptr[*pc + 2];
    z = ptr[*pc + 3];

    if (is_verbose())
        printf("sub-int v%d, v%d, #int%d\n", reg_idx_vx, reg_idx_vy, z);
    /* x = y - z */
    load_reg_to(vm, reg_idx_vy, (unsigned char *) &y);
    x = y - z;
    store_to_reg(vm, reg_idx_vx, (unsigned char *) &x);

    *pc = *pc + 4;
    return 0;
}

/* 0xda mul-int/lit8 vx,vy,lit8
 * Multiplies vy with lit8 8-bit literal constant and puts the result into vx.
 * DA00 0002 - mul-int/lit8 v0,v0, #int2
 * Multiplies v0 with literal 2 and puts the result into v0.
 */
static int 
op_mul_int_lit8(DexFileFormat *dex, simple_dalvik_vm *vm, u1 *ptr, int *pc)
{
    int reg_idx_vx = ptr[*pc + 1];
    int reg_idx_vy = ptr[*pc + 2];
    int x = 0, y = 0 ;
    int z = ptr[*pc + 3];

    if (is_verbose())
        printf("mul-int v%d, v%d, #int%d\n", reg_idx_vx, reg_idx_vy, z);
    /* x = y * z */
    simple_dvm_register *rx = &vm->regs[reg_idx_vx];
    simple_dvm_register *ry = &vm->regs[reg_idx_vy];
    if (reg_idx_vx != reg_idx_vy) {
      rx->data[0] = ry->data[0];
      rx->data[1] = ry->data[1];
      rx->data[2] = ry->data[2];
      rx->data[3] = ry->data[3];
    }

    *(int *)rx->data *= z;
    *pc = *pc + 4;
    return 0;
}

/* 0xdb div-int/lit8 vx,vy,lit8
 * Calculates vy/lit8 and stores the result into vx.
 * DB00 0203 - div-int/lit8 v0,v2, #int3
 * Calculates v2/3 and stores the result into v0.
 */
static int op_div_int_lit8(DexFileFormat *dex, simple_dalvik_vm *vm, u1 *ptr, int *pc)
{
    int reg_idx_vx = 0;
    int reg_idx_vy = 0;
    int x = 0, y = 0 ;
    int z = 0;
    reg_idx_vx = ptr[*pc + 1];
    reg_idx_vy = ptr[*pc + 2];
    z = ptr[*pc + 3];

    if (is_verbose())
        printf("div-int v%d, v%d, #int%d\n", reg_idx_vx, reg_idx_vy, z);
    /* x = y / z */
    load_reg_to(vm, reg_idx_vy, (unsigned char *) &y);
    x = y % z;
    x = (y - x) / z;
    store_to_reg(vm, reg_idx_vx, (unsigned char *) &x);

    *pc = *pc + 4;
    return 0;
}

static int op_sput_object(DexFileFormat *dex, simple_dalvik_vm *vm, u1 *ptr, int *pc)
{
    int reg_idx_vx = 0;
    int target_field_id = 0;
    int object_reference = 0;
    field_id_item *s = NULL, *t = NULL;

    reg_idx_vx = ptr[*pc + 1];
    target_field_id = ((ptr[*pc + 3] << 8) | ptr[*pc + 2]);

    load_reg_to(vm, reg_idx_vx, (unsigned char *) &object_reference);
    if (is_verbose()) {
        printf("sput-object v%d, field 0x%04x to 0x%04x\n", reg_idx_vx, object_reference, target_field_id);
    }
    memcpy(vm->static_field_value[target_field_id].data, (unsigned char*)&object_reference, sizeof(int));
    *pc = *pc + 4;
    return 0;

}

static int op_iput_wide(DexFileFormat *dex, simple_dalvik_vm *vm, u1 *ptr, int *pc){
    int reg_idx_vx = 0;
    int reg_idx_vx_p1 = 0;
    int reg_idx_vy = 0;
    int target_field_id = 0;
    reg_idx_vx = ptr[*pc + 1] & 0x0F;
    reg_idx_vy = (ptr[*pc + 1] >> 4) & 0x0F;
    reg_idx_vx_p1 = reg_idx_vx + 1;
	
    target_field_id = ((ptr[*pc + 3] << 8) | ptr[*pc + 2]);
    if (is_verbose()) {
        printf("iput-wide v%d, v%d, field 0x%04x\n", reg_idx_vx, reg_idx_vy, target_field_id);
    }
    int obj_ref;
    load_reg_to(vm,reg_idx_vy,(unsigned char*)&obj_ref);
    unsigned char *ptr_x = vm->object[obj_ref]->instance_fields[target_field_id].data;
    load_reg_to_double(vm, reg_idx_vx   , ptr_x + 4);    
    load_reg_to_double(vm, reg_idx_vx_p1, ptr_x );    
    *pc = *pc + 4;
    return 0;
}

static byteCode byteCodes[] = {
    { "move"              , 0x01, 2,  op_move },
    { "goto"              , 0x28, 2,  op_goto },
    { "cmp-long"          , 0x31, 4,  op_cmp_long },
    { "if-eq"             , 0x32, 4,  op_if_eq },
    { "if-ne"             , 0x33, 4,  op_if_ne },
    { "if-lt"             , 0x34, 4,  op_if_lt },
    { "if-ge"             , 0x35, 4,  op_if_ge },
    { "if-gt"             , 0x36, 4,  op_if_gt },
    { "if-le"             , 0x37, 4,  op_if_le },
    { "if-eqz"            , 0x38, 4,  op_if_eqz },
    { "if-nez"            , 0x39, 4,  op_if_nez },
    { "if-ltz"            , 0x3a, 4,  op_if_ltz },
    { "if-gez"            , 0x3b, 4,  op_if_gez },
    { "if-gtz"            , 0x3c, 4,  op_if_gtz },
    { "if-lez"            , 0x3d, 4,  op_if_lez },
    { "long-to-int"       , 0x84, 2,  op_long_to_int},
    { "int-to-long"       , 0x84, 2,  op_int_to_long},
    { "int-to-char"       , 0x8e, 2,  op_int_to_char},
    { "sub-long/2addr"    , 0xbb, 2,  op_add_long_2addr },
    { "sub-long/2addr"    , 0xbc, 2,  op_sub_long_2addr },
    { "mul-long/2addr"    , 0xbd, 2,  op_mul_long_2addr },
    { "add-long"          , 0x9b, 4,  op_add_long },
    { "sub-long"          , 0x9c, 4,  op_sub_long },
    { "mul-long"          , 0x9d, 4,  op_mul_long },
    { "div-long"          , 0x9e, 4,  op_div_long },
    { "move-result"       , 0x0a, 2,  op_move_result },
    { "move-result-wide"  , 0x0B, 2,  op_move_result_wide },
    { "move-result-object", 0x0C, 2,  op_move_result_object },
    { "return-void"       , 0x0e, 2,  op_return_void },
    { "return"            , 0x0f, 2,  op_return },
    { "const/4"           , 0x12, 2,  op_const_4 },
    { "const/16"          , 0x13, 4,  op_const_16 },
    { "const-wide/16"     , 0x16, 4,  op_const_wide_16 },
    { "const-wide/32"     , 0x17, 6,  op_const_wide_32 },
    { "const-wide/high16" , 0x19, 4,  op_const_wide_high16 },
    { "check-cast"        , 0x1f, 4,  op_check_cast },
    { "const-string"      , 0x1a, 4,  op_const_string },
    { "new-instance"      , 0x22, 4,  op_new_instance },
    { "filled-new-array " , 0x24, 6,  op_filled_new_array },
    { "new-array"         , 0x23, 4,  op_new_array },
    { "sget-object"       , 0x62, 4,  op_sget_object },
    { "sput-object"       , 0x69, 4,  op_sput_object },
    { "invoke-virtual"    , 0x6e, 6,  op_invoke_virtual },
    { "invoke-direct"     , 0x70, 6,  op_invoke_direct },
    { "invoke-static"     , 0x71, 6,  op_invoke_static },
    { "double-to-int"     , 0x8a, 2,  op_double_to_int},
    { "add-int"           , 0x90, 4,  op_add_int },
    { "sub-int"           , 0x91, 4,  op_sub_int },
    { "mul-int"           , 0x92, 4,  op_mul_int },
    { "div-int"           , 0x93, 4,  op_div_int },
    { "add-int/2addr"     , 0xb0, 2,  op_add_int_2addr},
    { "add-int/2addr"     , 0xb1, 2,  op_sub_int_2addr},
    { "add-double/2addr"  , 0xcb, 2,  op_add_double_2addr},
    { "mul-double/2addr"  , 0xcd, 2,  op_mul_double_2addr},
    { "add-int/lit8"      , 0xd8, 4,  op_add_int_lit8 },
    { "sub-int/lit8"      , 0xd9, 4,  op_sub_int_lit8 },
    { "mul-int/lit8"      , 0xda, 4,  op_mul_int_lit8 },
    { "div-int/lit8"      , 0xdb, 4,  op_div_int_lit8 },
    { "iput-wide"         , 0x5a, 4,  op_iput_wide }
};
static unsigned int byteCode_size = sizeof(byteCodes) / sizeof(byteCode);

opCodeFunc findOpCodeFunc(unsigned char op)
{
    int i = 0;
    for (i = 0; i < byteCode_size; i++)
        if (op == byteCodes[i].opCode)
            return byteCodes[i].func;
    return 0;
}

