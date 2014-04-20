/*
 * Simple Dalvik Virtual Machine Implementation
 *
 * Copyright (C) 2014 Jim Huang <jserv.tw@gmail.com>
 * Copyright (C) 2013 Chun-Yu Wang <wicanr2@gmail.com>
 */

#include "simple_dvm.h"
#include "java_lib.h"

#ifdef threaded_code

#define NEXT_OP *byteCodes[ptr[*pc]]
static int op_utils_invoke(char *name, DexFileFormat *dex, simple_dalvik_vm *vm,
                           invoke_parameters *p);
static int op_utils_invoke_35c_parse(DexFileFormat *dex, u1 *ptr, int *pc,
                                     invoke_parameters *p);
void printRegs(simple_dalvik_vm *vm);

void dispatch_DTC(DexFileFormat *dex, simple_dalvik_vm *vm, u1 *ptr, int *pc){

static void* byteCodes[] = {
    &&nop, //0x00
    &&op_move,                //0x01
    &&nop, //0x02
    &&nop, //0x03
    &&nop, //0x04
    &&nop, //0x05
    &&nop, //0x06
    &&nop, //0x07
    &&nop, //0x08
    &&nop, //0x09
    &&op_move_result,         //0x0a
    &&op_move_result_wide,    //0x0b
    &&op_move_result_object,  //0x0c
    &&nop, //0x0d
    &&op_return_void,         //0x0e
    &&op_return,              //0x0f
    &&nop, //0x10
    &&nop, //0x11
    &&op_const_4,             //0x12
    &&op_const_16,            //0x13
    &&nop, //0x14
    &&nop, //0x15
    &&op_const_wide_16,       //0x16
    &&op_const_wide_32,       //0x17
    &&nop, ///0x18
    &&op_const_wide_high16,   //0x19
    &&op_const_string,        //0x1a
    &&nop, //0x1b
    &&nop, //0x1c
    &&nop, //0x1d
    &&nop, //0x1e
    &&op_check_cast,          //0x1f
    &&nop, //0x20
    &&nop, //0x21
    &&op_new_instance,        //0x22
    &&op_new_array,           //0x23
    &&op_filled_new_array,    //0x24
    &&nop, //0x25
    &&nop, //0x26
    &&nop, //0x27
    &&op_goto,                //0x28
    &&nop, //0x29
    &&nop, //0x2a
    &&op_packed_switch,       //0x2b
    &&nop, //0x2c
    &&nop, //0x2d
    &&nop, //0x2e
    &&nop, //0x2f
    &&nop, //0x30

    &&op_cmp_long,            //0x31
    &&op_if_eq,               //0x32
    &&op_if_ne,               //0x33
    &&op_if_lt,               //0x34
    &&op_if_ge,               //0x35
    &&op_if_gt,               //0x36
    &&op_if_le,               //0x37
    &&op_if_eqz,              //0x38
    &&op_if_nez,              //0x39
    &&op_if_ltz,              //0x3a
    &&op_if_gez,              //0x3b
    &&op_if_gtz,              //0x3c
    &&op_if_lez,              //0x3d
    &&nop, //0x3e
    &&nop, //0x3f
    &&nop, //0x40
    &&nop, //0x41
    &&nop, //0x42
    &&nop, //0x43

    &&op_aget,                //0x44
    &&nop, //0x45
    &&op_aget,                //0x46
    &&nop, //0x47
    &&nop, //0x48
    &&nop, //0x49
    &&nop, //0x4a

    &&op_aput,                //0x4b
    &&nop, //0x4c
    &&nop, //0x4d
    &&nop, //0x4e
    &&nop, //0x4f

    &&nop, //0x50
    &&nop, //0x51

    &&op_iget,               //0x52
    &&op_iget_wide,          //0x53
    &&op_iget,               //0x54
    &&nop, //0x55
    &&nop, //0x56
    &&nop, //0x57
    &&nop, //0x58

    &&op_iput,               //0x59
    &&op_iput_wide,          //0x5a
    &&op_iput,               //0x5b
    &&nop, //0x5c
    &&nop, //0x5d
    &&nop, //0x5e
    &&nop, //0x5f

    &&op_sget_object,        //0x60
    &&nop, //0x61

    &&op_sget_object,        //0x62
    &&op_sget_object,        //0x63
    &&nop, //0x64

    &&op_sget_object,        //0x65
    &&nop, //0x66
    &&op_sput_object,        //0x67
    &&nop, //0x68
    &&op_sput_object,        //0x69
    &&op_sput_object,        //0x6a
    &&nop, //0x6b

    &&op_sput_object,        //0x6c
    &&nop, //0x6d
    &&op_invoke_virtual,     //0x6e
    &&nop, //0x6f

    &&op_invoke_direct,      //0x70
    &&op_invoke_static,      //0x71
    &&nop, //0x72
    &&nop, //0x73
    &&nop, //0x74
    &&nop, //0x75
    &&nop, //0x76
    &&nop, //0x77
    &&nop, //0x78
    &&nop, //0x79
    &&nop, //0x7a
    &&nop, //0x7b
    &&nop, //0x7c
    &&nop, //0x7d
    &&nop, //0x7e
    &&nop, //0x7f


    &&nop, //0x80
    &&op_int_to_long,        //0x81
    &&nop, //0x82
    &&nop, //0x83
    &&op_long_to_int,        //0x84
    &&nop,                   //0x85
    &&nop,                   //0x86
    &&nop,                   //0x87
    &&nop,                   //0x88
    &&nop,                   //0x89
    &&op_double_to_int,      //0x8a
    &&nop, //0x8b
    &&nop, //0x8c
    &&nop, //0x8d
    &&op_int_to_char,        //0x8e
    &&nop, //0x8f

    &&op_add_int,            //0x90
    &&op_sub_int,            //0x91
    &&op_mul_int,            //0x92
    &&op_div_int,            //0x93
    &&nop, //0x94
    &&nop, //0x95
    &&nop, //0x96
    &&nop, //0x97
    &&nop, //0x98
    &&nop, //0x99
    &&nop, //0x9a
    &&op_add_long,            //0x9b
    &&op_sub_long,            //0x9c
    &&op_mul_long,            //0x9d
    &&op_div_long,            //0x9e
    &&nop, //0x9f

    &&nop, //0xa0
    &&nop, //0xa1
    &&nop, //0xa2
    &&nop, //0xa3
    &&nop, //0xa4
    &&nop, //0xa5
    &&nop, //0xa6
    &&nop, //0xa7
    &&nop, //0xa8
    &&nop, //0xa9
    &&nop, //0xaa
    &&nop, //0xab
    &&nop, //0xac
    &&nop, //0xad
    &&nop, //0xae
    &&nop, //0xaf

    &&op_add_int_2addr,        //0xb0
    &&op_sub_int_2addr,        //0xb1
    &&nop, //0xb2
    &&nop, //0xb3
    &&nop, //0xb4
    &&nop, //0xb5
    &&nop, //0xb6
    &&nop, //0xb7
    &&nop, //0xb8
    &&nop, //0xb9
    &&nop, //0xba

    &&op_add_long_2addr,       //0xbb
    &&op_sub_long_2addr,       //0xbc
    &&op_mul_long_2addr,       //0xbd
    &&nop, //0xbe
    &&nop, //0xbf

    &&nop, //0xc0
    &&nop, //0xc1
    &&nop, //0xc2
    &&nop, //0xc3
    &&nop, //0xc4
    &&nop, //0xc5
    &&nop, //0xc6
    &&nop, //0xc7
    &&nop, //0xc8
    &&nop, //0xc9
    &&nop, //0xca
    &&op_add_double_2addr,      //0xcb
    &&nop, //0xcc
    &&op_mul_double_2addr,      //0xcd
    &&nop, //0xce
    &&nop, //0xcf

    &&nop, //0xd0
    &&nop, //0xd1
    &&nop, //0xd2
    &&nop, //0xd3
    &&nop, //0xd4
    &&nop, //0xd5
    &&nop, //0xd6
    &&nop, //0xd7
    &&op_add_int_lit8,          //0xd8
    &&op_sub_int_lit8,          //0xd9
    &&op_mul_int_lit8,          //0xda
    &&op_div_int_lit8           //0xdb
};

goto NEXT_OP;

nop:
{
    printRegs(vm);
    printf("Unknow OpCode =%02x \n", ptr[*pc]);
    exit(0);
}

op_move_result:
{
    int reg_idx_vx = 0;
    reg_idx_vx = ptr[*pc + 1];
#ifdef debug
    if (is_verbose())
        printf("move-result v%d\n", reg_idx_vx);
#endif
    move_bottom_half_result_to_reg(vm, reg_idx_vx);
    *pc = *pc + 2;
    goto NEXT_OP;
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
op_move_result_wide:
{
    int reg_idx_vx = 0;
    int reg_idx_vy = 0;
    reg_idx_vx = ptr[*pc + 1];
    reg_idx_vy = reg_idx_vx + 1;
#ifdef debug
    if (is_verbose())
        printf("move-result-wide v%d,v%d\n", reg_idx_vx, reg_idx_vy);
#endif
    move_bottom_half_result_to_reg(vm, reg_idx_vx);
    move_top_half_result_to_reg(vm, reg_idx_vy);
    *pc = *pc + 2;
    goto NEXT_OP;
}

/* 0x0c, move-result-object vx
 *
 * Move the result object reference of
 * the previous method invocation into vx.
 *
 * 0C00 - move-result-object v0
 */

op_move:
{
    int reg_idx_vx = ptr[*pc + 1] & 0x0F;
    int reg_idx_vy = (ptr[*pc + 1]>>4) & 0x0F;
#ifdef debug
    if (is_verbose())
        printf("move v%d,v%d\n", reg_idx_vx, reg_idx_vy);
#endif
    simple_dvm_register *rx = &vm->regs[reg_idx_vx];
    simple_dvm_register *ry = &vm->regs[reg_idx_vy];
    rx->data[0] = ry->data[0]; 
    rx->data[1] = ry->data[1]; 
    rx->data[2] = ry->data[2]; 
    rx->data[3] = ry->data[3]; 

    *pc = *pc + 2;
    goto NEXT_OP;
}

op_move_result_object:
{
    int reg_idx_vx = 0;
    reg_idx_vx = ptr[*pc + 1];
#ifdef debug
    if (is_verbose())
        printf("move-result-object v%d\n", reg_idx_vx);
#endif
    move_bottom_half_result_to_reg(vm, reg_idx_vx);
    *pc = *pc + 2;
    goto NEXT_OP;
    
}

/* 0x0e , return-void
 * Return without a return value
 * 0E00 - return-void
 */
op_return_void:
{
#ifdef debug
    if (is_verbose())
        printf("return-void\n");
#endif
    *pc = 100000;
    return; 
}

/* 0x0f , return-vx
 * Return with vx return value
 * 0F00 - return-v0
 */
op_return:
{
    int reg_idx_vx = 0;
    reg_idx_vx = ptr[*pc + 1];

#ifdef debug
    if (is_verbose())
        printf("return v%d\n",reg_idx_vx);
#endif
    move_reg_to_bottom_result(vm, reg_idx_vx);
    *pc = 1000000;
    return; 
}


/* 0x12, const/4 vx,lit4
 * Puts the 4 bit constant into vx
 * 1221 - const/4 v1, #int2
 * Moves literal 2 into v1.
 * The destination register is in the lower 4 bit
 * in the second byte, the literal 2 is in the higher 4 bit.
 */
op_const_4:
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
#ifdef debug
    if (is_verbose())
        printf("const/4 v%d, #int%d\n", reg_idx_vx , value);
#endif
    *pc = *pc + 2;
    goto NEXT_OP;
    
}

/* 0x13, const/16 vx,lit16
 * Puts the 16 bit constant into vx
 * 1300 0A00 - const/16 v0, #int 10
 * Puts the literal constant of 10 into v0.
 */
op_const_16:
{
    int reg_idx_vx = 0;
    int value = 0;
    reg_idx_vx = ptr[*pc + 1];
    value = (ptr[*pc + 3] << 8 | ptr[*pc + 2]);

    store_to_reg(vm, reg_idx_vx, (unsigned char *) &value);
#ifdef debug
    if (is_verbose())
        printf("const/16 v%d, #int%d\n", reg_idx_vx, value);
#endif
    *pc = *pc + 4;
    goto NEXT_OP;
    
}

/* 0x19, const-wide/high16 vx,lit16
 * Puts the 16 bit constant into the highest 16 bit of vx
 * and vx+1 registers.
 * Used to initialize double values.
 * 1900 2440 - const-wide/high16 v0, #double 10.0 // #402400000
 * Puts the double constant of 10.0 into v0 register.
 */
op_const_wide_high16:
{
    long long value = 0L;
    unsigned char *ptr2 = (unsigned char *) &value;
    int reg_idx_vx = 0;
    reg_idx_vx = ptr[*pc + 1];
    ptr2[1] = ptr[*pc + 3];
    ptr2[0] = ptr[*pc + 2];
#ifdef debug
    if (is_verbose())
        printf("const-wide/hight16 v%d, #long %lld\n", reg_idx_vx, value);
#endif
    store_to_reg(vm, reg_idx_vx, ptr2+4);
    store_to_reg(vm, reg_idx_vx+1, ptr2);
    value = 1L;
    *pc = *pc + 4;
    goto NEXT_OP;
    
}

/* 0x17, const-wide/32 vx, lit32
 * Puts the 32 bit constant into vx and vx+1 registers, expanding the integer constant into a long constant.
 * 1702 4e61 bc00 - const-wide/32 v2, #long 12345678 // #00bc614e
 * Puts #12345678 into v2 and v3 registers.
*/
op_const_wide_32:
{
    long long value = 0L;
    unsigned char *ptr2 = (unsigned char *) &value;
    int reg_idx_vx = 0;
    reg_idx_vx = ptr[*pc + 1];
    ptr2[3] = ptr[*pc + 5];
    ptr2[2] = ptr[*pc + 4];
    ptr2[1] = ptr[*pc + 3];
    ptr2[0] = ptr[*pc + 2];

#ifdef debug
    if (is_verbose())
        printf("const-wide/32 v%d, #%02x%02x %02x%02x\n", reg_idx_vx, ptr2[3], ptr2[2], ptr2[1], ptr2[0]);
#endif
    store_double_to_reg(vm, reg_idx_vx  , ptr2+4);
    store_double_to_reg(vm, reg_idx_vx+1, ptr2);
    *pc = *pc + 6;
    goto NEXT_OP;
    
}

op_const_wide_16:
{
    long long value = 0L;
    unsigned char *ptr2 = (unsigned char *) &value;
    int reg_idx_vx = 0;
    reg_idx_vx = ptr[*pc + 1];
    ptr2[1] = ptr[*pc + 3];
    ptr2[0] = ptr[*pc + 2];

#ifdef debug
    if (is_verbose())
        printf("const-wide/16 v%d, #%02x%02x\n", reg_idx_vx, ptr2[1], ptr2[0]);
#endif
    store_double_to_reg(vm, reg_idx_vx  , ptr2+4);
    store_double_to_reg(vm, reg_idx_vx+1, ptr2);
    *pc = *pc + 4;
    goto NEXT_OP;
    
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

op_check_cast:
{
    int reg_idx_vx = 0;
    int object_reference = 0;
    int type_id = 0;
    int s_type_idx = 0;
    reg_idx_vx = ptr[*pc + 1];
    type_id = ((ptr[*pc + 3] << 8) | ptr[*pc + 2]);

#ifdef debug
    if (is_verbose())
        printf("check-cast v%d, type_id 0x%04x\n", reg_idx_vx , type_id);
#endif
    load_reg_to(vm, reg_idx_vx, (unsigned char *) &object_reference);
    
    if(object_reference >= 8192){
        object_reference -= 8192;
        s_type_idx = (vm->array[object_reference])->type_idx;
    }
    
    //s = get_field_item(dex,source_field_id);
    if(s_type_idx == type_id) {
        *pc = *pc + 4;
        goto NEXT_OP;
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
op_const_string:
{
    int reg_idx_vx = 0;
    int string_id = 0;
    reg_idx_vx = ptr[*pc + 1];
    string_id = ((ptr[*pc + 3] << 8) | ptr[*pc + 2]);

#ifdef debug
    if (is_verbose())
        printf("const-string v%d, string_id 0x%04x\n",
               reg_idx_vx , string_id);
#endif
    store_to_reg(vm, reg_idx_vx, (unsigned char *) &string_id);
    *pc = *pc + 4;
    goto NEXT_OP;
    
}

/* 0x22 new-instance vx,type
 * Instantiates an object type and puts
 * the reference of the newly created instance into vx
 * 2200 1500 - new-instance v0, java.io.FileInputStream // type@0015
 * Instantiates type@0015 (entry #15H in the type table)
 * and puts its reference into v0.
 */
op_new_instance:
{
    int reg_idx_vx = 0;
    int type_id = 0;
    type_id_item *type_item = 0;

    reg_idx_vx = ptr[*pc + 1];
    type_id = ((ptr[*pc + 3] << 8) | ptr[*pc + 2]);

    type_item = get_type_item(dex, type_id);

#ifdef debug
    if (is_verbose()) {
        printf("new-instance v%d, type_id 0x%04x", reg_idx_vx, type_id);
        if (type_item != 0) {
            printf(" %s", get_string_data(dex, type_item->descriptor_idx));
        }
        printf("\n");
    }
#endif
    int obj_ref = create_instance(dex, vm, type_id);
    store_to_reg(vm, reg_idx_vx, (unsigned char*)&obj_ref);
    *pc = *pc + 4;
    goto NEXT_OP;
    
}

/* 0x23 new-array vy, vx,type_id
 * Generates a new array of type_id type and vy element
 * size and puts the reference to the array into vx.
 *
 * 2312 2500 - new-array v2, v1, char[] // type@0025
 * Generates a new array of type@0025 type and v1 size
 * and puts the reference to the new array into v2.
 */
op_new_array:
{
    int reg_idx_vx = 0;
    int reg_idx_vy = 0;
    int type_id = 0, size = 0;
    type_id_item *type_item = 0;
    reg_idx_vx = ptr[*pc + 1] & 0x0F ;
    reg_idx_vy = (ptr[*pc + 1] >> 4) & 0x0F ;

    type_id = ((ptr[*pc + 3] << 8) | ptr[*pc + 2]);

    type_item = get_type_item(dex, type_id);

#ifdef debug
    if (is_verbose()) {
        printf("new-array v%d, v%d ,type_id 0x%04x", reg_idx_vx, reg_idx_vy, type_id);
        if (type_item != 0) {
            printf(" %s", get_string_data(dex, type_item->descriptor_idx));
        }
        printf("\n");
    }
#endif
    load_reg_to(vm, reg_idx_vy, (unsigned char*)&size);
    int arr_ref = create_array(dex, vm, type_id, size);
    store_to_reg(vm, reg_idx_vx, (unsigned char*)&arr_ref);
    *pc = *pc + 4;
    goto NEXT_OP;
    
}

/*Not sure implementation is correct or not*/
op_filled_new_array:
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

#ifdef debug
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
#endif
    load_reg_to(vm, p1, (unsigned char*)&size);
    int ref = create_array_filled(dex, vm, type_id, size, content);
    move_int_to_bottom_result(vm,ref);
    *pc = *pc + 6;
    goto NEXT_OP;
    
}





/* invoke-virtual { parameters }, methodtocall */
/*
 * 6E53 0600 0421 - invoke-virtual { v4, v0, v1, v2, v3}, Test2.method5:(IIII)V // method@0006
 * 6e20 0200 3200   invoke-virtual {v2, v3}, Ljava/io/PrintStream;.println:(Ljava/lang/String;)V // method@0002
 */
op_invoke_virtual:
{
    int pc_current = *pc;
    int object_id, class_id;
    int method_id = ((ptr[*pc + 3] << 8) | ptr[*pc + 2]);
    int p1 = ptr[*pc + 4] & 0x0F ;
    load_reg_to(vm, p1, (unsigned char*)&object_id);
    op_utils_invoke_35c_parse(dex, ptr, pc, &vm->p);
    if(object_id > vm->object_length){
        if(!op_utils_invoke("invoke-virtual", dex, vm, &vm->p) && object_id >= 8192)
            invoke_method_entry(dex,vm, dex->string_data_item[dex->method_id_item[method_id].name_idx].data,0);
        *pc = pc_current + 6;
        goto NEXT_OP;
        
    }
    //printf("=======invoke-virtual(%d) %s\n",object_id,dex->string_data_item[dex->method_id_item[method_id].name_idx].data);
    //printf("classId = %d, methodId = %d, (v%d)object_id = %d\n",class_id,method_id,p1,object_id);
    class_id = vm->object[object_id]->class_idx;
    invoke_virtual_method(dex, vm, class_id, method_id);
    *pc = pc_current + 6;
    goto NEXT_OP;
    
}

/* invoke-direct
 * 7010 0400 0300  invoke-direct {v3}, Ljava/lang/StringBuilder;.<init>:()V // method@0004
 */
op_invoke_direct:
{
    int pc_current = *pc;
    op_utils_invoke_35c_parse(dex, ptr, pc, &vm->p);

    op_utils_invoke_35c_parse(dex, ptr, pc, &vm->p);
    method_id_item *m = get_method_item(dex, vm->p.method_id);
    if(!op_utils_invoke("invoke-direct", dex, vm, &vm->p))
        invoke_method_entry(dex,vm, get_string_data(dex, m->name_idx),1);
    //op_utils_invoke("invoke-direct", dex, vm, &vm->p);
    *pc = pc_current + 6;
    goto NEXT_OP;
    
}

/* 0x71 invoke-direct
 * 7100 0300 0000  invoke-static {}, Ljava/lang/Math;.random:()D // method@0003
 */
op_invoke_static:
{
    int pc_current = *pc;
    op_utils_invoke_35c_parse(dex, ptr, pc, &vm->p);

    method_id_item *m = get_method_item(dex, vm->p.method_id);
    if(!op_utils_invoke("invoke-static", dex, vm, &vm->p))
        invoke_method_entry(dex,vm, get_string_data(dex, m->name_idx),1);

    *pc = pc_current + 6;
    goto NEXT_OP;
    
}

/* 0x62 sget-object vx,field_id
 * Reads the object reference field identified by the field_id into vx.
 * 6201 0C00 - sget-object v1, Test3.os1:Ljava/lang/Object; // field@000c
 * Reads field@000c (entry #CH in the field id table) into v1.
 */
op_sget_object:
{
    int field_id = 0;
    int reg_idx_vx = 0;
    reg_idx_vx = ptr[*pc + 1];
    field_id = ((ptr[*pc + 3] << 8) | ptr[*pc + 2]);

#ifdef debug
    if (is_verbose()) {
        printf("sget-object v%d, field 0x%04x\n", reg_idx_vx, field_id);
    }
#endif
    store_to_reg(vm, reg_idx_vx, vm->static_field_value[field_id].data);
    /* TODO */
    *pc = *pc + 4;
    goto NEXT_OP;
    
}

/* 0x90 add-int vx,vy vz
 * Calculates vy+vz and puts the result into vx.
 * 9000 0203 - add-int v0, v2, v3
 * Adds v3 to v2 and puts the result into v0.
 */
op_add_int:
{
    int reg_idx_vx = 0;
    int reg_idx_vy = 0;
    int reg_idx_vz = 0;
    int x = 0, y = 0 , z = 0;
    reg_idx_vx = ptr[*pc + 1];
    reg_idx_vy = ptr[*pc + 2];
    reg_idx_vz = ptr[*pc + 3];

#ifdef debug
    if (is_verbose())
        printf("add-int v%d, v%d, v%d\n", reg_idx_vx, reg_idx_vy,
               reg_idx_vz);
#endif
    /* x = y + z */
    load_reg_to(vm, reg_idx_vy, (unsigned char *) &y);
    load_reg_to(vm, reg_idx_vz, (unsigned char *) &z);
    x = y + z;
    printf(" x = %d, y = %d, z = %d\n",x,y,z);
    store_to_reg(vm, reg_idx_vx, (unsigned char *) &x);
    *pc = *pc + 4;
    goto NEXT_OP;
    

}

/* 0x91 sub-int vx,vy,vz
 * Calculates vy-vz and puts the result into vx.
 * 9100 0203 - sub-int v0, v2, v3
 * Subtracts v3 from v2 and puts the result into v0.
 */
op_sub_int:
{
    int reg_idx_vx = 0;
    int reg_idx_vy = 0;
    int reg_idx_vz = 0;
    int x = 0, y = 0 , z = 0;
    reg_idx_vx = ptr[*pc + 1];
    reg_idx_vy = ptr[*pc + 2];
    reg_idx_vz = ptr[*pc + 3];

#ifdef debug
    if (is_verbose())
        printf("sub-int v%d, v%d, v%d\n", reg_idx_vx, reg_idx_vz,
               reg_idx_vy);
#endif
    /* x = y + z */
    load_reg_to(vm, reg_idx_vy, (unsigned char *) &y);
    load_reg_to(vm, reg_idx_vz, (unsigned char *) &z);
    x = y - z;
    store_to_reg(vm, reg_idx_vx, (unsigned char *) &x);
    *pc = *pc + 4;
    goto NEXT_OP;
    
}

/* 0x92 mul-int vx, vy, vz
 * Multiplies vz with wy and puts the result int vx.
 * 9200 0203 - mul-int v0,v2,v3
 * Multiplies v2 with w3 and puts the result into v0
 */
op_mul_int:
{
    int reg_idx_vx = 0;
    int reg_idx_vy = 0;
    int reg_idx_vz = 0;
    int x = 0, y = 0 , z = 0;
    reg_idx_vx = ptr[*pc + 1];
    reg_idx_vy = ptr[*pc + 2];
    reg_idx_vz = ptr[*pc + 3];

#ifdef debug
    if (is_verbose())
        printf("mul-int v%d, v%d, v%d\n", reg_idx_vx, reg_idx_vy, reg_idx_vz);
#endif
    /* x = y + z */
    load_reg_to(vm, reg_idx_vy, (unsigned char *) &y);
    load_reg_to(vm, reg_idx_vz, (unsigned char *) &z);
    x = y * z;
    store_to_reg(vm, reg_idx_vx, (unsigned char *) &x);
    *pc = *pc + 4;
    goto NEXT_OP;
    
}

/* 0x93 div-int vx,vy,vz
 * Divides vy with vz and puts the result into vx.
 * 9303 0001 - div-int v3, v0, v1
 * Divides v0 with v1 and puts the result into v3.
 */
op_div_int:
{
    int reg_idx_vx = 0;
    int reg_idx_vy = 0;
    int reg_idx_vz = 0;
    int x = 0, y = 0 , z = 0;
    reg_idx_vx = ptr[*pc + 1];
    reg_idx_vy = ptr[*pc + 2];
    reg_idx_vz = ptr[*pc + 3];

#ifdef debug
    if (is_verbose())
        printf("div-int v%d, v%d, v%d\n", reg_idx_vx, reg_idx_vy, reg_idx_vz);
#endif
    /* x = y + z */
    load_reg_to(vm, reg_idx_vy, (unsigned char *) &y);
    load_reg_to(vm, reg_idx_vz, (unsigned char *) &z);
    x = y % z;
    x = (y - x) / z;
    store_to_reg(vm, reg_idx_vx, (unsigned char *) &x);
    *pc = *pc + 4;
    goto NEXT_OP;
    

}

/* 0x8A double-to-int vx, vy
 * Converts the double value in vy,vy+1 into an integer value in vx.
 * 8A40  - double-to-int v0, v4
 * Converts the double value in v4,v5 into an integer value in v0.
 */
op_double_to_int:
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
#ifdef debug
    if (is_verbose()) {
        printf("double-to-int v%d, v%d\n", reg_idx_vx, reg_idx_vy);
        printf("(%f) to (%d) \n", d , i);
    }
#endif

    store_to_reg(vm, reg_idx_vx, (unsigned char *) &i);
    *pc = *pc + 2;
    goto NEXT_OP;
    
}

/* 0xb0 add-int/2addr vx,vy
 * Adds vy to vx.
 * B010 - add-int/2addr v0,v1 Adds v1 to v0.
 */
op_add_int_2addr:
{
    int reg_idx_vx = 0;
    int reg_idx_vy = 0;
    int x = 0, y = 0;
    reg_idx_vx = ptr[*pc + 1] & 0x0F ;
    reg_idx_vy = (ptr[*pc + 1] >> 4) & 0x0F ;
#ifdef debug
    if (is_verbose())
        printf("add-int/2addr v%d, v%d\n", reg_idx_vx, reg_idx_vy);
#endif
    load_reg_to(vm, reg_idx_vx, (unsigned char *) &x);
    load_reg_to(vm, reg_idx_vy, (unsigned char *) &y);
    x = x + y;
    store_to_reg(vm, reg_idx_vx, (unsigned char *) &x);

    *pc = *pc + 2;
    goto NEXT_OP;
    
}

/* 0xb1 sub-int/2addr vx,vy
 * Subtracts vy to vx.
 * B110 - add-int/2addr v0,v1 subtracts v1 to v0.
 */
op_sub_int_2addr:
{
    int reg_idx_vx = 0;
    int reg_idx_vy = 0;
    int x = 0, y = 0;
    reg_idx_vx = ptr[*pc + 1] & 0x0F ;
    reg_idx_vy = (ptr[*pc + 1] >> 4) & 0x0F ;
#ifdef debug
    if (is_verbose())
        printf("add-int/2addr v%d, v%d\n", reg_idx_vx, reg_idx_vy);
#endif
    load_reg_to(vm, reg_idx_vx, (unsigned char *) &x);
    load_reg_to(vm, reg_idx_vy, (unsigned char *) &y);
    x = x - y;
    store_to_reg(vm, reg_idx_vx, (unsigned char *) &x);

    *pc = *pc + 2;
    goto NEXT_OP;
    
}


/* 0xcb , add-double/2addr
 * Adds vy to vx.
 * CB70 - add-double/2addr v0, v7
 * Adds v7 to v0.
 */
op_add_double_2addr:
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


#ifdef debug
    if (is_verbose()) {
        printf("add-double/2addr v%d, v%d\n", reg_idx_vx, reg_idx_vy);
        printf("%f(%llx) + %f(%llx) = %f\n", x, (long long unsigned int)x, y,(long long unsigned int) y , y + x);
    }
#endif
    x = x + y;
    store_double_to_reg(vm, reg_idx_vx, ptr_x + 4);
    store_double_to_reg(vm, reg_idx_vx + 1, ptr_x);
    *pc = *pc + 2;
    goto NEXT_OP;
    
}

/* 0xcd , mul-double/2addr
 * Multiplies vx with vy
 * CD20 - mul-double/2addr v0, v2
 * Multiplies the double value in v0,v1 with the
 * double value in v2,v3 and puts the result into v0,v1.
 */
op_mul_double_2addr:
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

#ifdef debug
    if (is_verbose()) {
        printf("mul-double/2addr v%d, v%d\n", reg_idx_vx, reg_idx_vz);
        printf(" %f * %f = %f\n", x, y, x * y);
    }
#endif

    x = x * y;

    store_double_to_reg(vm, reg_idx_vx, ptr_x + 4);
    store_double_to_reg(vm, reg_idx_vy, ptr_x);

    load_reg_to_double(vm, reg_idx_vx, ptr_y + 4);
    load_reg_to_double(vm, reg_idx_vy, ptr_y);

    *pc = *pc + 2;
    goto NEXT_OP;
    
}

