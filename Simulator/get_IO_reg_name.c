#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>

int get_IO_reg_name(int inst_regs[], char IOReg_name[]) {
	if (inst_regs[1] + inst_regs[2] == 0) strcpy(IOReg_name, "irq0enable");
	if (inst_regs[1] + inst_regs[2] == 1) strcpy(IOReg_name, "irq1enable");
	if (inst_regs[1] + inst_regs[2] == 2) strcpy(IOReg_name, "irq2enable");
	if (inst_regs[1] + inst_regs[2] == 3) strcpy(IOReg_name, "irq0status");
	if (inst_regs[1] + inst_regs[2] == 4) strcpy(IOReg_name, "irq1status");
	if (inst_regs[1] + inst_regs[2] == 5) strcpy(IOReg_name, "irq2status");
	if (inst_regs[1] + inst_regs[2] == 6) strcpy(IOReg_name, "irqhandler");
	if (inst_regs[1] + inst_regs[2] == 7) strcpy(IOReg_name, "irqreturn");
	if (inst_regs[1] + inst_regs[2] == 8) strcpy(IOReg_name, "clks");
	if (inst_regs[1] + inst_regs[2] == 9) strcpy(IOReg_name, "leds");
	if (inst_regs[1] + inst_regs[2] == 10) strcpy(IOReg_name, "display7seg");
	if (inst_regs[1] + inst_regs[2] == 11) strcpy(IOReg_name, "timerenable");
	if (inst_regs[1] + inst_regs[2] == 12) strcpy(IOReg_name, "timercurrent");
	if (inst_regs[1] + inst_regs[2] == 13) strcpy(IOReg_name, "timermax");
	if (inst_regs[1] + inst_regs[2] == 14) strcpy(IOReg_name, "diskcmd");
	if (inst_regs[1] + inst_regs[2] == 15) strcpy(IOReg_name, "disksector");
	if (inst_regs[1] + inst_regs[2] == 16) strcpy(IOReg_name, "diskbuffer");
	if (inst_regs[1] + inst_regs[2] == 17) strcpy(IOReg_name, "diskstatus");
	if (inst_regs[1] + inst_regs[2] == 20) strcpy(IOReg_name, "monitoraddr");
	if (inst_regs[1] + inst_regs[2] == 21) strcpy(IOReg_name, "monitordata");
	if (inst_regs[1] + inst_regs[2] == 22) strcpy(IOReg_name, "monitorcmd");
	return 0;
}