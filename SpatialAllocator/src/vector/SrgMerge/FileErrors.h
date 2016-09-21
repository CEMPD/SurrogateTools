#ifndef __FILE_ERRORS_H
#define __FILE_ERRORS_H

#include<string>

class FileNotFoundException{
  string msg;
  
 public:
  FileNotFoundException(string aString){
    msg = aString;
  }
 
  string toString(){
    return msg;
  }
};

#endif