op_goto:{
    char offset = (char) ptr[*pc + 1];
#ifdef debug
    if(is_verbose())
        printf("goto %#x\n",(int)offset);
#endif
    *pc = *pc + offset * 2;
    goto NEXT_OP;
    
}

/* 0x31 cmp-long vx, vy, vz
 * Compares the long values in vy and vz and sets the integer value in vx accordingly.
 * 3100 0204 - cmp-long v0, v2, v4
 * Compares the long values in v2 and v4 then sets v0 accordingly.
 */
op_cmp_long:
{
  int reg_idx_vx = 0;
  int reg_idx_vy = 0;
  int reg_idx_vz = 0;
  reg_idx_vx = ptr[*pc + 1];
  reg_idx_vy = ptr[*pc + 2];
  reg_idx_vz = ptr[*pc + 3];

  int vx = 0;
  long vy, vz;
  unsigned char *ptr_x = (unsigned char *) &vx;
  unsigned char *ptr_y = (unsigned char *) &vy;
  unsigned char *ptr_z = (unsigned char *) &vz;
  load_reg_to_double(vm, reg_idx_vz, (unsigned char *) ptr_z+4);
  load_reg_to_double(vm, reg_idx_vz+1, (unsigned char *) ptr_z);
  load_reg_to_double(vm, reg_idx_vy, (unsigned char *) ptr_y+4);
  load_reg_to_double(vm, reg_idx_vy+1, (unsigned char *) ptr_y);

#ifdef debug
  if (is_verbose()) {
    printf("cmp-long v%d, v%d (%ld), v%d(%ld)\n", reg_idx_vx, reg_idx_vy, vy, reg_idx_vz, vz);
  }
#endif

  if (vy == vz) {
    vx = 1;
  } else{
    vx = 0;
  }
  store_to_reg(vm, reg_idx_vx, ptr_x);
  *pc = *pc + 4;
    goto NEXT_OP;
  
}

