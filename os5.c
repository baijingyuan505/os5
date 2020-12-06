#include<stdio.h>
#include<stdlib.h>
#include<time.h>

#define NUM 320
#define CONST 1000

typedef struct pagetable
{
    int page_num;
    int access_time[32];
    bool status[32];
    int block_num[32];
}PageTable;
typedef struct pagequeque
{
    int front;
    int rear;
    int* data;
}PageQueue;


int Ins[NUM];

void InitIns();
void InitPageTable(PageTable* pagetable,int frame);
void InitQueue(PageQueue* queue,int frame);
void FCFO(PageTable* pagetable,int frame,PageQueue* queue);
bool isHitted(int frame,int i,PageTable* pagetable);
bool isFull(PageQueue* queue,int frame);
bool isEmpty(PageQueue* queue);
void EnQueue(PageQueue* queue,int blocknum,int frame);
void incTime(PageTable* pagetable);
int LeastUsed(PageTable* pagetable);
void LRU(PageTable* pagetable,int frame,PageQueue* queue);
void OPT(PageTable* pagetable,int frame);
void LFR(PageTable* pagetable,int frame);

void InitIns()
{
   srand(time(NULL));
   for(int i=0;i<NUM;i=i+4)
   {
       int m=rand()%NUM;
       Ins[i]=(m+1)%NUM;
       int m1=rand()%(m+2);
       Ins[i+1]=m1;
       Ins[i+2]=(m1+1)%NUM;
       Ins[i+3]=rand()%(NUM-m1-2)+m1+2;
   }
}
void InitPageTable(PageTable* pagetable,int frame)
{
   pagetable->page_num=frame;
   for(int i=0;i<32;i++)
   {
      pagetable->access_time[i]=0;
      pagetable->status[i]=false;
      pagetable->block_num[i]=-1;
   }
}
void InitQueue(PageQueue* queue,int frame)
{
    queue->front=0;
    queue->rear=0;
    queue->data=(int*)malloc(sizeof(int)*(frame+1));
}
bool isHitted(int frame,int i,PageTable* pagetable)
{
    bool arg=false;
    int pagenum=Ins[i]/10;
    for(int j=0;j<frame;j++)
    {
        if(pagetable->block_num[j]==pagenum)
        {
             arg=true;
             break;
        }
    }
    return arg;
}
bool isFull(PageQueue* queue,int frame)
{
    if((queue->rear+1)%(frame+1)==queue->front)
        return true;
    else
        return false;
        
}
bool isEmpty(PageQueue* queue)
{
    if(queue->front==queue->rear)
        return true;
    else
        return false;
}
void EnQueue(PageQueue* queue,int blocknum,int frame)
{
     if(!isFull(queue,frame))
     {
          queue->data[queue->rear]=blocknum;
          queue->rear=(queue->rear+1)%(frame+1);
     }
     else
          printf("Error,this queue is full now!\n");
}
int DeQueue(PageQueue* queue,int frame)
{
     if(!isEmpty(queue))
     {
          int temp=queue->data[queue->front];
          queue->front=(queue->front+1)%(frame+1);
          return temp;
     }
     else
     {
          printf("Error,this queue is Empty!\n");
          return 0;
     }
}
void incTime(PageTable* pagetable)
{
    for(int i=0;i<32;i++)
    {
        if(pagetable->status[i]==true)
            pagetable->access_time[i]++;
    }
}
int LeastUsed(PageTable* pagetable)
{
    int k=pagetable->access_time[0];
    int max=0;
    for(int i=0;i<32;i++)
    {
        if(pagetable->access_time[i]>k)
        {
            k=pagetable->access_time[i];
            max=i;
        }
    }
    return max;
}
void FCFO(PageTable* pagetable,int frame,PageQueue* queue)
{
    int p_fault=0;
    InitPageTable(pagetable,frame);
    
    for(int i=0;i<NUM;i++)
    {
        if(!isHitted(frame,i,pagetable))
        {
             p_fault++;
             if(isFull(queue,frame)==false)
             {
                  EnQueue(queue,Ins[i]/10,frame);
                  pagetable->status[Ins[i]/10]=true;
                  for(int j=0;j<frame;j++)
                  {
                       if(pagetable->block_num[j]==-1)
                          pagetable->block_num[j]=Ins[i]/10;
                  }
             }
             else
             {
                  int temp=DeQueue(queue,frame);
                  EnQueue(queue,Ins[i]/10,frame);
                  pagetable->status[Ins[i]/10]=true;
                  pagetable->status[temp]=false;
                  for(int j=0;j<frame;j++)
                  {
                       if(pagetable->block_num[j]==temp)
                          pagetable->block_num[j]=Ins[i]/10;
                  }
             }
        }
    }
    double hitrate=(double)p_fault/ NUM;
    printf("fault num=%d\n",p_fault);
    printf("%f\n",1.0-hitrate);
    free(queue);
}
void LRU(PageTable* pagetable,int frame,PageQueue* queue)
{
    int p_fault=0;
    InitPageTable(pagetable,frame);
    
    for(int i=0;i<NUM;i++)
    {
        if(!isHitted(frame,i,pagetable))
        {
             p_fault++;
             int full_arg=0;
             for(int j=0;j<32;j++)
             {
                 if(pagetable->status[j]==true)
                      full_arg++;
             }
             if(full_arg<frame)
             {
                  pagetable->status[Ins[i]/10]=true;
                  pagetable->access_time[Ins[i]/10]=1;
                  incTime(pagetable);
                  for(int j=0;j<frame;j++)
                  {
                       if(pagetable->block_num[j]==-1)
                          pagetable->block_num[j]=Ins[i]/10;
                  }
             }
             else
             {
                  int temp=LeastUsed(pagetable);
                  pagetable->access_time[temp]=0;
                  pagetable->status[Ins[i]/10]=true;
                  pagetable->access_time[Ins[i]/10]=1;
                  pagetable->status[temp]=false;
                  for(int j=0;j<frame;j++)
                  {
                       if(pagetable->block_num[j]==temp)
                          pagetable->block_num[j]=Ins[i]/10;
                  }
             }
        }
    }
    double hitrate=(double)p_fault/ NUM;
    printf("fault num=%d\n",p_fault);
    printf("%f\n",1.0-hitrate);
    free(queue);
}
void OPT(PageTable* pagetable,int frame)
{
    int p_fault=0;
    InitPageTable(pagetable,frame);
    
    for(int i=0;i<NUM;i++)
    {
        if(!isHitted(frame,i,pagetable))
        {
             p_fault++;
             int full_arg=0;
             for(int j=0;j<32;j++)
             {
                 if(pagetable->status[j]==true)
                      full_arg++;
             }
             if(full_arg<frame)
             {
                  pagetable->status[Ins[i]/10]=true;
                  for(int j=0;j<frame;j++)
                  {
                       if(pagetable->block_num[j]==-1)
                          pagetable->block_num[j]=Ins[i]/10;
                  }
             }
             else
             {
                  int* distance=(int*)malloc(sizeof(int)*frame);
                  for(int k=0;k<frame;k++)
                     distance[k]=0;
                  for(int k=0;k<frame;k++)
                  {
                      for(int m=i+1;m<NUM;m++)
                      {
                           distance[k]++;
                           if(pagetable->block_num[k]==Ins[m]/10)
                               break;
                      }
                  }
                  int max=0;
                  for(int k=1;k<frame;k++)
                  {
                       if(distance[k]>=max)
                            max=k;
                  }
                  int temp=pagetable->block_num[max];
                  pagetable->status[Ins[i]/10]=true;
                  pagetable->status[temp]=false;
                  for(int j=0;j<frame;j++)
                  {
                       if(pagetable->block_num[j]==temp)
                          pagetable->block_num[j]=Ins[i]/10;
                  }
             }
        }
    }
    double hitrate=(double)p_fault/ NUM;
    printf("fault num=%d\n",p_fault);
    printf("%f\n",1.0-hitrate);
}
void LFR(PageTable* pagetable,int frame)
{
    int p_fault=0;
    InitPageTable(pagetable,frame);
    int* visited=(int*)malloc(sizeof(int)*frame);
    
    for(int i=0;i<frame;i++)
        visited[i]=0;
    for(int i=0;i<NUM;i++)
    {
        if(!isHitted(frame,i,pagetable))
        {
             p_fault++;
             int full_arg=0;
             for(int j=0;j<32;j++)
             {
                 if(pagetable->status[j]==true)
                      full_arg++;
             }
             if(full_arg<frame)
             {
                  pagetable->status[Ins[i]/10]=true;
                  for(int j=0;j<frame;j++)
                  {
                       if(pagetable->block_num[j]==-1)
                       {
                          pagetable->block_num[j]=Ins[i]/10;
                          visited[j]++;
                       }
                  }
             }
             else
             {
                  int min=0;
                  for(int k=1;k<frame;k++)
                  {
                       if(visited[min]>visited[k])
                          min=k;
                  }
                  int temp=pagetable->block_num[min];
                  pagetable->status[Ins[i]/10]=true;
                  pagetable->status[temp]=false;
                  for(int j=0;j<frame;j++)
                  {
                       if(pagetable->block_num[j]==temp)
                       {
                          pagetable->block_num[j]=Ins[i]/10;
                          visited[j]=1;
                       }
                  }
             }
        }
        else
        {
             int index=0;
             for(int k=0;k<frame;k++)
             {
                  if(pagetable->block_num[k]==Ins[i]/10)
                      index=k;
             }
             visited[index]++;
        }
    }
    double hitrate=(double)p_fault/ NUM;
    printf("fault num=%d\n",p_fault);
    printf("%f\n",1.0-hitrate);
    free(visited);
}
int main()
{
   InitIns();
   printf("This is FCFO scheme!\n");
   for(int frame=4;frame<=32;frame++)
   {
       PageTable* pagetable=(PageTable*)malloc(sizeof(PageTable));
       PageQueue* queue=(PageQueue*)malloc(sizeof(PageQueue));
       InitQueue(queue,frame);
       printf("frame num:%d\n",frame);
       FCFO(pagetable,frame,queue);
       free(pagetable);
   }
   printf("This is LRU scheme!\n");
   for(int frame=4;frame<=32;frame++)
   {
       PageTable* pagetable=(PageTable*)malloc(sizeof(PageTable));
       PageQueue* queue=(PageQueue*)malloc(sizeof(PageQueue));
       InitQueue(queue,frame);
       printf("frame num:%d\n",frame);
       LRU(pagetable,frame,queue);
       free(pagetable);
   }
   printf("This is OPT scheme!\n");
   for(int frame=4;frame<=32;frame++)
   {
       PageTable* pagetable=(PageTable*)malloc(sizeof(PageTable));
       printf("frame num:%d\n",frame);
       OPT(pagetable,frame);
       free(pagetable);
   }
   printf("This is LFR scheme!\n");
   for(int frame=4;frame<=32;frame++)
   {
       PageTable* pagetable=(PageTable*)malloc(sizeof(PageTable));
       printf("frame num:%d\n",frame);
       LFR(pagetable,frame);
       free(pagetable);
   }
   return 0;
   
}
