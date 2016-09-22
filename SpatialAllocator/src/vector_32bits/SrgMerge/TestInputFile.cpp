#include "TestInputFile.h"


int allTest_testInputFile(){
  cout << "start testing TestInputFiles" << endl;
  testSE_outfile();
  testSE_refFile();
  testSE_cmdLine();
  testSE_delim();
  testFNF_refFile();
  testFNF_srgFile();
  testSE_div();
  testSE_subtraction();
  testSE_dupSrgEntries();
  testSE_xrefDupEntries();
  cout << "end of testing TestInputFiles" << endl;
        return 0;
}

void result(string expMsg,string resMsg,string testName){
  if(expMsg.compare(resMsg)==0){
    cout << "   " << testName << ": Sucess" <<endl; 
  }
  else{
    cout << "   " <<  testName << ": Failed" <<endl; 
  }
}

/** Syntax Error: Information file does not have output file*/
void testSE_outfile(){
  char* fName = "./testFiles/SE_outfile.txt";
  FILE* fp ;
  string expMsg = "Syntax Error: output file name incorrectly specified ";
  string tn = "testSE_outfile()";
  if( (fp=fopen(fName,"r")) == NULL){
    cerr << "Fail to open the srgmerge input file '" << fName << "'" << endl;
    exit(1);
  }  
  GapfillInputFile input(fp);  
  
  try{
    input.readFile();
  }
  catch(SyntaxError e){
    string resMsg = e.toString();
    result(expMsg,resMsg,tn);      
  }
  fclose(fp); 
}

/** Syntax Error: Information file does not have ref file*/
void testSE_refFile(){
  char* fName = "./testFiles/SE_refFile.txt";
  FILE* fp ;
  string expMsg = "Syntax Error: reference file name incorrectly specified.";
  string tn = "testSE_refFile()";
  if( (fp=fopen(fName,"r")) == NULL){
    cerr << "Fail to open the srgmerges input file '" << fName << "'" << endl;
    exit(1);
  }  
  GapfillInputFile input(fp);  
  
  try{
    input.readFile();
  }
  catch(SyntaxError e){
    result(expMsg,e.toString(),tn);
    //cout << "Error Message: " << e.toString()<< endl;
  }
  fclose(fp); 
}

/** Syntax Error: Not Specifying Surrogate information with "OUTSRG=" */
void testSE_cmdLine(){
  char* fName = "./testFiles/SE_cmdLine.txt";
  FILE* fp ;
  string expMsg = "Syntax Error: Check whether tags are correctly specified";
  string tn = "testSE_cmdLine";
  if( (fp=fopen(fName,"r")) == NULL){
    cerr << "Fail to open the srgmerge input file '" << fName << "'" << endl;
    exit(1);
  }  
  GapfillInputFile input(fp);  
  
  try{
    input.readFile();
  }
  catch(SyntaxError e){
    result(expMsg,e.toString(),tn);
    //cout << "Error Message: " << e.toString()<< endl;
  }
  fclose(fp);
}

/** Syntax Error: Incorrect delimiter */
void testSE_delim(){
  char* fName = "./testFiles/SE_delim.txt";
  FILE* fp ;
  string expMsg = "Syntax Error: check the function specification. Expecting a '|' between file name and surrogate name";
  string tn = "testSE_delim";
  if( (fp=fopen(fName,"r")) == NULL){
    cerr << "Fail to open the srgmerge input file '" << fName << "'" << endl;
    exit(1);
  }  
  GapfillInputFile input(fp);  
  
  try{
    input.readFile();
  }
  catch(SyntaxError e){
    result(expMsg,e.toString(),tn);
    //cout << "Error Message: " << e.toString()<< endl;
  }
  fclose(fp);
}

/**File Not Exist Error: ref file */ 

void testFNF_refFile(){
  
  char* fName = "./testFiles/fileNotExist_refFile.txt";
  FILE* fp ;
  string expMsg = "File Not Found: The file './testFiles/no_file.txt' does not exist";
  string tn = "tesetFNF_refFile()";
  if( (fp=fopen(fName,"r")) == NULL){
    cerr << "Fail to open the srgmerge input file '" << fName << "'" << endl;
    exit(1);
  }  
  GapfillInputFile input(fp);  
  
  try{
    input.readFile();
  }
  catch(FileNotFoundException e){
    result(expMsg,e.toString(),tn);
    //cout << "Error Message: " << e.toString()<< endl;
  }
  fclose(fp);
  
}


