#ifndef __SYNTAX_ERROR__H
#define __SYNTAX_ERROR__H

#include<string>
using namespace std;
class SyntaxError{
  string msg;
  
 public:
  SyntaxError(string aString){
    msg = aString;
  }
 
  string toString(){
    return msg;
  }
};
#endif
