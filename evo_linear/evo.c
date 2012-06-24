#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define MEM_SIZE 256
#define CROSSING_OVER_RATE 3	//0~100
#define MUTATION_RATE 1
#define PRG_SIZE 30
#define BEST_LIST_SIZE 10
#define INPUTS 15

int POP_SIZE = 100000;
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
	uint8 instructs[PRG_SIZE + 2];
	uint8 addr;
	uint8 mem[MEM_SIZE];
	uint8 input;
	uint8 output;
	uint8 op;
	long instrs;
	int init;
	int p;
};

void init_prog(struct program *prog)
{
	prog->ip = 0;
	prog->instrs = 0;
	prog->input = 0;
	prog->output = 0;
	prog->op = 0;
	if (prog->init != -1) {
		for (int i = 0; i < PRG_SIZE + 2; i++) {
			prog->instructs[i] = rand() % 256;
			prog->mem[i] = 0;
		}
	} else {
		for (int i = 0; i < 256; i++) {
			prog->mem[i] = 0;
		}
	}
	prog->init = -1;
	prog->addr = 0;
}

void step(struct program *prog)
{
#if 0
	printf("#####STEP#####\n");
	printf("I = [...%i, %i, >>%i<<, %i, %i...]\n",
	       prog->instructs[prog->ip - 2], prog->instructs[prog->ip - 1],
	       prog->instructs[prog->ip], prog->instructs[prog->ip + 1],
	       prog->instructs[prog->ip + 2]);
	printf("IP = %i | ADDR = %i\n", prog->ip, prog->addr);
	printf("M = [...%i, %i, >>%i<<, %i, %i...]\n",
	       prog->mem[prog->addr - 2], prog->mem[prog->addr - 1],
	       prog->mem[prog->addr], prog->mem[prog->addr + 1],
	       prog->mem[prog->addr + 2]);
#endif
	++prog->instrs;
	switch (prog->instructs[prog->ip] % 0x20) {
	case 0x0:
		prog->mem[prog->addr] = prog->instructs[prog->ip + 1];
		prog->ip += 2;
		break;
	case 0x1:
		prog->mem[prog->addr] =
		    prog->mem[prog->instructs[prog->ip + 1]];
		prog->ip += 2;
		break;
	case 0x2:
		prog->mem[prog->addr] +=
		    prog->mem[prog->instructs[prog->ip + 1]];
		prog->ip += 2;
		break;
	case 0x3:
		prog->mem[prog->addr] += prog->instructs[prog->ip + 1];
		prog->ip += 2;
		break;
	case 0x4:
		prog->mem[prog->addr] -=
		    prog->mem[prog->instructs[prog->ip + 1]];
		prog->ip += 2;
		break;
	case 0x5:
		prog->mem[prog->addr] -= prog->instructs[prog->ip + 1];
		prog->ip += 2;
		break;
	case 0x6:
		prog->mem[prog->addr] *=
		    prog->mem[prog->instructs[prog->ip + 1]];
		prog->ip += 2;
		break;
	case 0x7:
		prog->mem[prog->addr] *= prog->instructs[prog->ip + 1];
		prog->ip += 2;
		break;
	case 0x8:
		if (prog->mem[prog->instructs[prog->ip + 1]] != 0) {
			prog->mem[prog->addr] /= prog->
			    mem[prog->instructs[prog->ip + 1]];
		} else {
			prog->mem[prog->addr] = 0;
		}
		prog->ip += 2;
		break;
	case 0x9:
		if (prog->instructs[prog->ip + 1] != 0) {
			prog->mem[prog->addr] /= prog->instructs[prog->ip + 1];
		} else {
			prog->mem[prog->addr] = 0;
		}
		prog->ip += 2;
		break;
	case 0xA:
		if (prog->instructs[prog->ip + 1] != prog->mem[prog->addr])
			prog->ip = prog->instructs[prog->ip + 2] % PRG_SIZE;
		prog->instructs[prog->ip + 2] %= PRG_SIZE;
		prog->ip += 3;
		break;
	case 0xB:
		if (prog->mem[prog->addr] !=
		    prog->mem[prog->instructs[prog->ip + 1]])
			prog->ip = prog->instructs[prog->ip + 2] % PRG_SIZE;
		prog->instructs[prog->ip + 2] %= PRG_SIZE;
		prog->ip += 3;
		break;
	case 0xC:
		if (prog->instructs[prog->ip + 1] == prog->mem[prog->addr])
			prog->ip = prog->instructs[prog->ip + 2] % PRG_SIZE;
		prog->instructs[prog->ip + 2] %= PRG_SIZE;
		prog->ip += 3;
		break;
	case 0xD:
		if (prog->mem[prog->addr] ==
		    prog->mem[prog->instructs[prog->ip + 1]])
			prog->ip = prog->instructs[prog->ip + 2] % PRG_SIZE;
		prog->instructs[prog->ip + 2] %= PRG_SIZE;
		prog->ip += 3;
		break;
	case 0xE:
		prog->mem[prog->addr] &=
		    prog->mem[prog->instructs[prog->ip + 1]];
		prog->ip += 2;
		break;
	case 0xF:
		prog->mem[prog->addr] &= prog->instructs[prog->ip + 1];
		prog->ip += 2;
		break;
	case 0x10:
		prog->mem[prog->addr] |=
		    prog->mem[prog->instructs[prog->ip + 1]];
		prog->ip += 2;
		break;
	case 0x11:
		prog->mem[prog->addr] ^=
		    prog->mem[prog->instructs[prog->ip + 1]];
		prog->ip += 2;
		break;
	case 0x12:
		prog->mem[prog->addr] = 256 - prog->mem[prog->addr];
		//prog->mem[prog->addr] ^=
		//    prog->mem[prog->instructs[prog->ip + 1]];
		prog->ip += 1;
		break;
	case 0x1010:
		prog->mem[prog->addr] ^= prog->instructs[prog->ip + 1];
		prog->ip += 2;
		break;
	case 0x14:
		prog->addr = prog->instructs[prog->ip + 1];
		prog->ip += 2;
		break;
	case 0x15:
		prog->addr = prog->mem[prog->instructs[prog->ip + 1]];
		prog->ip += 2;
		break;
	case 0x16:
		++prog->addr;
		++prog->ip;
		break;
	case 0x17:
		--prog->addr;
		++prog->ip;
		break;
	case 0x18:
		prog->mem[prog->addr] = prog->input;
		++prog->ip;
		break;
	case 0x19:
		prog->output = prog->mem[prog->addr];
		prog->op = 1;
		++prog->ip;
		break;
	case 0x1a:
		++prog->mem[prog->addr];
		++prog->ip;
		break;
	case 0x1b:
		--prog->mem[prog->addr];
		++prog->ip;
		break;
	case 0x1c:
		prog->ip = prog->mem[prog->addr];
		break;
	case 0x1d:
		prog->ip = 0;
		break;
	case 0x1e:
		prog->ip = prog->instructs[prog->ip + 1];
		prog->instructs[prog->ip + 1] %= PRG_SIZE;
		break;
	case 0x1f:
		++prog->mem[prog->addr];
		++prog->ip;
		break;
	case 0x20:
		--prog->mem[prog->addr];
		++prog->ip;
		break;
	default:
		++prog->ip;
		break;
	}
}

