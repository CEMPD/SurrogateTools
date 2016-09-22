#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <math.h>
#include <vector>
#include <iostream>
#include "GapfillInputFile.h"
#include "ReadSrgInfo.h"
#include "GapfillOutput.h"
#include "MergeOutput.h"
#include "SyntaxError.h"
#include "FileErrors.h"
#include "Errors.h"

#ifdef GAPFILL__MAIN
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

string VERSION="Version 2, 10/31/2006"; 

char *prog_name;

int main(int numargs, char* args[]){
  if(numargs != 2 )
  {
    cerr << "usage: srgmerge.exe srgmerge_input.txt " << endl;
    exit(1);
  }
  cout << "Running srgmerge " << VERSION << endl;
  cout << "Using input file '" << args[1] << "'" << endl;
  FILE* fp;
  if( (fp=fopen(args[1],"r")) == NULL){
    cerr << "The input file '" << args[1] << "'" 
         << " failed to open." << endl;
    exit(1);
  }
  
  prog_name = args[0];

  GapfillInputFile input(fp);
  try{    
    input.readFile();     
    fclose(fp);
    if(input.GAPFILL){
      ReadSrgInfo info;
      info.readInfo(input);
      GapfillOutput output;
      output.writeOutput(info,args[1],input);
    }
    else{
      MergeOutput output;
      output.writeOutput(input.mergeFunc,args[1]);
    }
  }
  catch(SyntaxError e){
    cerr << "Exception:" << e.toString()<< endl;
    fclose(fp);
    exit(1);
  }  
  catch(FileNotFoundException e){
    cerr <<"Exception:" << e.toString()<< endl;
    fclose(fp);
    exit(1);
  }
  catch(ChkErrors e){
    cerr << "Exception:" << e.toString() << endl;
    fclose(fp);
    exit(1);
  }
  
  cout << "Srgmerge completed sucessfully" << endl;
  return 0;
}//main()

#endif
/** =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */
