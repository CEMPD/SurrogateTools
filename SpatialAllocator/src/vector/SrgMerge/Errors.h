#ifndef __ERRORS__H
#define __ERRORS__H

#include<string>

using namespace std;
class ChkErrors{
  string msg;
  
 public:
  ChkErrors(string aString){
    msg = aString;
  }
 
  string toString(){
    return msg;
  }
};
#endif
