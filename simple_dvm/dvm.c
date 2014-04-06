#include "simple_dvm.h"
#include "java_lib.h"

opCodeFunc findOpCodeFunc(unsigned char op);

static int find_const_string(DexFileFormat *dex, char *entry)
{
    int i = 0;
    for (i = 0; i < dex->header.stringIdsSize; i++) {
        if (memcmp(dex->string_data_item[i].data, entry, strlen(entry)) == 0) {
            if (is_verbose())
                printf("find %s in dex->string_data_item[%d] %s\n", entry, i, (char*)dex->string_data_item[i].data);
            return i;
        }
    }
    return -1;
}

static void printRegs(simple_dalvik_vm *vm)
{
    int i = 0;
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
}

void runMethod(DexFileFormat *dex, simple_dalvik_vm *vm, encoded_method *m)
{
    u1 *ptr = (u1 *) m->code_item.insns;
    unsigned char opCode = 0;
    opCodeFunc func = 0;

    vm->pc = 0;
    while (1) {
        if (vm->pc >= m->code_item.insns_size * sizeof(ushort))
            break;
        opCode = ptr[vm->pc];
        func = findOpCodeFunc(opCode);
        if (func != 0) {
            func(dex, vm, ptr, &vm->pc);
        } else {
            printRegs(vm);
            printf("Unknow OpCode =%02x \n", opCode);
            exit(0);
        }
    }
    printf("end Method\n");
}

void copy_parameter( simple_dalvik_vm *vm, int reg_size, int reg_count, int *reg_idx){
     int i = 0;
     int m = reg_size - 1;
     simple_dvm_register tmp[5];
     if (is_verbose() > 2)
        printf("copy parameter, function reg_size = %d, reg_count = %d\n",reg_size,reg_count);
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
            if (is_verbose() > 2)
            printf("find method %d in class item %d, direct_method %d\n",method_idx,class_idx,i);
            method_idx = i;
            break;
        }
    }
    encoded_method *m = m_Arr + method_idx;
    if (is_verbose() > 2)
        printf("encoded_method method_id = %d, insns_size = %d, reg_size = %d\n",
               m->method_idx_diff, m->code_item.insns_size, m->code_item.registers_size);
    copy_parameter(vm, m->code_item.registers_size, vm->p.reg_count, vm->p.reg_idx);
    runMethod(dex, vm, m);
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
            if (is_verbose() > 2)
                printf("find %s in class_idx[%d], method_id = %d\n",
                       entry, dex->method_id_item[i].class_idx, i);
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
            if (is_verbose() > 2)
                printf("find class_idx in class item %d\n",i);
            break;
        }
    }
    if(isDirect)
        invoke_direct_by_id(dex, vm, method_idx, class_def_idx);
    else
        invoke_virtual_by_id(dex, vm, method_idx, class_def_idx);
}

void simple_dvm_startup(DexFileFormat *dex, simple_dalvik_vm *vm, char *entry)
{
    char clinit[] = "<clinit>";
    memset(vm , 0, sizeof(simple_dalvik_vm));
    vm->static_field_value = (field_value*) malloc(sizeof(field_value) * (dex->header.fieldIdsSize) );
    invoke_method_entry(dex,vm,clinit,1);
    invoke_method_entry(dex,vm,entry,1);
}

