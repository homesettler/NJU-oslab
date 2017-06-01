#include "common.h"
// #include "x86.h"
// #include "device.h"
#include "sched.h"
void kEntry(void) {

	initSerial();// initialize serial port
	initIdt(); // initialize idt
	initIntr(); // iniialize 8259a
	initTimer();
	initSeg(); // initialize gdt, tss
	loadUMain(); // load user program, enter user space
	asm volatile("movl %%eax,%%esp"::"a"(idle.stack + 4096));
	asm volatile("sti");
	while(1)
	{
		asm volatile("hlt");
	}
	assert(0);
}
