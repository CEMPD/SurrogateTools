/********************************************************************
 * This program includes some common utilities used by other programs: 
 *  1. FileExists - Check file and directory existence. 
 *  2. trim, trimL and trimR - Trim a string
 *  3. getEnviVariable - Get environmental variable
 *  4. processDirName - Process dir to make that it ends with path separator
 *  5. stringToUpper - Convert a string to upper case
 *  6. getValueInProj4Str - Get a value parameter from PROJ4 string
 *  7. getStrInProj4Str - Get a string parameter from PROJ4 string
 *  8. convert2LatLongProjection - Convert PROJ4 projection to PROJ4 latlong projection
 * 10. string2stringVector - convert string to string vector

 * Written by the Institute for the Environment at UNC, Chapel Hill
 * in support of the EPA NOAA CMAS Modeling, 2007-2008.
 * 
 * By Limei Ran, Feb-March 2008
 *
 * Usage: to be included in other programs
***********************************************************************/

#include <string>
#include <vector>
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;
float MISSIING_VALUE=-9999.0;    

/************************************************************************/
/*        void FileExists(const char *strFilename, int eCode)           */
/************************************************************************/

void FileExists(const char *strFilename, int eCode) {
  struct stat stFileInfo;
  int intStat;

  /*  eCode
      0 = file or directory has to exist
      1 = file or directory has to be new 
      2 = directory has to exist, if not create it 
      3 = file exists and remove it
  */
      
  // Attempt to get the file attributes
  intStat = stat(strFilename,&stFileInfo);
  //printf("File = %s   intStat = %d   eCode = %d\n",strFilename,intStat,eCode);
  //file had to be new
  if(intStat == 0 && eCode == 1) 
  {
    // get the file attributes, so the file obviously exists.
    //eCode = 1: the file has to be new 
    printf("Error: File exists: %s\n",strFilename);
    exit(1);
  } 

  //file has to exist
  if (intStat != 0 && eCode == 0)  
  {
    // We were not able to get the file attributes.
    // This may mean that we don't have permission to
    // access the folder which contains this file. If you
    // need to do that level of checking, lookup the
    // return values of stat which will give you
    // more details on why stat failed.

    //eCode = 0 : the file has to exist
   
     printf("Error: File doe not exist: %s\n",strFilename);
     exit(1);
  }

  //create new dir
  if (intStat != 0 && eCode == 2) 
  {
    // We were not able to get the file attributes.
    // This may mean that we don't have permission to
    // access the folder which contains this file. If you
    // need to do that level of checking, lookup the
    // return values of stat which will give you
    // more details on why stat failed.

    //eCode = 2 : the directory has to exist
    if(mkdir(strFilename,0777)==-1)//creating a directory
    {
        printf ("Error: could not create directory: %s\n",strFilename);
        exit(1);
    } 
  }

  //file exists and remove it
  if(intStat == 0 && eCode == 3)
  {
     // get the file attributes, so the file obviously exists.
     //eCode = 3: delete the file
     if( remove( strFilename ) != 0 )
     {
        printf( "Error deleting file: %s\n", strFilename );
        exit (1); 
     }
     else
     {
        printf("Deleted existing file: %s\n",strFilename);
     }
    
  }
  
}


/************************************************************************/
/*                               trimL(string& s)                       */
/************************************************************************/

//! \return modified string ``s'' with spaces trimmed from left
std::string& trimL(std::string& s) 
{
  int pos(0);
  for ( ; s[pos]==' ' || s[pos]=='\t'; ++pos );
  s.erase(0, pos);
  return s;
}


/************************************************************************/
/*                               trimR(string& s)                       */
/************************************************************************/
//! \return modified string ``s'' with spaces trimmed from right
std::string& trimR(std::string& s) 
{
  int pos(s.size());
  for ( ; pos && s[pos-1]==' ' || s[pos]=='\t'; --pos );
  s.erase(pos, s.size()-pos);
  return s;
}


/************************************************************************/
/*                               trim(string& s)                       */
/************************************************************************/
// return a string trim by both edge spaces
std::string& trim(std::string& s) 
{
   return trimL(trimR ( s ) );
}


/************************************************************************/
/*                        char   *  getEnviVar(const char * name)      */
/************************************************************************/
char   *getEnviVariable(const char * name)
{
       char   *enviVar;   //environmental variable to get


       enviVar = getenv (name);
       if ( enviVar == NULL )
       {
          printf("  Error: environmental variable -- %s is not set.\n",name);
          exit (1);
       }
       return enviVar;
}   


