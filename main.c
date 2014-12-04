#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "paho-pic.h"
#include "transport.h"
#include "wificonnect.h"
#define NULL 0

unsigned char   remoteipAddress[4]  = {x, x,  x, x }; //ip of the broker server
int remotePort = 1883;                                //port of the broker
int localPort = 80;
char socketHandle, socketHandlesub;
char conn = 1;
char subflag = 0;
char pubflag = 0;
int subtmr = 0;

unsigned char bufresp[20] = {0};

char wificonnect();

////////////////////////////////////////////////////////////////////////////////
unsigned int wifiTmr = 0, netStatusTmr = 0;
void Timer1_interrupt() iv IVT_TIMER_1 ilevel 7 ics ICS_SOFT {
  wifiTmr++;
  netStatusTmr++;
  if(wifiTmr >= 5) {
    Net_Wireless_MCW1001_Time++ ;
    wifiTmr = 0;
  }
  T1IF_bit = 0 ;                     // clear timer1 overflow flag
}
////////////////////////////////////////////////////////////////////////////////

void interrupt() iv IVT_UART_2 ilevel 7 ics ICS_AUTO {
  Net_Wireless_MCW1001_Receive();
  U2RXIF_bit = 0;           // ensure interrupt not pending
}

////////////////////////////////////////////////////////////////////////////////
void Timer2_3Interrupt() iv IVT_TIMER_3 ilevel 6 ics ICS_AUTO{   //publish & subscribe timer
  EnableInterrupts();
  T3IF_bit = 0;
  LATA = ~ PORTA;
  //pubflag = 1;
  subflag = 1;
  subtmr++;
  if(subtmr == 10) {
            pubflag = 1;
            subtmr = 0;
  }
}
////////////////////////////////////////////////////////////////////////////////

void InitMcu() {

  JTAGEN_bit = 0;
  
  TRISA  = 0;               // initialize PORTB as output
  LATA   = 0xAAAA;          // Initialize PORTB value

  // Initialization of timer1 interrupt
  TMR1 = 0;                 // reset timer value to zero
  PR1 = 65535;              // Load period register

  T1IP0_bit = 1;            // set interrupt
  T1IP1_bit = 1;            // priority
  T1IP2_bit = 1;            // to 7

  TCKPS0_bit = 1;           // Set Timer Input Clock
  TCKPS1_bit = 1;           // Prescale value to 1:256

  T1IE_bit = 1;             // Enable Timer1 Interrupt
  ON__T1CON_bit = 1;        // Enable Timer1
  
  //TIMER 2-3  Enable Timer 2-3 Interrupt
  T2CON        = 0x0;
  T3CON        = 0x0;
  TMR2        = 0;
  TMR3        = 0;
  T3IE_bit = 1;
  T3IF_bit = 0;
  
  T3IP0_bit = 0;
  T3IP1_bit = 1;
  T3IP2_bit = 1;
  PR3 = 1220;
  PR2 = 47300;
  T2CONbits.TON         = 1;
  T2CONbits.T32         = 1;

  // Initialization of UART interrupt..............
  U2IP0_bit = 1;            // set interrupt
  U2IP1_bit = 1;            // priority
  U2IP2_bit = 1;            // to 7

  U2STA.B7 = 0;   // 0x = Interrupt flag bit is set when a character is received
  U2RXIF_bit = 0;           // ensure interrupt not pending
  U2RXIE_bit = 1;           // enable intterupt

  EnableInterrupts();       // Enable all interrupts

  Net_Wireless_MCW1001_HwReset();

}



void main() {

     unsigned int time;
     int res = 1;
     int buflen = 0, payloadlen;
     int publen;
     int len = 1;
     unsigned char buf[200] = {0};

     char topic[10] = "pippo";              //publish topic
     char topicsub[10]= "pic32";            //subscribe topic
     char payload[100] = "mypayload";       //payload
     char clientID[10] = "pic";             //clientID
     char * ptr = buf;
     short packetid = 0;
     buflen = sizeof(buf);
     
     InitMcu();                             //initialize MCU
     Sound_Init(&PORTD, 3);

     Sound_Play(800, 150);                  //starting!!!
     Delay_ms(50);
     Sound_Play(800, 150);
     wificonnect();                        //connect to your wifi network-
                                           //modify the file wificonnection.c
                                           
     //ping the broker host to see if it is available
     while(res != 0) {
               res = Net_Wireless_MCW1001_PingSend(remoteIPAddress, &time);
     }
     //open a TCP socket with the broker
     socketHandle = transport_open(remoteIPAddress, remotePort, localPort);
     //connect to the broker
     len = MQTTSNSerialize_connect(buf, buflen, clientID);
     do{
        transport_sendPacketBuffer(socketHandle, buf, len);
        do{
           buflen = transport_getdata(socketHandle,bufresp, 20);
        }while(buflen == 0);
     }while(conn = IsConnack(bufresp) != 0);
     //Subscribe to the topic
     len = MQTTSNSerialize_Subscribe(buf, topicsub, 1,1);
     transport_sendPacketBuffer(socketHandle, buf, len);
     //infinite loop evey 10 second it publish every 1 second it check if there
     //any messages
     while(1) {
              if(pubflag == 1) {
                         len = MQTTSNSerialize_publish(buf, payload, topic, packetID);
                         transport_sendPacketBuffer(socketHandle, buf, len);
                         pubflag = 0;
              }
              if(subflag == 1) {
                         buflen = transport_getdata(socketHandle,bufresp, 20);
                         if(buflen != 0 && isArrived(bufresp) == 1) {
                                   Sound_Play(440,1000);
                         }
              subflag = 0;
              }
     }
}