/* 0x32 if-eq
 * Jumps to target if vx==vy. vx and vy are integer values.
 * 32b3 6600 - if-eq v3, v11, 0080 // +0066
 * Jumps to the current position+66H words if v3==v11. 0080 is the label of the target instruction.
 */
op_if_eq:
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

#ifdef debug
  if (is_verbose()) {
    printf("if-get v%d (%d), v%d (%d), %d\n", reg_idx_vx, vx, reg_idx_vy, vy, jumpOffset);
  }
#endif

  if (vx == vy) {
    *pc = *pc + jumpOffset*2;
  } else {
    *pc = *pc + 4;
  }
    goto NEXT_OP;
  
}

/* 0x33 if-ne
 * Jumps to target if vx!=vy. vx and vy are integer values.
 * 33A3 1000 - if-ne v3, v10, 002c // +0010
 * Jumps to the current position+10H words if v3!=v10.
 * 002c is the label of the target instruction.
 */
op_if_ne:
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

#ifdef debug
  if (is_verbose()) {
    printf("if-get v%d (%d), v%d (%d), %d\n", reg_idx_vx, vx, reg_idx_vy, vy, jumpOffset);
  }
#endif

  if (vx != vy) {
    *pc = *pc + jumpOffset*2;
  } else {
    *pc = *pc + 4;
  }
    goto NEXT_OP;
  
}