void run(struct program *prog, uint8 input)
{
	init_prog(prog);
	prog->input = input;
	for (long int i = 0; i < PRG_SIZE * 5; i++) {
		step(prog);
		if (prog->op || prog->ip >= PRG_SIZE - 1)
			break;
	}
}

double fitness(struct program *prog, uint8 input, uint8 desired_output)
{
	run(prog, input);
	double err =
	    sqrt((prog->output - desired_output) * (prog->output -
						    desired_output));
	return err + prog->instrs / 50000.0;
}

void mutate(struct program *receiver, struct program *p1, struct program *p2)
{
	uint8 newinstrs[256];
	uint8 divpos[256];
	int divs = 0;
	for (int i = 0; i < PRG_SIZE + 2; i++) {
		if (rand() % 100000 / 100000.0 * 100 <
		    receiver->instructs[PRG_SIZE - 1] / 10.0)
			divpos[divs++] = i;
	}
	divpos[divs++] = PRG_SIZE + 1;
	for (int i = 0; i < divs; i += 2) {
		if (i == 0) {
			for (int j = 0; j < divpos[i]; j++) {
				newinstrs[j] = p1->instructs[j];
			}
		} else {
			for (int j = divpos[i]; j < divpos[i + 1]; j++) {
				newinstrs[j] = p1->instructs[j];
			}
		}
		if (i + 2 < divs) {
			for (int j = divpos[i + 1]; j < divpos[i + 2]; j++) {
				newinstrs[j] = p2->instructs[j];
			}
		}
	}
	for (int i = 0; i < PRG_SIZE + 2; i++) {
		if (rand() % 100000 / 100000.0 * 100 <
		    receiver->instructs[PRG_SIZE - 2] / 100.0 + 0.001) {
			receiver->instructs[i] = rand() % 0xFF;
		} else {
			receiver->instructs[i] = newinstrs[i];
		}
	}
}

