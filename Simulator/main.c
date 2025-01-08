#include <stdio.h>
int registers[16];

int three();
void five();


/* 
	Fetch: - ZHARS
	Takes imem file pointer and PC, outputs instruction as long long

	Decode: - DANIEL
	Takes long long, outputs int opcode, int [4] reg addresses, int [2] imm values

	Execute: - SHRAGA
	Decides what to do with the opcode, registers and immediates.
	Let's implement only "add" for now.
*/

int main(int argc, char * argv[]) {

	printf("I am a simulator bleep bloop: %d\n", three());
	five();
	return 0;
}



