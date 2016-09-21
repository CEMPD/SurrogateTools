#ifndef __GAPFILL_OUTPUT_H
#define __GAPFILL_OUTPUT_H
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <algorithm>
#include <math.h>
#include <vector>
#include <iostream>
#include "GapfillInputFile.h"
#include "ReadSrgInfo.h"
#include "Surrogates.h"
#include "Errors.h"

using namespace std;

class GapfillOutput{
  int writeHeader(ReadSrgInfo &info,char* pInputFName,FILE* fp);
  void writeData(GapfillInputFile &input,ReadSrgInfo &info, FILE* fp);
  int output(ReadSrgInfo &info,int outSrgID,vector<Surrogate> primSrg, FILE  *fp,Polygons &polys);
  char* myindex (char *buffer, int c);
  string trim(string& s);
  void output(ReadSrgInfo &info,int outSrgID, vector<Surrogates> srgsV,FILE *fp,Polygons &polys);
  void checkGridInfo(vector<string> &gridInfo);
  void freeDebugInfo(vector<Surrogates> srgsV);
  /**
   * reeturn the index to the end point of the county ID
   * @pre srg should be sorted in the asending order by county id
   * from < srg.size()
   * srg.at(from).countyID == countyID
   */
  long nextCountyID(long currCID,vector<Surrogate>&srg);
  void update(long cid, long nextICID [], vector<Surrogates> srgsV);
  /** @pre cid is exist in srgV and srgV is sorted
   */

  long endCIDIndex(long cid,vector<Surrogate> srgV);

  // void  print(long cid,vector<Surrogate>srg, FILE *fp);
  void  print(long cid, int outSrgID,vector<Surrogate>srg, FILE *fp,Polygons &polys,ReadSrgInfo &info);
  void  print(long cid, int outSrgID,vector<Surrogate>srg, FILE *fp,bool prim, Polygons &polys,ReadSrgInfo &info);
 public:
  int writeOutput(ReadSrgInfo &info, char* pInputFName, GapfillInputFile &input);  
  int readSrgtFile(const char* fname, int srgID, vector<Surrogate> &s,vector<string> &gridInfo,Polygons &polys );
  int readSrgFiles(Command aCommand,vector<Surrogates> &srgsV, ReadSrgInfo srgInfo,vector<string> &gridInfo, Polygons &polys);
};

#endif
