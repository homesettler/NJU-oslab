#include "sched.h"




void addpcb(PCB **list,PCB *data)
{
    if(*list==NULL)
    {
        data->next = NULL;
        data->prev = NULL;
        *list = data;
    }
    else
    {
        PCB *t=*list;
        while(t->next!=NULL)
            t=t->next;
        t->next = data;
        data->prev = t;
        data->next=NULL;
    }
}

PCB *delfirst(PCB **list)
{
    if((*list)==NULL)
        return NULL;
    PCB *temp = *list;
    (*list)=(*list)->next;
    if((*list)!=NULL)
        (*list)->prev=NULL;
    temp->next=NULL;
    return temp;
}

PCB pcb[MAX_PCB_NUM];
PCB *runnable = NULL;
PCB *current = NULL;
PCB *blocked = NULL;
PCB *free = NULL;
PCB idle;
extern TSS tss;
extern SegDesc gdt[NR_SEGMENTS];

void initpcb()
{
    int i=0;
    for(;i<MAX_PCB_NUM;i++)
    {
        pcb[i].state=DEAD;
        pcb[i].timeCount = 0;
        pcb[i].sleepTime=0;
        pcb[i].pid=i;
        pcb[i].next=NULL;
        pcb[i].prev=NULL;
    }
}

void Block2Runnable()
{
    PCB *t = blocked;
    while(t!=NULL)
    {
        t->sleepTime--;
        if(t->sleepTime<=0)
        {
            t->state=RUNNABLE;
            t->timeCount = 10;
            PCB *r = t;
            if(r->prev==NULL&&r->next==NULL)
                blocked = NULL;
            if(r->prev!=NULL)
                r->prev->next = r->next;
            if(r->next!=NULL)
                r->next->prev = r->prev;
            t=t->next;
            addpcb(&runnable,r);
        }
        else
            t=t->next;
    }
}

void FORK()
{
    gdt[7] = SEG(STA_X|STA_R,0x100000,0x100000,DPL_USER);
    gdt[8] = SEG(STA_W,0x100000,0x100000,DPL_USER);
    int i;
    for(i=0;i<0x100000;i++)
    {
        *(char *)(0x300000+i) = *(char *)(0x200000+i);
    }
    PCB *t = delfirst(&free);
    *t = (*current);
    t->pid = t - pcb;
    t->tf = (void*)(((uint32_t)t - (uint32_t)current) + (uint32_t)current->tf);
    t->tf->ds = (USEL(8));
    t->tf->es = (USEL(8));
    t->tf->ss = (USEL(8));
    t->tf->cs = (USEL(7));
    t->state = RUNNABLE;
    t->timeCount = 10;
    t->tf->eax = 0;
    current->tf->eax = t->pid;
    addpcb(&runnable,t);
}
void SLEEP(unsigned time)
{
    current->sleepTime = time;
    current->state = BLOCKED;
    addpcb(&blocked,current);
    current = delfirst(&runnable);
    if(current==NULL)
    {
        current = &idle;
    }
    else
    {
        current->state = RUNNING;
        current->timeCount = 10;
        tss.esp0 = (int)current->stack + 4096;
    }
}
void EXIT()
{
    current->state = DEAD;
    addpcb(&free,current);
    current = delfirst(&runnable);
    if(current==NULL)
        current = &idle;
    else
    {
        current->state = RUNNING;
        current->timeCount = 10;
        tss.esp0 =(int)current->stack + 4096;
    }
}

void schedule()
{
    if(current == &idle)
    {
        current = delfirst(&runnable);
        if(current==NULL)
            current = &idle;
        else
        {
            current->state = RUNNING;
            current->timeCount = 10;
            tss.esp0 = (int)current->stack + 4096;
        }
        return;
    }
    if(current->timeCount<=0)
    {
        current->state = RUNNABLE;
        addpcb(&runnable,current);
        current = delfirst(&runnable);
        current->state = RUNNING;
        current->timeCount = 10;
        tss.esp0 = (int)current->stack + 4096;
    }
    return;
}