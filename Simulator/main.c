#include <stdio.h>
int registers[16];




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
	FILE* mcode;//file pointer to imemin.txt
	mcode = fopen(argv[2], "r");//read and write
	int PC = 0;


	long long instruction = fetch(mcode,PC);
	// zohar put your long long output in this guy.
	
	
	
	int opcode, regs[4], imm[2];
	opcode = decode(instruction,regs,imm);
	//Takes long long, outputs int opcode, int[4] reg addresses, int[2] imm values
	// changes their values in decode as such: 
	// opcode is returned 
	// reg[0] = rd, reg[1] = rs, reg[2] = rt, reg[3] = rm
	// imm[0] = immediate 1, imm[1] = immediate 2
	printf("Opcode is %d\nReg rd is %d\nReg rs is %d\nReg rt is %d\nReg rm is %d\nimmediate 1 is %d\nimmediate 2 is %d.", opcode, regs[0], regs[1], regs[2], regs[3], imm[0], imm[1]);




	return 0;
}



