#include "ReadSrgInfo.h"

void ReadSrgInfo:: readInfo(GapfillInputFile data){
  string desTag ="#SRGDESC=";
  string refSrgFName = data.refSrgF;
  int refID = -1;
  vector<Command>commands =  data.commands;
  //cout << "size= " << commands.size() << endl;
  
  for(int i=0; i< commands.size(); i++){
    Command command = commands.at(i);
    string oSrgName= command.oSrgName;
    //cout << "output srg name=" << oSrgName << endl;
    findRefSrgID(refSrgFName,oSrgName,refID);
    //cout << "refID=" << refID << endl;
    if(refID == -1){
      string msg = "READ SRG INFO Could not find output surrogate name '"+ 
        oSrgName + "' in the xref file '" + refSrgFName + "'.";
      throw ChkErrors(msg);
    } 
    else{
      addValue(refID,oSrgName);
    }
    refID = -1;
    //check whether the iSrgName exist in the header info of the relevant file, no print a warning and search in xref file   
    int index = command.inputF.size()-1;
    string iSrgFName = command.inputF.at(index);
    readGridInfo(iSrgFName);
    readInputSrgFile(refSrgFName, command.inputF, command.iSrgNames);
  }//for(i)  
}//readInfo

void ReadSrgInfo:: readGridInfo(string &fName){
  const char* charFName = fName.c_str();
  FILE *fp;
  if((fp = fopen(charFName,"r")) == NULL){
    cerr << "The file '"<< fName << "' failed to open."<< endl;
    exit(1);
  }
  char buffer[MAXLINE];

  while(fgets(buffer,MAXLINE,fp)!= NULL){
    string tempString(buffer);
    tempString = trim(tempString);
    if(tempString.find("#GRID\t")!= string:: npos || tempString.find("#POLYGON\t")!= string:: npos){
      gridInfo = tempString;
      fclose(fp);
      //cout << "ReadSrgInfo.cpp: grid info=" << gridInfo << endl;
      return;
    }
    else{
      cerr << "The file '"<< fName << "' does not have right header (#GRID or #POLYGON)."<< endl;
      exit(1);
    }
  }
  fclose(fp);

}


void ReadSrgInfo:: findRefSrgID(string &fName, string &oSrgName,int &refID){
  char buffer[MAXLINE];
  string desTag = "#SRGDESC=";
  FILE* fp;
  int id = -1;
  int lineCounter =0;
  string tempName;
  bool allreadyFound = false;
  const char* charFName = fName.c_str();
  //ut << "oSrgName=" << oSrgName << endl;
  if((fp = fopen(charFName,"r")) == NULL){
    cerr << "The file '"<< fName << "' failed to open."<< endl;
    exit(1);
  }
  while(fgets(buffer,MAXLINE,fp)!=NULL){
    lineCounter ++;
    string aLine(buffer);
    aLine = trim(aLine);
    ///cout << "aLIne=" << aLine << endl;
    if(aLine.compare(0,desTag.size(),desTag)==0){
      string substring = aLine.substr(desTag.size());
      const char* toChar = substring.c_str(); 
      sscanf(toChar,"%d",&id);
      //cout << "id=" << id << endl;
      int index = substring.find_first_of(',');
      if(index < 0 ){
        cerr<< "Syntax Error: The reference file in not "<<
          "in the expected format: " << lineCounter << endl;
        cerr << "   " << aLine << endl;
        exit(1);
      }
      int indexq1 = substring.find_first_of('"');
      if (indexq1 < 0) {
         tempName=substring.substr(index+1);
      }
      else {
        int indexq2 = substring.find_last_of('"');
        tempName=substring.substr(indexq1+1,indexq2-indexq1-1);
      }
        
      tempName = trim(tempName);
      //cout << "  tempName=" << tempName << endl;    
      //ut << "  oSrgName.compare(tempName)=" << oSrgName.compare(tempName)<< endl;
      if(oSrgName.compare(tempName)==0){
        if(allreadyFound){
          string msg =  "Syntax Error: Duplicate entries for '"+ oSrgName+
            "' surrogate in the file '" + fName + "'" ;
          throw SyntaxError(msg);
        }
        refID = id;
        //cout << "inside findRefSrgID, id=" << id << endl;
        allreadyFound = true;
      }
    }//if(desTag)
  }//while
  fclose(fp);

}

