

#include "mbed.h"
#include "terminal.hpp"
#include "utilities.hpp"
#include "mbed.h"
#include "Dht11.h"
#include <iostream>
#include <list>
// #include <chrono>
// Serial pc(USBTX, USBRX);
AnalogIn  ch1(A4);
AnalogIn  ch2(A5);

DigitalOut heartbeat(LED1);

DigitalOut fridge(PA_11);
DigitalOut inverter(PA_12);

AnalogIn dimmer(A0);
PwmOut light(PC_6);
Dht11 roomTemp(PA_13);
Dht11 driveTemp(PA_14);


Serial theTerm(USBTX,USBRX,115200);// tx, rx

EventQueue queue(32 * EVENTS_EVENT_SIZE);
Thread t,thb;

static char LEFTKEY   =0x44;
static char RIGHTKEY  =0x43;
static char UPKEY     =0x41;
static char DOWNKEY   =0x42;
static char arrowSeq1 =0x1b;
static char arrowSeq2 =0x5b;
string commandBuffer;


void parseCommand();
void toggleRelay(int chan, bool onoff);

void parseCommand(){
  string cmd= commandBuffer;
  if (strInStr("RELAY",cmd)){ // RELAY
    // printf("precall %s\n",cmd);
    int chnum=stoi(cmd.substr(6,7));
    int onoff=stoi(cmd.substr(9,9));
    toggleRelay(chnum,onoff);

  }
  else if (strInStr("STATUS",cmd)){ // print all stages
    roomTemp.read();
    driveTemp.read();
    ThisThread::sleep_for(200);
    printf("CH1: %.4f, CH2: %.4f, DM: %.4f, IT: %.2f, IH: %d, CT: %.2f, CH: %d, FR %i, IV: %i \n", ch1.read(), ch2.read(), dimmer.read(), roomTemp.getFahrenheit(), roomTemp.getHumidity(), driveTemp.getFahrenheit(), driveTemp.getHumidity(), (bool)fridge, (bool)inverter);

  }
  else if (strInStr("RESET ",cmd)){ // print all stages
      // queue.call(selftest);
  }

  else if (strInStr("HELP",cmd)){
      printf("POSSUM Real Time Assistant :)\n"
      "Have a nice trip!\n"
      "Commands:\n"
      "RELAY [ch(i 2.0)] [0|1]\n"
      "Toggle relay to state\n"
      "RESET\n"
      "Donezo\n");
  }


}
void toggleRelay(int chan, bool onoff){
  printf("in toggle %i %i\n",chan,onoff);
  switch (chan) {
    case 0:
      fridge=onoff;
      printf("fridge %i\n",onoff);
      break;
    case 1:
      inverter=onoff;
      printf("inverver %i\n",onoff);
      break;
  }
}
template <unsigned N>
double approxRollingAverage (double avg, double input) {
    avg -= avg/N;
    avg += input/N;
    return avg;
}

void hbThread()
{
    while (true) {
        printf("I'm alive\n")
        heartbeat = !heartbeat;
        ThisThread::sleep_for(500);
    }
}

