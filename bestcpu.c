#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define ENTER printf("\n_____ %s %d ____\n",__func__,__LINE__);


/* CPU regiseters */
unsigned int R0;
unsigned int R1;
unsigned int R2;
unsigned int R3;
unsigned int R4;
unsigned int R5;
unsigned int R6;
unsigned int R7;
unsigned int R8;
unsigned int R9;
unsigned int R10;
unsigned int R11;
unsigned int R12;
unsigned int R13;
unsigned int R14;
unsigned int R15;

/* Temp registers */
unsigned int R16;
unsigned int R17;
unsigned int R18;
unsigned int R19;
unsigned int R20;
unsigned int R21;
unsigned int R22;
unsigned int R23;
unsigned int R24;
unsigned int R25;
unsigned int R26;
unsigned int R27;
unsigned int R28;
unsigned int R29;
unsigned int R30;
unsigned int R31;

/* Program Counter */
unsigned int pc;

/* Stack Pointer */
unsigned int sp;

/* Base Pointer */
unsigned int bp;

/* Memory block of 1024 bytes */
unsigned char memory[1024];

/* Flags Register */
unsigned int flag;

/* Bits position for Flag register */
enum flags{
     CF = 0, /* Carry Flag */
     PF = 2, /* Parity Flag */
     AF = 4, /* Adjust Flag */
     ZF = 6, /* Zero Flag */
     SF = 7, /* Sign Flag */
     TF = 8, /* Trap Flag */
     IF = 9, /* Interrupt Enable Flag */
     DF = 10, /* Direction Flag */
     OF = 11, /* Overflow Flag */
     IN   /*  Invalid */
};

unsigned char show_memory = 1;

/* Opcodes */
enum OPCODE{

	STORE_MEMORY 	= 3,
	LOAD_MEMORY 	= 4,
	ADD		= 5,
	SUB		= 6,
	MUL		= 7,
	DIV		= 8,
	MOD		= 9,
	PUSH	= 10,
	POP		= 11
};


#define STACK_BOTTOM 		896
#define STACK_TOP 			1024
#define STACK_MEMORY_BASE 	896
#define DATA_MEMORY_BASE	384
#define INSTR_MEMORY_BASE 	128
#define BOOT_MEMORY_BASE 	0

/* CPU Register index */ 
enum CPURegisterIndex{

	Index_R0 =0,	Index_R1,	Index_R2,	Index_R3,
	Index_R4,		Index_R5,	Index_R6,	Index_R7,
	Index_R8,		Index_R9,	Index_R10,	Index_R11,
	Index_R12,		Index_R13,	Index_R14,	Index_R15,

	Index_R16 = 16,	Index_R17,	Index_R18,	Index_R19,
	Index_R20,		Index_R21,	Index_R22,	Index_R23,
	Index_R24,		Index_R25,	Index_R26,	Index_R27,
	Index_R28,		Index_R29,	Index_R30,	Index_R31,

	Index_BP = 32,
	Index_SP = 33,
	Index_PC = 34,

	Index_INVALID
};