/* 0x34 if-lt
 * Jumps to target is vx<vy. vx and vy are integer values.
 * 3432 CBFF - if-lt v2, v3, 0023 // -0035
 * Jumps to the current position-35H words if v2<v3.
 * 0023 is the label of the target instruction.
 */
op_if_lt:
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

#ifdef debug
  if (is_verbose()) {
    printf("if-get v%d (%d), v%d (%d), %d\n", reg_idx_vx, vx, reg_idx_vy, vy, jumpOffset);
  }
#endif

  if (vx < vy) {
    *pc = *pc + jumpOffset*2;
  } else {
    *pc = *pc + 4;
  }
    goto NEXT_OP;
  
}

op_if_ge:{
    int reg_idx_vx = ptr[*pc + 1] & 0x0F;
    int reg_idx_vy = (ptr[*pc + 1]>>4) & 0x0F;
    int offset = (int) ptr[*pc + 2] + (int) ptr[*pc + 3] * (int) 256;
    int x,y;
    load_reg_to(vm, reg_idx_vy, (unsigned char*) &y);
    load_reg_to(vm, reg_idx_vx, (unsigned char*) &x);
#ifdef debug
    if(is_verbose()){
        printf("if-ge v[%d]:%d, v[%d]:%d, off:%#x\n",reg_idx_vy,y,reg_idx_vx,x,offset);
    }
#endif
    if(x >= y)
        *pc = *pc + offset * 2;
    else
        *pc = *pc + 4;
    goto NEXT_OP;
    
}

