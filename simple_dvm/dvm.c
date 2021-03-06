#include "simple_dvm.h"
#include "java_lib.h"

void initOpCode();
opCodeFunc findOpCodeFunc(unsigned char op);

static int find_const_string(DexFileFormat *dex, char *entry)
{
    int i = 0;
    for (i = 0; i < dex->header.stringIdsSize; i++) {
        if (memcmp(dex->string_data_item[i].data, entry, strlen(entry)) == 0) {
#ifdef debug
            if (is_verbose())
                printf("find %s in dex->string_data_item[%d] %s\n", entry, i, (char*)dex->string_data_item[i].data);
#endif
            return i;
        }
    }
    return -1;
}

void printRegs(simple_dalvik_vm *vm)
{
    int i = 0;
#ifdef debug
    if (is_verbose()) {
        printf("pc = %08x\n", vm->pc);
        for (i = 0; i < 16 ; i++) {
            unsigned int value = 0;
            load_reg_to(vm, i, (unsigned char*) &value);
            printf("Reg[%2d] = %4d (%04x) ",
                   i, (int) value, value);
            if ((i + 1) % 4 == 0) printf("\n");
        }
    }
#endif
}
extern opCodeFunc look_up_table[256];
void runMethod(DexFileFormat *dex, simple_dalvik_vm *vm, encoded_method *m)
{
    u1 *ptr = (u1 *) m->code_item.insns;
    unsigned char opCode = 0;
    opCodeFunc func = 0;

    vm->pc = 0;
    int *check = &(vm->pc);
    int size = m->code_item.insns_size * sizeof(ushort);
    while (*check < size) {
#ifndef threaded_code
        opCode = ptr[vm->pc];
        func = look_up_table[opCode];
        //if (func != 0) {
            func(dex, vm, ptr, &vm->pc);
#else
        dispatch_DTC(dex, vm, ptr, &vm->pc);
#endif
        /*} else {
            printRegs(vm);
            printf("Unknow OpCode =%02x \n", opCode);
            exit(0);
        }*/
    }
}

void copy_parameter( simple_dalvik_vm *vm, int reg_size, int reg_count, int *reg_idx){
     int i = 0;
     int m = reg_size - 1;
     simple_dvm_register tmp[5];
#ifdef debug
     if (is_verbose() > 2)
        printf("copy parameter, function reg_size = %d, reg_count = %d\n",reg_size,reg_count);
#endif
     for(;i<reg_count;i++)
         tmp[i] = vm->regs[reg_idx[i]];
     for(;reg_count>=0;m--)
         vm->regs[m] = tmp[--reg_count];
}

void invoke_method(DexFileFormat *dex, simple_dalvik_vm *vm, encoded_method* m_Arr, int size, int method_idx, int class_idx){
    int i = 0;
    int method_acc_idx = 0;
    for(i=0;i<size;i++){
        method_acc_idx += m_Arr[i].method_idx_diff;
        if(method_acc_idx == method_idx){
#ifdef debug
            if (is_verbose() > 2)
            printf("find method %d in class item %d, direct_method %d\n",method_idx,class_idx,i);
#endif
            method_idx = i;
            break;
        }
    }
    encoded_method *m = m_Arr + method_idx;
#ifdef debug
    if (is_verbose() > 2)
        printf("encoded_method method_id = %d, insns_size = %d, reg_size = %d\n",
               m->method_idx_diff, m->code_item.insns_size, m->code_item.registers_size);
#endif
    simple_dvm_register tmp[32];
    for(i=0;i<32;i++)
        tmp[i] = vm->regs[i];
    copy_parameter(vm, m->code_item.registers_size, vm->p.reg_count, vm->p.reg_idx);
    runMethod(dex, vm, m);
    for(i=0;i<32;i++)
        vm->regs[i] = tmp[i];
}

void invoke_direct_by_id(DexFileFormat *dex, simple_dalvik_vm *vm, int method_idx, int class_idx){
    invoke_method(dex, vm,
                 dex->class_data_item[class_idx].direct_methods,
                 dex->class_data_item[class_idx].direct_methods_size,
                 method_idx,class_idx);
}

