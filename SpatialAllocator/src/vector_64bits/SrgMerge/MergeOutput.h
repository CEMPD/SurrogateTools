#ifndef __MERGE_OUTPUT_H
#define __MERGE_OUTPUT_H
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <algorithm>
#include <math.h>
#include <vector>
#include <iostream>
#include "MergeFunction.h"
#include "Surrogates.h"
#include "Errors.h"


using namespace std;

#define LARGE_VALUE 10000000
#define TOLERANCE 1e-5
struct CountyTotal {
  map<long,double> cTotal;
};

struct ExistSum{
  bool existAll;
  bool initSum;
};


class MergeOutput{
  int getMinSrg(int track[],bool trackFinish [],Surrogate &minSrg);
  int compare(Surrogate &a, Surrogate &b);
 public:
  vector<Surrogates> srgsV;
  //int equal(Surrogate a);
  int readSrgFiles(Function &aFunc,vector<CountyTotal> &totals,vector<string> &gridInfo, Polygons &polys);
  void makeAllCounty(vector<CountyTotal> &total,map<long,ExistSum> &allCounty);
  void moveIndex(long trackE[],double trackS[], int i, CountyTotal ct);
  bool findMin(long track[],long &min,int size);
  string trim(string& s);
  void checkGridInfo(vector<string> gridInfo);
  void writeOutput(MergeFunction &mergeFunc, char* inputFN);   
  void writeHeader(vector<Function> &a, FILE* fp, char* inputFN, string &headerInfo);
  void readGridInfo(string fName,string &headerInfo);
  int readSrgFile(const char* fname, int srgID,vector<Surrogate> &s ,CountyTotal &total,vector<string> &gridInfo, Polygons &polys);
  char * myindex (char *buffer, int c);
};
#endif

