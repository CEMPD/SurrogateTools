#include "TestSrgFiles.h"

int allTest_testSrgFiles(){
  cout << "start testing TestSrgFiles" << endl;
  test_sameGridInfo();
  test_srgExistXRefFile();
  test_srgExistSrgFile();
  cout << "end testing TestSrgFiles" << endl;
  return 0;
}

/* This file will have test for checking whether srg file specified are have expected informations
 */

void test_sameGridInfo(){
  char* fName = "./testFiles/inputDiffHeader.txt";
  FILE* fp;
  string expMsg = "The surrogate files specified do not belong to the same grid";
  string tn = "test_sameGridInfo()";
  if( (fp=fopen(fName,"r")) == NULL){
    cerr << "Failed to open '" << fName << "'" << endl;
    exit(1);
  }
  GapfillInputFile input(fp);
  try{  
    input.readFile();
    ReadSrgInfo info;
    info.readInfo(input);
    GapfillOutput output;
    output.writeOutput(info,fName,input);
  }
  catch(SyntaxError e){
    cout << "Exception:" << e.toString()<< endl;
  }  
  catch(FileNotFoundException e){
    cout <<"Exception:" << e.toString()<< endl;
  }
  catch(ChkErrors e){
    result(expMsg,e.toString(),tn);
    //cout << "Exception: " << e.toString() << endl;
  }
  fclose(fp);
}

/**Test whether output surrogates specified is exist in xref file */
void test_srgExistXRefFile(){
  char* fName = "./testFiles/input_xrefNotExist.txt";
  FILE* fp;
  string fs = "./testFiles/srg_xref.txt";
  string oSrgName = "Filled Roads";
  string expMsg = "Could not find output surrogate name '"+ oSrgName+ 
    "' in the xref file '"+ fs +  "'.";
  string tn = "test_ExistXRefFile()";
  if( (fp=fopen(fName,"r")) == NULL){
    cerr << "Failed to open '" << fName << "'" << endl;
    exit(1);
  }
  GapfillInputFile input(fp);
  try{  
    input.readFile();
    ReadSrgInfo info;
    info.readInfo(input);
    //GapfillOutput output;
    //output.writeOutput(info,fName,input);
  }
  catch(SyntaxError e){
    cout << "Exception:" << e.toString()<< endl;
  }  
  catch(FileNotFoundException e){
    cout <<"Exception:" << e.toString()<< endl;
  }
  catch(ChkErrors e){
   result(expMsg,e.toString(),tn);
   //cout << "Exception: " << e.toString() << endl;
  }
  fclose(fp);
}


/**Test whether the input surrogates is exist in the surrogates file*/
void test_srgExistSrgFile(){
  char* fName = "./testFiles/input_srgNotExist.txt";
  FILE* fp;
  string oSrgName = "Roads";
  string fs = "./testFiles/test_diffHeader1.txt";
  string expMsg = "Could not find input surrogate name '"+ 
    oSrgName + "' in both the surrogate file '" + fs+
          "' and xref file.";
  string tn = "test_ExistSrgFile()";
  if( (fp=fopen(fName,"r")) == NULL){
    cerr << "Failed to open '" << fName << "'" << endl;
    exit(1);
  }
  GapfillInputFile input(fp);
  try{  
    input.readFile();
    ReadSrgInfo info;
    info.readInfo(input);
    GapfillOutput output;
    output.writeOutput(info,fName,input);
  }
  catch(SyntaxError e){
    cout << "Exception:" << e.toString()<< endl;
  }  
  catch(FileNotFoundException e){
    cout <<"Exception:" << e.toString()<< endl;
  }
  catch(ChkErrors e){
    result(expMsg,e.toString(),tn);
    //cout << "Exception: " << e.toString() << endl;
  }
  fclose(fp);
}
