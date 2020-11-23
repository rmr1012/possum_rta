#include "utilities.hpp"


std::vector<char> str2HexVec(std::string inStr, char deli){

    std::vector<char> hexvals;
    char * pch;
    unsigned int temp;

    pch = strtok ((char*)inStr.c_str(),&deli);
    while (pch != NULL)
    {
        sscanf(pch, "%X", &temp);
        pch = strtok (NULL, &deli);
        if(temp!='\0')
            hexvals.push_back((char)temp);
    }
    return hexvals;
}

bool strInStr(std::string needle,std::string haystack){
  if(haystack.find(needle) != std::string::npos)
    return true;
  return false;
}

bool translateTruth(std::string inTruth){
  if(inTruth=="TRUE"||inTruth=="1"||inTruth=="HIGH")
    return true;
  else
    return false;
}


std::string toUpper(std::string inStr){
    std::string outStr;
    for(char c :inStr){
        //outStr+=(c-0x20);
        outStr+=toupper(c);
    }
    return outStr;
}
