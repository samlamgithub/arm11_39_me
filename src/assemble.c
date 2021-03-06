#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <stdint.h>
#include <string.h>

int lineofout;
int sourceline = 0;
int acc = 0;
int objectcode[50];
char *sourcecode[50];

_Bool readbit(int, int);
int takedeximal(char[], int);
int takehex(char *, int);
int map(char*);
int getlength(char[]);
void boot();
void firstpass();
void secondpass();
_Bool isinstruc(int);
void insertLabel(char[], int);
int power(int, int);
void datapro(char[]);
int typeofinstrc(int);
void lsl(char[]);
void andeq(char[]);
int ror(int, int);
void branch(char[]);
void sdTrans(char[]);
void asseminstrc(int);
void mul(char[]);
int seto2rnrd(char[], int, int);
int mapchartodecimal(char);
int setBit(int, int);
void reader(char *);
void writter(char *);
void list_init();
int imm1(int);
int imm2(int);
int takexpree(char*, int);
int calrs(int);
int calrm(int);
ssize_t getline(char **lineptr, size_t *n, FILE *stream);
int getlen(char *);
int takeint(int, int, int);
int seto2(int, int, char *, char *, char *, char *);
int typeofdatapro(char[]);
int dataprosetcond(int);
int dataprosetmov(int inst);
int dataprosetcmp(int inst);
int dataprosettst(int inst);
int dataprosetteq(int inst);
int dataprosetand(int inst);
int dataprosetadd(int inst);
int dataproseteor(int inst);
int dataprosetorr(int inst);
int dataprosetsub(int inst);
int dataprosetrsb(int inst);
int setrn(int inst, int rn);
int setrd(int inst, int rd);
char *symbol_to_mov_instruction(char *);
void ldr_equ(char *ins);
int offset_to_binary_in_int(int offset, int instrc);
int takeint(int, int, int);
int setBit2(int, int);
int asr(int, int);
int getOff(char *);
int getV(char *);
_Bool useMov(char *);
int extra;
void ldrdirect(char *);

//void list_destroy(struct list *);

struct list maplist; // symbol table

int main(int argc, char **argv) {
	printf("%s", "start\n");

	char *point = argv[1];

	reader(point); // read file

	//read complete
	extra = 0;
	// start assemble
	list_init(&maplist); //initialise symbol table
	firstpass();

	secondpass();

	//end assemble

	char *pointer = argv[2];

	writter(pointer); // write file

	//list_destroy(&maplist); //free memory occupied by list

	printf("%s", "end\n");
	return 0;
	// complete write

	//return 0;
}

void reader(char *pf) {
	printf("read begin\n");
	FILE *pfile = fopen(pf, "r"); // read source file
	if (pfile == NULL) {
		perror("Error opening file!");
		exit(EXIT_FAILURE);
	}
	int i = 0;
	while (!feof(pfile)) {
		char *line = NULL;
		size_t num = 51;
		getline(&line, &num, pfile);
		if (line[0] != '\n') {
			sourcecode[i] = line;
			i++;
		}
	}
	i--;
	sourceline = i;
	printf("source code has %i lines\n", i);
	printf("read complete\n");
	fclose(pfile); // close file, return 0 if success
}

void writter(char *pf) {
	printf("write begin\n");
	FILE *fp = fopen(pf, "wb");
	char obj[4 * lineofout];
	for (int i = 0; i < sourceline+extra; i++) {
		for (int j = 0; j < 4; j++) {
			obj[4 * i + j] = takeint(objectcode[i], 8 * j, 8);
		}
	}
	fwrite(&obj, sizeof(obj[0]), 4 * (lineofout + extra), fp);
	fclose(fp);
	printf("write complete\n");
	// write complete
}

void firstpass() {
	printf("begin first pass\n");
	int currentline;
	for (int i = 0; i < sourceline; i++) {
		currentline = i;
		if (!isinstruc(currentline)) { // is assembly directive
			char *code;
			code = sourcecode[currentline];
			int length = getlen(code);
			char *label;
			label = code + 100;
			for (int i = 0; i < length - 2; i++) {
				label[i] = code[i];
			}
			label[length - 2] = '\0';
			insertLabel(label, currentline);
			acc += 1;
		}
	}
	printf("first pass complete\n");
}

void secondpass() {
	printf("begin second pass\n");
	lineofout = 0;
	int currentline;
	for (int i = 0; i < sourceline; i++) {
		currentline = i;
		if (isinstruc(currentline)) { // is assembly instruction
			asseminstrc(currentline);
		}
	}
	printf("complete second pass\n");
}

_Bool isinstruc(int line) {
	char *code;
	code = sourcecode[line];
	int length = getlen(code);
	if (code[length - 2] == ':') {
		printf("is not instruction\n");
		return false;
	} else {
		printf("is instruction\n");
		return true;
	}
}

