#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "linkedlist.h"
#include "hashtable.h"
#include "riscv.h"

/************** BEGIN HELPER FUNCTIONS PROVIDED FOR CONVENIENCE ***************/
const int R_TYPE = 0;
const int I_TYPE = 1;
const int MEM_TYPE = 2;
const int U_TYPE = 3;
const int UNKNOWN_TYPE = 4;

/**
 * Return the type of instruction for the given operation
 * Available options are R_TYPE, I_TYPE, MEM_TYPE, UNKNOWN_TYPE
 */
static int get_op_type(char *op)
{
    const char *r_type_op[] = {"add", "sub", "and", "or", "xor", "slt", "sll", "sra"};
    const char *i_type_op[] = {"addi", "andi", "ori", "xori", "slti"};
    const char *mem_type_op[] = {"lw", "lb", "sw", "sb"};
    const char *u_type_op[] = {"lui"};
    for (int i = 0; i < (int)(sizeof(r_type_op) / sizeof(char *)); i++)
    {
        if (strcmp(r_type_op[i], op) == 0)
        {
            return R_TYPE;
        }
    }
    for (int i = 0; i < (int)(sizeof(i_type_op) / sizeof(char *)); i++)
    {
        if (strcmp(i_type_op[i], op) == 0)
        {
            return I_TYPE;
        }
    }
    for (int i = 0; i < (int)(sizeof(mem_type_op) / sizeof(char *)); i++)
    {
        if (strcmp(mem_type_op[i], op) == 0)
        {
            return MEM_TYPE;
        }
    }
    for (int i = 0; i < (int)(sizeof(u_type_op) / sizeof(char *)); i++)
    {
        if (strcmp(u_type_op[i], op) == 0)
        {
            return U_TYPE;
        }
    }
    return UNKNOWN_TYPE;
}
/*************** END HELPER FUNCTIONS PROVIDED FOR CONVENIENCE ****************/

const int MEM_ENTRIES = 128;
registers_t *registers;
hashtable_t * memory;

void init(registers_t *starting_registers)
{
    registers = starting_registers;
    memory = ht_init(MEM_ENTRIES);
}

void end()
{
    ht_free(memory);
}

/** Returns register number. */
int get_register(char ** instruction){
    strsep(instruction, "x");
    return atoi(strsep(instruction, ","));
}

/** Performs add, sub RISCV instructions.
 *  Form: add rd, rs1, rs2
 */
void do_math(char * instruction, int sub){
    // ex: add x3, x2, x1 
    int rd = get_register(&instruction); // rd = 3
    int rs1 = get_register(&instruction); // rs1 = 2
    int rs2 = get_register(&instruction); // rs2 = 1
    if (sub == 1){ // sub op
        registers->r[rd] = registers->r[rs1] - registers->r[rs2];
    } else { // add op
        registers->r[rd] = registers->r[rs1] + registers->r[rs2];
    }
}

/** Performs and, or, xor RISCV instructions.
 *  Form: or rd, rs1, rs2
 */
void do_logical(char * instruction, int and){
    // ex: and x3, x2, x1 
    int rd = get_register(&instruction); // rd = 3
    int rs1 = get_register(&instruction); // rs1 = 2
    int rs2 = get_register(&instruction); // rs2 = 1
    if (and == 00){ // and op
        registers->r[rd] = registers->r[rs1] && registers->r[rs2];
    } else if (and == 01) { // or op
        registers->r[rd] = registers->r[rs1] || registers->r[rs2];
    } else if (and == 10) { // xor op
        registers->r[rd] = registers->r[rs1] ^ registers->r[rs2];
    }
}

/** Performs slt RISCV instruction.
 *  Form: slt rd, rs1, rs2
 */
void do_lessthan(char * instruction){
    // ex: slt x3, x2, x1 
    int rd = get_register(&instruction); // rd = 3
    int rs1 = get_register(&instruction); // rs1 = 2
    int rs2 = get_register(&instruction); // rs2 = 1
    int is_less = registers->r[rs1] < registers->r[rs2];
    if (is_less == 1){
        registers->r[rd] = 1;
    } else {
        registers->r[rd] = 0;
    }
}

/** Performs sll, sra RISCV instructions.
 *  Form: sll rd, rs1, rs2
 */
void do_shift(char * instruction, int is_sll){
    // ex: sll x3, x2, x1 
    int rd = get_register(&instruction); // rd = 3
    int rs1 = get_register(&instruction); // rs1 = 2
    int rs2 = get_register(&instruction); // rs2 = 1
    if (is_sll == 1){ // sll op
        registers->r[rd] = registers->r[rs1] << registers->r[rs2];
    } else { // sra op
        registers->r[rd] = registers->r[rs1] >> registers->r[rs2];
    }
}

/** Performs addi RISCV instruction.
 *  Form: addi rd, rs1, imm
 */
void do_add_imm(char * instruction){
    // ex: addi x9, x7, 13
    int rd = get_register(&instruction); // rd = 9
    int rs1 = get_register(&instruction); // rs1 = 7
    int imm = (int)strtol(strsep(&instruction, "\0"), NULL, 0); // strtol handles hex and int, imm = 13
    registers->r[rd] = registers->r[rs1] + imm;
}

/** Performs andi, ori, xori RISCV instructions.
 *  Form: andi rd, rs1, imm
 */
void do_logical_imm(char * instruction, int andi){
    // ex: andi x9, x7, 13
    int rd = get_register(&instruction); // rd = 9
    int rs1 = get_register(&instruction); // rs1 = 7
    int imm = (int)strtol(strsep(&instruction, "\0"), NULL, 0); // strtol handles hex and int, imm = 13
    if (andi == 00){ // andi op
        registers->r[rd] = registers->r[rs1] && imm;
    } else if (andi == 01) { // ori op
        registers->r[rd] = registers->r[rs1] || imm;
    } else if (andi == 10){ // xori op
        registers->r[rd] = registers->r[rs1] ^ imm;
    }
}