/* 0x36 if-gt
 * 3610 1B00 - if-ge v0, v1, 002b // +001b
 * Jumps to the current position+1BH words if v0>v1.
 * 002b is the label of the target instruction.
 */
op_if_gt:
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

#ifdef debug
  if (is_verbose()) {
    printf("if-get v%d (%d), v%d (%d), %d\n", reg_idx_vx, vx, reg_idx_vy, vy, jumpOffset);
  }
#endif

  if (vx > vy) {
    *pc = *pc + jumpOffset*2;
  } else {
    *pc = *pc + 4;
  }
    goto NEXT_OP;
  
}

/* 0x37 if-le
 * Jumps to target is vx<=vy. vx and vy are integer values.
 * 3756 0B00 - if-le v6, v5, 0144 // +000b
 * Jumps to the current position+0BH words if v6<=v5.
 * 0144 is the label of the target instruction.
 */

op_if_le:
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

#ifdef debug
  if (is_verbose()) {
    printf("if-get v%d (%d), v%d (%d), %d\n", reg_idx_vx, vx, reg_idx_vy, vy, jumpOffset);
  }
#endif

  if (vx <= vy) {
    *pc = *pc + jumpOffset*2;
  } else {
    *pc = *pc + 4;
  }
    goto NEXT_OP;
  
}

/* 0x38 if-eqz
 * Jumps to target if vx==0, vx is an integer value.
 * 3802 1900 - if-eqz v2, 0038 // +0019
 * Jumps to the current position+19H words if v2==0.
 * 0038 is the label of the target instruction.
 */

op_if_eqz:
{
  int reg_idx_vx = 0;
  int jumpOffset = 0;

  reg_idx_vx = ptr[*pc + 1] & 0x0F;
  jumpOffset = (int)(ptr[*pc + 3] << 8 | ptr[*pc + 2]);

  int vx;
  load_reg_to(vm, reg_idx_vx, (unsigned char *) &vx);

#ifdef debug
  if (is_verbose()) {
    printf("if-eqz v%d (%d), %d\n", reg_idx_vx, vx, jumpOffset);
  }
#endif

  if (vx == 0) {
    *pc = *pc + jumpOffset*2;
  } else {
    *pc = *pc + 4;
  }
    goto NEXT_OP;
  
}

/* 0x39 if-nez
 * Checks vx and jumps if vx is nonzero2. 
 * 3902 1200 - if-nez v2, 0014 // +0012
 * Jumps to current position+18 words (hex 12) if v2 is nonzero.
 * 0014 is the label of the target instruction.
 */
op_if_nez:
{
  int reg_idx_vx = 0;
  int jumpOffset = 0;

  reg_idx_vx = ptr[*pc + 1] & 0x0F;
  jumpOffset = (int)(ptr[*pc + 3] << 8 | ptr[*pc + 2]);

  int vx;
  load_reg_to(vm, reg_idx_vx, (unsigned char *) &vx);

#ifdef debug
  if (is_verbose()) {
    printf("if-get v%d (%d), %d\n", reg_idx_vx, vx, jumpOffset);
  }
#endif

  if (vx != 0) {
    *pc = *pc + jumpOffset*2;
  } else {
    *pc = *pc + 4;
  }
    goto NEXT_OP;
  
}

/* 0x3a if-ltz
 * Checks vx and jumps if vx<0. 
 * 3A00 1600 - if-ltz v0, 002d // +0016
 * Jumps to the current position+16H words if v0<0.
 * 002d is the label of the target instruction.
 */
op_if_ltz:
{
  int reg_idx_vx = 0;
  int jumpOffset = 0;

  reg_idx_vx = ptr[*pc + 1] & 0x0F;
  jumpOffset = (int)(ptr[*pc + 3] << 8 | ptr[*pc + 2]);

  int vx;
  load_reg_to(vm, reg_idx_vx, (unsigned char *) &vx);

#ifdef debug
  if (is_verbose()) {
    printf("if-get v%d (%d), %d\n", reg_idx_vx, vx, jumpOffset);
  }
#endif

  if (vx < 0) {
    *pc = *pc + jumpOffset*2;
  } else {
    *pc = *pc + 4;
  }
    goto NEXT_OP;
  
}

/* 0x3b if-gez
 * Checks vx and jumps if vx>=0.
 * 3B00 1600 - if-gez v0, 002d // +0016
 * Jumps to the current position+16H words if v0 >=0.
 * 002d is the label of the target instruction.
 */
op_if_gez:
{
  int reg_idx_vx = 0;
  int jumpOffset = 0;

  reg_idx_vx = ptr[*pc + 1] & 0x0F;
  jumpOffset = (int)(ptr[*pc + 3] << 8 | ptr[*pc + 2]);

  int vx;
  load_reg_to(vm, reg_idx_vx, (unsigned char *) &vx);

#ifdef debug
  if (is_verbose()) {
    printf("if-get v%d (%d), %d\n", reg_idx_vx, vx, jumpOffset);
  }
#endif

  if (vx >= 0) {
    *pc = *pc + jumpOffset*2;
  } else {
    *pc = *pc + 4;
  }
    goto NEXT_OP;
  
}

/* 0x3c if-gtz
 * Checks vx and jumps if vx>02.
 * 3C00 1D00 - if-gtz v0, 004a // +001d
 * Jumps to the current position+1DH words if v0>0.
 * 004A is the label of the target instruction. 
 */
op_if_gtz:
{
  int reg_idx_vx = 0;
  int jumpOffset = 0;

  reg_idx_vx = ptr[*pc + 1] & 0x0F;
  jumpOffset = (int)(ptr[*pc + 3] << 8 | ptr[*pc + 2]);

  int vx;
  load_reg_to(vm, reg_idx_vx, (unsigned char *) &vx); 

#ifdef debug
  if (is_verbose()) {
    printf("if-getz v%d (%d), %d\n", reg_idx_vx, vx, jumpOffset);
  }
#endif

  if (vx > 0) {
    *pc = *pc + jumpOffset*2;
  } else {
    *pc = *pc + 4;
  }
    goto NEXT_OP;
  
}

/* 0x3d if-lez
 * Checks vx and jumps if vx<=0. 
 * 3D00 1D00 - if-lez v0, 004a // +001d
 * Jumps to the current position+1DH words if v0<=0.
 * 004A is the label of the target instruction.
 */
op_if_lez:
{
  int reg_idx_vx = 0;
  int jumpOffset = 0;

  reg_idx_vx = ptr[*pc + 1] & 0x0F;
  jumpOffset = (int)(ptr[*pc + 3] << 8 | ptr[*pc + 2]);

  int vx;
  load_reg_to(vm, reg_idx_vx, (unsigned char *) &vx);

#ifdef debug
  if (is_verbose()) {
    printf("if-get v%d (%d), %d\n", reg_idx_vx, vx, jumpOffset);
  }
#endif

  if (vx <= 0) {
    *pc = *pc + jumpOffset*2;
  } else {
    *pc = *pc + 4;
  }
    goto NEXT_OP;
  
}