/** File Not Exist Error: file for a surrogate */
void testFNF_srgFile(){
  char* fName = "./testFiles/fileNotExist_srgFile.txt";
  FILE* fp ;
  string expMsg = "File Not Found: The file './testFiles/no_srgFile.txt' does not exist";
  string tn = "testFNF_srgFile()";
  if( (fp=fopen(fName,"r")) == NULL){
    cerr << "Fail to open the srgmerge input file '" << fName << "'" << endl;
    exit(1);
  }  
  GapfillInputFile input(fp);  
  
  try{
    input.readFile();
  }
  catch(FileNotFoundException e){
    result(expMsg,e.toString(),tn);
    //cout << "Error Message: " << e.toString()<< endl;
  }
  fclose(fp);

}

/**Syntax Error: Division is not allowed for Merging */
void testSE_div(){
  char* fName = "./testFiles/merge_inputDiv.txt";
  FILE* fp ; 
  string expMsg = "SyntaxError: Subtraction and Division are not allowed in the function specfiction";
  string tn = "testSE_div()";
  if( (fp=fopen(fName,"r")) == NULL){
    cerr << "Fail to open the srgmerge input file '" << fName << "'" << endl;
    exit(1);
  }  
  GapfillInputFile input(fp);  
  
  try{
    input.readFile();
  }
  catch(SyntaxError e){
    result(expMsg,e.toString(),tn);
    //cout << "Error Message: " << e.toString()<< endl;
  }
  fclose(fp);
}

/**Syntax Error: Subtraction is not allowed for Merging */
void testSE_subtraction(){
  char* fName = "./testFiles/merge_inputSub.txt";
  string expMsg = "SyntaxError: Subtraction and Division are not allowed in the function specfiction";
  string tn = "testSE_subtraction()";
  FILE* fp ; 
  if( (fp=fopen(fName,"r")) == NULL){
    cerr << "Fail to open the srgmerge input file '" << fName << "'" << endl;
    exit(1);
  }  
  GapfillInputFile input(fp);  
  
  try{
    input.readFile();
  }
  catch(SyntaxError e){
    result(expMsg,e.toString(),tn);
    //cout << "Error Message: " << e.toString()<< endl;
  }
  fclose(fp);

}

/**Syntax Error: Duplicate input surrogates entries are not allowed for GAPFILL"*/
void testSE_dupSrgEntries(){
  char* fName = "./testFiles/inputDupSrgs.txt";
  string aSrgName = "Airports";
  string expMsg = "Syntax Error: Duplicate entries for '" + aSrgName + 
    "' in the GAPFILL statement, cannot GAPFILL.";
  string tn = "testSE_dupSrgEntries()";
  FILE* fp ; 
  if( (fp=fopen(fName,"r")) == NULL){
    cerr << "Fail to open the srgmerge input file '" << fName << "'" << endl;
    exit(1);
  }  
  GapfillInputFile input(fp);  
  
  try{
    input.readFile();
  }
  catch(SyntaxError e){
    result(expMsg,e.toString(),tn);
    //cout << "Error Message: " << e.toString()<< endl;
  }
  fclose(fp);

}

/**Syntax Error: Duplicate output surrogates entries in the xref file"*/
void testSE_xrefDupEntries(){
  char* fName = "./testFiles/xrefDupSrgs.txt";
  string fns = "./testFiles/srg_xrefDup.txt";
  string aSrgName = "Filled Airports";
  string expMsg = "Syntax Error: Duplicate entries for '"+ aSrgName+
    "' surrogate in the file '" + fns+ "'" ;
  string tn = "testSE_xrefDupSrgEntries()";
  FILE* fp ; 
  if( (fp=fopen(fName,"r")) == NULL){
    cerr << "Fail to open the srgmerge input file '" << fName << "'" << endl;
    exit(1);
  }  
  GapfillInputFile input(fp);  
  
  try{
    input.readFile();
    if(input.GAPFILL){
      ReadSrgInfo info;
      info.readInfo(input);
    }
  }
  catch(SyntaxError e){
    result(expMsg,e.toString(),tn);
    //cout << "Error Message: " << e.toString()<< endl;
  }
  fclose(fp);

}
