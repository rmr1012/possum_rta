#ifndef UTILITIES_H
#define UTILITIES_H
#include <cstring>
#include <stdio.h>
#include <vector>
#include <string>
#include <utility>
#include <stdarg.h>
std::vector<char> str2HexVec(std::string inStr, char deli);
bool strInStr(std::string needle,std::string haystack);
bool translateTruth(std::string inTruth);

std::string toUpper(std::string inStr);

class Inarticulate{
public:

    // static void bprintf(const char * inStr){
    //     buffer+=inStr;
    // }
    static void bprintf(const char *format, ...){
        va_list args;
        va_start(args, format);
        char buff[100];
        vsnprintf(buff, sizeof buff, format, args);
        // std::string buffAsStdStr = buff;
        va_end(args);
        buffer+=buff;
    }

    static std::string getBuffer(){
        std::string outbuf=buffer;
        buffer="";
        return outbuf;
    }
    static bool isReady(){
        return buffer != "";
    }
private:
    static std::string buffer;
};



#endif