void asseminstrc(int line) {
	int typ = typeofinstrc(line);
	char *ins;
	ins = sourcecode[line];
	switch (typ) {
	case 1:
		printf("datapro\n");
		datapro(ins);
		break;
	case 2:
		printf("mul\n");
		mul(ins);
		break;
	case 3:
		printf("sdtrans\n");
		sdTrans(ins);
		break;
	case 4:
		printf("branch\n");
		branch(ins);
		break;
	case 5:
		printf("andeq\n");
		andeq(ins);
		break;
	case 6:
		printf("lsl\n");
		lsl(ins);
		break;
	default:
		printf("%s", "Impossible instruction\n");
	}

}

int typeofinstrc(int line) {
	char *code;
	code = sourcecode[line];
	switch (code[0]) {
	case 'a':
		if (code[3] == 'e') {
			return 5;
		} else {
			return 1;
		}
	case 's':
		if (code[1] == 'u') {
			return 1;
		} else {
			return 3;
		}
	case 'r':
		return 1;
	case 'e':
		return 1;
	case 'o':
		return 1;
	case 'm':
		if (code[1] == 'o') {
			return 1;
		} else {
			return 2;
		}
	case 't':
		return 1;
	case 'c':
		return 1;
	case 'l':
		if (code[1] == 'd') {
			return 3;
		} else {
			return 6;
		}
	case 'b':
		return 4;
	default:
		printf("%s\n", "Wrong Mnemonic! Maybe a directive?");
		return 0;
	}
}

int getlength(char inst[]) {
	int length = 0;
	while (inst[length] != '\n') {
		length++;
	}
	return length;
}

char *replace(char *inst) {
	char * re;
	re = inst;
	int length = 0;
	while (inst[length] != '\n') {
		length++;
	}
	inst[length] = '\0';
	return re;
}

int getlen(char *inst) {
	return strlen(inst);
}

int setrn(int inst, int rn) {
	int instr = inst;
	instr += power(2, 16) * rn;
	return instr;
}

int setrd(int inst, int rd) {
	int instr = inst;
	instr += power(2, 12) * rd;
	return instr;
}

int setrn2(int inst, int rn) {
	int instr = inst;
	instr += power(2, 12) * rn;
	return instr;
}

int setrd2(int inst, int rd) {
	int instr = inst;
	instr += power(2, 16) * rd;
	return instr;
}

int setrm(int inst, int rm) {
	int instr = inst;
	instr += power(2, 0) * rm;
	return instr;
}

int setrs(int inst, int rs) {
	int instr = inst;
	instr += power(2, 8) * rs;
	return instr;
}

int seto2rnrd(char inss[], int type, int inst) {
	char *ins;
	ins = &inss[0];
	int length = 0;
	while (ins[length] != '\n') {
		length++;
	}
	ins[length] = '\0';
	int rn = 0;
	int rd = 0;
	int instr = inst;
	char *delim = ",";
	char *regist;
	char *x1;
	char *x2;
	char *x3;
	char *x4;
	regist = strtok(ins + 4, delim);
	x1 = strtok(NULL, delim);
	x2 = strtok(NULL, delim);
	x3 = strtok(NULL, delim);
	x4 = strtok(NULL, delim);
	if (regist[0] == ' ') {
		regist++;
	}
	if (type == 2 || type == 3) { // mov or tst/teq/cmp
	//int o2len = strlen(o22);
		printf("register ======== %s\n", regist);
		printf("register ======== %c\n", regist[0]);
		printf("register ======== %c\n", regist[1]);
		int reg = 0;
		if (strlen(regist) == 2) {
			reg = mapchartodecimal(regist[1]);
		} else {
			reg = mapchartodecimal(regist[1]) * 10
					+ mapchartodecimal(regist[2]);
		}
		if (type == 2) { //Rd
			rd = reg;
		} else { // Rn
			rn = reg;
		}
	} else { // and/eor/add/orr/sub/rsb
		//int o2len = strlen(o22);
		printf("register ======== %s\n", regist);
		if (strlen(regist) == 2) {
			rd = mapchartodecimal(regist[1]);
		} else {
			rd = mapchartodecimal(regist[1]) * 10 + mapchartodecimal(regist[2]);
		}
		if (strlen(x1) == 2) {
			rn = mapchartodecimal(x1[1]);
		} else {
			rn = mapchartodecimal(x1[1]) * 10 + mapchartodecimal(x1[2]);
		}
		x1 = x2;
		x2 = x3;
		x3 = x4;
	}
	instr = seto2(instr, type, x1, x2, x3, x4); //set o2
	printf("rd ==== %i\n", rd);
	printf("rn ==== %i\n", rn);
	instr = setrn(instr, rn); // set Rn
	instr = setrd(instr, rd); // set Rd
	return instr;
}

int seto2(int inst, int type, char *x1, char *x2, char *x3, char *x4) {
	int instr = inst;
	int v = 0;
	if (x1[0] == ' ') {
		x1++;
	}
	if (x1[0] == '#') { //immediate value
		int len = strlen(x1);
		printf("length === %d\n", len);
		instr = setBit(instr, 25); // set I
		if (x1[2] == 'x') { // hexadecimal
			printf("%s", "hex\n");
			for (int i = 3; i <= len - 1; i++) {
				v += mapchartodecimal(x1[i]) * power(16, len - 1 - i);
			}
		} else { // decimal
			for (int i = 1; i <= len - 1; i++) {
				v += mapchartodecimal(x1[i]) * power(10, len - 1 - i);
			}
		}
		if (v < -2147483648 || v > 2147483647) { // out of range
			perror("out of range");
			exit(EXIT_FAILURE);
		}
		printf("v === %d\n", v);
		if (v >= 0 && v < power(2, 8)) {
			instr += v; // set o2
		} else {
			int imm = imm1(v);
			int rot = imm2(v);
			printf("imm ===========%d\n", imm);
			printf("rot ===========%d\n", rot);
			v = imm + rot * power(2, 8);
			instr += v; // set o2
		}
	} else { // shifted value
		int rm;
		int len1 = strlen(x1);
		if (len1 == 2) {
			rm = mapchartodecimal(x1[1]);
		} else {
			rm = mapchartodecimal(x1[2]) + 10 * mapchartodecimal(x1[1]);
		}
		printf("rm = %d\n", rm);
		instr += rm;  //  set rm
		if (x2 == NULL) {

		} else {
			if (x2 != NULL) {
				int len2 = strlen(x2);
				int shifttype;
				if (x2[0] == 'a') { // asr
					shifttype = 2;
				} else if (x2[0] == 'r') { // ror
					shifttype = 3;
				} else if (x2[2] == 'l') { // lsl
					shifttype = 0;
				} else { // lsr
					shifttype = 1;
				}
				instr += shifttype * power(2, 5);  //   set shift type
				x2 += 4; // points to register or expression
				int reg = 0;
				if (x2[0] == '#') { // is expression
					int ex = takexpree(x2, len2 - 4);
					instr += ex * power(2, 7);  //set integer
				} else {
					instr = setBit(instr, 4); //set bit 4
					if (len2 == 6) { //reg < 10
						reg = x2[5];
					} else { // reg > 9
						reg = x2[5] * 10 + x2[6];
					}
					//get reg
					instr += reg * power(2, 8); // set reg
				}
				// shifted register end
			}
		}
	}
	return instr;
}

void datapro(char *ins) {
	printf("begin data processing\n");
	int instrc = 0;
	instrc = dataprosetcond(instrc); // set Condition
	int ty = typeofdatapro(ins);
	if (ty == 1) { // is mov
		printf("is mov\n");
		instrc = dataprosetmov(instrc);
		instrc = seto2rnrd(ins, 2, instrc);
	} else if (ty == 2) { // is tst, teq, cmp
		printf("is tst/teq/cmp\n");
		instrc = setBit(instrc, 20); //set S
		if (ins[0] == 'c') { // cmp
			printf("%s", "cmp\n");
			instrc = dataprosetcmp(instrc);  // set opcode
		} else if (ins[1] == 's') { // tst
			printf("%s", "tst\n");
			instrc = dataprosettst(instrc); // set opcode
		} else { // teq
			printf("%s", "teq\n");
			instrc = dataprosetteq(instrc); // set opcode
		}
		instrc = seto2rnrd(ins, 3, instrc);
	} else { // is add, sub, rsb, and, eor, orr
		printf("is add/sub/rsb/and/eor/orr\n");
		if (ins[0] == 's') { // sub
			printf("%s", "sub\n");
			instrc = dataprosetsub(instrc);  // set opcode
		} else if (ins[0] == 'r') { // rsb
			printf("%s", "rsb\n");
			instrc = dataprosetrsb(instrc); // set opcode
		} else if (ins[0] == 'e') { // eor
			printf("%s", "eor\n");
			instrc = dataproseteor(instrc); // set opcode
		} else if (ins[0] == 'o') { // orr
			printf("%s", "orr\n");
			instrc = dataprosetorr(instrc);  // set opcode
		} else if (ins[1] == 'd') { // add
			printf("%s", "add\n");
			instrc = dataprosetadd(instrc);  // set opcode// set opcode
		} else { // and
			printf("%s", "and\n");
			instrc = dataprosetand(instrc);  // set opcode
		}
		instrc = seto2rnrd(ins, 1, instrc);
	}
	objectcode[lineofout] = instrc;
	lineofout++;
	printf("complete data processing\n");
}

int typeofdatapro(char ins[]) {
	if (ins[0] == 'm') { // is mov
		return 1;
	} else if (ins[0] == 't' || ins[0] == 'c') { // is tst, teq, cmp
		return 2;
	} else { // is add, sub, rsb, and, eor, orr
		return 3;
	}
}

int dataprosetcond(int inst) {
	int instrc = inst;
	instrc = setBit(instrc, 31); // set cond
	instrc = setBit(instrc, 30);
	instrc = setBit(instrc, 29);
	return instrc;
}

int dataprosetmov(int inst) {
	int instrc = inst;
	instrc = setBit(instrc, 24); // set opcode
	instrc = setBit(instrc, 23);
	instrc = setBit(instrc, 21);
	return instrc;
}

