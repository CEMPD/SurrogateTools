#include "MergeOutput.h"

using namespace std;

extern "C"  int compileExpression(char infixExpression[]);
extern "C" double calculateExpression(double values[]);
extern bool comp_surr(const Surrogate  &s1, const Surrogate &s2);


/** =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */
void MergeOutput:: writeOutput(MergeFunction &mergeFunc,char* inputFN){  
  //cout << "Starting  output to file '" <<mergeFunc.oFName << "'" <<  endl;
  const  char* oFName = mergeFunc.oFName.c_str();
  FILE* fp;
  Polygons  polys;
  string headerInfo;

  if(( fp=fopen(oFName,"w")) == NULL){
    cerr << "Failed to create the file  '"<< oFName << "'" << endl;
    exit(1);
  }
  vector<Function>functions = mergeFunc.funcVar;
  writeHeader(functions,fp, inputFN, headerInfo);
  vector<string>gridInfo;
  vector<CountyTotal> totals;
  map<long,ExistSum> allCounty;
  vector<string>infixFunc = mergeFunc.infixFunc;
  Function aFunc;
  string aInfixFunc;
  char wrongValue [] = " Error - Invalid value: 0.0<=value<=1.0";
  char wrongTotal [] = " Warning - Invalid total: sum is not 1 for the county, sum = ";
  char wrongTV [] = "Error - Invalid value & total for the county, sum=";
  const char* charInfix;
  //cout << "number of functions=" << functions.size() << endl;

  // loop over each output surrogate to create via a function
  for(int i=0; i< functions.size(); i++){

    cout << "Processing output for '" << functions.at(i).outSrgName << "'" <<endl;

    aInfixFunc = infixFunc.at(i);
    charInfix = aInfixFunc.c_str();
    char infix[200];
    strcpy(infix, charInfix);   
    //cout << "Infix string :" << infix << endl;    
    compileExpression(infix);
    aFunc = functions.at(i);

	// read all the surrogate files needed for this run
    readSrgFiles(aFunc,totals,gridInfo,polys);
    checkGridInfo (gridInfo);

	// find all the counties in the surrogates files
    makeAllCounty(totals,allCounty);

    // map<int,ExistSum>::iterator iter= allCounty.begin();
    //     while(iter!= allCounty.end()){
    //       cout << " countyID="<< iter-> first << endl;
    //       cout << " existAll=" << (iter->second).existAll << flush;
    //       cout << " sum=" << (iter -> second).initSum << endl;
    //       iter++;
    //     }
    //cout << "finished reading srgt files" << endl;

    Surrogates ss = srgsV.at(0);

    vector<Surrogate> srgV = ss.srgV;

    Surrogate minSurrogate;

    Surrogate surrogate; 
    
    vector<string> polyIDs;    
    vector<long> polyIDIndexes;    
    string polyID;    
    polyIDs = polys.polyIDs;    
    polyIDIndexes = polys.polyIDIndexes;
    
    cout << "polyIDs size= " << polyIDs.size() << endl;
    cout << "polyIDIndexes size= " << polyIDIndexes.size() << endl;
        
    int size = srgsV.size();
    double *values = new double [size*sizeof(int)];  // values for operands
    int *track = new int[size*sizeof(int)]; //contain indices for surrogates.
    bool *trackFinish = new bool [size*sizeof(bool)]; 	// track whether done traversing each surrogate    

    for(int j=0; j< size; j++)  //initializing
	{
		track[j] = 0; 
		values[j] = 0.0;
		trackFinish[j] = false;
	}

    bool flag = true;
    int index = -1;//which surrogate has the minIndex
    //Surrogate minSrg;    
    int finished = 0;
    double eValue = 0.0;
    long countyID = srgsV.at(0).srgV.at(0).countyID;
    double checkSum = 0.0;
    char debugInfo[255];
    bool existAll = false;
    bool sumCheck = false;

	sprintf(debugInfo,"");
    
	// loop over counties and the  surrogates to process
    while(flag){
      //index = getMinIndex(track,trackFinish,minIndex,size);
      //cout << "While starting: minIndex=" << minIndex << ", index="<<index << ", finished=" << finished << endl;

      //ss = srgsV.at(index);
      //minSurrogate = ss.srgV.at(minIndex);

	  // find the surrogate that has the most counties
      getMinSrg(track,trackFinish,minSurrogate);

      //out << "minSrg.cid=" << minSurrogate.countyID << endl;           

      map<long,ExistSum>::iterator iter = allCounty.find(minSurrogate.countyID);

	  // if we haven't finished processing all the counties...
      if(iter != allCounty.end()){
        existAll = (iter->second).existAll;
        sumCheck = (iter->second).initSum;
      }

      // for each surrogate to be processed, fill the values array with the operands
	  // for the expression
      for(int j=0; j< size; j++){
        //cout << "for: j=" << j<< endl; // j will be 0 and 1 for a two surrogate merge
        ss = srgsV.at(j);

        //cout <<"   track[" << j << "]=" <<track[j] << endl;
        surrogate = ss.srgV.at(track[j]);

        //cout << "   srg value= " << surrogate.value << endl;
        //cout << "    compare(minSurrogate,surrogate)=" << compare(minSurrogate,surrogate)<< endl;

		// if this is the lowest common denominator surrogate, store the value
        if(compare(minSurrogate,surrogate) ==0)
		{
          values[j] = surrogate.value;
          //cout << "   If equal: values[" << j<< "] = " << values[j] << endl;

          if(track[j] < ss.srgV.size()-1)
          // was: if(track[j] < ss.srgV.size()-1)
		  { // shift the pointer if the particluar srg list is not traversed to the end
            track[j]++; // go to the next item in the list
          }
           else if(finished == size-1)  
		  { 
			//if the particular srg list is traversed and still some more srg list are to be traversed then continue
			cout << "Done a list, for j = " << j << endl;
            //AME: flag = false;    
            //finished ++;
          }
          else{ // finished traversing all the srg lists;
            finished ++;
 			cout << "Done all lists" << endl;
            trackFinish[j] = true;
			flag = false;
          }
        }//if(minSurrogate.compare(surrogate))
        else{
          if(existAll){
            values[j] =0;
          }
        }//else
        //cout << "   values[" << j<< "] = " << values[j] << endl;
      }//for(j,size)

	  // once you have all the operands, compute the value of the expression
      if(existAll){
        eValue = calculateExpression(values);
        cout << "countyID = " << countyID << 
              " mincountyID = " << minSurrogate.countyID << " vals = " <<
			  values[0] << "," << values[1]<<" result = " << eValue << endl;
        //Error checking

        if(eValue < (0.0-TOLERANCE) || eValue > (1.0+TOLERANCE)){
          sprintf(debugInfo,"%s",wrongValue);
        }
        else{
          sprintf(debugInfo,"");
        }
        if(countyID == minSurrogate.countyID){
          checkSum  += eValue;  
        }
        else{
          countyID = minSurrogate.countyID;
          checkSum = eValue;
        }

        //cout << "(1.0+TOLERANCE)=" << (1.0+TOLERANCE)<< endl;

        if(checkSum <(0-TOLERANCE) || checkSum >(1.0+TOLERANCE)){
          //cout << "checkSum should be between 1 and 0, but = " << checkSum << endl; 
          //char buf[50];
          //sprintf(buf,"%d",minSurrogate.countyID);   

          if(strlen(debugInfo) == 0){
			cout << "Total = " << checkSum << endl;
			sprintf(debugInfo,"%s %f",wrongTotal,checkSum);
            //debugInfo = wrongTotal;
          }
          else{    
            sprintf(debugInfo,"%s %f",wrongTV, checkSum);
          }
          //debugInfo = strcat(debugInfo,buf);
        }     
        //cout << "eValue=" << eValue << endl;

        if(strlen(debugInfo) == 0){
		  // Keep going even if there is a problem, because some problems are only
		  // small deviations from 1 - e.g. due to summing line lengths

          if(headerInfo.find("#GRID\t")!= string::npos || headerInfo.find("#GRID ")!= string::npos) {
            fprintf(fp,"%u\t%lu\t%u\t%lu\t%.8lf\n",aFunc.outSrgID,minSurrogate.countyID,minSurrogate.lat,minSurrogate.lon,eValue);
          }
          else {
            //cout << "POLYGON cid= " << tmpSrg.countyID << endl;
            //cout << "lon= " << tmpSrg.lon << endl;
            //cout << "Index= " << polyIDIndexes.at(tmpSrg.lon) << endl;
            polyID = polyIDs.at(polyIDIndexes.at(minSurrogate.lon));
            //cout << "polyID= " << polyID << endl;
            fprintf(fp,"%u\t%lu\t%s\t%.8lf\n",aFunc.outSrgID,minSurrogate.countyID,polyID.c_str(),eValue);
          }
          //printf("%u, %lu, %u, %lu, %lf\n",aFunc.outSrgID,minSurrogate.countyID,minSurrogate.lat,minSurrogate.lon,eValue);
        }
        else // debugInfo is not null
        {
          if(headerInfo.find("#GRID\t")!= string::npos || headerInfo.find("#GRID ")!= string::npos) {        
            fprintf(fp,"%u\t%lu\t%u\t%lu\t%.8lf !%s\n",aFunc.outSrgID,minSurrogate.countyID,minSurrogate.lat,minSurrogate.lon,eValue,debugInfo);
            fprintf(stderr,"ERROR: %u\t%lu\t%u\t%lu\t%.8lf !%s\n",aFunc.outSrgID,minSurrogate.countyID,minSurrogate.lat,minSurrogate.lon,eValue,debugInfo);
          }
          else { // polygon version
            polyID = polyIDs.at(polyIDIndexes.at(minSurrogate.lon));  
            fprintf(fp,"%u\t%lu\t%s\t%.8lf !%s\n",aFunc.outSrgID,minSurrogate.countyID,polyID.c_str(),eValue,debugInfo);  
            fprintf(stderr,"ERROR: %u\t%lu\t%s\t%.8lf !%s\n",aFunc.outSrgID,minSurrogate.countyID,polyID.c_str(),eValue,debugInfo);  
          }
        }  // end if debugInfo is not null
    }
    else
    {
        string msg1 = "WARNING: No data is available for county id=";
        string msg2 = " for one of the input surrogates.";
        fprintf(fp,"#%s%d%s\n",msg1.c_str(),minSurrogate.countyID,msg2.c_str());
        cout << msg1 << minSurrogate.countyID << msg2.c_str()<<  endl;
        //cout << "WARNING: County ID=" << minSurrogate.countyID << " needs to be gapfilled" << endl;
        }
      sprintf(debugInfo,"");
    }//while();

    delete values;
    delete track;
    delete trackFinish;
    srgsV.clear();
    totals.clear();  
  }//for(i function)

  //cout << "End of output" << endl;
}//writeOutput


