#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>

int get_IO_reg_name(int inst_regs[], int registers[], char IOReg_name[]) {
	if ((registers[inst_regs[1]] + registers[inst_regs[2]]) == 0) strcpy(IOReg_name, "irq0enable");
	if ((registers[inst_regs[1]] + registers[inst_regs[2]]) == 1) strcpy(IOReg_name, "irq1enable");
	if ((registers[inst_regs[1]] + registers[inst_regs[2]]) == 2) strcpy(IOReg_name, "irq2enable");
	if ((registers[inst_regs[1]] + registers[inst_regs[2]]) == 3) strcpy(IOReg_name, "irq0status");
	if ((registers[inst_regs[1]] + registers[inst_regs[2]]) == 4) strcpy(IOReg_name, "irq1status");
	if ((registers[inst_regs[1]] + registers[inst_regs[2]]) == 5) strcpy(IOReg_name, "irq2status");
	if ((registers[inst_regs[1]] + registers[inst_regs[2]]) == 6) strcpy(IOReg_name, "irqhandler");
	if ((registers[inst_regs[1]] + registers[inst_regs[2]]) == 7) strcpy(IOReg_name, "irqreturn");
	if ((registers[inst_regs[1]] + registers[inst_regs[2]]) == 8) strcpy(IOReg_name, "clks");
	if ((registers[inst_regs[1]] + registers[inst_regs[2]]) == 9) strcpy(IOReg_name, "leds");
	if ((registers[inst_regs[1]] + registers[inst_regs[2]]) == 10) strcpy(IOReg_name, "display7seg");
	if ((registers[inst_regs[1]] + registers[inst_regs[2]]) == 11) strcpy(IOReg_name, "timerenable");
	if ((registers[inst_regs[1]] + registers[inst_regs[2]]) == 12) strcpy(IOReg_name, "timercurrent");
	if ((registers[inst_regs[1]] + registers[inst_regs[2]]) == 13) strcpy(IOReg_name, "timermax");
	if ((registers[inst_regs[1]] + registers[inst_regs[2]]) == 14) strcpy(IOReg_name, "diskcmd");
	if ((registers[inst_regs[1]] + registers[inst_regs[2]]) == 15) strcpy(IOReg_name, "disksector");
	if ((registers[inst_regs[1]] + registers[inst_regs[2]]) == 16) strcpy(IOReg_name, "diskbuffer");
	if ((registers[inst_regs[1]] + registers[inst_regs[2]]) == 17) strcpy(IOReg_name, "diskstatus");
	if ((registers[inst_regs[1]] + registers[inst_regs[2]]) == 20) strcpy(IOReg_name, "monitoraddr");
	if ((registers[inst_regs[1]] + registers[inst_regs[2]]) == 21) strcpy(IOReg_name, "monitordata");
	if ((registers[inst_regs[1]] + registers[inst_regs[2]]) == 22) strcpy(IOReg_name, "monitorcmd");
	return 0;
}