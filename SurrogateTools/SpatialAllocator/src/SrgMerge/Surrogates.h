#ifndef __SURROGATES_H
#define __SURROGATES_H

#include <vector>

struct Surrogate{		
  int surrID;	
  long countyID;
  int lat;	
  long lon;
  double  value;	
  char *debugInfo;
};	

struct Surrogates{
  vector<Surrogate> srgV;
};


struct Polygons{
  vector<string>polyIDs;
  vector<long>polyIDIndexes;
};
#endif