int MergeOutput:: getMinSrg(int track [],bool trackFinish[], Surrogate &minSrg)
{
  int index =0;
  int size = srgsV.size();
  Surrogates ss = srgsV.at(0);
  minSrg = ss.srgV.at(track[0]);
  //cout << "getMinSrg: min cid = " << minSrg.countyID << endl;

  for(int i=1; i< size; i++){
    //cout << " inside for " << endl;
    ss = srgsV.at(i);
    if(compare(minSrg,ss.srgV.at(track[i]))>0){
      //cout << "2getMinSrg: min cid = " << minSrg.countyID << endl;
      minSrg = ss.srgV.at(track[i]);
      index =i;
    }
  }
  //cout <<"r:getMinSrg: min cid= "<< minSrg.countyID << endl; 

  return index;
}


void MergeOutput:: writeHeader(vector<Function> &funs, FILE* fp, char* inputFN, string &headerInfo ){
  string fName = funs.at(0).fileName.at(0);
  readGridInfo(fName,headerInfo);
  fprintf(fp,"%s\n",headerInfo.c_str());
  
  FILE* fi;
  if((fi = fopen(inputFN, "r"))== NULL){
    cerr << "The file '" << inputFN << "' failed to open" <<  endl;
    exit(1);
  }

  char  buffer[MAXLINE];

  fprintf(fp,"%s\n","#############COPY OF THE INPUT FILE############################");
  while(fgets(buffer,MAXLINE,fi)!=NULL){
    //cout << buffer << endl;
    fprintf(fp,"#%s\n",buffer);    
  }
  fprintf(fp,"%s\n","#############END OF INPUT FILE###############################");
  Function fun;
  for(int i=0; i< funs.size(); i++){
    fun = funs.at(i);
    int oSrgID = fun.outSrgID;
    const char* oSrgName = fun.outSrgName.c_str();
    fprintf(fp,"#SRGDESC=%d, %s\n",oSrgID, oSrgName);
    //printf("#SRGDESC=%d, %s\n",oSrgID, oSrgName);
  }
}