int dataprosetadd(int inst) {
	int instrc = inst;
	instrc = setBit(instrc, 23); // set opcode
	return instrc;
}

int dataprosetcmp(int inst) {
	int instrc = inst;
	instrc = setBit(instrc, 24); // set opcode
	instrc = setBit(instrc, 22);
	return instrc;
}

int dataprosettst(int inst) {
	int instrc = inst;
	instrc = setBit(instrc, 24); // set opcode
	return instrc;
}

int dataprosetteq(int inst) {
	int instrc = inst;
	instrc = setBit(instrc, 24); // set opcode
	instrc = setBit(instrc, 21);
	return instrc;
}

int dataprosetsub(int inst) {
	int instrc = inst;
	instrc = setBit(instrc, 22); // set opcode
	return instrc;
}

int dataprosetrsb(int inst) {
	int instrc = inst;
	instrc = setBit(instrc, 22); // set opcode
	instrc = setBit(instrc, 21);
	return instrc;
}

int dataproseteor(int inst) {
	int instrc = inst;
	instrc = setBit(instrc, 21); // set opcode
	return instrc;
}

int dataprosetand(int inst) {
	int instrc = inst;
	return instrc;
}

int dataprosetorr(int inst) {
	int instrc = inst;
	instrc = setBit(instrc, 24); // set opcode
	instrc = setBit(instrc, 23);
	return instrc;
}

int calrn(int rn) {
	int re = 0;
	int rem = 0;
	rem = rn % 2;
	if (rem == 1) {
		re += power(2, 16);
	}
	rn = (rn - rem) / 2;
	rem = 0;
	rem = rn % 2;
	if (rem == 1) {
		re += power(2, 17);
	}
	rn = (rn - rem) / 2;
	rem = 0;
	rem = rn % 2;
	if (rem == 1) {
		re += power(2, 18);
	}
	rn = (rn - rem) / 2;
	rem = 0;
	rem = rn % 2;
	if (rem == 1) {
		re += power(2, 19);
	}
	return re;
}

int calrd(int rd) {
	int re = 0;
	int rem = 0;
	rem = rd % 2;
	if (rem == 1) {
		re += power(2, 12);
	}
	rd = (rd - rem) / 2;
	rem = 0;
	rem = rd % 2;
	if (rem == 1) {
		re += power(2, 13);
	}
	rd = (rd - rem) / 2;
	rem = 0;
	rem = rd % 2;
	if (rem == 1) {
		re += power(2, 14);
	}
	rd = (rd - rem) / 2;
	rem = 0;
	rem = rd % 2;
	if (rem == 1) {
		re += power(2, 15);
	}
	return re;
}

void mul(char *ins) { //Shuang Xia
	int instrc = 0;
	int rd = 0;
	int rn = 0;
	int rs = 0;
	int rm = 0;
	instrc = setBit(instrc, 31);
	instrc = setBit(instrc, 30);
	instrc = setBit(instrc, 29); // set cond
	instrc = setBit(instrc, 4);
	instrc = setBit(instrc, 7); //set bit 4-7
	char *delim = ",";
	char *d;
	char *m;
	char *s;
	char *n;
	d = strtok(ins + 4, delim);
	m = strtok(NULL, delim);
	s = strtok(NULL, delim);
	n = strtok(NULL, delim);
	int l = strlen(d);
	printf("ld = %d\n", l);
	l = strlen(m);
	printf("lm = %d\n", l);
	l = strlen(s);
	printf("ls = %d\n", l);
	rd = mapchartodecimal(d[1]);
	if (strlen(d) != 2) {
		rd = rd * 10 + mapchartodecimal(d[2]);
	}
	if (m[0] == ' ') {
		m++;
	}
	rm = mapchartodecimal(m[1]);
	if (strlen(m) != 2) {
		rm = rm * 10 + mapchartodecimal(m[2]);
	}
	if (s[0] == ' ') {
		s++;
	}
	rs = mapchartodecimal(s[1]);

	if (ins[1] == 'u') { //mul;
		if (strlen(s) != 3) {
			rs = rs * 10 + mapchartodecimal(s[2]);
		}
	} else { //mla
		if (strlen(s) != 2) {
			rs = rs * 10 + mapchartodecimal(s[2]);
		}
		instrc = setBit(instrc, 21); // set A
		if (n != NULL) {
			if (n[0] == ' ') {
				n++;
			}
			rn = mapchartodecimal(n[1]);
			if (strlen(n) != 3) {
				rn = rn * 10 + mapchartodecimal(n[2]);
			}
		}
	}
	printf("rd = %d\n", rd);
	printf("rn = %d\n", rn);
	printf("rs = %d\n", rs);
	printf("rm = %d\n", rm);
	instrc = setrd2(instrc, rd);
	instrc = setrn2(instrc, rn);
	instrc = setrs(instrc, rs);
	instrc = setrm(instrc, rm);
	objectcode[lineofout] = instrc;
	lineofout++;
}

