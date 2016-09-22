/*******************************************************
*       Version of Raster Spatial Allocator            *
*******************************************************/
char *prog_version = "Spatial Allocator Raster Version 3.6, 03/10/2009\n";

/*******************************************************
*                Utility prototypes                    *
*******************************************************/
#include <vector>
void FileExists (const char *strFilename, int code);  //code = 0 existing file, code = 1 new file code = 2 existing dir
std::string& trimL ( std::string& s );
std::string& trimR ( std::string& s );
std::string& trim ( std::string& s );
char   *getEnviVariable(const char * name);
std::string& processDirName(std::string& dirStr);
std::string& stringToUpper(std::string& str);
float getValueInProj4Str(std::string proj4Str, const char *param);
std::string getStrInProj4Str(std::string proj4Str, const char *param);
char * convert2LatLongProjection (char *pszProj4_shp);
std::vector<std::string> string2stringVector (std::string strVar, const char *sep);
