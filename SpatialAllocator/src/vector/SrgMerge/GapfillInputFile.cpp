#include "GapfillInputFile.h"
#include "SyntaxError.h"
#include "FileErrors.h"

GapfillInputFile:: GapfillInputFile(FILE* fi){
   inputFile =fi;  
   GAPFILL = false;
   MERGE = false;
} 
 
int GapfillInputFile:: readFile(){
  string outputFTag = "OUTFILE=";
  string refFTag = "XREFFILE=";
  string cmdTag = "OUTSRG=";
  string inputFTag = "INFILE=";
  string gapfillTag = "GAPFILL=";
  char  buffer[MAXLINE];
  int index = -1;
  int lineCounter = 0;
  while(fgets(buffer,MAXLINE,inputFile)!=NULL){
    lineCounter ++;
    string aLine(buffer);
    aLine=trim(aLine);
    //cout << "aLine=" << aLine << " size=" << aLine.size()<< endl;
    if(aLine.size()!=0){
      if(aLine.at(0) == '#'){
        //DO NOTHING since it's a comment
      }
      else if(aLine.compare(0,outputFTag.size(),outputFTag)==0){
        if(aLine.size() > outputFTag.size()){
          outputF  = aLine.substr((outputFTag.size()),aLine.size());
          outputF = trim(outputF);
          //isFileExist(outputF,lineCounter);        
        }
        else{
          string msg = "Syntax Error: output file name incorrectly specified ";
          //msg.append("line no="); char* no ;sprintf("%d",no,lineCounter);
          //msg.append(no); msg.append("\n   ");/msg.append(aLine);
          throw SyntaxError(msg);
        }
      }    
      else if(aLine.compare(0,refFTag.size(),refFTag)==0){
        if(aLine.size() > refFTag.size()){
          refSrgF = aLine.substr((refFTag.size()),aLine.size());
          refSrgF = trim(refSrgF);
          isFileExist(refSrgF,lineCounter);
          //cout << "refF=" << refSrgF << endl;
        }
        else{
          throw SyntaxError("Syntax Error: reference file name incorrectly specified.") ;
          //cerr << "line no=" << lineCounter<< endl; cerr << "  " << aLine << endl;
        }
      }//else if(refFile)    
      else if(aLine.compare(0,cmdTag.size(),cmdTag)==0){      
        //cout << "GAPFILL="<< GAPFILL << " MERGE=" << MERGE << endl;
        if(aLine.size() > cmdTag.size()){
          if(GAPFILL && MERGE){
            throw SyntaxError("SyntaxError: In a single inputfile either Gapfilling or Merging is supported, not both");
          }
          if(aLine.find(gapfillTag) != string:: npos){
            //call the gapfill read
            GAPFILL= true;
            readGapfillCmdInput(aLine,cmdTag,lineCounter);
          }
          else{//process for function
            MERGE = true;
            mergeFunc.oFName= outputF;
            mergeFunc.readMergeFunction(aLine, refSrgF,lineCounter);
          }
        }
        else{
          throw SyntaxError("Syntax Error: Either function or Gapfill is not specified Surrogate file" );
          //cerr << "line no=" << lineCounter<< endl; cerr << "  " << aLine << endl;
        }
      }//else if(cmdLine)        
      else{
        throw SyntaxError("Syntax Error: Check whether tags are correctly specified") ;
        //cerr << "line no=" << lineCounter<< endl; cerr << "  " << aLine << endl;
      }
    }//if(aLine.size()!=0){
  }//while

  if(GAPFILL && MERGE){
    throw SyntaxError("SyntaxError: In a single inputfile either Gapfilling or Merging is supported, not both");
  }
  //check whether atleast a output file, reference file and one command of input are specified
  if(outputF.size() == 0){
    throw SyntaxError("The output file is not specified");
  }
  if(refSrgF.size() == 0){
    throw SyntaxError("The reference file is not specified");
  }
  if(GAPFILL){
    if(commands.size() == 0 ){
      throw SyntaxError("The surrogate information incorrectly specified");
    }
  }
  else{
    if(mergeFunc.funcVar.size() ==0){
      throw SyntaxError("The surrogate information incorrectly specified");
    }
  }
  return 0;
}//readFile
 