int calrs(int rs) {
	int re = 0;
	int rem = 0;
	rem = rs % 2;
	if (rem == 1) {
		re += power(2, 8);
	}
	rs = (rs - rem) / 2;
	rem = 0;
	rem = rs % 2;
	if (rem == 1) {
		re += power(2, 9);
	}
	rs = (rs - rem) / 2;
	rem = 0;
	rem = rs % 2;
	if (rem == 1) {
		re += power(2, 10);
	}
	rs = (rs - rem) / 2;
	rem = 0;
	rem = rs % 2;
	if (rem == 1) {
		re += power(2, 11);
	}
	return re;
}

int calrm(int rm) {
	int re = 0;
	int rem = 0;
	rem = rm % 2;
	if (rem == 1) {
		re += power(2, 8);
	}
	rm = (rm - rem) / 2;
	rem = 0;
	rem = rm % 2;
	if (rem == 1) {
		re += power(2, 9);
	}
	rm = (rm - rem) / 2;
	rem = 0;
	rem = rm % 2;
	if (rem == 1) {
		re += power(2, 10);
	}
	rm = (rm - rem) / 2;
	rem = 0;
	rem = rm % 2;
	if (rem == 1) {
		re += power(2, 11);
	}
	return re;
}

void sdTrans(char *ins) {
	int instrc = 0;
	int rn = 0;
	int rd = 0;
	instrc = setBit(instrc, 31);
	instrc = setBit(instrc, 29);
	instrc = setBit(instrc, 26);
	instrc = setBit(instrc, 30); // set 26-31
	rd = ins[5] - '0';
	int i = 5;
	if (ins[6] != ',') {
		rd = rd * 10 + ins[6] - '0';
		i++;
	}
	instrc += calrd(rd);
	i += 2;
	if (ins[i] == ' ') {
		i++;
	}
	if (ins[0] == 'l' && ins[i] == '=') { //ldr = instruction
		ldrdirect(ins);
	    return;
	}
	i += 2;
	rn = ins[i] - '0';
	i++;
	if ((ins[i] != ']') && (ins[i] != ',')) {
		rn = rn * 10 + ins[i] - '0';
		i++;
	}
	instrc = setrn(instrc, rn);
	int offset = 0;
	if (!(ins[i] == ']' && ins[i + 1] == ',')) { //pre-indexed
		instrc = instrc + power(2, 24);
		if (ins[i + 1] != '\n') { //pre-indexed situation 2
			i++;
			if (ins[i] == ' ') {
				i++;
			}

			if (ins[i] == 'r') {
				instrc += power(2, 25);
			}
			i++;
			if (ins[i] != '-') {
				instrc += power(2, 23);
			} else {
				i++;
			}
			if (ins[i] == '0' && ins[i + 1] == 'x') {
				i += 2;
				offset = ins[i++] - '0';
				while (ins[i] != ']') {
					offset = offset * 16 + ins[i++] - '0';
				}
			} else {
				offset = ins[i++] - '0';
				while (ins[i] != ']') {
					offset = offset * 10 + ins[i++] - '0';
				}
			}
		} else {
			instrc += power(2, 23);
			i += 2;
		}
		instrc = offset_to_binary_in_int(offset, instrc);
		if (ins[0] == 'l') {
			instrc += power(2, 20);
		}
	} else { //post-indexed
		i += 2;
		if (ins[i] == '0' && ins[i + 1] == 'x') {
			i += 2;
			offset = ins[i++] - '0';
			while (ins[i] != ']') {
				offset = offset * 16 + ins[i++] - '0';
			}
		} else {
			offset = ins[i++] - '0';
			while (ins[i] != ']') {
				offset = offset * 10 + ins[i++] - '0';
			}
		}
		instrc = offset_to_binary_in_int(offset, instrc);
		if (ins[0] == 'l') {
			instrc += power(2, 20);
		}
		instrc += power(2, 23);
	}
	objectcode[lineofout] = instrc;
	lineofout++;
}

void ldrdirect(char *ins) {
	printf("%s\n", ins);
	if (useMov(ins)) {
		ldr_equ(ins);
		return;
	} else {
		printf("\n%s\n", "ldrdirect no mov");
		printf("\nsourceline = %d\n", sourceline);
		printf("\nlineofout = %d\n", lineofout);
		printf("\nacc = %d\n", acc);
		int instrc = 0;
		instrc = setBit(instrc, 31);
		instrc = setBit(instrc, 30);
		instrc = setBit(instrc, 29);
		instrc = setBit(instrc, 26);
		instrc = setBit(instrc, 24);
		instrc = setBit(instrc, 23);
		instrc = setBit(instrc, 20);
		int rd = 0;
		rd = mapchartodecimal(ins[5]);
		if (ins[6] != ',') {
			rd = 10 * rd + mapchartodecimal(ins[6]);
		}
		instrc = setrd(instrc, rd);
		instrc = setrn(instrc, 15); // set rn to PC
		int off = getOff(ins);
		instrc += off;
		objectcode[lineofout] = instrc;
			lineofout++;
	}
}