void sort(double min[], struct program *best[], double fit,
	  struct program *prog)
{
	int i, j;
	for (i = 0; i < BEST_LIST_SIZE; i++) {
		if (min[i] > fit)
			break;
	}
	for (j = BEST_LIST_SIZE - 1; j > i; j--) {
		min[j] = min[j - 1];
		best[j] = best[j - 1];
	}
	if (i < BEST_LIST_SIZE) {
		min[i] = fit;
		best[i] = prog;
	}
}

int main(void)
{
	FILE *mp = fopen("mut_data", "w+");
	FILE *fp = fopen("f_data", "w+");
	struct program *prog;
	prog = (struct program *)malloc(sizeof(struct program) * POP_SIZE);
	srand(time(0));
	uint8 inp[256], out[256];

	int i;
	for (i = 0; i < POP_SIZE; i++)
		prog[i].p = i;
	for (i = 0; i < INPUTS; i++) {
		inp[i] = i;
		out[i] = i * (i + 1) / 2;
	}
	for (int k = 0; k < 5000000; k++) {
		double min[BEST_LIST_SIZE], fit;
		for (i = 0; i < BEST_LIST_SIZE; i++)
			min[i] = 10000;
		struct program *best[BEST_LIST_SIZE];
		for (i = 0; i < POP_SIZE; i++) {
			fit = 0;
			for (int j = 0; j < INPUTS; j++) {
				fit += fitness(&prog[i], inp[j], out[j]);
			}
			fit /= INPUTS;
			sort(min, best, fit, &prog[i]);
			//if(i % POP_SIZE / 10 == 0) printf("%f%\n", (double)i / POP_SIZE * 100);
		}
		if (k % 30 == 0) {
			printf("####### Generation: %i ######\n", k);
			for (int p = 0;
			     p < (BEST_LIST_SIZE < 3 ? BEST_LIST_SIZE : 3);
			     p++) {
				printf("\n");
				printf("%i th BEST: Fitness %f\n", p + 1,
				       min[p]);
				printf("NORMAL\tOUTPUT = ");
				for (int inp = 0; inp < 14; inp++) {
					run(best[p], inp);
					printf("%i:%i|", inp, best[p]->output);
				}
				printf("\n");
				printf("\nGEN \tCODE = ");
				for (i = 0; i < PRG_SIZE + 2; i++) {
					printf("0x%x,", best[p]->instructs[i]);
				}
				printf("\nCO prob = %f \t MUT prob = %f\n",
				       best[p]->instructs[PRG_SIZE - 1] / 10.0,
				       best[p]->instructs[PRG_SIZE - 2] / 10.0);
				printf("\n");
			}
		}
		float sum0 = 0, sum1 = 0;
		for (int p = 0; p < BEST_LIST_SIZE; p++) {
			prog[p] = *best[p];
			sum0 += best[p]->instructs[PRG_SIZE - 1] / 10.0;
			sum1 += best[p]->instructs[PRG_SIZE - 2] / 10.0;
		}
		fprintf(mp, "%f, %f\n", sum0, sum1);
		fprintf(fp, "%f\n", min[0]);
		int con, ind;
		for (i = 0; i < POP_SIZE; i++) {
			con = 0;
			ind = rand() % BEST_LIST_SIZE;
			for (int oind = 0; oind < BEST_LIST_SIZE; oind++) {
				if (prog[i].p == best[oind]->p) {
					con = 1;
					break;
				}
			}
			if (con)
				continue;
			if (rand() % 2) {
				mutate(&prog[i], best[ind],
				       &prog[rand() % POP_SIZE]);
			} else {
				mutate(&prog[i], &prog[rand() % POP_SIZE],
				       best[ind]);
			}
		}
		for (i = POP_SIZE / 100 * 96; i < POP_SIZE; i++) {
			con = 0;
			for (int oind = 0; oind < BEST_LIST_SIZE; oind++) {
				if (prog[i].p == best[oind]->p) {
					con = 1;
					break;
				}
			}
			if (con)
				continue;
			prog[i].init = 10;
		}
		POP_SIZE = 500;
	}
	fclose(fp);
}
