#include "terminal.hpp"
#include <string>
#include "mbed.h"


string banner=
"\nhello!!!"
"\nhello!!!"
"\nhello!!!"
"\nhello!!!"
"\nhello!!!"
"\nhello!!!"
"\nhello!!!"
"\nhello!!!"
"\nhello!!!"
"\nhello!!!"
"\nhello!!!"
"\nhello!!!"
"\nhello!!!"
"\\nterm$ ";

char LEFTKEY   =0x44;
char RIGHTKEY  =0x43;
char UPKEY     =0x41;
char DOWNKEY   =0x42;
// char LEFTKEY   =0x4B;
// char RIGHTKEY  =0x4D;
// char UPKEY     =0x48;
// char DOWNKEY   =0x50;

char arrowSeq1=0x1b;
char arrowSeq2=0x5b;
// char arrowSeq1=0xc3;
// char arrowSeq2=0xa0;
void serialTerminal::printStr(string& inStr){
  this->printf(inStr.c_str());
}
#ifdef TERMDEBUG
void serialTerminal::printDebug(string inStr){
  debug->printf("[DEBUG] ");
  debug->printf((char*)inStr.c_str());
}
void serialTerminal::printDebug(const char* inStr){
  debug->printf("[DEBUG] ");
  debug->printf(inStr);
}
#endif
// void serialTerminal::attach(Callback<void()> inFunc){
//   commandCallback=inFunc;
// }


recallBuffer::recallBuffer(uint8_t inMax):max(inMax){
}

void recallBuffer::add(std::string inStr){
    if (buffer.size()>=max){
        buffer.pop_back();
    }
    buffer.push_front(inStr);
    //return StatusResult{true;}
}
std::string recallBuffer::at(uint8_t pos){
    uint8_t limit;
    if (buffer.size()<max)
        limit=buffer.size();
    else
        limit=max;
    while(pos>=limit){
        pos-=limit;
    }
    return buffer.at(pos);
}
size_t recallBuffer::getSize(){
    return buffer.size();
}

serialTerminal::serialTerminal(PinName tx,PinName rx,int inbaud):RawSerial(tx,rx){// tx, rx
  //term=new Serial(tx,rx);
  myBuffer= new recallBuffer(10);
  // this->printf("before IRQ attach\n");
  this->attach(callback(this,&serialTerminal::serialIRQHandler));
  // this->printf("after IRQ attach\n");
  this->baud(inbaud);
  this->printf(banner.c_str());

  // commandThread = new Thread(callback(this,&serialTerminal::commandWorker));
  commandThread = new Thread(callback(&queue, &EventQueue::dispatch_forever));

  this->printf("\e[4;h");
}
void serialTerminal::attachParser(Callback<void(string)> inFunc){
  commandFunc=inFunc;
}
void serialTerminal::commandWorker(){
#ifdef TERMDEBUG
    this->printDebug("working...\n");
#endif
    string copyBuffer=commandBuffer;
    commandReady=false;
    commandBuffer="";
    this->printf("\n");
    commandFunc(copyBuffer);
    this->printf("term$ ");
}
#ifdef TERMDEBUG
void serialTerminal::setDebug(RawSerial* inSerial){
  debug=inSerial;
}
#endif

void serialTerminal::serialIRQHandler(){

    static bool arrowWatcher5B;
    static bool arrowWatcher;
    static uint8_t cursor; //horizontal position
    static uint8_t upkeys;
    while(this->readable()){
        char theChar=(char) this->getc();
        #ifdef TERMDEBUG
        debug->printf("%02X\n", theChar);
        #endif

        if(theChar=='\b'&& serialBuffer.size()>0 && cursor>0) //BACKSPACE
        {
          this->putc(theChar); //print back what you typed in
          this->printf("\e[s"); // save cursor position

          cursor--;
          serialBuffer.erase(cursor,1);

          this->printf("\rterm$ ");
          this->printf("\e[K");
          this->printf(serialBuffer.c_str());
          this->printf(" ");

          this->printf("\e[u");// restore cursor position

        }
        else if(theChar==0x03){//ctrl+c pressed
          this->printf("\nterm$ ");
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

              this->printf("\e[1;D");
            }
          }
          else if(theChar==RIGHTKEY){
            if(cursor<serialBuffer.length()){
              cursor++;

              this->printf("\e[1;C");
            }
            #ifdef TERMDEBUG
            printDebug("right key detected\n");
            #endif
          }
          else if(theChar==UPKEY){
            #ifdef TERMDEBUG
            printDebug("up key detected\n");
            #endif

            this->printf("\e[1;M\r"); // clear current row

            serialBuffer=myBuffer->at(upkeys);
            upkeys++;


            this->printf("\e[0;`"); // bounce cursor back to head

            this->printf("term$ ");
            this->printf(serialBuffer.c_str());
            cursor=serialBuffer.length();
          }
          else if(theChar==DOWNKEY){
            #ifdef TERMDEBUG
            printDebug("down key detected\n");
            #endif
            if(upkeys>=0){


            upkeys--;
            serialBuffer=myBuffer->at(upkeys);

            this->printf("\e[1;M\r"); // clear current row // bounce cursor back to head

            // this->printf("\e[0;`"); // bounce cursor back to head

            this->printf("term$ ");
            this->printf(serialBuffer.c_str());
            cursor=serialBuffer.length();
            }

          }
        }

        else if(theChar>=32){//theChar!='\b'&&theChar!='\t'&&theChar!='\r'&&theChar!='\n'){// do not append control characters
          // arrowWatcher sequece failed
          arrowWatcher=false;
          arrowWatcher5B=false;
          this->putc(theChar); //print back what you typed in
          serialBuffer.insert(cursor,1,theChar); // append latest char to string

          cursor++;
          #ifdef TERMDEBUG
          debug->printf("len: %u\n",serialBuffer.length());
          debug->printf("buff: ");
          printDebug(serialBuffer);
          debug->printf("\n");
          #endif

        }
        if(theChar=='\n' | theChar=='\r'){
          // if(serialBuffer.size()>1){
            commandBuffer=toUpper(serialBuffer);
            commandReady=true;
            // commandCallback();
            this->putc(theChar);
            // myBuffer->add(serialBuffer);
            serialBuffer="";
            cursor=0;
            upkeys=0;
            // queue.call(this,&serialTerminal::commandWorker);
          // }
          // else{
          //   this->printf("\nterm$ ");
          //   serialBuffer="";
          //   cursor=0;
          // }
        }

    }


}
