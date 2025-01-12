#include <stdio.h>
int registers[16];
int execute();
int decode();
long long fetch();
int PC = 0;
/* 
	Fetch: - ZHARS
	Takes imem file pointer and PC, outputs instruction as long long

	Decode: - DANIEL - DONE
	Takes long long, outputs int opcode, int [4] reg addresses, int [2] imm values

	Execute: - SHRAGA - DONE
	Decides what to do with the opcode, registers and immediates.
	Let's implement only "add" for now.
*/

int main(int argc, char * argv[]) {
	FILE* mcode; //file pointer to imemin.txt
	mcode = fopen(argv[1], "r"); //read and write


	long long instruction = fetch(mcode,PC);
	// zohar put your long long output in this guy.
	printf("instruction: %012llX\n", instruction);
	
	int opcode, inst_regs[4], imm[2];
	opcode = decode(instruction, inst_regs, imm);
	//Takes long long, outputs int opcode, int[4] reg addresses, int[2] imm values
	// changes their values in decode as such: 
	// opcode is returned 
	// reg[0] = rd, reg[1] = rs, reg[2] = rt, reg[3] = rm
	// imm[0] = immediate 1, imm[1] = immediate 2

	printf("Opcode is %d\nReg rd is %d\nReg rs is %d\nReg rt is %d\nReg rm is %d\nimmediate 1 is %d\nimmediate 2 is %d.\n", opcode, inst_regs[0], inst_regs[1], inst_regs[2], inst_regs[3], imm[0], imm[1]);

	execute(opcode, &inst_regs, &imm, &registers);
	printf("Values in registers after operation: %d, %d, %d, %d\n", registers[inst_regs[0]], registers[inst_regs[1]], registers[inst_regs[2]], registers[inst_regs[3]]);
	fclose(mcode);
	return 0;
}



