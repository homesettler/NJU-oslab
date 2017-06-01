#include "semaphore.h"
Semaphore semaphore[MAX_SEM];

void W(Semaphore *s)
{
    if(s->next==NULL)
    {
        s->next=current;
        current->next=NULL;
        current->prev=NULL;
        current->state = BLOCKED;
        current = delfirst(&runnable);
        if(current == NULL)
            current = &idle;
    }
    else
    {
        PCB *temp=s->next;
        while(temp->next!=NULL)
        {
            temp=temp->next;
        }
        temp->next=current;
        current->state=BLOCKED;
        current->prev=temp;
        current->next=NULL;
        current=delfirst(&runnable);
    }
}

void R(Semaphore *s)
{
    if(s->next==NULL)
        return;
    else
    {
        PCB *temp=s->next;
        s->next=s->next->next;
        if(s!=NULL)
            s->next->prev=NULL;
        temp->state=RUNNABLE;
        temp->next=NULL;
        temp->prev=NULL;
        addpcb(&runnable,temp);
    }
}

void P(Semaphore *s)
{
    s->value--;
    if(s->value<0)
        W(s);
}
void V(Semaphore *s)
{
    s->value++;
    if(s->value<=0)
        R(s);
}
void seminit()
{
    int i=0;
    for(;i<MAX_SEM;i++)
    {
        semaphore[i].ifuse=0;
        semaphore[i].id=i;
        semaphore[i].value=0;
        semaphore[i].next=NULL;
    }
}
int getsem()
{
    int i=0;
    for(;i<MAX_SEM;i++)
    {
        if(semaphore[i].ifuse==0)
            break;
    }
    if(i!=MAX_SEM)
        return i;
    else
        return -1;
}