void MergeOutput:: readGridInfo(string fName, string &headerInfo){
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
    if(tempString.find("#GRID\t")!= string:: npos || tempString.find("#GRID ")!= string:: npos ||
       tempString.find("#POLYGON\t") != string::npos || tempString.find("#POLYGON ") != string::npos){

      headerInfo.append(tempString);
      fclose(fp);
      return;
    }
    else {
     cerr << "The file '"<< fName << "' does not have right header (#GRID or #POLYGON)."<< endl;
     exit(1);
    }
  }
  fclose(fp);
}





int MergeOutput:: readSrgFiles(Function &aFunc,vector<CountyTotal> &totals,vector<string> &gridInfo, Polygons &polys){
  vector<string> fNames = aFunc.fileName;
  vector<int> srgIDs = aFunc.srgID;
  string fName;
  const char* charFName;
  int srgID;

  //fill first element in polygon ID vectors
  polys.polyIDIndexes.push_back(0);
  polys.polyIDs.push_back("FILLFIRSTONE");

  for(int i=0; i< fNames.size(); i++){
    Surrogates ss;
    vector<Surrogate> s ;
    CountyTotal aTotal;
    fName = fNames.at(i);
    charFName = fName.c_str();
    srgID = srgIDs.at(i);
    readSrgFile(charFName,srgID,s,aTotal,gridInfo, polys);
    //cout << "i=" << i << ", length=" <<s.size()<< endl;
    //cout << "i=" << i << ", aTotal.cTotal.length=" <<aTotal.cTotal.size()<< endl;
    //map<int,double>::const_iterator iter;
   //for( iter=aTotal.cTotal.begin(); iter != aTotal.cTotal.end(); ++iter) {
    //  cout << iter->second << " " << iter->first << endl;
    //}
    ss.srgV = s;
    srgsV.push_back(ss);
    totals.push_back(aTotal);
  }//for(i)
  //cout <<"LENGTH=" <<srgsV.at(0).srgV.size()<< endl;
  //cout <<"LENGTH=" <<srgsV.at(1).srgV.size()<< endl;
  return 0; // BB 1/11/2005
}