int print_cpu_state(void)
{
	int i;

	printf("\n*******************************************************\n");
	printf("\t\tGeneral Purpose Registers\n");
	printf("*******************************************************\n");
	printf("R0 = %u\t\tR1 = %u\t\tR2 = %u\t\tR3 = %u\n", R0, R1, R2, R3);
	printf("R4 = %u\t\tR5 = %u\t\tR6 = %u\t\tR7 = %u\n", R4, R5, R6, R7);
	printf("R8 = %u\t\tR9 = %u\t\tR10 = %u\t\tR11 = %u\n", R8, R9, R10, R11);
	printf("R12 = %u\t\tR13 = %u\t\tR14 = %u\t\tR15 = %u\n", R12, R12, R14, R15);

	printf("\n*******************************************************\n");
	printf("\tProgram Counter\n");
	printf("*******************************************************\n");
	printf("\tPC = %u\n", pc);


	printf("\n*******************************************************\n");
	printf("\tStack Pointer\n");
	printf("*******************************************************\n");
	printf("\tSP = %u\n", sp);

	printf("\n*******************************************************\n");
	printf("\tBase Pointer\n");
	printf("*******************************************************\n");
	printf("\tBP = %u\n", bp);

	printf("\n*******************************************************\n");
	printf("\tFlags\n");
	printf("*******************************************************\n");
	printf("\tFlags = %u\n", flag);

	if(show_memory)
	{
		printf("\n*******************************************************\n");
		printf("\tBootstrap Memory\n");
		printf("*******************************************************\n");
		
		printf("Address    \t\tMemory\n");
		
		for (i = 0; i < INSTR_MEMORY_BASE; i = i + 16)
		{
		    printf("%06d   %02X %02X %02X %02X  ", i, memory[i], memory[i+1], memory[i+2], memory[i+3]);
		    printf("%02X %02X %02X %02X  ", memory[i+4], memory[i+5], memory[i+6], memory[i+7]);
		    printf("%02X %02X %02X %02X  ", memory[i+8], memory[i+9], memory[i+10], memory[i+11]);
		    printf("%02X %02X %02X %02X  \n", memory[i+12], memory[i+13], memory[i+14], memory[i+15]);
		}

		printf("\n*******************************************************\n");
		printf("\tInstruction Memory\n");
		printf("*******************************************************\n");
		printf("Address    \t\tMemory\n");
		for (i = INSTR_MEMORY_BASE; i < DATA_MEMORY_BASE; i = i+16)
		{
		    printf("%06d   %02X %02X %02X %02X  ", i, memory[i], memory[i+1], memory[i+2], memory[i+3]);
		    printf("%02X %02X %02X %02X  ", memory[i+4], memory[i+5], memory[i+6], memory[i+7]);
		    printf("%02X %02X %02X %02X  ", memory[i+8], memory[i+9], memory[i+10], memory[i+11]);
		    printf("%02X %02X %02X %02X  \n", memory[i+12], memory[i+13], memory[i+14], memory[i+15]);
		}

		printf("\n*******************************************************\n");
		printf("\tData Memory\n");
		printf("*******************************************************\n");
		printf("Address    \t\tMemory\n");
		for (i = DATA_MEMORY_BASE; i < STACK_MEMORY_BASE; i = i+16)
		{
		    printf("%06d   %02X %02X %02X %02X  ", i, memory[i], memory[i+1], memory[i+2], memory[i+3]);
		    printf("%02X %02X %02X %02X  ", memory[i+4], memory[i+5], memory[i+6], memory[i+7]);
		    printf("%02X %02X %02X %02X  ", memory[i+8], memory[i+9], memory[i+10], memory[i+11]);
		    printf("%02X %02X %02X %02X  \n", memory[i+12], memory[i+13], memory[i+14], memory[i+15]);
		}

		printf("\n*******************************************************\n");
		printf("\tStack Memory\n");
		printf("*******************************************************\n");
		printf("Address    \t\tMemory\n");
		for (i = STACK_MEMORY_BASE; i < 1024; i = i+16)
		{
		    printf("%06d   %02X %02X %02X %02X  ", i, memory[i], memory[i+1], memory[i+2], memory[i+3]);
		    printf("%02X %02X %02X %02X  ", memory[i+4], memory[i+5], memory[i+6], memory[i+7]);
		    printf("%02X %02X %02X %02X  ", memory[i+8], memory[i+9], memory[i+10], memory[i+11]);
		    printf("%02X %02X %02X %02X  \n", memory[i+12], memory[i+13], memory[i+14], memory[i+15]);
		}

		
		    printf("\n\n");
	}
}

int readRegister(int reg)
{
	switch (reg)
	{ 
		case Index_R0: return R0;
		case Index_R1: return R1;
		case Index_R2: return R2;
		case Index_R3: return R3;
		case Index_R4: return R4;
		case Index_R5: return R5;
		case Index_R6: return R6;
		case Index_R7: return R7;
		case Index_R8: return R8;
		case Index_R9: return R9;
		case Index_R10: return R10;
		case Index_R11: return R11;
		case Index_R12: return R12;
		case Index_R13: return R13;
		case Index_R14: return R14;
		case Index_R15: return R15;
		case Index_R16: return R16;
		case Index_R17: return R17;
		case Index_R18: return R18;
		case Index_R19: return R19;
		case Index_R20: return R20;
		case Index_R21: return R21;
		case Index_R22: return R22;
		case Index_R23: return R23;
		case Index_R24: return R24;
		case Index_R25: return R25;
		case Index_R26: return R26;
		case Index_R27: return R27;
		case Index_R28: return R28;
		case Index_R29: return R29;
		case Index_R30: return R30;
		case Index_R31: return R31;
		case Index_SP: return sp;
		case Index_BP: return bp;
		case Index_PC: return pc;

		default: printf("$$$$$$$ read reg bug \n\n\n");break;
	}

	printf("invalid reg read for %d\n", reg);
}