int getOff(char *inst) {
	int value = getV(inst);
	int offset = 0;
	int add = sourceline-acc+extra;
	extra++;
    objectcode[add] = value;
    int pc = lineofout+2;
    offset = add - pc;
    offset *= 4;
    printf("\noffset = %d\n", offset);
	return offset;
}

_Bool useMov(char *inst) {
	char *ins;
	ins = inst + 200;
	ins = memcpy(ins, inst, strlen(inst) + 1);
	int re = 0;
	char *delim = ",";
	char *v;
	v = strtok(ins, delim);
	v = strtok(NULL, delim);
	if (v[0] == ' ') {
		v++;
	}
	int len = strlen(v);
	if (v[2] == 'x') { // hex
		if (len > 6) {
			return false;
		} else {
			for (int i = 3; i < len - 1; i++) {
				re += mapchartodecimal(v[i]) * power(16, len - 2 - i);
			}
			return (re <= 255);
		}
	} else { // deci
		if (len > 5) {
			return false;
		} else {
			for (int i = 1; i < len - 1; i++) {
				re += mapchartodecimal(v[i]) * power(10, len - 2 - i);
			}
			return (re <= 255);
		}
	}
	return true;
}

int getV(char *inst) {
	char *ins;
	ins = inst + 200;
	ins = memcpy(ins, inst, strlen(inst) + 1);
	int re = 0;
	char *delim = ",";
	char *v;
	v = strtok(ins, delim);
	v = strtok(NULL, delim);
	if (v[0] == ' ') {
		v++;
	}
	int len = strlen(v);
	printf("length of v is =========== %d\n", len);
	printf("v[0] is %c\n", v[0]);
	if (v[2] == 'x') { // hex
		for (int i = 3; i < len - 1; i++) {
			re += mapchartodecimal(v[i]) * power(16, len - 2 - i);
		}
	} else { // deci
		for (int i = 1; i < len - 1; i++) {
			re += mapchartodecimal(v[i]) * power(10, len - 2 - i);
		}
	}
	return re;
}

void ldr_equ(char *ins) {
	printf("%s\n", ins);
	char *instruction = symbol_to_mov_instruction(ins);
	printf("%s\n", instruction);
	datapro(instruction);
}

char *symbol_to_mov_instruction(char *ins) {
	printf("%s\n", ins);
	char *in;
	in = ins;
	in[0] = 'm';
	in[1] = 'o';
	in[2] = 'v';
	for (int i = 0; i < strlen(in) - 1; i++) {
		if (in[i] == '=') {
			in[i] = '#';
			break;
		}
	}
	return in;
}

int offset_to_binary_in_int(int offset, int instrc) {
	int i = 0;
	int result = 0;
	while (offset > 0 && i < 12) {
		result = offset % 2;
		offset = (int) offset / 2;
		if (result == 1) {
			instrc += power(2, i);
		}
		i++;
	}
	return instrc;
}

void branch(char *ins) {
	int instrc = 0;
	instrc = setBit(instrc, 25);
	instrc = setBit(instrc, 27);
	char *delim = " ";
	char *off;
	off = strtok(ins, delim);
	off = strtok(NULL, delim);
	int x = strlen(off);
	char *a;
	a = off;
	int i;
	for (i = 0; i < x - 2; i++) {
		a[i] = off[i];
	}
	if (off[x - 2] != ';') {
		a[x - 2] = off[x - 2];
		a[x - 1] = '\0';
	} else {
		a[x - 2] = '\0';
	}
	int offset = map(a);
	printf("\n%d\n", offset);
	printf("\n%d\n", lineofout);
	offset = offset - lineofout - 1 - 2;
	printf("\n%d\n", offset);
	if (offset < 0) {
		for (int i = 24; i < 32; i++) {
			offset = setBit2(offset, i);
		}
	} else {
		offset >>= 2;
	}
	instrc += offset; // setoffset
	printf("\n%d\n", offset);

	if (ins[1] == 'e') { // beq
		printf("%s\n", "beq");
		//do nothing
	} else if (ins[1] == 'n') { // bne
		printf("%s\n", "bne");
		instrc = setBit(instrc, 28);
	} else if (ins[1] == 'g' && ins[2] == 'e') { // bge
		printf("%s\n", "bge");
		instrc = setBit(instrc, 29);
		instrc = setBit(instrc, 31);
	} else if (ins[1] == 'l' && ins[2] == 't') { // blt
		printf("%s\n", "blt");
		instrc = setBit(instrc, 31);
		instrc = setBit(instrc, 29);
		instrc = setBit(instrc, 28);
	} else if (ins[1] == 'g') { // bgt
		printf("%s\n", "bgt");
		instrc = setBit(instrc, 31);
		instrc = setBit(instrc, 30);
	} else if (ins[1] == 'l') { // ble
		printf("%s\n", "ble");
		instrc = setBit(instrc, 31);
		instrc = setBit(instrc, 30);
		instrc = setBit(instrc, 28);
	} else { // b/bal
		printf("%s\n", "b/bal");
		instrc = setBit(instrc, 29);
		instrc = setBit(instrc, 30);
		instrc = setBit(instrc, 31);
	}
	objectcode[lineofout] = instrc;
	lineofout++;
}

