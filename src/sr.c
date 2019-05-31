#include "../include/simulator.h"
#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>
#include<strings.h>
#include<time.h>

#define MSG_SIZE 20

struct queue {  
  char*buff;
  int cksm;
  int seqnum; 
  float expire;
  int ackornot;
};

static float current_time=0.0;
static float time_per_packet=27.0; 
static struct queue q[1000];
static struct queue q_recv[1000];


static int bit=0;
static int secnumsend=0;
static int secnumrecv=0;

static int acknumsend=0;
static int acknumrecv=0;
static int msg_in_transit=0;
static int checks[1500];
static char *var_msg=NULL;
static int q_start=0;
static int q_end=0;
static int q_startrec=0;
static int q_stoprec=0;
int i;
char *d;
int sum;
 
static int windowsize=0;
static int reqdnumber=0;





/* ******************************************************************
 ALTERNATING BIT AND GO-BACK-N NETWORK EMULATOR: VERSION 1.1  J.F.Kurose

   This code should be used for PA2, unidirectional data transfer 
   protocols (from A to B). Network properties:
   - one way network delay averages five time units (longer if there
     are other messages in the channel for GBN), but can be larger
   - packets can be corrupted (either the header or the data portion)
     or lost, according to user-defined probabilities
   - packets will be delivered in the order in which they were sent
     (although some can be lost).
**********************************************************************/

/********* STUDENTS WRITE THE NEXT SEVEN ROUTINES *********/

/* called from layer 5, passed the data to be sent to other side */
void A_output(message)
  struct msg message;
{
   
      char *c=(char*) malloc(sizeof(char)*MSG_SIZE);
      memset(c,'\0',MSG_SIZE);
      sum=0;
      strcpy(q[q_end].buff,message.data);
      q_end++;
      for(i =0;i<windowsize;i++)
        { 
          if (q_end>q_start)
            { 
              strcpy(c,q[q_start].buff);
                for( i=0;i<20;i++)
                {
                  sum=sum+(int)c[i];
                }  
                struct pkt packet;
                packet.seqnum=secnumsend;
                q[q_start].seqnum=secnumsend;
                packet.acknum=123;

                packet.checksum=(sum+secnumsend+123);
            q[q_start].cksm=packet.checksum;
                strcpy(packet.payload,q[q_start].buff);
                q[q_start].expire=current_time+time_per_packet;
          secnumsend=secnumsend+1;
              tolayer3(0,packet);
                q_start++;
            
            }
        } 




}

/* called from layer 3, when a packet arrives for layer 4 */
void A_input(packet)
  struct pkt packet;
{
    if(packet.acknum>=acknumsend)
    { 
      if(packet.acknum<acknumsend+windowsize)
      {

        q[packet.acknum].ackornot=1;

      }
  }


}

/* called when A's timer goes off */
void A_timerinterrupt()
{
          current_time=current_time+1.0;
  
      if(secnumsend-windowsize>0)  
         {
            for(i=acknumsend;i<(windowsize-secnumsend);i++)
            {
              if(q[i].ackornot==0 && q[i].expire<current_time)
              {   
                  struct pkt packet;
                  packet.seqnum=i;
                  packet.checksum=q[i].cksm;
                  packet.acknum=123;
                  strcpy(packet.payload,q[i].buff);
                  printf("%s\n",packet.payload);
                  tolayer3(0,packet);
                  q[i].expire=current_time+time_per_packet;
                    
              }
              
          }starttimer(0,1.0);
          
        }
        else
        {
           for(i=acknumsend;i<secnumsend-1;i++)
            { if(q[i].ackornot==0 && q[i].expire<current_time)
              { 
                struct pkt packet;
                packet.seqnum=i;
                packet.checksum=q[i].cksm;
                packet.acknum=123;
                strcpy(packet.payload,q[i].buff);
                printf("%s\n",packet.payload);
                tolayer3(0,packet);
                q[i].expire=current_time+time_per_packet;
                
              }
              
              
          }starttimer(0,1.0);
          
       
    }
}  

/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
void A_init()
{
   q_start=0;
      q_end=0;
      sum=0;
      var_msg=(char*) malloc(sizeof(char)*MSG_SIZE);
      memset(var_msg,'\0',MSG_SIZE);
      for(i=0;i<1000;i++)
      {
          q[i].buff =(char*) malloc(sizeof(char)*MSG_SIZE);
          memset(q[i].buff,'\0',MSG_SIZE);
          q[i].ackornot=0;
      }
      windowsize=getwinsize();
      starttimer(0,1.0);
}

/* Note that with simplex transfer from a-to-B, there is no B_output() */

/* called from layer 3, when a packet arrives for layer 4 at B*/
void B_input(packet)
  struct pkt packet;
{
  sum=0;
      if(packet.seqnum>=secnumrecv && packet.seqnum < secnumrecv+windowsize)
      {          char *d=(char*) malloc(sizeof(char)*MSG_SIZE);
                  memset(d,'\0',MSG_SIZE);  
                  strcpy(d,packet.payload);
                  for(int i=0;i<20;i++)
                  {
                    sum=sum+(int)d[i];
                  } 
                sum=sum+packet.seqnum+packet.acknum;
                if(sum == packet.checksum)
                { 
                  if (packet.seqnum==secnumrecv)
                    {
                      tolayer5(1,d);
                      secnumrecv=secnumrecv+1;
          
                }
                    q_recv[packet.seqnum].buff=packet.payload;
                    packet.acknum=packet.seqnum;
                    tolayer3(1,packet);
                  
              }
      }
}

/* the following rouytine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
void B_init()
{
          sum=0;

     for(i=0;i<1000;i++)
  {
    q_recv[i].buff=(char*)malloc(sizeof(char)*MSG_SIZE);                                                     //initialize buffer  recv
    memset(q_recv[i].buff,'\0',MSG_SIZE);
  }
  windowsize=getwinsize();
}
