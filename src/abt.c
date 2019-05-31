#include "../include/simulator.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<strings.h>
#include<stdbool.h>
#define MSG_SIZE 20


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

/********* STUDENTS WRITE THE NEXT SIX ROUTINES *********/
static int bit=0;

static int ack_n=0;
static int bit_r=0;
static int msg_in_transit=0;

static char *var_msg=NULL;
static int q_start=0;
static int q_end=0;
int i;
char *d;
int sum;
 int cksm=0;

 struct queue {
  char *buff;
};
static struct queue q[1500];


/* called from layer 5, passed the data to be sent to other side */
void A_output(message)
  struct msg message;
{

      char *c=(char*) malloc(sizeof(char)*MSG_SIZE);
      memset(c,'\0',MSG_SIZE);
      sum=0;
      strcpy(q[q_end].buff,message.data);
      q_end++;
      if(msg_in_transit==0)
        {
           msg_in_transit=1;

            strcpy(c,q[q_start].buff);
            for( i=0;i<20;i++)
              {
              sum=sum+(int)c[i];
              }  
             struct pkt packet;
              packet.seqnum=bit;

              packet.acknum=123;
              packet.checksum=(sum+bit+123);
              cksm=packet.checksum;
              strcpy(packet.payload,q[q_start].buff);
              strcpy(var_msg,q[q_start].buff);

              tolayer3(0,packet);
              q_start++;
              starttimer(0,10.0); 

        } 

}

/* called from layer 3, when a packet arrives for layer 4 */
void A_input(packet)
  struct pkt packet;
{

  if(packet.acknum==bit)
  {

    bit=!(bit);
    stoptimer(0);
    msg_in_transit=0;

  }
}

/* called when A's timer goes off */
void A_timerinterrupt()
{

  struct pkt packet;
          packet.seqnum=bit;
          packet.checksum=cksm;
          packet.acknum=123;
          strcpy(packet.payload,var_msg);
          printf("%s\n",packet.payload);
          tolayer3(0,packet);
          starttimer(0,10.0); 

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
      }

}

/* Note that with simplex transfer from a-to-B, there is no B_output() */

/* called from layer 3, when a packet arrives for layer 4 at B*/
void B_input(packet)
  struct pkt packet;
{
         sum=0;

    if(packet.seqnum == bit_r)
   {        char *d=(char*) malloc(sizeof(char)*MSG_SIZE);
            memset(d,'\0',MSG_SIZE);  
              strcpy(d,packet.payload);

            for(int i=0;i<20;i++)
            {
            sum=sum+(int)d[i];
           } 
          sum=sum+packet.seqnum+packet.acknum;
                 
      if(sum == packet.checksum)
       { 
          packet.acknum=packet.seqnum;
          bit_r=!(bit_r);

          tolayer3(1,packet);

          tolayer5(1,d);

      }
    }

}

/* the following routine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
void B_init()
{
    sum=0;
  
}
