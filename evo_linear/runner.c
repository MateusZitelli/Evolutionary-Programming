#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define MEM_SIZE 256
#define PROG_SIZE 20

/*INSTRUCTION SET
0x0 - mov inst mem
0x1 - mov mem mem
0x2 - add mem mem -> mem[addr]
0x3 - add mem[addr] inst -> mem[addr]
0x4 - sub mem mem -> mem[addr]
0x5 - sub mem[addr] inst -> mem[addr]
0x6 - mul mem mem -> mem[addr]
0x7 - mul mem[addr] inst -> mem[addr]
0x8 - div mem mem -> mem[addr]
0x9 - div mem[addr] inst -> mem[addr]
0xa - jne inst mem inst
0xb - jne mem mem inst
0xc - je inst mem inst
0xd - je mem mem inst
0xe - and inst mem -> mem[addr]
0xf - and mem mem -> mem[addr]
0x10 - or inst mem -> mem[addr]
0x11 - or mem mem -> mem[addr]
0x12 - xor inst mem -> mem[addr]
0x13 - xor mem mem -> mem[addr]
0x14 - mov inst addr
0x15 - mov mem addr
0x16 - inc addr
0x17 - dec addr
0x18 - mov input mem[addr]
0x19 - mov mem[addr] output
0x1a - inc mem[addr]
0x1b - dec mem[addr]
0x1c - mov mem[addr], ip
0x1d - mov 0, ip
0x1e - mov inst, ip
0x1f - inc mem[addr]
0x20 - dec mem[addr]
...
0xff - nop
*/
typedef unsigned char uint8;
struct program {
	uint8 ip;
	uint8 instructs[MEM_SIZE];
	uint8 addr;
	uint8 mem[MEM_SIZE];
	uint8 input;
	uint8 output;
	uint8 op;
	long instrs;
	int init;
};