//======================================================

void MergeOutput:: makeAllCounty(vector<CountyTotal> &total,map<long,ExistSum> &allCounty){

  int size = total.size();
  long min = LARGE_VALUE;
  long *trackE = new long[size*sizeof(long)];
  //int trackE[size];
  double *trackS = new double[size*sizeof(double)];
  CountyTotal ct;
  bool sum1= true;
  bool existAll = true;

  //initialization of track;
  for(int i=0; i< size; i++){
    ct = total.at(i);
    map<long,double>::iterator iter = ct.cTotal.begin();
    trackE[i] = iter->first;
    trackS[i] = iter->second;
    //cout<<"trackE["<< i << "]=" << trackE[i] << endl;
    //cout<<"trackS["<< i << "]=" << trackS[i] << endl;
  }

  while(true){
    //min

    if(!findMin(trackE,min,size)){
      delete trackE;
      delete trackS;
      return;
    }

    for(int i=0; i< size; i++){
      if(min == trackE[i] && trackE[i] != -1){
        //cout << "ma:trackS[" <<i<<"]="<<trackS[i]<<endl;
        if(trackS[i] >= 1.0 -TOLERANCE && trackS[i] <= 1.0 + TOLERANCE){
          //do nothing  
        }
        else{
          sum1= false;
        }

        //move the track[i] to next countyID
        ct = total.at(i);
        moveIndex(trackE,trackS,i,ct);

      }
      else{
        existAll = false;
        sum1= false;
      }
    }//for

    ExistSum es;
    es.existAll = existAll;
    es.initSum = sum1;
    allCounty.insert(pair<long,ExistSum> (min,es));
    existAll = true;
    sum1 = true;
  }//while

  delete trackE;
  delete trackS;

}//makeAllCounty



