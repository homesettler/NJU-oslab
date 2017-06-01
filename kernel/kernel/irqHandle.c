
#include "sched.h"
void syscallHandle(struct TrapFrame *tf);

void GProtectFaultHandle(struct TrapFrame *tf);

void irqHandle(struct TrapFrame *tf) {
	/*
	 * 中断处理程序
	 */
	/* Reassign segment register */
	current->tf = tf;
	switch(tf->irq) {
		case -1:
			break;
		case 0xd:
			GProtectFaultHandle(tf);
			break;
		case 0x80:
			syscallHandle(tf);
			break;
		case 0x20:
			putChar('t');
			current->timeCount--;
			Block2Runnable();
			break;
		default:assert(0);
	}
	schedule();
}
void roll()
{
	asm volatile("movw %%ax,%%gs"::"a"(KSEL(SEG_VEDIO)));
	int i=0;
	asm volatile("push %edi");
	for(;i<25*80*2;i=i+2)
	{
		int j=i+160;
		asm volatile("movl %0,%%edi"::"m"(j));
		asm volatile("movw %gs:(%edi),%bx");
		asm volatile("movl %0,%%edi"::"m"(i));
		asm volatile("movw %ebx,%gs:(%edi)");
	}
	asm volatile("pop %edi");
}
void syscallHandle(struct TrapFrame *tf) {
	asm volatile("push %edi");
	/* 实现系统调用*/
	switch(tf->eax)
	{
		case 2:
		{
			asm volatile("movw %%ax,%%gs"::"a"(KSEL(SEG_VEDIO)));
			unsigned int temp = tf->ecx;
			if(temp != 10)
			{				
				unsigned int x=0;
				unsigned char h=0;
				unsigned char l=0;
				outByte(0x3d4,14);
				h=inByte(0x3d5);
				outByte(0x3d4,15);
				l=inByte(0x3d5);
				unsigned int x2=0;
				x=l+h*256;
				x2=x*2;
				x=x+1;
				if(x>25*80)
				{
					roll();
					x-=80;
					x2-=160;
				}
				l=(x&0xff);
				h=((x>>8)&0xff);
				outByte(0x3d5,l);
				outByte(0x3d4,14);
				outByte(0x3d5,h);
				asm volatile("movb $0x0c,%bh");
				asm volatile("movb %0,%%bl"::"m"(temp));
				asm volatile("movl %0,%%edi"::"m"(x2));
				asm volatile("movw %bx,%gs:(%edi)");
			}
			else
			{
				unsigned int x=0;
				unsigned char h=0;
				unsigned char l=0;
				outByte(0x3d4,14);
				h=inByte(0x3d5);
				outByte(0x3d4,15);
				l=inByte(0x3d5);
				x=l+h*256;
				x=x+(80-x%80);
				if(x>25*80)
				{
					roll();
					x-=80;
				}
				l=(x&0xff);
				h=((x>>8)&0xff);
				outByte(0x3d5,l);
				outByte(0x3d4,14);
				outByte(0x3d5,h);
			}
			break;
		}
		case 3:
			FORK();
			break;
		case 4:
			SLEEP(tf->ecx);
			break;
		case 5:
			EXIT();
			break;
		default:assert(0);
	}
	asm volatile("pop %edi");
}
void GProtectFaultHandle(struct TrapFrame *tf){
	assert(0);
	return;
}
