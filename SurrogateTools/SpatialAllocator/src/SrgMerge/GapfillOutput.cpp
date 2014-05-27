#include "GapfillOutput.h"
using namespace std;

bool  comp_surr(const Surrogate  &s1, const Surrogate &s2);

int GapfillOutput:: writeOutput(ReadSrgInfo &info, char* pInputFName, GapfillInputFile &input){
  const char* outputFName = input.outputF.c_str();
  FILE* fp;
  if((fp=fopen(outputFName,"w"))== NULL){
    cerr << "Failed to create the file '" << outputFName  << "'" << endl;
    exit(1);
  }
  //writing the header information
  writeHeader(info,pInputFName,fp);
  //writing the data information
  writeData(input,info,fp);
  fclose(fp);
  return 0;  
}//class GapfillOutput

int GapfillOutput:: writeHeader(ReadSrgInfo &info, char* pInputFName, FILE* fp){
  //print out the input file of the program as an header of the output
  FILE* pInputFile;
  
  char buffer[MAXLINE];
  if((pInputFile=fopen(pInputFName,"r"))== NULL){
    cerr << "Failed to open the file '" << pInputFName  << "'" <<  endl;
    exit(1);
  }
  //cout << "gridInfo=" << info.gridInfo << endl;
  //print the grid/polygon information
  fprintf(fp,"%s\n",info.gridInfo.c_str());

  fprintf(fp,"%s\n","#############COPY OF THE INPUT FILE############################");
  while(fgets(buffer,MAXLINE,pInputFile)!=NULL){
    fprintf(fp,"#%s\n",buffer);
  }
  fprintf(fp,"%s\n","#############END OF THE INPUT FILE############################");
 
  char* desTag = "#SRGDESC=";
  vector<int> srgID = info.srgID;
  vector<string> srgName = info.srgName;
  
  for(int i=0; i< srgID.size(); i++){
    const char* charSrgName = srgName.at(i).c_str();
    fprintf(fp,"%s%d,%s\n",desTag,srgID.at(i),charSrgName);   
  }
  //cout << "Finish writing header" << endl;
  return 0;
}


/** =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */
void GapfillOutput:: writeData(GapfillInputFile &input,ReadSrgInfo &info, FILE* fp){  
  vector<Command>commands = input.commands;
  vector<string>inputFiles;
  vector<string>srgNames;
  vector<Surrogates> srgsV;
  vector<string> gridInfo; //to store the first line of the file
  int outSrgID;
  const char* outSrgName;
  Polygons  polys;

  for(int i=0; i< commands.size(); i++){
    Command aCommand = commands.at(i);
    cout << "Processing output for '" << aCommand.oSrgName << "'" << endl;
    outSrgName = aCommand.oSrgName.c_str();
    outSrgID = info.getID(aCommand.oSrgName);
    
    cout << "outSrgID="<< outSrgID << endl;
    readSrgFiles(aCommand,srgsV,info,gridInfo,polys);
    cout << "Finished reading srg files srgsV.size()=" << srgsV.size() << endl;
    checkGridInfo(gridInfo);
    if(srgsV.size() == 1){ 
       output(info,outSrgID,srgsV.at(0).srgV,fp,polys);
    }
    else{
      output(info,outSrgID,srgsV,fp,polys);
    }
    freeDebugInfo(srgsV);
    //empty the vectors
    srgsV.clear();
  }//for(i<commands.size());
}//writeData

