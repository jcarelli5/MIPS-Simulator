// Jonathan Carelli
// CDA 3103 - Fall 2013
// Final Project - MySPIM Simulator

#include "spimcore.h"

/* ALU */
void ALU(unsigned A,unsigned B,char ALUControl,unsigned *ALUresult,char *Zero)
{

// Performing ALU operations on A and B

	switch ((int) ALUControl){

	// add
	case 000:
		*ALUresult = A + B;
		break;

	// sub
	case 001:
		*ALUresult = A - B;
		break;

	// slt
	case 010:
		if ((signed)A < (signed)B)
			*ALUresult = 1;

		else
			*ALUresult = 0;

		break;

	// sltu
	case 011:
		if (A < B)
			*ALUresult = 1;

		else
			*ALUresult = 0;

		break;

	// and
	case 100:
		*ALUresult = A & B;
		break;

	// or
	case 101:
		*ALUresult = A | B;
		break;

	// sll by 16 bits
	case 110:
		B << 16;
		break;

	// not A
	case 111:
		*ALUresult = ~A;
		break;

	// end switch
	}

	// when ALUresult = 0, flag for ALU zero
	if(*ALUresult == 0)
		*Zero = 1;

	// not zero
	else
		*Zero = 0;
}

/* instruction fetch */
int instruction_fetch(unsigned PC,unsigned *Mem,unsigned *instruction)
{
	// finding the proper memory index
	unsigned memInd = PC >> 2;

	// checking for word allignment
   	 if (PC % 4 == 0) {
           *instruction = Mem[memInd];
           return 0;
           }

	// Halting if PC is not correct memory index format
    	else
		return 1;
}

/* instruction partition */
void instruction_partition(unsigned instruction, unsigned *op, unsigned *r1,unsigned *r2, unsigned *r3, unsigned *funct, unsigned *offset, unsigned *jsec)
{

// we set these values in order to assist with bit isolation
	unsigned rBits		= 0x1f;
	unsigned functOpBits	= 0x0000003f;
	unsigned offsetBits	= 0x0000ffff;
	unsigned jsecBits	= 0x03ffffff;

	// instruction [31-26]
	*op = (instruction >> 26) & functOpBits;

	// instruction [25-21]
	*r1 = (instruction >> 21) & rBits;

	// instruction [20-16]
	*r2 = (instruction >> 16) & rBits;

	// instruction [15-11]
	*r3 = (instruction >> 11) & rBits;

	// instruction [5-0]
	*funct = instruction & functOpBits;

	// instruction [15-0]
	*offset	= instruction & offsetBits;

	// instruction [25-0]
	*jsec = instruction & jsecBits;
}

/* instruction decode */
// Returns 1 if halt
int instruction_decode(unsigned op,struct_controls *controls)
{
	switch(op){

	// r-type instruction (add, sub, .. etc)
	case 0x0:
		controls->RegDst = 1;
		controls->Jump = 0;
		controls->Branch = 0;
		controls->MemRead = 0;
		controls->MemtoReg = 0;
		controls->ALUOp = 7;
		controls->MemWrite = 0;
		controls->ALUSrc = 0;
		controls->RegWrite = 1;
                break;

	// add-i instruction
	case 0x8:
		controls->RegDst = 0;
		controls->Jump = 0;
		controls->Branch = 0;
		controls->MemRead = 0;
		controls->MemtoReg = 0;
		controls->ALUOp = 0;
		controls->MemWrite = 0;
		controls->ALUSrc = 1;
		controls->RegWrite = 1;
		break;

	// slt-i instruction
	case 0xa:
		controls->RegDst = 0;
		controls->Jump = 0;
		controls->Branch = 0;
		controls->MemRead = 0;
		controls->MemtoReg = 0;
		controls->ALUOp = 2;
		controls->MemWrite = 0;
		controls->ALUSrc = 1;
		controls->RegWrite = 1;
		break;

	// sltu instruction
	case 0xb:
		controls->RegDst = 0;
		controls->Jump = 0;
		controls->Branch = 0;
		controls->MemRead = 0;
		controls->MemtoReg = 0;
		controls->ALUOp = 3;
		controls->MemWrite = 0;
		controls->ALUSrc = 1;
		controls->RegWrite = 1;
		break;

	// beq instruction
	case 0x4:
		controls->RegDst = 2;
		controls->Jump = 0;
		controls->Branch = 1;
		controls->MemRead = 0;
		controls->MemtoReg = 2;
		controls->ALUOp = 1;
		controls->MemWrite = 0;
		controls->ALUSrc = 0;
		controls->RegWrite = 0;
		break;

	// jump instruction
	case 0x2:
		controls->RegDst = 0;
		controls->Jump = 1;
		controls->Branch = 0;
		controls->MemRead = 0;
		controls->MemtoReg = 0;
		controls->ALUOp = 0;
		controls->MemWrite = 0;
		controls->ALUSrc = 0;
		controls->RegWrite = 0;
		break;

	// lw instruction
	case 0x23:
		controls->RegDst = 0;
		controls->Jump = 0;
		controls->Branch = 0;
		controls->MemRead = 1;
		controls->MemtoReg = 1;
		controls->ALUOp = 0;
		controls->MemWrite = 0;
		controls->ALUSrc = 1;
		controls->RegWrite = 1;
		break;

	// lui instruction
	case 0xf:
		controls->RegDst = 0;
		controls->Jump = 0;
		controls->Branch = 0;
		controls->MemRead = 0;
		controls->MemtoReg = 0;
		controls->ALUOp = 6;
		controls->MemWrite = 0;
		controls->ALUSrc = 1;
		controls->RegWrite = 1;
		break;

	// sw instruction
	case 0x2b:
		controls->RegDst = 2;
		controls->Jump = 0;
		controls->Branch = 0;
		controls->MemRead = 0;
		controls->MemtoReg = 2;
		controls->ALUOp = 0;
		controls->MemWrite = 1;
		controls->ALUSrc = 1;
		controls->RegWrite = 0;
		break;

	// if hault occurs
	default:
		return 1;
	}

	return 0;
}

