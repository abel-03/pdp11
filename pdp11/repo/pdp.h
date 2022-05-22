#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <assert.h>

typedef unsigned char byte;
typedef unsigned short int word;
typedef word Address;
#define MEMSIZE (1024 * 64)

byte mem[MEMSIZE];

word reg[8];		// registers R0,...,R7
#define pc reg[7]

void b_write(Address adr, byte b);	// пишем байт b по адресу adr
byte b_read(Address adr);		// читаем байт по адресу adr
void w_write(Address adr, word w);	// пишем слово w по адресу adr
word w_read(Address adr);		// читаем слово по адресу adr

void test_mem() {
	byte b0 = 0x0a;
	b_write(2, b0);
	byte bres = b_read(2);
	printf("%02hhx = %02hhx\n", b0, bres);
	assert(b0 == bres);

	Address a = 4;
	byte b1 = 0xcb;
	b0 = 0x0a;
	word w = 0xcb0a;
	b_write(a, b0);
	b_write(a + 1, b1);
	word wres = w_read(a);
	printf("ww/br \t %04hx=%02hhx%02hhx\n", wres, b1, b0);
	assert(w == wres);

	w_write(a, w);
	printf("%04hx=%02hhx%02hhx\n", w, mem[a + 1], mem[a]);
	wres = w_read(a);
	assert(w == wres);
}

void _test() {
	reg[0] = 2;
	reg[1] = 3;
	w_write(01000, 060001);
}

void test_sob() {
	//	remember to put pc = 1006
	w_write(0, 5);
	w_write(02, 6);
	w_write(04, 7);
	w_write(01006, 012701);
	w_write(01010, 4);
	w_write(01012, 012203);
	w_write(01014, 060300);
	w_write(01016, 077103);
	w_write(01020, 000000);
}

/*
int main() {
	test_mem();
	//system("pause");
	return 0;
}
*/

void b_write(Address adr, byte b) {
	mem[adr] = b;
}
byte b_read(Address adr) {
	return mem[adr];
}
void w_write(Address adr, word w) {
	mem[adr] = w | mem[adr];
	w = w >> 8;
	mem[adr + 1] = w | mem[adr + 1];
}
word w_read(Address adr) {
	word w = ((word)mem[adr + 1]) << 8;
	w = w | mem[adr];
	return w;
}