op_long_to_int:{
    int reg_idx_vx = ptr[*pc + 1] & 0x0F;
    int reg_idx_vy = (ptr[*pc + 1]>>4) & 0x0F;
    int reg_idx_vz = reg_idx_vy + 1;
    long value = 0;
    unsigned char *ptr_value = (unsigned char*) &value;
    load_reg_to_long(vm, reg_idx_vy, ptr_value + 4);
    load_reg_to_long(vm, reg_idx_vz, ptr_value);
    int value_to = (int)value;
    store_to_reg(vm, reg_idx_vx, (unsigned char*)&value_to);
#ifdef debug
    if(is_verbose()){
        printf("long to int v%d v%d, %ld -> %d\n",reg_idx_vx,reg_idx_vy,value,value_to);
    }
#endif
    *pc = *pc + 2;
    goto NEXT_OP;
    
}

/* 0x81 int-to-long vx, vy
 * Converts the integer in vy into a long in vx,vx+1.
 * 8106 - int-to-long v6, v0
 * Converts an integer in v0 into a long in v6,v7.
 */

op_int_to_long:
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
#ifdef debug
  if (is_verbose()) {
    printf("int-to-long v%d, v%d\n", reg_idx_vx, reg_idx_vy);
    printf("(%d) to (%ld) \n", i , l);
  }
#endif

  store_double_to_reg(vm, reg_idx_vx, ptr_l+4);
  store_double_to_reg(vm, reg_idx_vx + 1, ptr_l);
  *pc = *pc + 2;
    goto NEXT_OP;
  
}

/* 0x8e int-to-char vx,vy
 * Converts the int value in vy to a char value and stores it in vx. 
 *
 * 8E33  - int-to-char v3, v3
 * Converts the integer in v3 into a char and puts the char value into v3.
 */
op_int_to_char:{
    int reg_idx_vx = ptr[*pc + 1] & 0x0F;
    int reg_idx_vy = (ptr[*pc + 1]>>4) & 0x0F;
    int value = 0;
    unsigned char* ptr2 = (unsigned char*) &value;
    load_reg_to(vm, reg_idx_vy, ptr2);
    value = ptr2[0];
#ifdef debug
    if (is_verbose()) {
        printf("int-to-char v%d, v%d\n", reg_idx_vy, reg_idx_vx);
    }
#endif
    store_to_reg(vm, reg_idx_vx, (unsigned char*)&value);
    *pc = *pc + 2;
    goto NEXT_OP;
    
}



op_sub_long_2addr:{
    int reg_idx_vx = ptr[*pc + 1] & 0x0F;
    int reg_idx_vy = (ptr[*pc + 1]>>4) & 0x0F;
    long value1 = 0L, value2 = 0L;
    unsigned char *ptr_value1 = (unsigned char*) &value1;
    unsigned char *ptr_value2 = (unsigned char*) &value2;
    load_reg_to_long(vm, reg_idx_vx, ptr_value1 + 4);
    load_reg_to_long(vm, reg_idx_vx+1, ptr_value1);
    load_reg_to_long(vm, reg_idx_vy, ptr_value2 + 4);
    load_reg_to_long(vm, reg_idx_vy+1, ptr_value2);
#ifdef debug
    if(is_verbose()){
        printf("sub-long-2addr v%d v%d, %ld - %ld\n",reg_idx_vx,reg_idx_vy,value1,value2);
    }
#endif
    value1 = value1 - value2;
    store_long_to_reg(vm, reg_idx_vx, ptr_value1+4);
    store_long_to_reg(vm, reg_idx_vx+1, ptr_value1);
    *pc = *pc + 2;
    goto NEXT_OP;
    
}

/* 0xbd mul-long/2addr vx,vy
 * Calculates vx*vy and puts the result into vx
 * BD70 - mul-long/2addr v0, v7
 * Multiplies the long value in v7,v8 with the long value in v0,v1 and puts the result into v0,v1. 
 */
op_mul_long_2addr:
{
    int reg_idx_vx = 0;
    int reg_idx_vy = 0;
    long x = 0, y = 0;
    reg_idx_vx = ptr[*pc + 1] & 0x0F ;
    reg_idx_vy = (ptr[*pc + 1] >> 4) & 0x0F ;
    unsigned char *ptr_x = (unsigned char *) &x;
    unsigned char *ptr_y = (unsigned char *) &y;
#ifdef debug
    if (is_verbose())
        printf("mul-long/2addr v%d, v%d\n", reg_idx_vx, reg_idx_vy);
#endif
    load_reg_to_double(vm, reg_idx_vx, ptr_x + 4);
    load_reg_to_double(vm, reg_idx_vx + 1, ptr_x);
    load_reg_to_double(vm, reg_idx_vy, ptr_y + 4);
    load_reg_to_double(vm, reg_idx_vy + 1, ptr_y);
    x = x * y;
    store_double_to_reg(vm, reg_idx_vx, (unsigned char *) (&x)+4);
    store_double_to_reg(vm, reg_idx_vx, (unsigned char *) (&x)+0);

    *pc = *pc + 2;
    goto NEXT_OP;
    
}

op_add_long_2addr:
{
    int reg_idx_vx = 0;
    int reg_idx_vy = 0;
    long x = 0, y = 0;
    reg_idx_vx = ptr[*pc + 1] & 0x0F ;
    reg_idx_vy = (ptr[*pc + 1] >> 4) & 0x0F ;
    unsigned char *ptr_x = (unsigned char *) &x;
    unsigned char *ptr_y = (unsigned char *) &y;
#ifdef debug
    if (is_verbose())
        printf("add-long/2addr v%d, v%d\n", reg_idx_vx, reg_idx_vy);
#endif
    load_reg_to_double(vm, reg_idx_vx, ptr_x + 4);
    load_reg_to_double(vm, reg_idx_vx + 1, ptr_x);
    load_reg_to_double(vm, reg_idx_vy, ptr_y + 4);
    load_reg_to_double(vm, reg_idx_vy + 1, ptr_y);
    x = x + y;
    store_double_to_reg(vm, reg_idx_vx, (unsigned char *) (&x)+4);
    store_double_to_reg(vm, reg_idx_vx + 1, (unsigned char *) (&x)+0);

    *pc = *pc + 2;
    goto NEXT_OP;
    
}


/* 0x9b add-long vx,vy,vz
 * Adds vy to vz and puts the result into vx
 * 9B00 0305 - add-long v0, v3, v5
 * The long value in v3,v4 is added to the value in v5,v6 and the result is stored in v0,v1.
 */

op_add_long:
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

#ifdef debug
    if (is_verbose()) {
        printf("add-long v%d, v%d, v%d\n", reg_idx_vx, reg_idx_vy, reg_idx_vz);
    }
#endif
    /* x = y + z */
    load_reg_to_double(vm, reg_idx_vy, ptr_y+4);
    load_reg_to_double(vm, reg_idx_vy+1, ptr_y);
    load_reg_to_double(vm, reg_idx_vz, ptr_z+4);
    load_reg_to_double(vm, reg_idx_vz+1, ptr_z);
    x = y + z;
#ifdef debug
    if (is_verbose()) {
        printf("add-long %ld, %ld, %ld\n", y, z, y+z);
    }
#endif
    store_double_to_reg(vm, reg_idx_vx, ptr_x+4);
    store_double_to_reg(vm, reg_idx_vx+1, ptr_x);
    *pc = *pc + 4;
    goto NEXT_OP;
    
}

op_sub_long:
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

#ifdef debug
    if (is_verbose())
        printf("sub-long v%d, v%d, v%d\n", reg_idx_vx, reg_idx_vy, reg_idx_vz);
#endif
    /* x = y - z */
    load_reg_to_long(vm, reg_idx_vy, ptr_y+4);
    load_reg_to_long(vm, reg_idx_vy+1, ptr_y);
    load_reg_to_long(vm, reg_idx_vz, ptr_z+4);
    load_reg_to_long(vm, reg_idx_vz+1, ptr_z);
    x = y - z;
    store_long_to_reg(vm, reg_idx_vx, ptr_x+4);
    store_long_to_reg(vm, reg_idx_vx+1, ptr_x);
    *pc = *pc + 4;
    goto NEXT_OP;
    

}

/* 0x9d mul-long vx,vy,vz
 * Calculates vy*vz and puts the result into vx.
 * 9C00 0305 - sub-long v0, v3, v5
 * Subtracts the long value in v5,v6 from the long value in v3,v4 and puts the result into v0,v1.
 */
op_mul_long:
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

#ifdef debug
    if (is_verbose()) {
        printf("mul-long v%d, v%d, v%d\n", reg_idx_vx, reg_idx_vy, reg_idx_vz);
    }
