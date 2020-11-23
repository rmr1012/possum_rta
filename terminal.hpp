#ifndef TERMINAL_H
#define TERMINAL_H

#include <string>
#include <deque>
#include "mbed.h"
#include "utilities.hpp"




class recallBuffer{
  public:
    recallBuffer(uint8_t);
    uint8_t max=20;
    std::deque<std::string> buffer;
    void add(std::string inStr);
    //void clear();
    std::string at(uint8_t pos); // check if 3 chars are legal
    size_t getSize();
};


class serialTerminal: public RawSerial{
public:
  serialTerminal(PinName,PinName,int);// tx, rx
  bool ctrlc;
  void serialIRQHandler();
  serialTerminal* theTerm;
  bool commandReady;


  Thread* commandThread;
  void commandWorker();

  EventQueue queue;

  void attachParser(Callback<void(string)>);
  Callback<void(string)> commandFunc;

  string serialBuffer;
  string commandBuffer;
  recallBuffer* myBuffer;// recall last 10 commands

  void printStr(string&);
  #ifdef TERMDEBUG
  void setDebug(RawSerial *);
  RawSerial* debug;
  void printDebug(string);
  void printDebug(const char*);
  #endif
};



#endif