void step(struct program *prog)
{
#if 0
	printf("#####STEP#####\n");
	printf("I = [...0x%x, 0x%x, >>0x%x<<, 0x%x, 0x%x...]\n",
	       prog->instructs[prog->ip - 2], prog->instructs[prog->ip - 1],
	       prog->instructs[prog->ip], prog->instructs[prog->ip + 1],
	       prog->instructs[prog->ip + 2]);
	printf("IP = 0x%x | ADDR = 0x%x\n", prog->ip, prog->addr);
	printf("M = [...0x%x, 0x%x, >>0x%x<<, 0x%x, 0x%x...]\n",
	       prog->mem[prog->addr - 2], prog->mem[prog->addr - 1],
	       prog->mem[prog->addr], prog->mem[prog->addr + 1],
	       prog->mem[prog->addr + 2]);
#endif
	++prog->instrs;
	switch (prog->instructs[prog->ip] % 0x20) {
	case 0x0:
	  printf("MOV mem[%i], %i\n", prog->addr, prog->instructs[prog->ip + 1]);
		prog->mem[prog->addr] = prog->instructs[prog->ip + 1];
		prog->ip += 2;
		break;
	case 0x1:
	  printf("MOV mem[%i], mem[%i]\n", prog->addr, prog->instructs[prog->ip + 1]);
		prog->mem[prog->addr] =
		    prog->mem[prog->instructs[prog->ip + 1]];
		prog->ip += 2;
		break;
	case 0x2:
	  printf("ADD mem[%i], mem[%i]\n", prog->addr, prog->instructs[prog->ip + 1]);
		prog->mem[prog->addr] +=
		    prog->mem[prog->instructs[prog->ip + 1]];
		prog->ip += 2;
		break;
	case 0x3:
	  printf("ADD mem[%i], %i\n", prog->addr, prog->instructs[prog->ip + 1]);
		prog->mem[prog->addr] += prog->instructs[prog->ip + 1];
		prog->ip += 2;
		break;
	case 0x4:
	  printf("SUB mem[%i], mem[%i]\n", prog->addr, prog->instructs[prog->ip + 1]);
		prog->mem[prog->addr] -=
		    prog->mem[prog->instructs[prog->ip + 1]];
		prog->ip += 2;
		break;
	case 0x5:
	  printf("SUB mem[%i], %i\n", prog->addr, prog->instructs[prog->ip + 1]);
		prog->mem[prog->addr] -= prog->instructs[prog->ip + 1];
		prog->ip += 2;
		break;
	case 0x6:
	  printf("MUL mem[%i], mem[%i]\n", prog->addr, prog->instructs[prog->ip + 1]);
		prog->mem[prog->addr] *=
		    prog->mem[prog->instructs[prog->ip + 1]];
		prog->ip += 2;
		break;
	case 0x7:
	  printf("MUL mem[%i], %i\n", prog->addr, prog->instructs[prog->ip + 1]);
		prog->mem[prog->addr] *= prog->instructs[prog->ip + 1];
		prog->ip += 2;
		break;
	case 0x8:
	  printf("DIV mem[%i], mem[%i]\n", prog->addr, prog->instructs[prog->ip + 1]);
		if (prog->mem[prog->instructs[prog->ip + 1]] != 0) {
			prog->mem[prog->addr] /= prog->mem[prog->
							   instructs[prog->ip +
								     1]];
		} else {
			prog->mem[prog->addr] = 0;
		}
		prog->ip += 2;
		break;
	case 0x9:
	  printf("DIV mem[%i], %i\n", prog->addr, prog->instructs[prog->ip + 1]);
		if (prog->instructs[prog->ip + 1] != 0) {
			prog->mem[prog->addr] /= prog->instructs[prog->ip + 1];
		} else {
			prog->mem[prog->addr] = 0;
		}
		prog->ip += 2;
		break;
	case 0xA:
	  printf("JNE mem[%i], %i, %i\n", prog->addr, prog->instructs[prog->ip + 1], prog->instructs[prog->ip + 2] % PROG_SIZE);
		if (prog->instructs[prog->ip + 1] != prog->mem[prog->addr])
			prog->ip = prog->instructs[prog->ip + 2] % PROG_SIZE;
		prog->instructs[prog->ip + 2] %= PROG_SIZE;
		prog->ip += 3;
		break;
	case 0xB:
	  printf("JNE mem[%i], mem[%i], %i\n", prog->addr, prog->instructs[prog->ip + 1], prog->instructs[prog->ip + 2] % PROG_SIZE);
		if (prog->mem[prog->addr] !=
		    prog->mem[prog->instructs[prog->ip + 1]])
			prog->ip = prog->instructs[prog->ip + 2] % PROG_SIZE;
		prog->instructs[prog->ip + 2] %= PROG_SIZE;
		prog->ip += 3;
		break;
	case 0xC:
	  printf("JE mem[%i], %i, %i\n", prog->addr, prog->instructs[prog->ip + 1], prog->instructs[prog->ip + 2] % PROG_SIZE);
		if (prog->instructs[prog->ip + 1] == prog->mem[prog->addr])
			prog->ip = prog->instructs[prog->ip + 2] % PROG_SIZE;
		prog->instructs[prog->ip + 2] %= PROG_SIZE;
		prog->ip += 3;
		break;
	case 0xD:
	  printf("JE mem[%i], mem[%i], %i\n", prog->addr, prog->instructs[prog->ip + 1], prog->instructs[prog->ip + 2] % PROG_SIZE);
		if (prog->mem[prog->addr] ==
		    prog->mem[prog->instructs[prog->ip + 1]])
			prog->ip = prog->instructs[prog->ip + 2] % PROG_SIZE;
		prog->instructs[prog->ip + 2] %= PROG_SIZE;
		prog->ip += 3;
		break;
	case 0xE:
	  printf("AND mem[%i], mem[%i]\n", prog->addr, prog->instructs[prog->ip + 1]);
		prog->mem[prog->addr] &=
		    prog->mem[prog->instructs[prog->ip + 1]];
		prog->ip += 2;
		break;
	case 0xF:
	  printf("AND mem[%i], %i\n", prog->addr, prog->instructs[prog->ip + 1]);
		prog->mem[prog->addr] &= prog->instructs[prog->ip + 1];
		prog->ip += 2;
		break;
	case 0x10:
	  printf("OR mem[%i], mem[%i]\n", prog->addr, prog->instructs[prog->ip + 1]);
		prog->mem[prog->addr] |=
		    prog->mem[prog->instructs[prog->ip + 1]];
		prog->ip += 2;
		break;
	case 0x11:
	  printf("OR mem[%i], %i\n", prog->addr, prog->instructs[prog->ip + 1]);
		prog->mem[prog->addr] |= prog->instructs[prog->ip + 1];
		prog->ip += 2;
		break;
	case 0x12:
	  printf("OR mem[%i], %i\n", prog->addr, prog->instructs[prog->ip + 1]);
		prog->mem[prog->addr] |= prog->instructs[prog->ip + 1];
		prog->ip += 2;
		break;
	case 0x13:
	  printf("CPL mem[%i]\n", prog->addr, prog->instructs[prog->ip + 1]);
		prog->mem[prog->addr] = 256 - prog->mem[prog->addr];
		prog->ip += 2;
		break;
	case 0x14:
	  printf("MOV addr, %i\n", prog->instructs[prog->ip + 1]);
		prog->addr = prog->instructs[prog->ip + 1];
		prog->ip += 2;
		break;
	case 0x15:
	  printf("MOV addr, mem[%i]\n", prog->instructs[prog->ip + 1]);
		prog->addr = prog->mem[prog->instructs[prog->ip + 1]];
		prog->ip += 2;
		break;
	case 0x16:
	  printf("INC addr\n");
		++prog->addr;
		++prog->ip;
		break;
	case 0x17:
	  printf("DEC addr\n");
		--prog->addr;
		++prog->ip;
		break;
	case 0x18:
	  printf("MOV mem[%i], INPUT\n", prog->addr);
		prog->mem[prog->addr] = prog->input;
		++prog->ip;
		break;
	case 0x19:
	  printf("MOV OUTPUT, mem[%i]\n", prog->addr);
		prog->output = prog->mem[prog->addr];
		prog->op = 1;
		++prog->ip;
		break;
	case 0x1a:
	  printf("INC mem[%i]\n", prog->addr);
		++prog->mem[prog->addr];
		++prog->ip;
		break;
	case 0x1b:
	  printf("DEC mem[%i]\n", prog->addr);
		--prog->mem[prog->addr];
		++prog->ip;
		break;
	case 0x1c:
	  printf("MOV IP, mem[%i]\n", prog->addr);
		prog->ip = prog->mem[prog->addr];
		break;
	case 0x1d:
	  printf("MOV IP, 0\n");
		prog->ip = 0;
		break;
	case 0x1e:
	  printf("MOV IP, %i\n", prog->instructs[prog->ip + 1]);
		prog->ip = prog->instructs[prog->ip + 1];
		prog->instructs[prog->ip + 1] %= PROG_SIZE;
		break;
  case 0x1f:
    printf("INC mem[%i]\n", prog->addr);
    ++prog->mem[prog->addr];
    ++prog->ip;
    break;
  case 0x20:
    printf("SUB mem[%i]\n", prog->addr);
    --prog->mem[prog->addr];
    ++prog->ip;
    break;
	default:
		++prog->ip;
		break;
	}
}

void init_prog(struct program *prog)
{
	prog->ip = 0;
	prog->instrs = 0;
	prog->input = 0;
	prog->output = 0;
	prog->op = 0;
	for (int i = 0; i < 256; i++) {
		prog->mem[i] = 0;
	}
	prog->init = -1;
	prog->addr = 0;
}

void run(struct program *prog, uint8 input)
{
	init_prog(prog);
	prog->input = input;
	for (long int i = 0; i < 0xff; i++) {
		step(prog);
		if (prog->op || prog->ip >= PROG_SIZE - 1)
			break;
	}
}

int main(void){
  struct program prog;
  uint8 initProg[] ={
0x78,0x94,0x34,0x7,0x40,0xd,0xfe,0xb,0xba,0x7b,0x98,0xa7,0xfb,0x2,0xef,0x11,0xfc,0xf2,0xb9,0xe8
  };
  for(int i = 0; i < sizeof(initProg); i++){
    prog.instructs[i] = initProg[i];
  }
  for(int i = 0; i < 10; i++){
    printf("########### INPUT = %i ################\n", i);
    run(&prog, i);
    printf("OUT = %i\n", prog.output);
  }
  return(0);
}