#endif
    /* x = y * z */
    load_reg_to_double(vm, reg_idx_vy, ptr_y+4);
    load_reg_to_double(vm, reg_idx_vy+1, ptr_y);
    load_reg_to_double(vm, reg_idx_vz, ptr_z+4);
    load_reg_to_double(vm, reg_idx_vz+1, ptr_z);
    x = y * z;
#ifdef debug
    if (is_verbose()) {
        printf("sub-long %ld, %ld, %ld\n", y, z, y*z);
    }
#endif
    store_double_to_reg(vm, reg_idx_vx, ptr_x+4);
    store_double_to_reg(vm, reg_idx_vx+1, ptr_x);
    *pc = *pc + 4;
    goto NEXT_OP;
    
}

/* 0x9e div-long vx,vy,vz
 * Calculates vy/vz and puts the result into vx.
 * 9E06 0002 - div-long v6, v0, v2
 * Divides the long value in v0,v1 with the long value in v2,v3 and pust the result into v6,v7.
 */
op_div_long:
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

#ifdef debug
    if (is_verbose()) {
        printf("div-long v%d, v%d, v%d\n", reg_idx_vx, reg_idx_vy, reg_idx_vz);
    }
#endif
    /* x = y / z */
    load_reg_to_double(vm, reg_idx_vy, ptr_y+4);
    load_reg_to_double(vm, reg_idx_vy+1, ptr_y);
    load_reg_to_double(vm, reg_idx_vz, ptr_z+4);
    load_reg_to_double(vm, reg_idx_vz+1, ptr_z);
#ifdef debug
    if (is_verbose()) {
        printf("div-long %ld, %ld\n", y, z);
    }
#endif
    x = y / z;
    store_double_to_reg(vm, reg_idx_vx, ptr_x+4);
    store_double_to_reg(vm, reg_idx_vx+1, ptr_x);
    *pc = *pc + 4;
    goto NEXT_OP;
    
}


op_add_int_lit8:
{
    int reg_idx_vx = 0;
    int reg_idx_vy = 0;
    int x = 0, y = 0 ;
    int z = 0;
    reg_idx_vx = ptr[*pc + 1];
    reg_idx_vy = ptr[*pc + 2];
    z = ptr[*pc + 3];

#ifdef debug
    if (is_verbose())
        printf("add-int v%d, v%d, #int%d\n", reg_idx_vx, reg_idx_vy, z);
#endif
    /* x = y + z */
    load_reg_to(vm, reg_idx_vy, (unsigned char *) &y);
    x = y + z;
    store_to_reg(vm, reg_idx_vx, (unsigned char *) &x);

    *pc = *pc + 4;
    goto NEXT_OP;
    
}

/* 0xd9 sub-int/lit8 vx,vy,lit8
 * Calculates vy-lit8 and stores the result into vx.
 * D900 0201 - sub-int/lit8 v0,v2, #int1
 * Calculates v2-1 and stores the result into v0.
 */

op_sub_int_lit8:
{
    int reg_idx_vx = 0;
    int reg_idx_vy = 0;
    int x = 0, y = 0 ;
    int z = 0;
    reg_idx_vx = ptr[*pc + 1];
    reg_idx_vy = ptr[*pc + 2];
    z = ptr[*pc + 3];

#ifdef debug
    if (is_verbose())
        printf("sub-int v%d, v%d, #int%d\n", reg_idx_vx, reg_idx_vy, z);
#endif
    /* x = y - z */
    load_reg_to(vm, reg_idx_vy, (unsigned char *) &y);
    x = y - z;
    store_to_reg(vm, reg_idx_vx, (unsigned char *) &x);

    *pc = *pc + 4;
    goto NEXT_OP;
    
}

/* 0xda mul-int/lit8 vx,vy,lit8
 * Multiplies vy with lit8 8-bit literal constant and puts the result into vx.
 * DA00 0002 - mul-int/lit8 v0,v0, #int2
 * Multiplies v0 with literal 2 and puts the result into v0.
 */

op_mul_int_lit8:
{
    int reg_idx_vx = ptr[*pc + 1];
    int reg_idx_vy = ptr[*pc + 2];
    int x = 0, y = 0 ;
    int z = ptr[*pc + 3];

#ifdef debug
    if (is_verbose())
        printf("mul-int v%d, v%d, #int%d\n", reg_idx_vx, reg_idx_vy, z);
#endif
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
    goto NEXT_OP;
    
}

/* 0xdb div-int/lit8 vx,vy,lit8
 * Calculates vy/lit8 and stores the result into vx.
 * DB00 0203 - div-int/lit8 v0,v2, #int3
 * Calculates v2/3 and stores the result into v0.
 */
op_div_int_lit8:
{
    int reg_idx_vx = 0;
    int reg_idx_vy = 0;
    int x = 0, y = 0 ;
    int z = 0;
    reg_idx_vx = ptr[*pc + 1];
    reg_idx_vy = ptr[*pc + 2];
    z = ptr[*pc + 3];

#ifdef debug
    if (is_verbose())
        printf("div-int v%d, v%d, #int%d\n", reg_idx_vx, reg_idx_vy, z);
#endif
    /* x = y / z */
    load_reg_to(vm, reg_idx_vy, (unsigned char *) &y);
    x = y % z;
    x = (y - x) / z;
    store_to_reg(vm, reg_idx_vx, (unsigned char *) &x);

    *pc = *pc + 4;
    goto NEXT_OP;
    
}

op_sput_object:
{
    int reg_idx_vx = 0;
    int target_field_id = 0;
    int object_reference = 0;
    field_id_item *s = NULL, *t = NULL;

    reg_idx_vx = ptr[*pc + 1];
    target_field_id = ((ptr[*pc + 3] << 8) | ptr[*pc + 2]);

    load_reg_to(vm, reg_idx_vx, (unsigned char *) &object_reference);
#ifdef debug
    if (is_verbose()) {
        printf("sput-object v%d, field 0x%04x to 0x%04x\n", reg_idx_vx, object_reference, target_field_id);
    }
#endif
    memcpy(vm->static_field_value[target_field_id].data, (unsigned char*)&object_reference, sizeof(int));
    *pc = *pc + 4;
    goto NEXT_OP;
    

}

op_iput:{
    int reg_idx_vx = 0;
    int reg_idx_vy = 0;
    int target_field_id = 0;
    reg_idx_vx = ptr[*pc + 1] & 0x0F;
    reg_idx_vy = (ptr[*pc + 1] >> 4) & 0x0F;

    target_field_id = ((ptr[*pc + 3] << 8) | ptr[*pc + 2]);
#ifdef debug
    if (is_verbose()) {
        printf("iput v%d, v%d, field 0x%04x\n", reg_idx_vx, reg_idx_vy, target_field_id);
    }
#endif
    int obj_ref;
    load_reg_to(vm,reg_idx_vy,(unsigned char*)&obj_ref);
    //printf("obj_ref = %d, len = %d\n",obj_ref,vm->object_length);
    unsigned char *ptr_x = vm->object[obj_ref]->instance_fields[target_field_id].data;
    load_reg_to(vm, reg_idx_vx, ptr_x);
    *pc = *pc + 4;
    goto NEXT_OP;
    
}

op_iget:{
    int reg_idx_vx = 0;
    int reg_idx_vy = 0;
    int target_field_id = 0;
    reg_idx_vx = ptr[*pc + 1] & 0x0F;
    reg_idx_vy = (ptr[*pc + 1] >> 4) & 0x0F;

    target_field_id = ((ptr[*pc + 3] << 8) | ptr[*pc + 2]);
#ifdef debug
    if (is_verbose()) {
        printf("iget v%d, v%d, field 0x%04x\n", reg_idx_vx, reg_idx_vy, target_field_id);
    }
#endif
    int obj_ref;
    load_reg_to(vm,reg_idx_vy,(unsigned char*)&obj_ref);
    //printf("obj_ref = %d, len = %d\n",obj_ref,vm->object_length);
    unsigned char *ptr_x = vm->object[obj_ref]->instance_fields[target_field_id].data;
    store_to_reg(vm, reg_idx_vx, ptr_x);
    *pc = *pc + 4;
    goto NEXT_OP;
    
}



op_iput_wide:{
    int reg_idx_vx = 0;
    int reg_idx_vx_p1 = 0;
    int reg_idx_vy = 0;
    int target_field_id = 0;
    reg_idx_vx = ptr[*pc + 1] & 0x0F;
    reg_idx_vy = (ptr[*pc + 1] >> 4) & 0x0F;
    reg_idx_vx_p1 = reg_idx_vx + 1;
	
    target_field_id = ((ptr[*pc + 3] << 8) | ptr[*pc + 2]);
#ifdef debug
    if (is_verbose()) {
        printf("iput-wide v%d, v%d, field 0x%04x\n", reg_idx_vx, reg_idx_vy, target_field_id);
    }
#endif
    int obj_ref;
    load_reg_to(vm,reg_idx_vy,(unsigned char*)&obj_ref);
    unsigned char *ptr_x = vm->object[obj_ref]->instance_fields[target_field_id].data;
    load_reg_to_double(vm, reg_idx_vx   , ptr_x + 4);    
    load_reg_to_double(vm, reg_idx_vx_p1, ptr_x );    
    *pc = *pc + 4;
    goto NEXT_OP;
    
}