void writeRegister(int reg, int value)
{
	switch(reg)
	{ 
		case Index_R0:			R0 = value;		break;
		case Index_R1:			R1 = value;		break;
		case Index_R2:			R2 = value;		break;
		case Index_R3:			R3 = value;		break;
		case Index_R4:			R4 = value;		break;
		case Index_R5:			R5 = value;		break;
		case Index_R6:			R6 = value;		break;
		case Index_R7:			R7 = value;		break;
		case Index_R8:			R8 = value;		break;
		case Index_R9:			R9 = value;		break;
		case Index_R10:			R10 = value;		break;
		case Index_R11:			R11 = value;		break;
		case Index_R12:			R12 = value;		break;
		case Index_R13:			R13 = value;		break;
		case Index_R14:			R14 = value;		break;
		case Index_R15:			R15 = value;		break;
		case Index_R16:			R16 = value;		break;
		case Index_R17:			R17 = value;		break;
		case Index_R18:			R18 = value;		break;
		case Index_R19:			R19 = value;		break;
		case Index_R20:			R20 = value;		break;
		case Index_R21:			R21 = value;		break;
		case Index_R22:			R22 = value;		break;
		case Index_R23:			R23 = value;		break;
		case Index_R24:			R24 = value;		break;
		case Index_R25:			R25 = value;		break;
		case Index_R26:			R26 = value;		break;
		case Index_R27:			R27 = value;		break;
		case Index_R28:			R28 = value;		break;
		case Index_R29:			R29 = value;		break;
		case Index_R30:			R30 = value;		break;
		case Index_R31:			R31 = value;		break;
		case Index_BP:			bp = value;		break;
		case Index_SP:			sp = value;		break;
		case Index_PC:			pc = value;		break;

		default: printf("$$$$$$$ read reg bug \n\n\n");break;
		break;

	}
		  
}

void alu_store(int src, int dest)
{
	int data = 0;
	unsigned char *ptr = memory;
	if (dest < 0) {
		printf("\n\t##Memory reserved for Bootstrap and Instruction Memory");
		return;
	}	
	
	data = readRegister(src);
	//printf("\n%s:data = %u\n",__FUNCTION__,data);
	
	*(int*)(ptr + DATA_MEMORY_BASE + dest) = data;
}

void alu_load(int src, int dest)
{
	int data = 0;
	unsigned char *ptr = memory;
	if (src < 0) {
		printf("\n\t##Memory reserved for Bootstrap and Instruction memory");
		return;
	}	
	data = *(int *)(ptr + DATA_MEMORY_BASE + src); //reading memory
	writeRegister(dest, data);
}

void alu_push(int src)
{
	int data = 0;
	unsigned char *ptr = memory;
	data = readRegister(src);
	sp -= 4;
	*(int*)(ptr + sp) = data;
}

void alu_pop(int dest)
{
	int data = 0;
	unsigned char *ptr = memory;
	data = *(int *)(ptr + sp); //reading memory
	writeRegister(dest, data);
	sp += 4;
}

void ALU(char opcode, int src, int dest)
{
	switch(opcode)
	{
	case STORE_MEMORY:
		alu_store(src, dest);
	break;
	case LOAD_MEMORY:
		alu_load(src, dest);
	break;
	case PUSH:
		alu_push(src);
	break;
	case POP:
		alu_pop(dest);
	break;
	default: 
		break;
	}

}