bool MergeOutput:: findMin(long track[],long &min,int size){

  min = LARGE_VALUE;//set to large value

  //cout << "min=" << min << endl;

  bool check = false;

  for(int i=0; i< size; i++){
    if(track[i]<min && track[i] != -1){
      //cout <<"track[i]=" <<track[i] << endl;
      min = track[i];
      check = true;
    }
  }
  //cout << "check=" << check << endl;

  return check; //false means all values are -1,ie all surrgates are traced 
}


void MergeOutput:: moveIndex(long trackE[],double trackS[], int i, CountyTotal ct)
{
  long cid = trackE[i];
  map<long,double> c = ct.cTotal;
  map<long,double>::iterator iter =c.find(cid);
  if(iter != c.end()){
    if(++iter != c.end()){
      trackE[i] = iter->first;
      trackS[i] = iter->second;
    }
    else{
      trackE[i] = -1;
      trackS[i] = -1;
    }
  }
  else{
    trackE[i] = -1;
    trackS[i] = -1;
  }

}

void MergeOutput:: checkGridInfo(vector<string> gridInfo){
  // cout << "grid Info size=" <<gridInfo.size() << endl;
  if(gridInfo.size() > 0){
    string s = gridInfo.at(0);
    //cout << "s=" << s << endl;
    for(int i=1; i < gridInfo.size(); i++){
      if(s.compare(gridInfo.at(i)) != 0){
        throw ChkErrors("The surrogate files specified do not belong to the same grid");
      }
    }
  }
}


//======================================================
/** Compares this surrogate with the Surrogate a. 
 *if this < a reurn -1, this > a return 1, this = a return 0;
 */
int MergeOutput:: compare(Surrogate &a,Surrogate &b){
  //no need to check the srg id
  if(a.countyID < b.countyID){
    return -1;
  }
  else if(a.countyID > b.countyID){
    return 1;
  }
  
  if(a.lat< b.lat){
    return -1;
  }
  else if(a.lat > b.lat){
    return 1;
  }

  if(a.lon< b.lon){
    return -1;
  }
  else if(a.lon > b.lon){
    return 1;
  }
  else{
    return 0;
  }  
}

//======================================================