op_iget_wide:{
    int reg_idx_vx = 0;
    int reg_idx_vx_p1 = 0;
    int reg_idx_vy = 0;
    int target_field_id = 0;
    reg_idx_vx = ptr[*pc + 1] & 0x0F;
    reg_idx_vy = (ptr[*pc + 1] >> 4) & 0x0F;
    reg_idx_vx_p1 = reg_idx_vx + 1;

    target_field_id = ((ptr[*pc + 3] << 8) | ptr[*pc + 2]);
#ifdef debug
    if (is_verbose()) {
        printf("iget-wide v%d, v%d, field 0x%04x\n", reg_idx_vx, reg_idx_vy, target_field_id);
    }
#endif
    int obj_ref;
    load_reg_to(vm,reg_idx_vy,(unsigned char*)&obj_ref);
    unsigned char *ptr_x = vm->object[obj_ref]->instance_fields[target_field_id].data;
    store_double_to_reg(vm, reg_idx_vx   , ptr_x + 4);
    store_double_to_reg(vm, reg_idx_vx_p1, ptr_x );
    *pc = *pc + 4;
    goto NEXT_OP;
    
}

op_aput:{
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
    int arr_ref, arr_idx, value;
    load_reg_to(vm,reg_idx_vx,(unsigned char*)&value);
    load_reg_to(vm,reg_idx_vy,(unsigned char*)&arr_ref);
    load_reg_to(vm,reg_idx_vz,(unsigned char*)&arr_idx);
    if(arr_ref >= 8192)
        arr_ref = arr_ref - 8192;
    if(arr_idx >= 8192)
        arr_idx = arr_idx - 8192;
#ifdef debug
    if (is_verbose()) {
        printf("aput v%d, v%d, v%d\n", reg_idx_vx, reg_idx_vy, reg_idx_vz);
        printf("ref = %d, idx = %d, total_array_num = %d, ",arr_ref,arr_idx,vm->array_number);
        printf("array_size = %d\n",vm->array[arr_ref]->size);
    }
#endif
    memcpy(vm->array[arr_ref]->list[arr_idx].data, &value,sizeof(int));
    *pc = *pc + 4;
    goto NEXT_OP;
    
 
}


op_aget:{
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
    int arr_ref, arr_idx, value;
    load_reg_to(vm,reg_idx_vy,(unsigned char*)&arr_ref);
    load_reg_to(vm,reg_idx_vz,(unsigned char*)&arr_idx);
    if(arr_ref >= 8192)
        arr_ref = arr_ref - 8192;
    if(arr_idx >= 8192)
        arr_idx = arr_idx - 8192;
#ifdef debug
    if (is_verbose()) {
        printf("aget v%d, v%d, v%d\n", reg_idx_vx, reg_idx_vy, reg_idx_vz);
        printf("ref = %d, idx = %d, total_array_num = %d, \n",arr_ref,arr_idx,vm->array_number);
        printf("array_size = %d\n",vm->array[arr_ref]->size);
    }
#endif
    memcpy(&value,vm->array[arr_ref]->list[arr_idx].data,sizeof(int));
    store_to_reg(vm,reg_idx_vx,(unsigned char*)&value);
    *pc = *pc + 4;
    goto NEXT_OP;
    

}

op_packed_switch: // 0x2B
{
    int reg_idx_vx = 0;
    int offset = 0;
    int x;

    reg_idx_vx = ptr[*pc + 1];
    offset = (ptr[*pc + 5] << 24 | ptr[*pc + 4] << 16 | ptr[*pc + 3] << 8 | ptr[*pc + 2]);

    load_reg_to(vm, reg_idx_vx, (unsigned char *) &x);

#ifdef debug
    if (is_verbose()) {
        printf("packed-switch v%d (%d), offset %x\n", reg_idx_vx, x, offset);
    }
#endif

    int table_offset = *pc + 2 * offset;

    int indent = (ptr[table_offset + 1] << 8 | ptr[table_offset + 0]);
#ifdef debug
    if (is_verbose() && indent != 0x0100) {
        printf("  error, indent: %04x != 0x0100\n", indent);
        exit(1);
    }
#endif
    int size = (ptr[table_offset + 3] << 8 | ptr[table_offset + 2]);
    int first_key = (ptr[table_offset + 7] << 24 | ptr[table_offset + 6] << 16 | ptr[table_offset + 5] << 8 | ptr[table_offset + 4]);
#ifdef debug
    if (is_verbose()) {
        printf("size %d, first_key %d\n", size, first_key);
    }
#endif

    if (first_key <= x && x < first_key + size) {
        int index = x - first_key;
        int tatget_offset = table_offset + 8 + index * 2;
#ifdef debug
        if (is_verbose()) {
            printf("index %d, tatget_offset, %x\n", index, tatget_offset);
        }
#endif
        int branch_offset = (ptr[tatget_offset + 1] << 24 | ptr[tatget_offset + 0] << 16 | ptr[tatget_offset + 3] << 8 | ptr[tatget_offset + 2]);
#ifdef debug
        if (is_verbose()) {
            printf("branch_offset %x\n", branch_offset);
        }
#endif
        *pc = *pc + branch_offset * 2;
    }
    else {
        *pc = *pc + 6;
    }
    return;
}



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
#ifdef debug
            if (is_verbose())
                printf("%s {} method_id 0x%04x", name, p->method_id);
#endif
            break;
        case 1:
#ifdef debug
            if (is_verbose())
                printf("%s, {v%d} method_id 0x%04x",
                       name, p->reg_idx[0], p->method_id);
#endif
            break;
        case 2:
#ifdef debug
            if (is_verbose())
                printf("%s {v%d, v%d} method_id 0x%04x",
                       name,
                       p->reg_idx[0], p->reg_idx[1],
                       p->method_id);
#endif
            break;
        case 3:
#ifdef debug
            if (is_verbose())
                printf("%s {v%d, v%d, v%d} method_id 0x%04x",
                       name,
                       p->reg_idx[0], p->reg_idx[1], p->reg_idx[2],
                       p->method_id);
#endif
            break;
        case 4:
#ifdef debug
            if (is_verbose())
                printf("%s {v%d, v%d, v%d, v%d} method_id 0x%04x",
                       name,
                       p->reg_idx[0], p->reg_idx[1],
                       p->reg_idx[2], p->reg_idx[3],
                       p->method_id);
#endif
            break;
        case 5:
#ifdef debug
            if (is_verbose())
                printf("%s {v%d, v%d, v%d, v%d, v%d} method_id 0x%04x",
                       name,
                       p->reg_idx[0], p->reg_idx[1], p->reg_idx[2],
                       p->reg_idx[3], p->reg_idx[4],
                       p->method_id);
#endif
            break;
        default:
            break;
        }

        if (m != 0 && type_class != 0 && p->reg_count <= 5) {
            if (proto_item != 0)
                proto_type_list = get_proto_type_list(dex, m->proto_idx);
            if (proto_type_list != 0 && proto_type_list->size > 0) {
#ifdef debug
                if (is_verbose())
                    printf(" %s,%s,(%s)%s \n",
                           get_string_data(dex, type_class->descriptor_idx),
                           get_string_data(dex, m->name_idx),
                           get_type_item_name(dex,
                                              proto_type_list->type_item[0].type_idx),
                           get_type_item_name(dex,
                                              proto_item->return_type_idx));
#endif
                return invoke_java_lang_library(dex, vm,
                                         get_string_data(dex, type_class->descriptor_idx),
                                         get_string_data(dex, m->name_idx),
                                         get_type_item_name(dex, proto_type_list->type_item[0].type_idx));
            } else {
#ifdef debug
                if (is_verbose())
                    printf(" %s,%s,()%s \n",
                           get_string_data(dex, type_class->descriptor_idx),
                           get_string_data(dex, m->name_idx),
                           get_type_item_name(dex,
                                              proto_item->return_type_idx));
#endif
                return invoke_java_lang_library(dex, vm,
                                         get_string_data(dex, type_class->descriptor_idx),
                                         get_string_data(dex, m->name_idx), 0);
            }

        } else {
#ifdef debug
            if (is_verbose())
                printf("\n");
#endif
        }
    }
    return 0;
}
#endif