int setBit2(int instr, int bit) {
	if (bit == 31) {
		if (readbit(instr, bit)) {
			instr += power(2, bit);
		} else {
			instr -= power(2, bit);
		}
	} else {
		if (readbit(instr, bit)) {
			instr -= power(2, bit);
		} else {
			instr += power(2, bit);
		}
	}
	return instr;
}

_Bool readbit(int instrc, int bitadd) { //wenke yang; reading start from right to left. begin with index 0
	return takeint(instrc, bitadd, 1) == 1;
}

_Bool is_post_index(char ins[], int i, int instrc, int rn) {
	if (ins[i] == ']' && ins[i + 1] != '\0') {
		return true;
	}
	return false;
}

void andeq(char ins[]) {
	objectcode[lineofout] = 0;
	lineofout++;
}

void lsl(char ins[]) {
	int i = 5;
	uint16_t rn = ins[i] - '0';
	if (ins[i + 1] != ',') {
		rn = rn * 10 + ins[i] - '0';
	}
	uint16_t value = 0;
	i += 3;
	if ((ins[i] == '0') && (ins[i + 1] == 'x')) { //hex
		i += 2;
		int count = 0;
		for (int j = 0; ins[j + i] != '\n' || '\0'; j++) {
			count++;
		}
		for (int k = 0; k < count; k++) {
			uint16_t v;
			if (ins[i + k] > '9') {
				v = ins[i + k] - 'a' + 10;
			} else {
				v = ins[i + k] - '0';
			}
			value += v * power(16, (count - (k + 1)));
		}
	} else { //decimal
		int count = 0;
		for (int j = 0; ins[j + i] != '\n' || '\0'; j++) {
			count++;
		}
		for (int k = 0; k < count; k++) {
			uint16_t v = ins[i + k] - '0';
			value += v * power(10, (count - (k + 1)));
		}
	}
	uint32_t newins = 0;
	newins += rn;
	newins += value << 7;
	newins += rn << 12;
	uint16_t x = 57760;
	newins += x << 16;
	int instrc = 0;
	uint32_t mask = 1 << 31;
	if ((newins & mask) == 1) {
		uint32_t x = (uint32_t) pow(2, 31) - 1;
		instrc = (int) newins & x;
		instrc -= (int) pow(2, 31);
	} else {
		instrc = newins;
	}
	objectcode[lineofout] = instrc;
	lineofout++;
}

void boot() { //Shuang Xia
	for (int i = 0; i < 1024 * 64; i++) {
		objectcode[i] = 0;
	}
	for (int i = 0; i < 1024 * 64; i++) {
		for (int j = 0; j < 51; j++) {
			sourcecode[i][j] = 0;
		}
	}

}

/*****************************
 *  Helper Functions
 *****************************/

char *clone(const char *str) {
	size_t size = strlen(str) + 1;
	char *new = malloc(size);

	if (new == NULL) {
		perror("clone");
		exit(EXIT_FAILURE);
	}

	strncpy(new, str, size);
	return new;
}

struct labelMap { //wenke yang
	char* label;
	int instrcAddr;
};

struct list_elem { //wenke yang
	struct labelMap value;
	struct list_elem *prev;
	struct list_elem *next;
};

struct list { //wenke yang
	struct list_elem *header;
	struct list_elem *footer;
};

struct list_elem *list_alloc_elem(void) {
	struct list_elem *elem = malloc(sizeof(struct list_elem));
	if (elem == NULL) {
		perror("list_alloc_elem");
		exit(EXIT_FAILURE);
	}
	return elem;
}

void list_init(struct list *l) {
	l->header = list_alloc_elem();
	l->footer = list_alloc_elem();
	l->header->prev = NULL;
	l->footer->next = NULL;
	l->header->next = l->footer;
	l->footer->prev = l->header;
}

int map(char *str) { //wenke yang
	/*helper method to get all the ints appeared in assembly
	 languages to a list of int. This function based on
	 instruction has less than 4 integers values*/
	struct list_elem* first = maplist.header->next;
	if (strcmp(str, first->value.label) == 0) {
		printf("%s\n", "first ==");
		return first->value.instrcAddr;
	}
	struct list_elem* current = first->next;
	printf("%s\n", "not first");
	while (strcmp(str, current->value.label) != 0) {
		current = current->next;
	}
	printf("%s\n", "n ==");
	return current->value.instrcAddr;
}