int main() {
    thb.start(hbThread);
    t.start(callback(&queue, &EventQueue::dispatch_forever));
    wait(0.1);
    printf("Hello RTA \n");
    theTerm.puts("$rta ");

    fridge=1;
    inverter=1;

    light.period(0.005f);      // 4 second period
    light.write(0.20f);      // 50% duty cycle, relative to period

    int counter=20;
    double avg=0;


    bool ctrlc;

    bool commandReady;


    // Thread* commandThread;
    // EventQueue queue;

    Callback<void(string)> commandFunc;
    string serialBuffer;
    recallBuffer* myBuffer;// recall last 10 commands

    static bool arrowWatcher5B;
    static bool arrowWatcher;
    static uint8_t cursor; //horizontal position
    static uint8_t upkeys;

    while(1){
      heartbeat = !heartbeat;
       if (dimmer.read()<=0.05){
         light.write(0);
       }
       else{
         avg=approxRollingAverage<1000>(avg,(dimmer.read()-0.05)/0.95);
         light.write(avg);

       }

       while(theTerm.readable()){
           char theChar=(char) theTerm.getc();
           // printf("-");
           #ifdef TERMDEBUG
           debug->printf("%02X\n", theChar);
           #endif

           if(theChar=='\b'&& serialBuffer.size()>0 && cursor>0) //BACKSPACE
           {
             theTerm.putc(theChar); //print back what you typed in
             printf("\e[s"); // save cursor position

             cursor--;
             serialBuffer.erase(cursor,1);

             printf("\rterm$ ");
             printf("\e[K");
             printf(serialBuffer.c_str());
             printf(" ");

             printf("\e[u");// restore cursor position

           }
           else if(theChar==0x03){//ctrl+c pressed
             printf("\nterm$ ");
             ctrlc=true;
           }
           else if(theChar==arrowSeq1){ // got ESC char
             arrowWatcher5B=true;
             //printDebug("seq1\n");
           }
           else if(theChar==arrowSeq2&&arrowWatcher5B){ // ESC got pressed previously
             arrowWatcher5B=false;
             arrowWatcher=true;
             // printDebug("seq2\n");
           }
           else if((theChar==LEFTKEY||theChar==RIGHTKEY||theChar==UPKEY||theChar==DOWNKEY)&&arrowWatcher){
             // printDebug("seq3\n");
             arrowWatcher=false;

             if(theChar==LEFTKEY){
               #ifdef TERMDEBUG
               printDebug("left key detected\n");
               #endif
               if(cursor>0){
                 cursor--;

                 printf("\e[1;D");
               }
             }
             else if(theChar==RIGHTKEY){
               if(cursor<serialBuffer.length()){
                 cursor++;

                 printf("\e[1;C");
               }
               #ifdef TERMDEBUG
               printDebug("right key detected\n");
               #endif
             }
             else if(theChar==UPKEY){
               #ifdef TERMDEBUG
               printDebug("up key detected\n");
               #endif

               printf("\e[1;M\r"); // clear current row

               serialBuffer=myBuffer->at(upkeys);
               upkeys++;


               printf("\e[0;`"); // bounce cursor back to head

               printf("term$ ");
               printf(serialBuffer.c_str());
               cursor=serialBuffer.length();
             }
             else if(theChar==DOWNKEY){
               #ifdef TERMDEBUG
               printDebug("down key detected\n");
               #endif
               if(upkeys>=0){


               upkeys--;
               serialBuffer=myBuffer->at(upkeys);

               printf("\e[1;M\r"); // clear current row // bounce cursor back to head

               // printf("\e[0;`"); // bounce cursor back to head

               printf("term$ ");
               printf(serialBuffer.c_str());
               cursor=serialBuffer.length();
               }

             }
           }

           else if(theChar>=32){//theChar!='\b'&&theChar!='\t'&&theChar!='\r'&&theChar!='\n'){// do not append control characters
             // arrowWatcher sequece failed
             arrowWatcher=false;
             arrowWatcher5B=false;
             theTerm.putc(theChar); //print back what you typed in
             serialBuffer.insert(cursor,1,theChar); // append latest char to string

             cursor++;
             #ifdef TERMDEBUG
             debug->printf("len: %u\n",serialBuffer.length());
             debug->printf("buff: ");
             printDebug(serialBuffer);
             debug->printf("\n");
             #endif

           }
           if(theChar=='\n'){
             // if(serialBuffer.size()>1){
               // printf("|");
               commandBuffer=toUpper(serialBuffer);
               commandReady=true;
               // commandCallback();
               theTerm.putc(theChar);

               // myBuffer->add(serialBuffer);
               serialBuffer="";
               cursor=0;
               upkeys=0;
               parseCommand();
               theTerm.puts("$rta ");

           }


        }
  }
}


// serialTerminal theTerm(USBTX,USBRX,1250000);// tx, rx
