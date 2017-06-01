#include "x86.h"
#include "sched.h"
#define MAX_SEM 10
typedef struct Semaphore
{
    int value;
    int id;
    int ifuse;
    PCB *next;
}Semaphore;

Semaphore semaphore[MAX_SEM];
void P(Semaphore *s);
void V(Semaphore *s);
void seminit();
int getsem();