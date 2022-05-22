#include <string.h>
#include "imports.h"

word get_opcode(char *s) {
	int i;
	for(i = 0; i < 3; i++) {
		if(!(strcmp(s, cmd[i].name))) {
			return cmd[i].opcode;
		}
	}
}

word rem = 2;

word get_first_second(char c, FILE *f) {
	word res = 0;
	word n;
	switch(c) {
	case '#':
		rem = max(4, rem);
		res = 027;
		fscanf(f, "%ho", &n);
		w_write(pc + 2, n);
		break;
	case 'R':
		rem = max(2, rem);
		fscanf(f, "%ho", &n);
		res = n;
		break;
	}
	return res;
}

void read_info() {
	FILE *f;
	f = fopen("read.txt", "r");
	char s[4];
	while (1) {
		rem = 2;
		word wtd = 0;	// what to do
		word first, second;
		fscanf(f, "%s", s);
		//printf("%06o\n", get_opcode(s));

		char c;
		while(1) {
			fscanf(f, "%c", &c);
			if(&c != " ") { break; }
		}
		fscanf(f, "%c", &c);
		first = get_first_second(c, f) << 6;

		while(1) {
			fscanf(f, "%c", &c);
			if(&c != " ") { break; }
		}
		fscanf(f, "%c", &c);
		second = get_first_second(c, f);

		wtd = wtd | get_opcode(s) | first | second;
		//printf("%06ho\n", wtd);
		w_write(pc, wtd);
		pc += rem;
		if(!get_opcode(s)) {
			break;
		}
	}
	fclose(f);
}