void insertLabel(char *label, int value) { //wenke yang
	struct labelMap lmap;
	lmap.label = label;
	lmap.instrcAddr = value + 1 - acc;

	struct list_elem* first = maplist.header->next;
	struct list_elem* pre;
	struct list_elem* new = list_alloc_elem();

	if (first == maplist.footer) {
		printf("%s\n", "null list");
		pre = maplist.header;
		pre->next = new;

		new->value = lmap;
		new->prev = maplist.header;
		new->next = maplist.footer;
		maplist.footer->prev = new;

	} else {
		printf("%s\n", "not null list");
		pre = maplist.footer->prev;
		pre->next = new;

		new->value = lmap;
		new->next = maplist.footer;
		new->prev = pre;
		maplist.footer->prev = new;
	}
	//define a new label,address pair

}

void list_destroy(struct list *l) {
	struct list_elem *elem = l->header;
	while (elem != NULL) {
		struct list_elem *next = elem->next;
		free(elem);
		elem = next;
	}
}

int imm1(int value) {
	for (int j = 0; j < 256; j++) {
		for (int i = 0; i < 15; i++) {
			if (ror(j, 2 * i) == value) {
				return j;
			}
		}
	}
	return 0;
}

int imm2(int value) {
	for (int j = 0; j < 256; j++) {
		for (int i = 0; i < 15; i++) {
			if (ror(j, 2 * i) == value) {
				return i;
			}
		}
	}
	return 0;
}

int setBit(int instr, int bit) {
	if (bit == 31) {
		instr -= power(2, bit);
	} else {
		instr += power(2, bit);
	}
	return instr;
}

int takexpree(char *ex, int len) {
	int vex = 0;
	if (ex[2] == 'x') { // hexadecimal
		int newlen = len - 3;
		char hex[newlen];
		for (int i = 0; i < newlen; i++) {
			hex[i] = ex[i + 3];
		}
		vex = takehex(hex, newlen);
	} else { // decimal
		int newlen = len - 1;
		char dec[newlen];
		for (int i = 0; i < newlen; i++) {
			dec[i] = ex[i + 1];
		}
		vex = takedeximal(dec, newlen);
	}
	return vex;
}

int takedeximal(char arr[], int len) {
	int i;
	int re = 0;
	for (i = 0; i < len; i++) {
		re += power(10, i) * ((int) arr[len - i - 1] - 48);
	}
	return re;
}

int takehex(char *arr, int len) {
	int i;
	int re = 0;
	for (i = 0; i < len; i++) {
		re += power(16, i) * mapchartodecimal(arr[len - i - 1]);
	}
	printf("re ==========  %d", re);
	return re;
}

int mapchartodecimal(char c) {
	int i = 0;
	switch (c) {
	case '0':
		i = 0;
		break;
	case '1':
		i = 1;
		break;
	case '2':
		i = 2;
		break;
	case '3':
		i = 3;
		break;
	case '4':
		i = 4;
		break;
	case '5':
		i = 5;
		break;
	case '6':
		i = 6;
		break;
	case '7':
		i = 7;
		break;
	case '8':
		i = 8;
		break;
	case '9':
		i = 9;
		break;
	case 'A':
		i = 10;
		break;
	case 'B':
		i = 11;
		break;
	case 'C':
		i = 12;
		break;
	case 'D':
		i = 13;
		break;
	case 'E':
		i = 14;
		break;
	case 'F':
		i = 15;
		break;
	case 'a':
		i = 10;
		break;
	case 'b':
		i = 11;
		break;
	case 'c':
		i = 12;
		break;
	case 'd':
		i = 13;
		break;
	case 'e':
		i = 14;
		break;
	case 'f':
		i = 15;
		break;
	default:
		i = -1;
	}
	return i;
}

int power(int x, int y) { // Jiahao Lin
	if (x == 0) {
		return 0;
	} else if (y == 0) {
		return 1;
	} else {
		return x * power(x, y - 1);
	}
}

int lsll(int x, int y) {
	if (y != 0) {
		return x << y;
	} else {
		return x;
	}
}

int lsr(int x, int y) {
	if (y != 0) {
		return x >> y;
	} else {
		return x;
	}
}

int asr(int x, int y) {
	if (y != 0) {
		int z = lsr(x, y);
		int v = lsr(x, 31);
		if (v == 1) {
			v = lsll((power(2, y) - 1), 32 - y);
			return y | z;
		} else {
			return z;
		}
	} else {
		return x;
	}

}

int ror(int x, int y) {
	if (y != 0) {
		int z = x << (32 - y);
		int v = x >> y;
		return (z | v);
	} else {
		return x;
	}
}

int rol(int x, int y) {
	if (y != 0) {
		int z = x >> (32 - y);
		int v = x << y;
		return (z | v);
	} else {
		return x;
	}
}

int takeint(int instruc, int start, int length) { // start --- start+length-1
	if (start > 31 || start < 0) {
		printf("%s\n", "take int error");
		printf("start  =   %d    length = %d\n", start, length);
		return 0;
	} else {
		instruc >>= start;
		int temp = ~(~0 << length);
		return instruc & temp;
	}
}

void printBits(uint32_t x) {
	int i;
	uint32_t mask = 1 << 31;
	for (i = 0; i < 32; ++i) {
		printf("%i", (x & mask) != 0);
		x <<= 1;
	}
	printf("\n");
}
