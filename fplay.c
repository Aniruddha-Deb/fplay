#include <stdio.h>
#include <stdbool.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>

char* HELP_STR = 
"Float playground\n"
"This is just something I came up with to quickly convert floats->bin and to "
"other formats as well. Commands are c-style functions "
"and the commands you can use are: \n"
"\n"
"- float_to_bin(float f) - prints a binary representation of f, a float\n"
"  eg: float_to_bin(1.5) = 0 01111111 10000000000000000000000\n"
"\n"
"- bin_to_float(bin s, bin e, bin m) - prints a floating point representation "
"of sign, exponent and mantissa. Note they have to be in binary.\n"
"  eg: bin_to_float(0, 01111111, 10000000000000000000000) = 1.5\n"
"\n"
"- sem_to_float(int s, int e, int m) - same as above, but s, e, m can be specified "
"in octal, hex or decimal, c-style.\n"
"  eg: sem_to_float(1, 127, 0x400000) = 1.5\n"
"\n"
"- demo() - prints a short demo of the program\n"
"\n"
"- exit() - exits the program\n";

char* DEMO = 
"Let's start with converting a float to binary\n"
"\n"
">> float_to_bin(1.5)\n"
"0 01111111 10000000000000000000000\n"
"\n"
"This is easy to understand: (1.5)10 = (1.1)2, so the mantissa would be 100000...0 and the exponent is 0. Since float has an offset of 127, the exponent is E-127 hence E = 127. The sign bit is 0, as it's positive\n"
"\n"
"Let's try the same with the negative of this number:\n"
"\n"
">> float_to_bin(-1.5)\n"
"1 01111111 10000000000000000000000\n"
"\n"
"Notice how the sign bit became 1. \n"
"\n"
"0 is a special value: we can't represent 0 with the sign-exponent-mantissa formula, as the mantissa is always of the form 1.x this is why if all the bits are 0, then the value of the float is 0\n"
"\n"
">> sem_to_float(0,0,0)\n"
"0.000000\n"
"\n"
"One interesting question that arises is: what happens if the sign bit is 1 and the others are 0? Strangely, we get -0 in this case.\n"
"\n"
">> sem_to_float(1,0,0)\n"
"-0.000000\n"
"\n"
"these are defined quite nicely: 0 is the value one would get for the expression 1/infinity, while -0 is the value one would get for the expression -1/infinity. Another very interesting thing is that -0 == 0, that is, if you evaluate this conditional, you get true!\n"
"\n"
"-infinity and infinity are also defined quite nicely:\n"
"\n"
">> sem_to_float(0,0xff,0)\n"
"inf\n"
"\n"
">> sem_to_float(1,0xff,0)\n"
"-inf\n"
"\n"
"That leaves us with Not a Number (NaN), the result of floating point arithmetic errors (such as dividing by a zero, or taking the square root of -1 etc etc). These are defined in two types: quiet NaN's and Signalling NaN's (the first is a quiet NaN, mantissa's first bit 0 and atleast one other bit 1, while the latter is a signalling NaN, first bit 1 and atleast one other bit 1)\n"
"\n"
">> sem_to_float(0,0xff,0x300000)\n"
"nan\n"
">> sem_to_float(0,0xff,0x500000)\n"
"nan\n";

void float_to_bin(float f, char *b) {
	uint32_t bits = *(uint32_t*)&f; // slick ;) better than boring memcpy
	int stridx = 33;
	for (int i=0; i<32; i++) {
		if (i == 23 || i == 31) b[stridx--] = ' ';
		b[stridx--] = (bits & (1<<i))?'1':'0';
	}
	b[34] = '\0';
}

float bin_to_float(char *sign, char *exp, char *man) {
	uint32_t bits = 0;
	for (int i=0; i<23; i++) {
		bits |= ((man[i]^'0') << (22-i));
	}
	for (int i=0; i<8; i++) {
		bits |= ((exp[i]^'0') << (30-i));
	}
	bits |= ((sign[0]^'0') << 31);
	float ans = *(float*)&bits;
	char k[35];
	float_to_bin(ans, k);
	return ans;
}

float sem_to_float(uint32_t sign, uint32_t exponent, uint32_t mantissa) {
	uint32_t f = (sign << 31) | (exponent << 23) | mantissa;
	float ans = *(float*)&f;
	char k[35];
	float_to_bin(ans, k);
	printf("%s\n", k);
	return ans;
}

int main() {
	
	// floats are defined using the IEEE standard in C
	//
	// short note: real constants are treated as doubles by default in C.
	// To test this out, try uncommenting the following line
	//
	// printf("%lu\n", sizeof(3.2));
	//
	// you'll notice that the output is 8 and not 4. floats are 32 bits according
	// to IEEE, while doubles are 64, hence real constants are doubles.
	// We can force a real constant to be a float by suffixing it with f: see
	// below declaration for an example

#ifndef TEST
	printf("Float playground v.0.1\n");
	printf("type help() for help\n");
	char input[101];
	bool run = true;
	do {
		printf(">> ");
		fgets(input, 100, stdin);
		
		if (strcmp(input, "help()\n") == 0) {
			printf("%s", HELP_STR);
		}
		else if (strncmp("float_to_bin", input, strlen("float_to_bin")) == 0) {
			float f;
			char a[35];
			sscanf(input, "float_to_bin( %f )", &f);
			float_to_bin(f, a);
			printf("%s\n", a);
		}
		else if (strncmp("bin_to_float", input, strlen("bin_to_float")) == 0) {
			char s[10];
			char e[10];
			char m[30];
			// quick and dirty own parser, because scanf doesn't work well
			for (int i=0; i<strlen(input); i++) {
				if (input[i] == '(') {
					i++;
					while (input[i] != '0' && input[i] != '1' ) i++;
					int cs = 0;
					while (input[i] == '1' || input[i] == '0') {
						s[cs] = input[i];
						i++; cs++;
					}
					s[cs] = '\0';
					while (input[i] != '1' && input[i] != '0') i++;
					cs = 0;
					while (input[i] == '1' || input[i] == '0') {
						e[cs] = input[i];
						i++; cs++;
					}
					e[cs] = '\0';
					while (input[i] != '1' && input[i] != '0') i++;
					cs = 0;
					while (input[i] == '1' || input[i] == '0') {
						m[cs] = input[i];
						i++; cs++;
					}
					m[cs] = '\0';
					break;
				}
			}
			printf("%f\n", bin_to_float(s, e, m));
		}
		else if (strncmp("sem_to_float", input, strlen("sem_to_float")) == 0) {
			unsigned int s, e, m;
			sscanf(input, "sem_to_float( %i , %i , %i )", &s, &e, &m);
			printf("%f\n", sem_to_float(s, e, m));
		}
		else if (strcmp(input, "demo()\n") == 0) {
			printf("%s", DEMO);
		}
		else if (strcmp(input, "exit()\n") == 0) {
			run = false;
		}
	} while(run);
#endif

#ifdef TEST
	float inf = sem_to_float(0,0xff,0);
	float minf = sem_to_float(1,0xff,0);

	printf("%f %f\n", inf, minf);

	float pzero = 1.0f/inf;
	float nzero = 1.0f/minf;

	printf("%f %f\n", pzero, nzero);

	printf("%d\n", pzero == nzero);

#endif

	return 0;
}