void usage()
{
	printf("\n*******************************************************\n");
	printf("\n\t##Please Enter the assembly instruction as"
		       				"mentioned below##");
	printf("\n\t##Don't Use First 100 Bytes are reserved for Bootstrap");
	printf("\n\t##  ld 150, R1  ##");
	printf("\n\t##Above Instruction fetches data from 150 memory" 
						"location to Register 1##");
	printf("\n\t##  st R2, 200  ##");
	printf("\n\t##Above Instruction fetches data from R2"
				"Register to 200 Memory Location##");
	printf("\n\t##  lr $50, R5  ##");
	printf("\n\t##Above Instruction load resgister R5 with 50 value");

	printf("\n*******************************************************\n");
	return ;
}

/*This function Returns index of a given register string*/
int returnIndex(char *ptr)
{
	char i = 0;
	i = atoi(++ptr);	
	
	return i;
}

void execute_one_instr(void)
{
	unsigned char opcode = 0, dst = 0, src = 0;
	opcode = memory[pc ];
	src = memory[pc + 1];
	dst = memory[pc + 2];

	printf("\nExecuting Instruction : %2X %2X %2X", opcode, src, dst);
	ALU(opcode, src, dst);

	pc = pc + 4;
	return;
}

void initALU(void)
{
	sp = STACK_TOP;
	pc = INSTR_MEMORY_BASE;
	writeRegister(Index_R5, 10);
}

int loader()
{
	int  i = 0, count = 0, ret = -1;
	size_t len = 0;
	unsigned char opcode = 0, dst = 0, src = 0;
	char *instr;
	FILE *file_fd = 0;
	char *p1 = NULL, *p2 = NULL, *p3 = NULL;

	instr = (char *) malloc(200);
	file_fd = fopen("./instructions.txt","r");

	while( (ret = getline(&instr, &len, file_fd)) != -1)
	{
		/*Here parse the OPCODE, OPERANDS and perform corresponding operations*/
		printf("\n\tInstruction = %s", instr);

		p1 = strtok(instr, " ");
		if (!strcasecmp(p1, "ld")) {
			p2 = strtok(NULL, ",");	     /* Here loads from memory location to register*/	
			src = atoi(p2) - DATA_MEMORY_BASE;
			p3 = strtok(NULL, " ");
			dst = returnIndex(p3);
			opcode = LOAD_MEMORY;
		} else if (!strcasecmp(p1, "st")) { 
			p2 = strtok(NULL, ",");		
			src = returnIndex(p2);
			p3 = strtok(NULL, " ");		/* Here loads from Register to location*/	
			dst = atoi(p3) - DATA_MEMORY_BASE;
			opcode =  STORE_MEMORY;
		} else if (!strcasecmp(p1, "push")) { 
			p2 = strtok(NULL, " ");
			if(*p2 == 'R')
				src = returnIndex(p2);
			else if (strstr(p2,"bp"))
				src = Index_BP;
			else if (strstr(p2,"sp"))
				src = Index_SP;
			else if (strstr(p2,"pc"))
				src = Index_PC;

			opcode =  PUSH;

			dst = Index_INVALID;

		} else if (!strcasecmp(p1, "pop")) { 
			p2 = strtok(NULL, " ");
			if(*p2 == 'R')
				dst = returnIndex(p2);
			else if (strstr(p2,"bp"))
				dst = Index_BP;
			else if (strstr(p2,"sp"))
				dst = Index_SP;
			else if (strstr(p2,"pc"))
				dst = Index_PC;

			opcode =  POP;

			src = Index_INVALID;

		} else {
			usage();
		}

		printf("\n\tDecoded inst : %u %u %u\n", opcode, src, dst);

		memory[INSTR_MEMORY_BASE + i] = opcode;
		i++;
		memory[INSTR_MEMORY_BASE + i] = src;
		i++;
		memory[INSTR_MEMORY_BASE + i] = dst;
		i++;

		i++; // last increment is to make it 32 bit word align

		count++;
		p1 = strtok(NULL, " ");
	}

	return count;
}

int main(int argc, char **argv)
{
	int count = 0;

	initALU();

	count = loader();

	printf("\n*******************************************************\n");
	printf("\tCPU State before execution\n");
	printf("*******************************************************\n");
	print_cpu_state();

	while(count > 0){

		execute_one_instr();

		printf("\n*******************************************************\n");
		printf("\tCPU State After execution of instruction\n");
		printf("*******************************************************\n");
		print_cpu_state();
		count --;
	}

}


