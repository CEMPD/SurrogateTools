/*
 *This program reads an gapfill inputfile and then process the 
 *input file according to the convention.
 *OUTFILE will be the output file
 *OUTSRG will be the output surrogate and then primary, secondary, tertiary and
 *quadary surrogates are defined. Also the corresponding input files for these
 * surrogates. 
 * The important assumption here is that, it assumed that final surrogate will 
 * be a superset of the other surrogates for the entries.
 * Developed by Parthee R Partheepan CEP, UNC Chapel Hill,
 * in support of the EPA Multimedia Integrated Modeling System, 2004.
 */

#ifndef __GAPFILLINPUTFILE_H
#define __GAPFILLINPUTFILE_H
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <math.h>
#include <vector>
#include <iostream>
#include "StringTok.h"
#include "MergeFunction.h"

using namespace std;

struct Command{
  string  oSrgName;
  vector<string>inputF;
  vector<string>iSrgNames;
};


class GapfillInputFile {
  void readGapfillCmdInput(string &aLine,string cmdTag,int &lineCounter);
  FILE* inputFile;
 public:
  bool GAPFILL;
  bool MERGE;
  MergeFunction mergeFunc;
  string outputF;
  string refSrgF;
  GapfillInputFile(FILE* fi);
  ~GapfillInputFile();
  vector<Command> commands;
  int readFile();
  void isFileExist(string fName, int lineCounter);
  string trim(string &s);
};  

#endif