/* Read Register */
void read_register(unsigned r1,unsigned r2,unsigned *Reg,unsigned *data1,unsigned *data2)
{
	*data1 = Reg[r1];
	*data2 = Reg[r2];
}


/* Sign Extend */
void sign_extend(unsigned offset,unsigned *extended_value)
{

	unsigned extend = 0xffff0000;
	unsigned extendNorm = 0x0000ffff;
	unsigned negCheck = offset >> 15;

	// If negative check, extend negative
	if (negCheck == 1)
		*extended_value = offset | extend;

	// else perform normal extend
	else
		*extended_value = offset & extendNorm;
}

/* ALU operations */
// Returns 1 if halt
int ALU_operations(unsigned data1,unsigned data2,unsigned extended_value,unsigned funct,char ALUOp,char ALUSrc,unsigned *ALUresult,char *Zero)
{
	if(ALUSrc == 1){
		data2 = extended_value;
	}

	// R Type instruction
	if(ALUOp == 7){

		switch(funct) {

			// adding
			case 0x20:
				ALUOp = 0;
				break;

			// subtracting
			case 0x22:
				ALUOp = 1;
				break;

			// slt
			case 0x2a:
				ALUOp = 2;
				break;

			// sltu
			case 0x2b:
				ALUOp = 3;
				break;

			// and logical
			case 0x24:
				ALUOp = 4;
				break;

			// or logical
			case 0x25:
				ALUOp = 5;
				break;

			// sll
			case 0x6:
				ALUOp = 6;
				break;

			// nor
			case 0x27:
				ALUOp = 7;
				break;

			// halt
			default:
				return 1;

		}

		// send to the ALU
		ALU(data1,data2,ALUOp,ALUresult,Zero);
	}

	// else, send to the ALU (not a funct)
	else
		ALU(data1,data2,ALUOp,ALUresult,Zero);

	return 0;
}

/* Read / Write Memory */
// Returns 1 if halt
int rw_memory(unsigned ALUresult,unsigned data2,char MemWrite,char MemRead,unsigned *memdata,unsigned *Mem)
{
	// checking if memRead (reading from memory)
	if (MemRead == 1) {
		if((ALUresult % 4) == 0)
			*memdata = Mem[ALUresult >> 2];

		// not a correct address -- halting
		else
			return 1;

	}

	// checking if memWrite (writing to memory)
	if (MemWrite == 1) {
		if((ALUresult % 4) == 0){
			Mem[ALUresult >> 2] = data2;
		}
		// not a correct address -- halting
		else
			return 1;

	}
             return 0;
}


/* Write Register */
void write_register(unsigned r2,unsigned r3,unsigned memdata,unsigned ALUresult,char RegWrite,char RegDst,char MemtoReg,unsigned *Reg)
{
	// Writing to register
	if(RegWrite == 1){

		if (MemtoReg == 1 && RegDst == 0) {
			Reg[r2] = memdata;
		}

		else if(MemtoReg == 1 && RegDst == 1){
			Reg[r3] = memdata;
		}

		else if (MemtoReg == 0 && RegDst == 0) {
			Reg[r2] = ALUresult;
		}

		else if (MemtoReg == 0 && RegDst == 1){
			Reg[r3] = ALUresult;
		}
	}
}

/* PC update */
void PC_update(unsigned jsec,unsigned extended_value,char Branch,char Jump,char Zero,unsigned *PC)
{

	// Incrementing count by 4
	*PC += 4;

	// add extend value when branching
	if(Zero == 1 && Branch == 1)
		*PC += extended_value << 2;

	// jump shift instruction case
	if(Jump == 1)
		*PC = (jsec << 2) | (*PC & 0xf0000000);
}