void GapfillOutput:: freeDebugInfo(vector<Surrogates> srgsV){
  int size = srgsV.size();
  for(int i=0; i< size; i++){
    vector<Surrogate> srgV = srgsV.at(i).srgV;
    int length = srgV.size();
    for(int j=0; j<length; j++){
      if(srgV.at(j).debugInfo !=NULL){
        free(srgV.at(j).debugInfo);
      }
    }
  }
}
//======================================================
int GapfillOutput:: readSrgFiles(Command aCommand,vector<Surrogates> &srgsV, ReadSrgInfo info,
                                 vector<string>& gridInfo, Polygons &polys){
  vector<string>fNames = aCommand.inputF;
  vector<string>srgNames = aCommand.iSrgNames; 
  int srgID = 0;
  string fName;
  const char* charFName;

  //fill first element in polygon ID vectors
  polys.polyIDIndexes.push_back(0);
  polys.polyIDs.push_back("FILLFIRSTONE");

  for(int i=0; i< fNames.size(); i++){
    Surrogates ss;
    vector<Surrogate> s;
    fName = fNames.at(i);
    charFName = fName.c_str();
    srgID = info.getID(srgNames.at(i));
    if(srgID == -1){
      string msg = "Could not find output surrogate name '"+ 
        srgNames.at(i) + "' in the surrogate file '" + fName + "'.";
      throw ChkErrors(msg);
    }
    cout << "srgID=" << srgID << charFName << endl;
    readSrgtFile(charFName,srgID,s,gridInfo,polys);
    cout << "i=" << i << ", length=" <<s.size()<< endl;
    ss.srgV = s;
    srgsV.push_back(ss);
  }//for(i)
  //cout <<"LENGTH=" <<srgsV.at(0).srgV.size()<< endl;
  //cout <<"LENGTH=" <<srgsV.at(1).srgV.size()<< endl;
  return 0;
}


//======================================================
int GapfillOutput:: readSrgtFile(const char* fname, int srgID, vector<Surrogate> &s,
                                 vector<string>& gridInfo, Polygons &polys )
{
  FILE* fp;
  int n = 0; // number of surrogates  
  int surrID=0, lat=0; //temp storage for a relevant srg line
  long countyID=0,lon; //temp storage for a relevant srg line
  string polyID;
  char polyID_c[20];
  long i = 1;       //index for polyIDs array
  double value=0.0;                     // -do-
  char *debugInfo= NULL;                // -do-
  bool first = false;
  bool foundSrgID = false;
  char buffer[MAXLINE];
  char tempBuffer[MAXLINE];
  string headerString;            //store header line
  int   pushHeader_index = 1;    //for finding the right header to push in case multiple surrogates in one file

  if ((fp = fopen(fname,"r")) == NULL)
  {
    cerr << "The file '" << fname << "' failed to open." <<  endl;
    return -1;
  }
  
  while (fgets(tempBuffer, MAXLINE, fp) != NULL){
    string tempString(tempBuffer);
    tempString = trim(tempString);
    if(tempString.find("#GRID\t")!= string::npos || tempString.find("#GRID ")!= string::npos ||
       tempString.find("#POLYGON\t") != string::npos || tempString.find("#POLYGON ") != string::npos){    
      headerString = tempString;  //keep track the header for concatenated srg files
      //cout << "headerString=" << headerString << endl;
    }

    //const char* c = tempString.c_str();
    //char* buffer;
    //tempString.copy(buffer,tempString.size(),0);
    if(tempString.size()!=0){
      strcpy(buffer,tempString.c_str());
      //char* cpBuffer = buffer;
      //cout << "buffer=" << buffer << endl;
   
      if(buffer[0]!= '#'){
        if(headerString.find("#GRID\t")!= string::npos || headerString.find("#GRID ")!= string::npos) {
          sscanf(buffer,"%u%lu%u%lu%lf", &surrID, &countyID, &lat, &lon, &value);
          //cout<< "srgid=" << surrID << ", countyID="<< countyID << ", lat=" << lat<< ", lon=" << lon << ",value=" << value << endl;
        }
        else {
          sscanf(buffer,"%u%lu%s%lf", &surrID, &countyID, polyID_c, &value);
          polyID = polyID_c;
          //cout << "polyID=" << polyID << endl;
        }
        debugInfo = myindex(buffer, '!');    
        //cout << debugInfo << endl;
        if(surrID==srgID)  {
          if (pushHeader_index == 1) {
            gridInfo.push_back(headerString);
            pushHeader_index = 0;  //only store the header before surrogate data once 
          }
          foundSrgID = true; 
          Surrogate temp;
          temp.surrID= surrID;
          temp.countyID = countyID;

          if (headerString.find("#GRID\t")!= string::npos || headerString.find("#GRID ")!= string::npos) {
            temp.lat = lat;
            temp.lon = lon;
          }
          else {
            temp.lat = 1;   //transform polygon ID into 2-D format like lat and lon^M
            int alreadyIn = 0;
            for (i=1;i<polys.polyIDIndexes.size();i++) {
               if (polys.polyIDs.at(i).compare(polyID)==0) { //polyID exist in the vector
                 temp.lon = i;
                 alreadyIn = 1;
                 break;
               }  
            } 
            if (alreadyIn == 0) {
              polys.polyIDIndexes.push_back(i);
              polys.polyIDs.push_back(polyID); 
              temp.lon = i;
            }
          }
          temp.value = value;
          temp.debugInfo = NULL;
          if (debugInfo != NULL){
              temp.debugInfo = (char *)malloc(sizeof(char)*(strlen(debugInfo)+1));
              strcpy(temp.debugInfo, debugInfo);
              /*temp.debugInfo = strdup(debugInfo);*/
            }
          else
          {
             temp.debugInfo = strdup("");
          }

          //cout << "polyID=" << polyID << endl;
          //cout<< "srgid=" << surrID << ", countyID="<< countyID << ", polyID=" << polyID<< ", Lon=" << temp.lon << ",value=" << value << endl;
          //cout<< "srgid=" << surrID << ", countyID="<< countyID << ", lat=" << temp.lat<< ", lon=" << temp.lon << ",value=" << value << endl;
          s.push_back(temp);
          n++;
        }//f(surrID==srgID)      
      } //if starts with !#
    }//if(tempString.size()!=0){
  }//while

 
  fclose(fp);
  if(!foundSrgID){
    char buf[100];
    sprintf(buf,"%d",srgID);
    string bufS(buf);
    string msg = "Could not find surrogate id for '" + bufS +
      "' in the file '" + fname + "'";   
    throw ChkErrors(msg);
  }
  sort(s.begin(),s.end(),comp_surr);
  //sort(countyIDs.begin(),countyIDs.end());
  return n;
}//readSrgtFile()

