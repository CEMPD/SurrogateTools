#ifndef __READSRGINFO_H_
#define __READSRGINFO_H_
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <math.h>
#include <vector>
#include <iostream>

#include "GapfillInputFile.h"
#include "Errors.h"
#define MAXLINE 1024

using namespace std;

class ReadSrgInfo{

public:
  string gridInfo;
  vector<int> srgID;
  vector<string>srgName;
  bool addValue(int &key, string &value);
  bool isExist(int &key);
  void readGridInfo(string &fName);
  void findRefSrgID(string &fName, string &oSrgName, int &id);
  void readInputSrgFile(string refFName,vector<string>inputF,vector<string>iSrgNames);
  void readInfo(GapfillInputFile inputF);  
  int getID(string value);
  string trim(string& s);
};
#endif