int MergeOutput:: readSrgFile(const char* fname, int srgID, 
  vector<Surrogate> &s, CountyTotal &total, vector<string> &gridInfo, 
  Polygons &polys)
{
  FILE* fp;
  int n = 0; // number of surrogates  
  int surrID=0,lat=0; //temp storage for a relevant srg line
  long countyID=0,lon=0; //temp storage for a relevant srg line
  string polyID;
  char polyID_c[20];
  double value=0.0;                     // -do-

  long i = 1;       //index for polyIDs array
  char *debugInfo= NULL;                // -do-

  bool first = false;
  char buffer[MAXLINE];
  char tempBuffer[MAXLINE];
  long trackCountyID = -1;
  double totalValue = 0.0;
  bool flag = true; // once trackCountyID is initialized then flag is se to false
  string  headerString;            //store header line
  int   pushHeader_index = 1;    //for finding the right header to push in case multiple surrogates in one file

  if ((fp = fopen(fname,"r")) == NULL){
    cerr << " Error in opening the file '" << fname << "'" <<  endl;
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

    if(tempString.size()!=0){
      strcpy(buffer,tempString.c_str());       
      if(buffer[0]!= '#'){

        if(headerString.find("#GRID\t")!= string::npos || headerString.find("#GRID ")!= string::npos) {
        sscanf(buffer,"%u%lu%u%lu%lf", &surrID, &countyID, &lat, &lon, &value);

        //cout<< "srgid=" << surrID << ", countyID="<< countyID << ", lat=" << lat<< ", lon=" << lon << ",value=" << value << endl;
        }
        else {        
         sscanf(buffer,"%u%lu%s%lf", &surrID, &countyID, polyID_c, &value);
          polyID = polyID_c;   //convert to string type
          //cout << "polyID=" << polyID << endl;
        }

        //printf("%u\n",countyID);
        debugInfo = myindex(buffer, '!');    
        if(surrID==srgID){
         if (pushHeader_index == 1) {
            gridInfo.push_back(headerString);  //from readGridInfo function, not here
            pushHeader_index = 0;  //only store the header once before surrogate data 
          }
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
          //if (debugInfo != NULL){
            //temp.debugInfo = (char *)malloc(sizeof(char)*(strlen(debugInfo)+1));
            //strcpy(temp.debugInfo,debugInfo);
            //temp.debugInfo = strdup(debugInfo);
            //}
          //cout << "debugInfo=" << debugInfo << endl;
          if(trackCountyID ==countyID){
            totalValue += value;
            //cout << "if: cid" << countyID << " totalValue=" << totalValue << endl;
          }
          else{  
            if(!flag){
              //cout << "cid=" << trackCountyID << " totalValue=" << totalValue << endl;
              total.cTotal.insert(pair<long,double>(trackCountyID,totalValue));
            }
            trackCountyID = countyID;
            //cout << "else: trackCID=" << trackCountyID << endl;    
            totalValue = value;
            //cout << "else: cid" << countyID << " totalValue=" << totalValue << endl;
          }
          flag = false;
          s.push_back(temp);
          n++;
          //printf("%u\t%lu\t%u\t%lu\t%lf\n", surrID, countyID, lat, lon, value);
        }//if(surrID==srgID)      
      } //if starts with !#
    }//if(tempString.size()!=0){
  }//while  
  //cout << "cid=" << trackCountyID << " totalValue=" << totalValue << endl;
  total.cTotal.insert(pair<long,double>(trackCountyID,totalValue));
  fclose(fp);
  sort(s.begin(),s.end(),comp_surr);
  //cout << "n=" << n<< endl;
  return n;
}//readSrgFile()


/**
 * If s1 < s2 return true;
 * else return true;
 */
//* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= *\/

/* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */

string MergeOutput:: trim(string& s) {
  if(s.length() == 0)
    return s;
  int b = s.find_first_not_of(" \t\n\r");
  int e = s.find_last_not_of(" \t\n\r");
  if(b == -1){ // No non-spaces
    return "";
  }
  return string(s, b, e - b + 1);
}


char* MergeOutput:: myindex (char *buffer, int c)
{
   for (int i = 0; i < strlen(buffer); i++)
   {
     if (buffer[i] == c)
        return &buffer[i];
   }
   return NULL;
}

