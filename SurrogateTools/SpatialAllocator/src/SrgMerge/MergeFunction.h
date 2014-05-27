#ifndef __MERGEFUNCTION_H
#define __MERGEFUNCTION_H
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <math.h>
#include <vector>
#include <iostream>

#include "SyntaxError.h"
#include "Errors.h"

using namespace std;
#define MAXLINE 1024

//In a Function
struct Function{
  vector<string> varName;
  vector<string> fileName;
  vector<string> srgName;
  vector<int> srgID;
  string outSrgName;
  int outSrgID;
};


class MergeFunction{
 public:
  string oFName;
  vector<Function> funcVar ;
  vector<string>infixFunc;
  string trim(string &s);
  void readMergeFunction(string &aLine,string refFName, int lineCounter);
  void addFunctionInfo(string refFName,string &info, Function &func);
  void readGridInfo(string &fName);
  void findRefSrgID(string &fName, string &oSrgName,int &refID);
  void findRefID(string refFName, string oSrgName, Function &func );
};
#endif
