//Takes long long, outputs int opcode, int[4] reg addresses, int[2] imm values
// it masks, shifts and places each value.

int decode(long long input, int regs[], int imm[]) {
	imm[1] = (input & 0x7FF) - (input & 0x800);
	input = (input >> 12);
	imm[0] = (input & 0x7FF) - (input & 0x800);
	input = (input >> 12);
	regs[3] = (input & 0xF);
	input = (input >> 4);
	regs[2] = (input & 0xF);
	input = (input >> 4);
	regs[1] = (input & 0xF);
	input = (input >> 4);
	regs[0] = (input & 0xF);
	input = (input >> 4);
	return input;
}