void GapfillOutput:: checkGridInfo(vector<string> &gridInfo){
  // cout << "grid Info size=" <<gridInfo.size() << endl;
  if(gridInfo.size() > 0){
    string s = gridInfo.at(0);
    //cout << "s=" << s << endl;
    for(int i=1; i < gridInfo.size(); i++){
      //cout << "grid at(i)=" << gridInfo.at(i) << endl;
      if(s.compare(gridInfo.at(i)) != 0){
        throw ChkErrors("The surrogate files specified do not belong to the same grid");
      }
    }
  }
}

/* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */

int GapfillOutput:: output(ReadSrgInfo &info, int outSrgID, vector<Surrogate> primSrg, FILE  *fp, Polygons &polys){
  // cout << "beginning of the output "<< endl;
  Surrogate pTemp;
  int i;
  vector<string> polyIDs;
  vector<long>polyIDIndexes;  
  string polyID;

  polyIDs = polys.polyIDs;
  polyIDIndexes = polys.polyIDIndexes;


  cout << "primdSrg size = " << primSrg.size() << endl;
  for(i=0; i< primSrg.size(); i++){
      pTemp = (Surrogate)primSrg.at(i);              
      if(info.gridInfo.find("#GRID\t")!= string::npos || info.gridInfo.find("#GRID ")!= string::npos) { 
        fprintf(fp,"%u\t%lu\t%u\t%lu\t%.8lf\t%s\n",outSrgID,pTemp.countyID,
              pTemp.lat, pTemp.lon,pTemp.value, pTemp.debugInfo);
      } //end of grid output
      else {  //"#POLYGON " header 
        polyID = polyIDs.at(polyIDIndexes.at(pTemp.lon)); 
        fprintf(fp,"%u\t%lu\t%s\t%.8lf\t%s\n",outSrgID,pTemp.countyID,
              polyID.c_str(),pTemp.value,pTemp.debugInfo);
      } //end of poly output

      //printf("p: %u, %lu,  %u, %lu, %lf, %s\n", outSrgID,pTemp.countyID, 
      //     pTemp.lat, pTemp.lon,pTemp.value, pTemp.debugInfo);
  }//for(i)
  return 0;
}//output // one surrogates

 void GapfillOutput:: output(ReadSrgInfo &info, int outSrgID, vector<Surrogates> srgsV, FILE *fp, Polygons &polys){

   int size = srgsV.size();
   //cout <<"size=" << size << endl;
   vector<Surrogate> outerV = srgsV.at(size-1).srgV; 
   //cout << "outerV.size()="<< outerV.size()<< endl;
   vector<Surrogate> innerV = srgsV.at(0).srgV;
   long nextOCID = nextCountyID(-1,outerV);
   //cout << "nextOCID=" << nextOCID << endl;
   long *nextICID = new long [(size-1)* sizeof(long)];
   //initialize;
   for(int j=0; j<size-1; j++){
     nextICID[j] = nextCountyID(-1,srgsV.at(j).srgV);
     //cout << "nextICID[" << j << "]=" << nextICID[j]<< endl;
   }
   bool innerPrint = false;
   
   for(long i=0; i< outerV.size(); i++){
     //cout << "outerV[" << i << "].countyID=" << outerV.at(i).countyID << endl;
     nextOCID = outerV.at(i).countyID;
     for(int j=0; j < size-1; j++){
       if(nextOCID == nextICID[j] && nextICID[j] != -1){
         innerV = srgsV.at(j).srgV;
        // cout << "innerV.size()=" << innerV.size() << endl;
         long cid = nextICID[j];
         if(j==0){
            //cout << "First print done" << endl;
           print(cid,outSrgID,innerV,fp,true,polys,info);//print(innerV the whole CID
         }
         else{
           //cout << "Second print done" << endl;
           print(cid,outSrgID,innerV,fp,polys,info);
         }
         i = endCIDIndex(nextOCID,outerV);//move the i of  outerV to end nextICID // remember
         //cout << "endCIDIndex i = " << i << endl;
         //the assumption that outer is a superset of all other surrogates 
         //update the nextICID for all if =nextOCID
         //for(int k=0; k<size -1; k++){
         //cout << "b: nextICID[" << k << "]=" <<  nextICID[k] << endl;
         //}
         
         update(nextOCID,nextICID,srgsV);
         //cout << "after" << endl;
         //for(int k=0; k<size -1; k++){
         //  cout << "a: nextICID[" << k << "]=" <<  nextICID[k] << endl;
         //}
         //exit(1);
         
         innerPrint = true;
         break;
       }
       else{
         if(nextOCID > nextICID[0] && nextICID[0] != -1)
         {     
           cout << "       cid = " << nextOCID << "  nextICID = "<< nextICID[0] << endl; 
           throw ChkErrors("Original surrogate contains more counties than those in the lowest-level gapfilling surrogate");
         } 
       }       
     }//for(j)
     if(!innerPrint){
       print(nextOCID,outSrgID,outerV,fp,polys,info); //print the whole thing for the nextOCID
       i = endCIDIndex(nextOCID,outerV);//and then updates the i;
     }
     else{
       innerPrint = false;
     }
   }//for(i)  
   delete nextICID;
 }//output()