/************************************************************************/
/*           string& processDirName(string& dirStr)                     */
/************************************************************************/
// make sure that directory ends with path separator
std::string& processDirName(std::string& dirStr)
{
   int      i;

   dirStr = trim( dirStr );
   i = dirStr.length() - 1;            //last char for the dir
    if ( ! (dirStr.at(i) == '\\' || dirStr.at(i) == '/')  )
    {
       if ( dirStr.find_last_of("\\")!=string::npos )
       {
          dirStr.append("\\");
       }
       else if ( dirStr.find_last_of("/")!=string::npos )
       {
         dirStr.append("/");
       }
       else
       {
         printf( "Error: directory has to end with path separator: %s\n",dirStr.c_str() );
         exit (1);
       }
    }

   return ( dirStr );
}


/************************************************************************/
/*           string& stringToUpper(string& str)                         */
/************************************************************************/
std::string&  stringToUpper(std::string& myString) 
{
  const int length = myString.length();
  for(int i=0; i<length; i++)
  {
    myString[i] = std::toupper(myString[i]);
  }
  return myString;
}



/***********************************************
* Get the value from PROJ.4 projection string  *
***********************************************/
float getValueInProj4Str(std::string proj4Str, const char *param)
{
     float    value;
     int      pos;
     

     if( (pos = proj4Str.find(param)) != string::npos )
     {
          proj4Str.erase(0,pos);         //get rid of everything before this parm

          pos = proj4Str.find("="); 
          proj4Str.erase(0,pos+1);       //get rid of parm name and =

          pos = proj4Str.find(" ");
          value = (float) atof( proj4Str.substr(0,pos).c_str() );
          //printf ("%s= %f\n", param,value );
     }
     else
     {
         value = MISSIING_VALUE; 
         printf ("\tNo %s in GRID_PROJ and set it to: %f\n", param, value );
     } 

    return value;
}


/***********************************************
* Get the string from PROJ.4 projection string *
***********************************************/
std::string getStrInProj4Str(std::string proj4Str, const char *param)
{
     string    value;
     int       pos;
   

     if( (pos = proj4Str.find(param)) != string::npos )
     {
          proj4Str.erase(0,pos);         //get rid of everything before this parm

          pos = proj4Str.find("="); 
          proj4Str.erase(0,pos+1);       //get rid of parm name and =

          pos = proj4Str.find(" ");
          value =  proj4Str.substr(0,pos);
          //printf ("%s%s\n", param,value.c_str() );
     }
     else
     {
         printf ("\tNo %s in %s\n", param, proj4Str.c_str() );
     }

    return value;
}



/************************************************************************/
/*           char * convert2LatLongProjection (char *pszProj4_shp)      */
/************************************************************************/
char * convert2LatLongProjection (char *pszProj4_shp)
{
   
   string  projStr, projStr_geo;
   string parmStr;
   float  parmVal; 
   char   extStr[150];
   char   *proj4Chars;

   projStr = string (pszProj4_shp);

   parmStr = getStrInProj4Str(projStr, "+proj=");
   if ( parmStr.compare ("latlong") == 0 )
   {
      //the projection is already in lat and long
      projStr_geo = projStr;
   }
   else
   {
      //set a projection to latlong
      projStr_geo = string ( "+proj=latlong ");

      //get parm from input projection
      parmVal = getValueInProj4Str(projStr, "+a=");
      if ( parmVal != MISSIING_VALUE )
      {
         sprintf(extStr, " +a=%.5f\0",parmVal);
         projStr_geo.append (extStr);
      }

      parmVal = getValueInProj4Str(projStr, "+b=");
      if ( parmVal != MISSIING_VALUE )
      {
         sprintf(extStr, " +b=%.5f\0",parmVal);
         projStr_geo.append (extStr);
      }

      parmStr = getStrInProj4Str(projStr, "+datum=");
      if (! parmStr.empty() )
      {
         sprintf( extStr, " +datum=%s\0",parmStr.c_str() );
         projStr_geo.append (extStr);
      }

      parmStr = getStrInProj4Str(projStr, "+ellps=");
      if (! parmStr.empty() )
      {
         sprintf( extStr, " +ellps=%s\0",parmStr.c_str() );
         projStr_geo.append (extStr);
      }
   }

   proj4Chars = strdup ( projStr_geo.c_str() ); 

   return (proj4Chars);

}

/************************************************************************/
/*      Convert string with ',' to string vector                        */
/************************************************************************/
std::vector<std::string>  string2stringVector (std::string strVar, const char *sep)
{
   string temp_str = strVar;
   int pos;
   std::vector<string> strVector;

   while ( (pos = temp_str.find(sep)) != string::npos )
   {
      string strName = temp_str.substr(0,pos);
      strName = trim( strName );
      strVector.push_back ( strName );
      temp_str.erase(0,pos+1);
   }
   temp_str = trim( temp_str );
   strVector.push_back ( temp_str );

   return strVector;
}

