#include <stdlib.h>

#ifndef max
#define max(a, b)	((a > b) ? a : b)
#endif

typedef struct {
	word val;
	word adr;
	word mode;
} Arg;
Arg ss, dd;

byte R;
byte nn;
byte xx;
byte PSW = 0;
word ostat = 0177564;
word odata = 0177566;

void print_regs();
void do_halt() {
	printf("THE END!!!\n");
	print_regs();
	exit(0);
}
void do_mov() {
	if(dd.mode == 0) {
		reg[dd.adr] = ss.val;
	}
	else {
		w_write(dd.adr, ss.val);
	}
	PSW = 8 * (ss.val < 0) + 4 * (ss.val == 0) + 1 | PSW;
}
void do_add() {
	if(dd.mode == 0) {
		reg[dd.adr] = dd.val + ss.val;
		printf(" %o %o\n", dd.val, ss.val);
	}
	else {
		w_write(dd.adr, ss.val + dd.val);
	}
	PSW = 8 * (ss.val + dd.val < 0) + 4 * (ss.val + dd.val == 0) + (ss.val + dd.val >= 15);
}
void do_nothing() {}

void print_regs() {
	int i;
	for(i = 0; i < 8; i++) {
		printf("r%o:%o ", i, reg[i]);
	}
	printf("\n");
}

void tst(word w) {
	PSW = 8 * (w < 0) + 4 * (w == 0);
}
void tstb(byte b) {
	PSW = 8 * (b < 0) + 4 * (b == 0);
}
void cmp(word s, word d) {
	word w = s - d;
	PSW = 8 * (w < 0) + 4 * (w == 0) + (0000000);	//??????????
}
void cmpb(byte s, byte d) {
	word b = s - d;
	PSW = 8 * (b < 0) + 4 * (b == 0) + (0000000);	//??????????
}

void clN() {
	PSW = PSW & (0 * 8 + 1 * 4 + 1 * 2 + 1 * 1);
}
void clZ() {
	PSW = PSW & (1 * 8 + 0 * 4 + 1 * 2 + 1 * 1);
}
void clV() {
	PSW = PSW & (1 * 8 + 1 * 4 + 0 * 2 + 1 * 1);
}
void clC() {
	PSW = PSW & (1 * 8 + 1 * 4 + 1 * 2 + 0 * 1);
}

void seN() {
	PSW = PSW | (1 * 8 + 0 * 4 + 0 * 2 + 0 * 1);
}
void seZ() {
	PSW = PSW | (0 * 8 + 1 * 4 + 0 * 2 + 0 * 1);
}
void seV() {
	PSW = PSW | (0 * 8 + 0 * 4 + 1 * 2 + 0 * 1);
}
void seC() {
	PSW = PSW | (0 * 8 + 0 * 4 + 0 * 2 + 1 * 1);
}

void ccc() {
	PSW = PSW & (0 * 8 + 0 * 4 + 0 * 2 + 0 * 1);
}
void scc() {
	PSW = PSW | (1 * 8 + 1 * 4 + 1 * 2 + 1 * 1);
}
void NOP() {}

void get_nn() {
	nn = w_read(pc - 2) & 63;
}

void get_R() {
	R = (w_read(pc - 2) >> 6) & 7;
}

void get_xx() {
	xx = b_read(pc - 2);
	nn = xx & 0x0080;
	xx = xx & 0x00FF;
	if (xx > 128) {
		xx = 256 - xx;
	}
}

void do_br() {
	get_xx();
	printf("%o ", xx);
	if(nn) {
		pc -= 2 * xx;
	}
	else {
		pc += 2 * xx;
	}

}

void do_sob() {
	printf("%o %o", R, nn);
	reg[R] -= 1;
	if(reg[R] != 0) {
		pc -= 2 * nn;
	}
}

void CLR(word w) {
	w = w & (256 * 128 - 8);
}

Arg get_mr(word w) {
	Arg res;
	int X;
	int r = w & 7;
	int mode = (w >> 3) & 7;
	switch(mode) {
		case 0:		// R3
			res.adr = r;
			res.val = reg[r];
			printf("R%o ", r);
			break;
		case 1:		// (R3)
			res.adr = reg[r];
			res.val = w_read(res.adr);	// b_read
			printf("(R%o) ", r);
			break;
		case 2:		// (R3)+	#3
			res.adr = reg[r];
			res.val = w_read(res.adr);	// b_read
			reg[r] += 2;
			if (r == 7) {
				printf("#%o ", res.val);
			}
			else {
				printf("(R%o)+ ", r);
			}
			break;
		case 3:
			res.adr = reg[r];
			res.adr = mem[res.adr];
			res.val = mem[res.adr];
			reg[r] += 2;
			if (r == 7) {
				printf("@#%o ", res.val);
			}
			else {
				printf("@(R%o)+ ", r);
			}
			break;
		case 4:
			reg[r] -= 2;
			res.adr = reg[r];
			res.val = mem[res.adr];
			printf("-(R%o) ", r);
			break;
		case 5:
			reg[r] -= 2;
			res.adr = reg[r];
			res.val = mem[res.adr];
			printf("@-(R%o) ", r);
			break;
		case 6:
			X = mem[pc];
			pc += 2;
			res.adr = X + reg[r];
			res.val = mem[res.adr];
			if(r == 7) {
				printf("%o ", res.adr);
			}
			else {
				printf("%o(R%o) ", X, r);
			}
			break;
		case 7:
			X = mem[pc];
			pc += 2;
			res.adr = X + reg[r];
			res.adr = mem[res.adr];
			res.val = mem[res.adr];
			if(r == 7) {
				printf("@%o ", X + pc);
			}
			else {
				printf("@%o(R%o) ", X, r);
			}
			break;
		default:
			fprintf(stderr, "Mode %o not implemented yet\n", mode);
			exit(1);
	}
	return res;
}

typedef struct {
	word mask;
	word opcode;
	char *name;
	int params[5];	//	{ss, dd, R, nn, xx}
	void (*do_func)(void);
} Command;
Command cmd[] = {
	{0xFFFF, 000000, "halt", {0, 0, 0, 0, 0}, do_halt},
	{0xF000, 010000, "mov", {1, 1, 0, 0, 0}, do_mov},
	{0xF000, 060000, "add", {1, 1, 0, 0, 0}, do_add},
	{0xFE00, 077000, "sob", {0, 0, 1, 1, 0}, do_sob},
	{0xFFFF, 000250, "clN", {0, 0, 0, 0, 0}, clN},
	{0xFFFF, 000244, "clZ", {0, 0, 0, 0, 0}, clZ},
	{0xFFFF, 000242, "clV", {0, 0, 0, 0, 0}, clV},
	{0xFFFF, 000241, "clC", {0, 0, 0, 0, 0}, clC},
	{0xFFFF, 000257, "ccc", {0, 0, 0, 0, 0}, ccc},
	{0xFFFF, 000270, "seN", {0, 0, 0, 0, 0}, seN},
	{0xFFFF, 000264, "seZ", {0, 0, 0, 0, 0}, seZ},
	{0xFFFF, 000262, "seV", {0, 0, 0, 0, 0}, seV},
	{0xFFFF, 000261, "seC", {0, 0, 0, 0, 0}, seC},
	{0xFFFF, 000257, "scc", {0, 0, 0, 0, 0}, scc},
	{0xFF00, 000400, "br",	{0, 0, 0, 0, 1}, do_br},
	{0xFFFF, 0xFFFF, "unknown", {0, 0, 0, 0, 0}, do_nothing}
};
