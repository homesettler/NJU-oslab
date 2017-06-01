#include "x86.h"
#include "device.h"
#define DEAD 0
#define RUNNING 1
#define RUNNABLE 2
#define BLOCKED 3
#define MAX_STACK_SIZE 4096
#define MAX_PCB_NUM 10
typedef struct PCB{
    struct TrapFrame *tf;
    int state;
    int timeCount;
    int sleepTime;
    unsigned int pid;
    unsigned char stack[MAX_STACK_SIZE];
    struct PCB *prev;
    struct PCB *next;
}PCB;

extern PCB pcb[MAX_PCB_NUM];
extern PCB idle;
extern PCB *current;
extern PCB *runnable;
extern PCB *blocked;
extern PCB *free;


void addpcb(PCB **list,PCB *data);

PCB *delfirst(PCB **list);
void initpcb();
void Block2Runnable();
void FORK();
void SLEEP(unsigned time);
void EXIT();
void schedule();