/**
 * reeturn the index to the end point of the county ID
 * @pre srg should be sorted in the asending order by county id
 * from < srg.size()
 * srg.at(from).countyID == countyID
 */
long GapfillOutput:: nextCountyID(long currCID,vector<Surrogate>&srg){

  Surrogate tempSrg;
  for(int i=0; i< srg.size(); i++){
    tempSrg = srg.at(i);
    if(tempSrg.countyID > currCID){
        return tempSrg.countyID;
    }
  }
  return -1;
}

 void GapfillOutput:: update(long cid, long nextICID [], vector<Surrogates> srgsV){
   long id = 0;
   for(int i=0; i<srgsV.size()-1; i++){//excluding the first one
     if(cid == nextICID[i]){
       id = nextCountyID(cid,srgsV.at(i).srgV);
       //cout << "ocid=" << id << endl;
       nextICID[i] = id ;
     }
     else if(cid > nextICID[i] && nextICID[i]!= -1){ 
       cout << "ERROR cid=" << cid << " i="<< i << " nextICID="<< nextICID[i] << endl; 
       throw ChkErrors("This program based on the assumption that lower level  surrogate specified on the input file has all the counties of interest");
     }
   }
   
 }

 /** @pre cid is exist in srgV and srgV is sorted
  */
long GapfillOutput:: endCIDIndex(long cid,vector<Surrogate> srgV){ 
   Surrogate tmpSrg;
   for(int i=0; i< srgV.size(); i++){
     tmpSrg = srgV.at(i);
     if(tmpSrg.countyID  > cid){
       return i-1;;
     }
   }
   //no more new set of counties left in the surrogates set
   return srgV.size(); 
 }