void ReadSrgInfo:: readInputSrgFile(string refFName,vector<string>inputF, vector<string>iSrgNames){ 
  char buffer[MAXLINE];
  string desTag = "#SRGDESC=";
  FILE* fp;
  int id = -1;
  string value;
  int lineCounter = 0;
  string iFileName;
  string iSrgName;
  bool foundInSrgFile = false;
  for(int i=0; i<inputF.size(); i++){
    iFileName = inputF.at(i);
    iSrgName = iSrgNames.at(i);
    const char* charFName = iFileName.c_str();  
    if((fp =fopen(charFName,"r"))== NULL){
      cerr << "The file '"<< iFileName << "' failed to open."<< endl;
      exit(1);
    }
    while(fgets(buffer,MAXLINE,fp)!= NULL){
      lineCounter++;
      string aLine(buffer);
      aLine = trim(aLine);
      if(aLine.size() !=0){
        if(aLine.at(0) != '#'){
          continue;
        }
        //cout<< "aLine=" << aLine << endl;
        if(aLine.compare(0,desTag.size(),desTag)==0){
          string substring = aLine.substr(desTag.size());
          const char* toChar = substring.c_str(); 
          sscanf(toChar,"%d",&id);
          int index = substring.find_first_of(',');
          if(index < 0 ){
            cerr<< "Syntax Error: The reference file in not "<<
              "in the expected format: " << lineCounter << endl;
            cerr << "   " << aLine << endl;
            exit(1);
          }
          value=substring.substr(index+1);
          value = trim(value);
          if(value.compare(iSrgName)==0){
            foundInSrgFile = true;
          }
          //cout << "id=" << id << ", value=" << value << endl;
          addValue(id,value);
        }//if(compare())    
      }
    }//while
    if(!foundInSrgFile){
      string msg = "WARNING: Could not find #SRGDESC= for '"+ 
        iSrgName + "' in the surrogate file '" + iFileName +"'.";
      cerr << msg << endl;
      int refID = -1;
      findRefSrgID(refFName,iSrgName,refID);
      if(refID != -1 ){
        addValue(refID,iSrgName);
        //cout << "refId=" << refID << ", value=" << iSrgName << endl;
      }
      else{
        msg = "Could not find input surrogate name '"+ 
          iSrgName + "' in both the surrogate file '" + iFileName +
          "' and xref file.";
        throw ChkErrors(msg);
      }
    }
    
    foundInSrgFile = false;
    lineCounter =0;
    id -1;
  }//for(i)
}

bool ReadSrgInfo:: addValue(int &key, string &value){
  bool flag = isExist(key);
  if(!flag){
    //cout << "addValue: key="<< key << " value= " << value << endl;
    srgID.push_back(key);
    srgName.push_back(value);
    return true;
  }
  return false;
}//addValue

bool ReadSrgInfo:: isExist(int &key){
  int tempKey;
  for(int i=0; i< srgID.size();i++){
    tempKey = srgID.at(i);
    if(tempKey == key){
      return true;
    }
  }      
  return false;
}//isExist()

int ReadSrgInfo:: getID(string value){
  //cout << "value=" << value << endl;
  //cout << "srgName vector size=" << srgName.size() << endl;
  int id = -1;
  string aName;
  for(int i=0; i <srgName.size(); i++){
    aName=srgName.at(i);
    //cout << "srgName: value="<< aName << " key= " << srgID.at(i) << endl;
    if(aName.compare(value)==0){
      //cout << "Inside getID: arg value=" << value << " match id=" << srgID.at(i)<< endl;
      return id = srgID.at(i);
    }
  }
  return id;
}

string ReadSrgInfo:: trim(string& s) {
  if(s.length() == 0)
    return s;
  int b = s.find_first_not_of(" \t\n\r");
  int e = s.find_last_not_of(" \t\n\r");
  if(b == -1){ // No non-spaces
    return "";
  }
  return string(s, b, e - b + 1);
}