void invoke_virtual_by_id(DexFileFormat *dex, simple_dalvik_vm *vm, int method_idx, int class_idx){
    invoke_method(dex, vm,
                 dex->class_data_item[class_idx].virtual_methods,
                 dex->class_data_item[class_idx].virtual_methods_size,
                 method_idx,class_idx);
}

void invoke_method_entry(DexFileFormat *dex, simple_dalvik_vm *vm, char *entry, int isDirect){

    int i = 0;
    int method_idx = -1;
    int class_idx = -1;
    int class_def_idx = -1;
    int method_name_idx = find_const_string(dex, entry);

    for (i = 0 ; i < dex->header.methodIdsSize; i++)
        if (dex->method_id_item[i].name_idx == method_name_idx) {
#ifdef debug
            if (is_verbose() > 2)
                printf("find %s in class_idx[%d], method_id = %d\n",
                       entry, dex->method_id_item[i].class_idx, i);
#endif
            class_idx = dex->method_id_item[i].class_idx;
            method_idx = i;
            break;
        }

    if (class_idx < 0 || method_idx < 0) {
        printf("no method %s in dex\n", entry);
        return;
    }

    for(i=0;i<dex->header.classDefsSize;i++){
        if(class_idx == dex->class_def_item[i].class_idx){
            class_def_idx = i;
#ifdef debug
            if (is_verbose() > 2)
                printf("find class_idx in class item %d\n",i);
#endif
            break;
        }
    }
    if(isDirect)
        invoke_direct_by_id(dex, vm, method_idx, class_def_idx);
    else
        invoke_virtual_by_id(dex, vm, method_idx, class_def_idx);
}

void invoke_virtual_method(DexFileFormat *dex, simple_dalvik_vm *vm, int class_idx, int method_idx){
    encoded_method *m = vm->virtual_table[method_idx];
        //printf("m = %x\n",m);
    if(m==NULL) return;
#ifdef debug
    if (is_verbose() > 2)
        printf("encoded_method method_id = %d, insns_size = %d, reg_size = %d\n",
               m->method_idx_diff, m->code_item.insns_size, m->code_item.registers_size);
#endif
    int i = 0;
    simple_dvm_register tmp[32];
    for(i=0;i<16;i++)
        tmp[i] = vm->regs[i];
    copy_parameter(vm, m->code_item.registers_size, vm->p.reg_count, vm->p.reg_idx);
    runMethod(dex, vm, m);
    for(i=0;i<16;i++)
        vm->regs[i] = tmp[i];

}

void virtural_table_lookup(DexFileFormat *dex, simple_dalvik_vm *vm){
    int i,k;
    for(i=0;i<32;i++)
        vm->virtual_table[i] = NULL;

    for(i = 0 ; i < dex->header.methodIdsSize; i++){
        int found = 0;
        int class_idx =  dex->method_id_item[i].class_idx;
        for(k=0;k<dex->header.classDefsSize;k++){
            if(class_idx == dex->class_def_item[k].class_idx){
                //printf("i = %d, class_idx = %d, k = %d\n",i,class_idx,k);
                class_idx = k;
                found = 1;
                break;
            }
        }
        if(!found) continue;
        encoded_method* m_Arr = dex->class_data_item[class_idx].virtual_methods;
        int size = dex->class_data_item[class_idx].virtual_methods_size;
        int method_acc_idx = 0, method_idx = i;
        for(k=0;k<size;k++){
            method_acc_idx += m_Arr[k].method_idx_diff;
            if(method_acc_idx == method_idx){
                method_idx = k;
                break;
            }
        }
        encoded_method *m = m_Arr + method_idx;
        vm->virtual_table[i] = m;
    }
}

void simple_dvm_startup(DexFileFormat *dex, simple_dalvik_vm *vm, char *entry)
{

    char clinit[] = "<clinit>";
    memset(vm , 0, sizeof(simple_dalvik_vm));
    vm->static_field_value = (field_value*) malloc(sizeof(field_value) * (dex->header.fieldIdsSize) );
    initOpCode();
    invoke_method_entry(dex,vm,clinit,1);
    virtural_table_lookup(dex, vm);
    invoke_method_entry(dex,vm,entry,1);
}