/*
 void GapfillOutput:: print(long cid,vector<Surrogate>srg,FILE *fp,Polygons &polys,ReadSrgInfo &info){
   //cout << "cid= " << cid << endl;
   Surrogate tmpSrg;
   vector(string> polyIDs;
   vector<long>polyIDIndexes;  
   string polyID;

   polyIDs = polys.polyIDs;
   polyIDIndexes = polys.polyIDIndexes;


   for(int i=0; i< srg.size(); i++){
     tmpSrg = srg.at(i);
     if(tmpSrg.countyID == cid){
       if(info.gridInfo.find("#GRID\t")!= string::npos || info.gridInfo.find("#GRID ")!= string::npos) { 
          fprintf(fp,"%u\t%lu\t%u\t%lu\t%.8lf\t%s\n",tmpSrg.surrID,tmpSrg.countyID,
              tmpSrg.lat, tmpSrg.lon,tmpSrg.value, tmpSrg.debugInfo);
       }
       else {  //"#POLYGON " header 
         polyID = polyIDs.at(polyIDIndexes.at(tmpSrg.lon)); 
         fprintf(fp,"%u\t%lu\t%s\t%.8lf\t%s\n",tmpSrg.surrID,tmpSrg.countyID,
              polyID.c_str(),tmpSrg.value, tmpSrg.debugInfo);
       }

       //printf("o cid=%lu\n", tmpSrg.countyID);
     }
   }
 }
*/

 void GapfillOutput:: print(long cid,int outSrgID,vector<Surrogate>srg,FILE *fp,Polygons &polys,ReadSrgInfo &info){
   //cout << "cid= " << cid << endl;
   Surrogate tmpSrg;
   vector<string> polyIDs;
   vector<long>polyIDIndexes;  
   string polyID;

   polyIDs = polys.polyIDs;
   polyIDIndexes = polys.polyIDIndexes;


   for(int i=0; i< srg.size(); i++){
     tmpSrg = srg.at(i);
     if(tmpSrg.countyID == cid){
       if(info.gridInfo.find("#GRID\t")!= string::npos || info.gridInfo.find("#GRID ")!= string::npos) { 
          fprintf(fp,"%u\t%lu\t%u\t%lu\t%.8lf\t%s   GF: %u\n", outSrgID,tmpSrg.countyID,
              tmpSrg.lat,tmpSrg.lon,tmpSrg.value,tmpSrg.debugInfo, tmpSrg.surrID );
       }
       else {  //"#POLYGON " header 
         polyID = polyIDs.at(polyIDIndexes.at(tmpSrg.lon)); 
         fprintf(fp,"%u\t%lu\t%s\t%.8lf\t%s   GF: %u\n", outSrgID,tmpSrg.countyID,
              polyID.c_str(),tmpSrg.value,tmpSrg.debugInfo, tmpSrg.surrID );
       } //printf("i cid=%lu\n", tmpSrg.countyID);
     }
   }
 }

 void GapfillOutput:: print(long cid,int outSrgID,vector<Surrogate>srg,FILE *fp,bool prim, Polygons &polys,ReadSrgInfo &info){
   Surrogate tmpSrg;
   vector<string> polyIDs;
   vector<long>polyIDIndexes;  
   string polyID;

  //cout << "cid= " << cid << endl;
  polyIDs = polys.polyIDs;
  polyIDIndexes = polys.polyIDIndexes;
  //cout << "polyIDs size= " << polyIDs.size() << endl;
  //cout << "polyIDIndexes size= " << polyIDIndexes.size() << endl;

  
   for(int i=0; i< srg.size(); i++){
     tmpSrg = srg.at(i);
     if(tmpSrg.countyID == cid){
        //cout << "GRID = " << info.gridInfo << endl;
       if(info.gridInfo.find("#GRID\t")!= string::npos || info.gridInfo.find("#GRID ")!= string::npos) {
         //cout << "GRID cid= " << tmpSrg.countyID << endl;
         fprintf(fp,"%u\t%lu\t%u\t%lu\t%.8lf\t%s\n", outSrgID,tmpSrg.countyID,
               tmpSrg.lat,tmpSrg.lon,tmpSrg.value,tmpSrg.debugInfo );
       }
       else {  //"#POLYGON " header
         //cout << "POLYGON cid= " << tmpSrg.countyID << endl;
         //cout << "lon= " << tmpSrg.lon << endl;
         //cout << "Index= " << polyIDIndexes.at(tmpSrg.lon) << endl;
         polyID = polyIDs.at(polyIDIndexes.at(tmpSrg.lon));
         //cout << "polyID= " << polyID << endl;
         fprintf(fp,"%u\t%lu\t%s\t%.8lf\t%s\n", outSrgID,tmpSrg.countyID,
               polyID.c_str(),tmpSrg.value,tmpSrg.debugInfo );
         //cout << "END of Polygon writting " << endl;
       } //printf("i cid=%lu\n", tmpSrg.countyID);
     }
   }
 }

/* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */

char* GapfillOutput::  myindex (char *buffer, int c)
{
   for (int i = 0; i < strlen(buffer); i++)
   {
     if (buffer[i] == c)
        return &buffer[i];
   }
   return NULL;
}

string GapfillOutput:: trim(string& s) {
  if(s.length() == 0)
    return s;
  int b = s.find_first_not_of(" \t\n\r");
  int e = s.find_last_not_of(" \t\n\r");
  if(b == -1){ // No non-spaces
    return "";
  }
  return string(s, b, e - b + 1);
}

//* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= *\/
bool  comp_surr(const Surrogate  &s1, const Surrogate &s2)
{
  
  if (s1.surrID < s2.surrID){
    return true;
  }
  else if (s1.surrID > s2.surrID){
    return  false;
  }
  /* we get here only if id's are the same */
  if (s1.countyID < s2.countyID){
    return true;
  }
  else if (s1.countyID > s2.countyID){
    return  false;
  }
  /* we get here only if id's and cols are the same */
  if (s1.lat < s2.lat){
    return true;
  }
  else if (s1.lat > s2.lat){
    return  false;
  }
  /* we get here only if id's,cols anb lat are the same */
  if (s1.lon < s2.lon){
    return true;
  }
  else if (s1.lon > s2.lon){
    return  false;
  }
  return true;
}
