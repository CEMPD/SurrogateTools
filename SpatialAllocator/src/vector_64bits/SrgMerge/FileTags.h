#ifndef __FILE_TAGS_H
#define __FILE_TAGS_H

#include<string.h>
struct FileTags{
  static const string OUTPUT_F = "OUTFILE=";
  static const string REF_F = "XREFFILE=";
  static const string CMD = "OUTSRG=";
  //string inputFTag = "INFILE=";
  static const string GAP = "GAPFILL=";
  static const char OBRACE = '{';
  static const char CBRACE = '}';
}
#endif
