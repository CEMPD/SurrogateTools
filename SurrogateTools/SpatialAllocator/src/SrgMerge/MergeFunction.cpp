#include "MergeFunction.h"

using namespace std;

void MergeFunction:: readMergeFunction(string &aLine, string refFName,int lineCounter){
  string infix;
  Function aFunc;
  char oBrace = '{', cBrace='}';
  char DIV = '/';
  char SUB = '-';
  string cmdTag = "OUTSRG=";
  aLine = aLine.substr(cmdTag.size());
  int index = aLine.find_first_of(';');
  cout << "Processing Input File line '" << aLine << "'" << endl;
  string oSrgName = aLine.substr(0,index);
  //cout << "refFN=" << refFName << endl;
  findRefID(refFName,oSrgName,aFunc);
  int start=-1,end=-1,s=0;
  int countVar = 0;
  char varName[20];
  bool first= false;
  int npos = string:: npos;
  
  if(index != npos && ++index < aLine.size()){
    string fString = aLine.substr(index);  
    start = fString.find_first_of(oBrace,s);
    end = fString.find_first_of(cBrace,s);
    if(start== npos || end == npos || start > end){
      throw SyntaxError("SyntaxError: Function does not have an opening or/and closing braces");
    }
    //cout << "fString" << endl;
    while(s<fString.size()){
      //cout << "s=" << s << " start="<< start<< " end=" << end << endl;
      start = fString.find_first_of(oBrace,s);
      end = fString.find_first_of(cBrace,s);
      if(start == npos &&  end == npos){
        infix.append(fString.substr(s,fString.size()-s));
        break;
      }
      else if((start > end)||(start == npos &&  end != npos)||
              (start != npos &&  end == npos)){
        throw SyntaxError("SyntaxError: Function does not have either a opening  closing braces");
      }
      else{
        string tempS = fString.substr(start+1,end-start-1);
        sprintf(varName,"X%d",++countVar);
        aFunc.varName.push_back(varName);
        addFunctionInfo(refFName,tempS,aFunc);
        infix.append(fString.substr(s,(start-s)));
        //cout << fString.substr(s,(start-s)) << endl;
        //check for Division and Subtraction operators
        string tempS2 = fString.substr(s,start- s);
        //cout << "s=" << s << " start= " << start << " end=" << end << endl;
        //cout << "tempS2=" << tempS2 <<endl;
        if(tempS2.find_first_of(DIV) != npos || tempS2.find_first_of(SUB) != npos){
          throw SyntaxError("SyntaxError: Subtraction and Division are not allowed in the function specfiction");
        }
        infix.append(varName);
        s= end+1;
      }
    }//while 
  }
  else{
    //throw an error
    throw SyntaxError("SyntaxError: Expecting a ';' between output surrogate and function specification");;
  } 
  infixFunc.push_back(infix);
  funcVar.push_back(aFunc);
}


void MergeFunction:: findRefID(string refFName, string oSrgName, Function &func ){
  int refID = -1; 
  findRefSrgID(refFName,oSrgName,refID);
  //cout << "refFName='" << refFName << "', refID=" << refID <<endl; 
  if(refID == -1){
    string msg = "Could not find output surrogate name '"+ 
      oSrgName + "' in the xref file '" + refFName + "'.";
    throw ChkErrors(msg);
  }
  func.outSrgID = refID;
  //cout << "refID=" << refID << endl;
  func.outSrgName= oSrgName;
}

void  MergeFunction:: addFunctionInfo(string refFName, string &info, Function &func){
  //cout << "info=" << info << endl;
  int npos =  string:: npos;
  int index = info.find_first_of('|');
  string fileName;
  string srgName;
  int srgID = -1;
  if(index != npos){
    fileName = info.substr(0,index);
    //trim, check whether file name exist
    srgName = info.substr(index+1,info.size()-index -1);
    findRefSrgID(fileName,srgName,srgID);
    if(srgID == -1){
      string msg = "WARNING: Could not find #SRGDESC= for '"+ 
        srgName + "' in the surrogate file '" + fileName + "'.";
      cerr << msg << endl;
      findRefSrgID(refFName,srgName,srgID);
      if(srgID == -1){
        msg = "Could not find input surrogate name '"+ 
        srgName + "' in both the surrogate file '" + fileName +
          "' and xref file.";
         throw ChkErrors(msg);
      }
    } 
    func.srgName.push_back(srgName);
    func.srgID.push_back(srgID);
    func.fileName.push_back(fileName);
    //find the id for the srgName in the file for the id
    //cout << "fileName=" << fileName << " srgName=" << srgName << " SrgID=" << srgID <<  endl;
  } 
  else {
    throw SyntaxError("Syntax Error: check the function specification. Expecting a '|' between file name and surrogate name");
  }
}

void MergeFunction:: findRefSrgID(string &fName, string &srgName,int &srgID){  
  char buffer[MAXLINE];
  string desTag = "#SRGDESC=";
  FILE* fp;
  int id = -1;
  int lineCounter =0;
  string tempName;
  const char* charFName = fName.c_str();
  bool allreadyFound = false;

  if((fp = fopen(charFName,"r")) == NULL){
    cerr << "The file '"<< fName << "' fail to open."<< endl;
    exit(1);
  }
  while(fgets(buffer,MAXLINE,fp)!=NULL){
    lineCounter ++;
    string aLine(buffer);
    aLine = trim(aLine);
    //cout << "aLIne=" << aLine << endl;
    if(aLine.compare(0,desTag.size(),desTag)==0){
      string substring = aLine.substr(desTag.size());
      const char* toChar = substring.c_str(); 
      sscanf(toChar,"%d",&id);
      //cout << "id=" << id << endl;
      int index = substring.find_first_of(',');
      if(index < 0 ){
        string msg =  "Syntax Error: The surrogate file, srg info is not in the expected format: " + lineCounter;
        msg.append("\n");
        msg.append("   ");
        msg.append(aLine);
        throw SyntaxError(msg);
        //msg.concat(cerr << "   " << aLine << endl;
        //exit(1);
      }
      //cout << "srgName=" << srgName << endl;

      tempName=substring.substr(index+1);
      tempName = trim(tempName);
      //cout << "srgName='" << srgName<< "', tempName='" << tempName << "'"<<endl;
      if(srgName.compare(tempName)==0){
        if(allreadyFound){
          string msg =  "Syntax Error: Duplicate entries for '"+ srgName+
            "' surrogate in the file '" + fName + "'" ;
          throw SyntaxError(msg);
        }
        srgID = id;
        //cout << "inside findRefSrgID, id=" << id << endl;
        allreadyFound = true;
      }
    }//if(desTag)
  }//while
  fclose(fp);
}

string MergeFunction:: trim(string &s) {
  if(s.length() == 0)
    return s;
  int b = s.find_first_not_of(" \t\n\r");
  int e = s.find_last_not_of(" \t\n\r");
  if(b == -1) // No non-spaces
    return "";
  return string(s, b, e - b + 1);
}