void GapfillInputFile:: isFileExist(string fName, int lineCounter){
  //cout << "fName= '" << fName << "'" << endl;
  const char* charFName = fName.c_str();
  FILE* fp;
  if((fp=fopen(charFName,"r"))== NULL){
    string msg = "File Not Found: The file '" + fName + "' does not exist";
    //cout << msg << endl;
    throw FileNotFoundException(msg);
    //cerr << "In the input file line no:" << lineCounter 
    // << "\nThe file '"<< fName << "' does not exist." << endl;
  }
  fclose(fp);
}
void GapfillInputFile::  readGapfillCmdInput(string &aLine,string cmdTag, int &lineCounter){
    cout << "Processing Input File line '" << aLine << "'" << endl;
  string gfTag = "GAPFILL=";
  //cout << "index="<< index << endl;
  //cout << "a new aLine:" << aLine << endl;
  vector<string> fileSrgName;
  stringtok(fileSrgName,aLine,";");
  if(fileSrgName.size()< 2){
    throw  SyntaxError("Synatax Errror: Surrogate file names incorrectly specified");
    //cerr << "line no=" << lineCounter<< endl;  // cerr << "  " << aLine << endl; );
  }

  Command aCommand;
  string aString = fileSrgName.at(0);
  aCommand.oSrgName = aString.substr(cmdTag.size(),aString.size()); 
  aCommand.oSrgName = trim(aCommand.oSrgName);
  for(int i=1; i<fileSrgName.size(); i++){
    string tempString = fileSrgName.at(i);
    //cout << "tempString=" << tempString << endl;
    vector<string> names;
    stringtok(names,tempString,"|");
    if(names.size()!= 2){
      throw SyntaxError( "Synatax Error: Surrogate file names incorrectly specified. Check the delimiter.");
      //cerr << "line no=" << lineCounter<< endl;  cerr << "  " << aLine << endl;
    }
    else{
      //cout << "names.at(0)=" << names.at(0)<< endl;
      string aName = names.at(0);
      aName = trim(aName);
      int index = aName.find(gfTag);
      if(index != string:: npos){
        aName = aName.substr(gfTag.size());
        aName = trim(aName);
      }
      isFileExist(aName,lineCounter);      
      aCommand.inputF.push_back(aName);
      //cout << "fName = " << aName << endl;
      //check for duplicate file names
      string srgName = trim(names.at(1));
      for(int j=0; j< aCommand.iSrgNames.size(); j++){
        string aSrgName = aCommand.iSrgNames.at(j);
        if(aSrgName.compare(srgName) == 0){
          string msg = "Syntax Error: Duplicate entries for '" + aSrgName + 
            "' in the GAPFILL statement, cannot GAPFILL.";
          throw SyntaxError(msg);
        }
      }//for(j)
      aCommand.iSrgNames.push_back(srgName);
      
      //cout << "srgName=" << names.at(1)<< endl;
    }          
  }//for(i,fileSrgName)
  
  
  commands.push_back(aCommand);
}

GapfillInputFile:: ~GapfillInputFile(){

}


string GapfillInputFile:: trim(string& s) {
  if(s.length() == 0)
    return s;
  int b = s.find_first_not_of(" \t\n\r");
  int e = s.find_last_not_of(" \t\n\r");
  if(b == -1){ // No non-spaces
    return "";
  }
  return string(s, b, e - b + 1);
}


/**
int main(int numargs, char* args []){
  if(numargs != 2){
    cerr << "usage: a.out gapfill_input.txt" << endl;
    exit(1);
  }
  cout << "numargs=" << numargs << endl;
  cout << "args[0]=" << args[0] << endl;
  cout << "args[1]=" << args[1] << endl;
  FILE* fp = fopen(args[1],"r");
  if(fp == NULL){
    cerr << "error failed to open the file '" << args[1] << "'" << endl;
    exit(1);
  }
  GapfillInputFile gapfillIF(fp);
  gapfillIF.readFile();
  cout << "output file " << gapfillIF.outputF << endl;
  cout << "ref file " << gapfillIF.refSrgF << endl;
  vector<Command> com = gapfillIF.commands;
  for(int i=0; i< com.size(); i++){
    Command aCom = com.at(i);
    cout << "oSrgName=" << aCom.oSrgName<< flush;
    vector<string>inputFs = aCom.inputF;
    vector<string>names = aCom.iSrgNames;
    if(inputFs.size() != names.size()){
      cerr << "This is a bug unequal size " << endl;
      exit(1);
    }
    
    for(int i=0; i< inputFs.size(); i++){
      cout << "; 'input file="<< inputFs.at(i)<<flush;
      cout <<":srg name="<< names.at(i) << "'; "<< flush;
    }
    cout << endl;
  }//for(i)
  return 0;
}
*/
