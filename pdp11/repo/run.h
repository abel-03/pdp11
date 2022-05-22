#include "pdp.h"
#include <stdio.h>
#include "read_info.h"
//#include "/home/saro/pdp11/test/my_test/tests.h"


void run() {
	pc = 01000;
	while(1) {
		word w = w_read(pc);
		printf("%06o %06o:\t", pc, w);
		pc += 2;
		int i;
		for(i = 0; i < 15; i++) {
			if ((w & cmd[i].mask) == cmd[i].opcode) {
				printf("%s ", cmd[i].name);
				if (cmd[i].params[0]) {
					ss = get_mr(w >> 6);
				}
				if (cmd[i].params[1]) {
					dd = get_mr(w);
					dd.mode = (w >> 3) & 7;
				}
				if (cmd[i].params[2]) {
					get_R();
				}
				if (cmd[i].params[3]) {
					get_nn();
				}
				if (cmd[i].params[4]) {
					get_xx();
				}
				cmd[i].do_func();
				printf("\n");
				break;
			}
		}
		print_regs();
	}
}