/** Performs slli RISCV instruction.
 *  Form: slli rd, rs1, imm
 */
void do_shift_imm(char * instruction){
    // ex: slli x9, x7, 13
    int rd = get_register(&instruction); // rd = 9
    int rs1 = get_register(&instruction); // rs1 = 7
    int imm = (int)strtol(strsep(&instruction, "\0"), NULL, 0); // strtol handles hex and int, imm = 13
    registers->r[rd] = registers->r[rs1] << imm;
}

/** Performs lui RISCV instruction.
 *  Form: lui rd, imm
 */
void do_lui(char * instruction){
    // does lui
    // ex: lui x5, 15
    int rd = get_register(&instruction); // rd = 5
    int imm = (int)strtol(strsep(&instruction, "\0"), NULL, 0); // imm = 15
    registers->r[rd] = (imm << 12);
}

/** Returns offset of a RISCV memory instruction. */
int get_offset(char ** instruction){
    strsep(instruction, " ");
    return (int)strtol(strsep(instruction, "("), NULL, 0);
}

/** Returns register number of the register containing the base address of a RISCV memory instruction. */
int get_baseadr_reg(char ** instruction){
    strsep(instruction, "x");
    return atoi(strsep(instruction, ")"));
}

/** Performs lw, lb RISCV instruction.
 *  Form: lw rd, off(rs1)
 */
void do_load(char * instruction, int is_byte){
    // ex: lw x2, 2(x3)
    int rd = get_register(&instruction);
    int off = get_offset(&instruction);
    int rs1 = get_baseadr_reg(&instruction);
    int mem_addr = off + registers->r[rs1];
    int byte1 = ht_get(memory, mem_addr);
    int byte2 = ht_get(memory, mem_addr+1) << 8;
    int byte3 = ht_get(memory, mem_addr+2) << 16;
    int byte4 = ht_get(memory, mem_addr+3) << 24;
    if (is_byte == 1) { // lb op
        registers->r[rd] = byte1;
    } else { // lw op
        registers->r[rd] = byte4 + byte3 + byte2 + byte1;
    }
}

/** Performs sw, sb RISCV instruction.
 *  Form: sw rd, off(rs1)
 */
void do_store(char * instruction, int is_byte){
    // ex: sw x3, 5(x7)
    int rs2 = get_register(&instruction);
    int off = get_offset(&instruction);
    int rs1 = get_baseadr_reg(&instruction);
    int mem_addr = off + registers->r[rs1];
    int byte1 = registers->r[rs2] & 0x000000ff;
    int byte2 = (registers->r[rs2] >> 8) & 0x000000ff;
    int byte3 = (registers->r[rs2] >> 16) & 0x000000ff;
    int byte4 = (registers->r[rs2] >> 24) & 0x000000ff;
    if (is_byte == 1){ // sb op
        ht_add(memory, mem_addr, byte1);
    } else { // sw op
        ht_add(memory, mem_addr, byte1);
        ht_add(memory, mem_addr + 1, byte2);
        ht_add(memory, mem_addr + 2, byte3);
        ht_add(memory, mem_addr + 3, byte4);
    }
}

void step(char *instruction)
{
    // Extracts and returns the substring before the first space character,
    // by replacing the space character with a null-terminator.
    // `instruction` now points to the next character after the space
    // See `man strsep` for how this library function works
    char *op = strsep(&instruction, " ");
    // Uses the provided helper function to determine the type of instruction
    int op_type = get_op_type(op);
    // Skip this instruction if it is not in our supported set of instructions
    if (op_type == UNKNOWN_TYPE)
    {
        return;
    }
    registers->r[0] = 0;
    if (op_type == R_TYPE) {
        if (strcmp("add", op) == 0){
            do_math(instruction, 0);
        }
        if (strcmp("sub", op) == 0){
            do_math(instruction, 1);
        }
        if (strcmp("and", op) == 0){
            do_logical(instruction, 00);
        }
        if (strcmp("or", op) == 0){
            do_logical(instruction, 01);
        }
        if (strcmp("xor", op) == 0){
            do_logical(instruction, 10);
        }
        if (strcmp("slt", op) == 0){
            do_lessthan(instruction);
        }
        if (strcmp("sll", op) == 0){
            do_shift(instruction, 1);
        }
        if (strcmp("sra", op) == 0){
            do_shift(instruction, 0);
        }
    }
    if (op_type == I_TYPE){
        if (strcmp("addi", op) == 0){
            do_add_imm(instruction);
        }
        if (strcmp("andi", op) == 0){
            do_logical_imm(instruction, 00);
        }
        if (strcmp("ori", op) == 0){
            do_logical_imm(instruction, 01);
        }
        if (strcmp("xori", op) == 0){
            do_logical_imm(instruction, 10);
        }
        if (strcmp("slti", op) == 0){
            do_shift_imm(instruction);
        }
    }

    if (op_type == MEM_TYPE){
        if (strcmp("lw", op) == 0) {
            do_load(instruction, 0);
        }
        if (strcmp("lb", op) == 0) {
             do_load(instruction, 1);
        }
        if (strcmp("sw", op) == 0) {
            do_store(instruction, 0);
        }
        if (strcmp("sb", op) == 0) {
            do_store(instruction, 1);
        }
    }
    if (op_type == U_TYPE){
        do_lui(instruction);
    }
}
