/***************************************************************************
 * This program is used: 
 *  1. to compute grid cell landuse percentage information from:
 *            USGS NLCD landuse, 
 *            USGS NLCD canopy,
 *            USGS NLCD imperviousness, 
 *            NOAA C-GAP NLCD landuse along the coast,
 *            MODIS IGBP landuse.
 *  2. to output landuse informaton into a text file table.
 *  3. to output landuse informaton into a netcdf file.
 *
 * Written by the Institute for the Environment at UNC, Chapel Hill
 * in support of the EPA NOAA CMAS Modeling, 2007-2008.
 * 
 * By Limei Ran, April-June 2008
 *    Craig A. Mattocks, UNC-IE    Wrote original netCDF parts
 *
 * Usage:  computeGridLandUse.exe
 *  
 * Environment Variables needed: 
 *         GRID_ROWS  -- grid domain rows
 *         GRID_COLUMNS -- grid domain columns
 *         GRID_XMIN -- grid domain xmin
 *         GRID_YMIN -- grid domain ymin
 *         GRID_XCELLSIZE -- grid domain x grid size 
 *         GRID_YCELLSIZE -- grid domain y grid size 
 *         GRID_PROJ -- grid domain proj4 projection definition
 *         POLE_LATITUDE  -- pole latitude.  N sphere should be 90
 *         POLE_LONGITUDE -- pole longitude. should be 0
 *         GRID_RASTERFILE_NAME -- rasterized 30m grid domain file 
 *         OUTPUT_NLCDFILES_LIST -- preprocessed NLCD file list
 *         INCLUDE_USGS_LANDUSE -- YES or NO to include USGS NLCD landuse data in computation
 *         INCLUDE_USGS_IMPERVIOUSNESS -- YES or NO to include USGS NLCD imperviousness data in computation
 *         INCLUDE_USGS_CANOPY -- YES or NO to include USGS NLCD canopy data in computation
 *         INCLUDE_NOAA_LANDUSE -- YES or NO to include NOAA coastal NLCD landuse data in computation 
 *         INCLUDE_MODIS -- YES or NO to include NASA MODIS IGBP landuse data in computation
 *         OUTPUT_MODISFILE -- projected and clipped MODIS data for the given doamin
 *         OUTPUT_LANDUSE_TEXT_FILE -- text table output grid cell landuse information
 *         OUTPUT_LANDUSE_NETCDF_FILE -- netCDF output grid cell landuse information.  Only works for LCC now.

***********************************************************************************/
//for computing landuse info
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <set>
#include "cpl_conv.h"
#include "cpl_string.h"
#include "gdal_priv.h"
#include <ogr_spatialref.h>
#include <vrtdataset.h>
#include "commontools.h"

//for outputing netcdf format 
#include <vector>
#include <sstream>
#include <proj_api.h>
#include <netcdf.h>

#define BAD_VALUE "-999"
#define TIMES 1
/******************************************
* Map projections supported by WRF:       *
*    LATLONG = Latitude-Longitude         *
*    LCC  = Lambert Conformal Conic       *
*    UPS  = Universal Polar Stereographic *
*    MERC = Mercator                      *
******************************************/
#define LATLONG 0
#define LCC 1
#define UPS 2
#define MERC 3
using namespace std;

//function for computing landuse info
static void  fillLandClassHashTables();
void countGridIDs();
void computeIntersectionBox(int xCells, int yCells, double xmin, double ymin, double xmax, double ymax,
                            double xMin_grd, double yMin_grd, double xMax_grd, double yMax_grd,
                            int *col1, int *row1, int *col2, int *row2,
                            int *col1_grd, int *row1_grd, int *col2_grd, int *row2_grd);
void   computeImpe_Cano( std::vector<string> imageFiles, string fileType );
void   computeLandUse(std::vector<string> imageFiles, string fileType);
void   computeMODIS( string modisFile ) ;

//functions for outputing netcdf format
int getProjType(string proj4Str);
void tokenize   (const string &str, vector<string> &tokens, const string &delimiters);
void anyErrors(int e);
projUV  computeLatLong(projPJ proj4DF, double x, double y);


//define global variables
string                landType = "USGS NLCD Landuse Files";
string                impeType = "USGS NLCD Urban Imperviousness Files";
string                canoType = "USGS NLCD Tree Canopy Files";
string                cgapType = "NOAA CGAP NLCD Landuse Files";

GDALDataset           *poGrid;                      //store rasterized 30m grid image
GDALDriver            *poDrive;
GDALRasterBand        *poGridBand;                  //band 1 for rasterized 30m grid image
OGRSpatialReference   oSRS_std;                     //standard projection from grid domain raster   
double                xCellSize_grd, yCellSize_grd; //standard cell size from grid domain raster
int                   xCells_grd, yCells_grd;       //cells for grid domain raster 
double                xMin_grd,xMax_grd,yMin_grd,yMax_grd;  //modeling domain extent
OGRSpatialReference   oSRS_grd;
char                  *pszProj4_grd = NULL;

int                   gridRows,gridCols;                    //modeling grid rows and columns to compute grid IDs
int                   gridPixels;                           //total pixels in modeling grids
int                   *gridIDS=NULL;                        //array to store number of pixels in each modeling grids
double                *gridIMPE=NULL, *gridCANO=NULL;       //array to store grid percentage of imperviousness and canopy 
int                   *gridNLCD=NULL, *gridMODIS=NULL;      //array to store NLCD and MODIS data for modeling grid cells   

std::map<int,int>     nlcdIDS,modisIDS,noaaIDS;             //hash tables to store landuse class IDs and index  

//match NLCD and NOAA classes by array index
//USGS NLCD 127 is NODATA background
//NOAA NLCD 0 is NPDATA background and 1 is unclassified
//Add 75 - Tundra to USGS NLCD because of 24 Tundra in NOAA NLCD 
static int  nlcdClasses[] = {11,12,21,22,23,24,31,32,41,42,43,51,52,71,72,73,74,75,81,82,90,91,92,93,94,95,96,97,98,99,127};
static int  noaaClasses[] = {21,25, 5, 4, 3, 2,20,19, 9,10,11,26,12, 8,27,29,28,24, 7, 6,90,13,14,16,17,95,15,18,22,23,  0};
    
//MODIS IGBP 255 is NODATA background
static int  modisClasses[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,254,255,150};

const int   NLCD_CLASSES_NUM=31, MODIS_CLASSES_NUM=21;  //number classes in NLCD and MODIS landuse images
int         land_cat_len = 0;    //landuse items in output


/************************************************************************/
/*    fillLandClassHashTables ()                                        */
/************************************************************************/
static void   fillLandClassHashTables(  )
{  
    int      i;
    int      classID;

     printf( "\nFilling land classification hashtables for USGS, NOAA, and MODIS landuse classes...\n" );
 
    //NLCD landuse classes
    for (i=0; i<NLCD_CLASSES_NUM; i++)
    {
         classID = nlcdClasses[i];
         nlcdIDS[classID] = i;
         printf( "\tUSGS classID=%d  nlcdIDS=%d",classID,nlcdIDS[classID]);

         classID = noaaClasses[i];
         noaaIDS[classID] = i;
         printf( "\tNOAA classID=%d  noaaIDS=%d\n",classID,noaaIDS[classID]);
    }
    

    //MODIS IGBP landuse classes
    for (i=0; i<MODIS_CLASSES_NUM; i++)
    {
       classID =  modisClasses[i];
       modisIDS[classID] = i;
       printf( "\tclassID=%d    modisIDS=%d\n",classID,modisIDS[classID]);
    }

}

/************************************************************************/
/*                                main()                                */
/************************************************************************/

int main( int nArgc,  char* papszArgv[] )
{

    //for computing landuse info and output into txt table
    string                gridRasterFile; 
    string                dataFileList;          //file containing all processed NLCD file names
    string                inUSGSLand,inUSGSImpe,inUSGSCano;   //inclusion indicator
    string                inNOAALand,inNASALand;              //inclusion indicator
    string                modisFile;
    string                outTextFile, outNetcdfFile;        //output file names
    std::vector<string>   landFiles, impeFiles, canoFiles, cgapFiles; 
    std::ifstream         imageStream;             //input NLCD image file list  
    string                fileName, fileType;      //for reading in all NLCD files
    string                tmp_str;
    
    std::ofstream         outTxtStream;            //output text file stream 
    string                lineStr;
 
    int                   i,j,k;                    
    double                adfGeoTransform[6];
    double                xUL,yUL;
    double                intpart;

    const char            *pszWKT = NULL; 
    char                  *pszWKT_nc = NULL;
    double                gridArea,percent;

    //print program version
    printf ("\nUsing: %s\n", prog_version);

    //for outputing landuse info into netcdf format
    /*************************
    *    Grid variables      *
    *************************/
        int rows, cols;
        float x0, y0;
        float dx, dy;
        //domain extent in lat and long
        float cLat, cLon;
        float poleLat, poleLon;
        float swLat,nwLat, neLat,seLat, swLon,nwLon, neLon,seLon;
        double xmax,ymax,xx,yy;

    /***************************
    * Map projection variables *
    ***************************/
        projPJ proj4DF;
        projUV  xyP, latlongP;
        char *pszProj4;
        string projInfo;
        int proj;     //WRF projection type
        float stdLon,stdLat;
        float trueLat1, trueLat2;
        //temp vars
        string temp_proj;

    /***********************************
    * NetCDF variable array pointers   *
    ************************************/
        char       *dateStr;    //char array
        float      *x, *y;      //2d array  time*cols  time*rows
        float      *lat, *lon;  //3d array  time*rows*cols
        int        *lu_list;    //landuse class list times*classes
        float      *imperv;     //3d    time*rows*cols
        float      *canopy;     //3d    time*rows*cols
        float      *luf;        //landuse fraction array 4d  time*classes*rows*cols
        float        *lum;        //landuse mask 3d array  time*rows*cols
        float        *lud;        //landuse dominant class 3d  time*rows*cols
        int        times = TIMES;
   
    printf("\nCompute grid landuse percentage information... \n\n"); 
/* -------------------------------------------------------------------- */
/*      Register format(s).                                             */
/* -------------------------------------------------------------------- */
    GDALAllRegister();
    
/* -------------------------------------------------------------------- */
/*      Obtain environment variables                                    */
/* -------------------------------------------------------------------- */
    //no arguments to call this program
    if ( nArgc  == 1 )
    {
       /******************************************
       *   get variables for domain definitions  *
       ******************************************/
       printf( "Getting modeling grid domain variables...\n");
       tmp_str = string( getEnviVariable("GRID_ROWS") );
       gridRows = atoi( tmp_str.c_str() ); 
       rows = gridRows;
       tmp_str = string( getEnviVariable("GRID_COLUMNS") );
       gridCols = atoi( tmp_str.c_str() );
       cols = gridCols;
       printf( "Rows=%d    Cols=%d \n",rows,cols); 

       //get xmin and ymin for the domain
       x0 = (float) atof( getEnviVariable("GRID_XMIN") );
       y0 = (float) atof( getEnviVariable("GRID_YMIN") );
       printf( "xmin=%f    ymin=%f \n", x0, y0);

       //get x and y cell size
       dx = (float) atof( getEnviVariable("GRID_XCELLSIZE") );
       dy = (float) atof( getEnviVariable("GRID_YCELLSIZE") );
       printf( "xcell=%.0f    ycell=%.0f \n",dx,dy);

       //get projection info
       pszProj4 = getEnviVariable("GRID_PROJ");
       printf( "Grid domain projection proj4 = %s\n",pszProj4);

       poleLon = (float) atof( getEnviVariable("POLE_LONGITUDE") );
       poleLat = (float) atof( getEnviVariable("POLE_LATITUDE") );
       printf( "Polar point: longitude=%f   latitude =%f \n",poleLon,poleLat);

       //get projection parameters from Proj4 string
       temp_proj = string (pszProj4);
       proj = getProjType(temp_proj);
       printf( "proj type = %d \n",proj);
       trueLat1 = getValueInProj4Str ( temp_proj, "lat_1=");
       trueLat2 = getValueInProj4Str ( temp_proj, "lat_2=");
       stdLat = getValueInProj4Str ( temp_proj, "lat_0=");
       stdLon = getValueInProj4Str ( temp_proj, "lon_0=");

        
       /*******************************************
       *   get variables for landuse computation  *
       *******************************************/
       //printf( "Getting rasterized 30m grid domain file.\n");
       gridRasterFile = string( getEnviVariable("GRID_RASTERFILE_NAME") );
       gridRasterFile = trim( gridRasterFile );

       //printf( "Getting text file with processed NLCD image file list.\n");
       dataFileList = string( getEnviVariable("OUTPUT_NLCDFILES_LIST") );
       dataFileList = trim( dataFileList );

       //printf( "Getting indicator to include USGS NLCD landuse data.\n");
       inUSGSLand =  string ( getEnviVariable("INCLUDE_USGS_LANDUSE") ); 
       inUSGSLand = stringToUpper( inUSGSLand );
       inUSGSLand = trim( inUSGSLand );

       //printf( "Getting indicator to include USGS NLCD imperviousness data.\n");
       inUSGSImpe = string( getEnviVariable("INCLUDE_USGS_IMPERVIOUSNESS") );   
       inUSGSImpe = stringToUpper ( inUSGSImpe );
       inUSGSImpe = trim( inUSGSImpe );

       //printf( "Getting indicator to include USGS NLCD canopy data.\n");
       inUSGSCano = string( getEnviVariable("INCLUDE_USGS_CANOPY") );
       inUSGSCano = stringToUpper( inUSGSCano );
       inUSGSCano = trim( inUSGSCano );

       //printf( "Getting indicator to include NOAA coastal NLCD landuse data.\n");
       inNOAALand = string( getEnviVariable("INCLUDE_NOAA_LANDUSE") );
       inNOAALand = stringToUpper( inNOAALand );
       inNOAALand = trim( inNOAALand );

       //printf( "Getting indicator to include NASA MODIS IGBP landuse data.\n");
       inNASALand = string( getEnviVariable("INCLUDE_MODIS") );
       inNASALand = stringToUpper( inNASALand );
       inNASALand = trim( inNASALand );
       if ( inNASALand.compare("YES") == 0 )
       {
          //printf( "Getting projected and clipped MODIS image for the domain.\n");
          modisFile  = string( getEnviVariable("OUTPUT_MODISFILE") );
          modisFile =  trim( modisFile );
       }

       //printf( "Getting output text file name.\n"); 
       outTextFile = string( getEnviVariable("OUTPUT_LANDUSE_TEXT_FILE") );

       //printf( "Getting output NetCDF file name.\n");
       outNetcdfFile = string( getEnviVariable("OUTPUT_LANDUSE_NETCDF_FILE") );
    }
    else
    {
        printf( "\nError in  arguments.\n");
        printf( "  Usage: computeGridLandUse.exe\n");
        exit( 1 );
    } 

    //checking obtained environmental variables 
    
    //printf("Modeling domain grid size:  Columns = %d    Rows=%d\n",gridCols, gridRows);
    if (gridCols <1 || gridRows <1)
    {
       printf("  Error: grid rows and columns have to great than 1.\n");
       exit( 1 );
    }

    printf("Rasterized 30m grid domain file:  %s\n",gridRasterFile.c_str());
    //use .xml to check to check due to .bil does not work properly
    tmp_str = string(gridRasterFile);
    tmp_str.append(".aux.xml");
    FileExists(tmp_str.c_str(), 0 );  //the file has to exist.

    printf("Processed NLCD image file list:  %s\n",dataFileList.c_str());
    FileExists(dataFileList.c_str(), 0 );  //the file has to exist.

    printf( "Indicator to include USGS NLCD landuse data is: %s\n",inUSGSLand.c_str() ); 
    if ( inUSGSLand.compare("YES") != 0 && inUSGSLand.compare("NO") != 0)
    {
       printf( "  Error: Indicator to include USGS NLCD landuse data is not YES or NO.\n" );
       exit ( 1 );
    }         

    printf( "Indicator to include USGS NLCD imperviousness data is: %s\n",inUSGSImpe.c_str() );
    if ( inUSGSImpe.compare("YES") != 0 && inUSGSImpe.compare("NO") != 0)
    {
       printf( "  Error: Indicator to include USGS NLCD imperviousness data is not YES or NO.\n" );
       exit ( 1 );
    }   
  
    printf( "Indicator to include USGS NLCD canopy data is: %s\n",inUSGSCano.c_str() ); 
    if ( inUSGSCano.compare("YES") != 0 && inUSGSCano.compare("NO") != 0 )
    {
       printf( "  Error:  Indicator to include USGS NLCD canopy data is not YES or NO.\n" );
       exit ( 1 );
    }   
   
    printf( "Indicator to include NOAA coastal NLCD landuse data is: %s\n",inNOAALand.c_str() );
    if ( inNOAALand.compare("YES") != 0 && inNOAALand.compare("NO") != 0 )
    {
       printf( "  Error: Indicator to include NOAA coastal NLCD landuse data is not YES or NO.\n" );
       exit ( 1 );
    }   

    printf( "Indicator to include NASA MODIS IGBP landuse data is: %s\n",inNASALand.c_str() );
    if ( inNASALand.compare("YES") != 0 && inNASALand.compare("NO") != 0 )
    {
       printf( "  Error: Indicator to include NASA MODIS IGBP landuse data is not YES or NO.\n" );
       exit ( 1 );
    }   

    if ( inNASALand.compare("YES") == 0 )
    {
        printf( "Projected and clipped MODIS image for the domain: %s\n",modisFile.c_str() );
        FileExists(modisFile.c_str(), 0 );  //the file has to exist.
    }  

    printf( "Output text file name: %s\n",outTextFile.c_str() );
    FileExists(outTextFile.c_str(), 3 );  //the file has to be new.

    printf( "Output NetCDF file name: %s\n",outNetcdfFile.c_str() );
    FileExists(outNetcdfFile.c_str(), 3 );  //the file has to be new.

   
/* -------------------------------------------------------------------- */
/*     read in each NLCD images to get image vector for each data set   */
/* -------------------------------------------------------------------- */

    // read in image files to store them in vectors
    i = 0;
    imageStream.open( dataFileList.c_str() );
    if (imageStream.good() )
    {
       getline( imageStream, fileName);
       while ( !imageStream.eof() )
       {
          fileName = trim (fileName);  //get rid of spaces at edges
          i++;  //count the line number

          //get rid of empty line
          if ( fileName.size() == 0 )
          {
             goto newloop; 
          }

          //get the type of files
          if ( fileName.find(landType) !=string::npos || fileName.find(impeType) !=string::npos 
               || fileName.find(canoType) !=string::npos || fileName.find(cgapType) !=string::npos )

          {
            fileType = fileName;
            printf( "\nline %d: %s\n",i,fileType.c_str() );
          }
          else
          {
             //put images into different vectors based on file type
             printf( "line %d: %s\n",i,fileName.c_str() );
             FileExists( fileName.c_str(), 0 );     //the file has to exist.
 
             if ( fileType.find(landType) !=string::npos )      
             {
                landFiles.push_back(fileName); 
             }
      
             if ( fileType.find(impeType) !=string::npos )
             {
                impeFiles.push_back(fileName);
             }

             if ( fileType.find(canoType) !=string::npos )
             {
                canoFiles.push_back(fileName);
             }

             if ( fileType.find(cgapType) !=string::npos )
             {
                cgapFiles.push_back(fileName);
             }
          }

          newloop: 
          getline( imageStream, fileName);
       }  // while loop

       imageStream.close(); 
    } // if good 


/* -------------------------------------------------------------------- */
/*     Open rasterized 30m grid domain file  and obtain image info      */
/* -------------------------------------------------------------------- */
    //open the grid domain image 
    poGrid = (GDALDataset *) GDALOpen( gridRasterFile.c_str(), GA_Update );
    if( poGrid == NULL )
    {
       printf( "   Error: Open raster file failed: %s.\n", gridRasterFile.c_str() );
       exit( 1 ); 
    }
    poGridBand = poGrid->GetRasterBand( 1 );  // band 1

    // get rows and columns of the image
    xCells_grd = poGrid->GetRasterXSize();
    yCells_grd = poGrid->GetRasterYSize();
    printf( "Grid domain cell size is: %dx%dx%d\n", xCells_grd, yCells_grd, poGrid->GetRasterCount() );
      
    //get UL corner coordinates and grid size for the domain grid
    if( poGrid->GetGeoTransform( adfGeoTransform ) == CE_None )
    {  
         xUL = adfGeoTransform[0];
         yUL = adfGeoTransform[3];
         printf( "Doamin UL Origin = (%.6lf,%.6lf)\n", xUL,yUL );
         
         xCellSize_grd =  adfGeoTransform[1] ;
         yCellSize_grd =  -1 * adfGeoTransform[5] ;  // Make it positive.  Negative due to Origin is UL corner point
         printf( "Grid domain pixel zize = (%.3lf,%.3lf)\n", xCellSize_grd, yCellSize_grd );
         
         //check if UL corner coordinates are the standard NLCD grid corner
         if ( fabs( modf(xUL/xCellSize_grd,&intpart) )!= 0.5 || fabs (modf(yUL/yCellSize_grd,&intpart) ) != 0.5 )
         {  
            printf ( "  Error: UL corner not have NLCD corner coordinates: modf(fabs(xUL/30))=0.5 for: %s\n",gridRasterFile.c_str() );
            printf ( "         Regrid using gdal_translate utility.\n" );
            exit ( 1 );
         }
      
    }
      
    //compute extent of band 1 domain grid image (only one band for all images)
    xMin_grd = xUL;
    xMax_grd = xUL + xCellSize_grd * xCells_grd;
      
    yMax_grd = yUL;
    yMin_grd = yUL - yCellSize_grd * yCells_grd;
    printf( "domain grid raster extent:  minXY(%.3lf,%.3lf)   maxXY(%.3lf,%.3lf)\n", xMin_grd,yMin_grd,xMax_grd,yMax_grd );

    //get projection from the domain rater image
    if( (pszWKT = poGrid->GetProjectionRef())  == NULL )
    {
        printf( "  Error: Projection is not defined in the domain grid raster file: %s.\n", gridRasterFile.c_str() );
        printf( "         Can define it using gdal_translate utility.\n");
        exit( 1 );
    }

    //convert it to no const char
    pszWKT_nc =strdup ( pszWKT );
    oSRS_grd.importFromWkt( &pszWKT_nc );
    oSRS_grd.exportToProj4( &pszProj4_grd );
    printf ( "Proj4 for the domain raster image is: %s\n", pszProj4_grd);


/* -------------------------------------------------------------------- */
/*  Fill landuse class hash tables for index handling                   */
/* -------------------------------------------------------------------- */
    fillLandClassHashTables();

/* -------------------------------------------------------------------- */
/*     Allocate memory store 30m pixel number in each modeling grid           */
/* -------------------------------------------------------------------- */
    gridPixels = gridCols*gridRows;
    gridIDS = (int *) CPLCalloc(sizeof(int),gridPixels);
    countGridIDs();

/* -------------------------------------------------------------------- */ 
/*     compute USGS NLCD urban imperviousness percentage                */
/* -------------------------------------------------------------------- */ 
   if ( inUSGSImpe.compare("YES") == 0 )
   {
      printf( "\n\nUSGS NLCD Urban Imperviousness\n");
      gridIMPE = (double *) CPLCalloc(sizeof(double),gridPixels);
      computeImpe_Cano( impeFiles, impeType );
   }

/* -------------------------------------------------------------------- */
/*     compute USGS  NLCD canopy percentage                             */
/* -------------------------------------------------------------------- */
  if ( inUSGSCano.compare("YES") == 0 )
   {
      printf( "\n\nUSGS NLCD Tree Canopy\n");
      gridCANO = (double *) CPLCalloc(sizeof(double),gridPixels);
      computeImpe_Cano( canoFiles, canoType );
   }

/* -------------------------------------------------------------------- */
/*     compute NOAA costal NLCD landuse percentage                      */
/* -------------------------------------------------------------------- */
   //allocate memory to store NLCD info for a grid
   if ( inNOAALand.compare("YES") == 0 || inUSGSLand.compare("YES") == 0 )
   {
       gridNLCD = (int *) CPLCalloc(sizeof(int),gridPixels * NLCD_CLASSES_NUM);
   }
 
   if ( inNOAALand.compare("YES") == 0 )
   {
      printf( "\n\nNOAA coastal NLCD Landuse\n");
      computeLandUse( cgapFiles, cgapType );
      poGrid->FlushCache();   //write modified domain grid image
   }

/* -------------------------------------------------------------------- */
/*     compute USGS NLCD landuse percentage                             */
/* -------------------------------------------------------------------- */
   if ( inUSGSLand.compare("YES") == 0 )
   {
      printf( "\n\nUSGS NLCD Landuse\n");
      computeLandUse( landFiles, landType );
   }

/* -------------------------------------------------------------------- */
/*     compute NASA MODIS IGBP landuse percentage                       */
/* -------------------------------------------------------------------- */
   if ( inNASALand.compare("YES") == 0 )
   {
      gridMODIS = (int *) CPLCalloc(sizeof(int), gridPixels * MODIS_CLASSES_NUM);
      printf( "\n\nNASA MODIS IGBP Landuse\n");
      computeMODIS( modisFile );
   }


/* -------------------------------------------------------------------- */
/*     Output domain grid land information to text and netcdf files     */
/* -------------------------------------------------------------------- */
   printf("Open output text file to store landuse information:  %s\n",outTextFile.c_str());
   outTxtStream.open( outTextFile.c_str() );
   if (! outTxtStream.good() )
   {
       printf( "\tError in opening output file: %s\n",outTextFile.c_str() );
       exit ( 1 );
   }
  
   //---------------- write title -------------------------
   //---------------- allocate arrays for netcdf output ---
   char  temp_char[100];
   lineStr = string ("GRIDID,ROW,COL");
   if ( inUSGSImpe.compare("YES") == 0 )
   {
       lineStr.append( ",IMPERV" );
       if ( (imperv = (float *) calloc (times*rows*cols, sizeof(float)) ) == NULL)
       {
          printf( "Calloc imperv failed.\n");
          exit ( 1 );
       }
   }

   if ( inUSGSCano.compare("YES") == 0 )
   {
       lineStr.append( ",CANOPY" );
       if ( (canopy = (float *) calloc (times*rows*cols, sizeof(float)) ) == NULL)
       {
          printf( "Calloc canopy failed.\n");
          exit ( 1 );
       }
   }

   //get number of landuse classes
   if ( inNOAALand.compare("YES") == 0 ||  inUSGSLand.compare("YES") == 0 )
   {
      land_cat_len = NLCD_CLASSES_NUM - 1;  //number of landuse classes

   }
   if ( inNASALand.compare("YES") == 0 )
   {
      land_cat_len += MODIS_CLASSES_NUM - 1;  //number of landuse classes
   }
   printf( "Number of landuse classes: %d\n",land_cat_len);

   if (land_cat_len > 0 )
   {
      //allocate mem to landuse class list
      if ( (lu_list = (int*) calloc (times*land_cat_len, sizeof(int)) ) == NULL)
      {
         printf( "Calloc lu_list failed.\n");
         exit ( 1 );
      }

      //allocate mem to landuse fraction array
      if ( (luf = (float*) calloc (times*land_cat_len*rows*cols, sizeof(float)) ) == NULL)
      {
          printf( "Calloc luf failed.\n");
          exit ( 1 );
      }

      //allocate mem to landuse mask array
      if ( (lum = (float*) calloc (times*rows*cols, sizeof(float)) ) == NULL)
      {
          printf( "Calloc lum failed.\n");
          exit ( 1 );
      }

      //allocate mem to landuse dominant class array
      if ( (lud = (float*) calloc (times*rows*cols, sizeof(float)) ) == NULL)
      {
          printf( "Calloc lud failed.\n");
          exit ( 1 );
      }
   }

   k = 0; //landuse class index
   if ( inNOAALand.compare("YES") == 0 ||  inUSGSLand.compare("YES") == 0 )
   {
       //not output last element -- NODATA
       for (j=0; j< NLCD_CLASSES_NUM - 1; j++)
       {
           sprintf( temp_char,",1%d",nlcdClasses[j]);        
           lineStr.append( temp_char );
           tmp_str = string (temp_char);
           tmp_str.erase(0,1);
           lu_list[k] = atoi ( tmp_str.c_str() );
           k++;
       }
   }

   if ( inNASALand.compare("YES") == 0 )
   {
      //not output last element -- NODATA
      for (j=0; j< MODIS_CLASSES_NUM - 1; j++)
      {
          sprintf( temp_char,",2%d",modisClasses[j]);
          lineStr.append( temp_char );
          tmp_str = string (temp_char);
          tmp_str.erase(0,1);
          lu_list[k] = atoi ( tmp_str.c_str() );
          k++;
      }
   }
   lineStr.append( "\n" );
   outTxtStream.write( lineStr.c_str(), lineStr.size() );
   

   //-----------output grid level landuse information  ----------------
   //-----------write data to arrays for netcdf output ----------------
   int          gridID, col=0, row=0;
   double       waterP;   //total water class
   double       maxP;     //maximum percent class
   int          maxIndex;

   for (i=0; i<gridPixels; i++)
   {
      gridID = i+1;
      col= (gridID -1) % gridCols + 1;
      row = (gridID - col) / gridCols + 1;
      gridArea = gridIDS[i] * xCellSize_grd * yCellSize_grd;     //grid area in m2
   
      //printf ("gridID=%d  row=%d  col=%d\n",gridID,row,col);

      sprintf( temp_char,"%d,%d,%d",gridID,row,col);
      lineStr = string( temp_char );
   
      if ( inUSGSImpe.compare("YES") == 0 )
      {   
         sprintf( temp_char,",%.5lf",100.0*gridIMPE[i]/gridArea);   //area percent
         lineStr.append( temp_char );
         k = (row - 1) * cols + col - 1 ;  //index in 1d array
         imperv[k] = 100.0*gridIMPE[i]/gridArea;

         //printf ("IMPE=%lf\n",imperv[k]);
      }

      if ( inUSGSCano.compare("YES") == 0 )
      {
         sprintf( temp_char,",%.5lf",100.0*gridCANO[i]/gridArea);
         lineStr.append( temp_char );
         k = (row - 1) * cols + col - 1 ;  //index in 1d array
         canopy[k] = 100.0*gridCANO[i]/gridArea;

         //printf ("CANOP=%lf\n",canopy[k]);
      }

      int kk = 0;  //number of landuse classes
      waterP = 0.0;    //initlize it
      maxP  = -100.0;  //initialize it
      if ( inNOAALand.compare("YES") == 0 ||  inUSGSLand.compare("YES") == 0 )
      {
          //not output last element -- NODATA
          for (j=0; j< NLCD_CLASSES_NUM - 1; j++) 
          {
             percent = 100.0 * gridNLCD[j*gridPixels+i]/gridIDS[i];
             sprintf( temp_char,",%.5lf",percent);   //class counts percent
             lineStr.append( temp_char );
             k = kk * rows * cols + (row - 1) * cols + col - 1;
             luf[k] = percent; 
             if ( lu_list[kk] == 111 )
             {
                //get total water percent
                waterP += percent;
             }
             if (percent > maxP)
             {
                //get dominant class
                maxP = percent;
                maxIndex = kk;  //lu_list index with max percent
             }
             //else
             //{
             //   printf ("j=%d nlcdPercent = %lf\n",j, percent);
             //}

             kk++;
          }
      }

      if ( inNASALand.compare("YES") == 0 )
      {
         //not output last element -- NODATA
          for (j=0; j< MODIS_CLASSES_NUM - 1; j++)
          {
             percent = 100.0 * gridMODIS[j*gridPixels+i]/gridIDS[i];
             sprintf( temp_char,",%.5lf",percent);   //class counts percent
             lineStr.append( temp_char );
             k = kk * rows * cols + (row - 1) * cols + col - 1;
             luf[k] = percent;
             if ( lu_list[kk] == 20 || lu_list[kk] == 217 || lu_list[kk] == 2255 )
             {
                //get total water percent
                waterP += percent;
             }
             if (percent > maxP)
             {
                //get dominant class
                maxP = percent;
                maxIndex = kk;  //lu_list index with max percent
             }
             //else
             //{
             //   printf ("j=%d modisPercent = %lf\n",j, percent);
             //}

             kk++;
          }
      }

      
      if (land_cat_len > 0 )
      {
         k = (row - 1) * cols + col - 1 ;  //index in array
         //land = 1 and water = 0  mask
         if (waterP > 50.00)
         {
            lum[k] = 0.0;
         }
         else
         { 
            lum[k] = 1.0;
         }

         //printf ("i=%d  maxIndex = %d \n",i,maxIndex);
         lud[k] = lu_list[maxIndex];   //get class number with maximum percent
      }

      lineStr.append( "\n" );

      outTxtStream.write( lineStr.c_str(), lineStr.size() ); 
   } //end of i
   
   //close text file
   outTxtStream.close ();

   //checking
   //printf ( " r1 c7 IMP=%f  CAN=%lf  LUF=%lf  LUM=%d  LUD=%d\n",imperv[6], canopy[6],luf[2456506],lum[6],lud[6]);
   //printf ( " r1 c7 IMP=%f  CAN=%lf  LUF=%lf  LUM=%d  LUD=%d\n",imperv[34999], canopy[34999],luf[179499],lum[34999],lud[34999]);
   //printf ( " r1 c7 IMP=%f  CAN=%lf  LUF=%lf  LUM=%d  LUD=%d\n",imperv[72249], canopy[72249],luf[2962249],lum[72249],lud[72249]);
  
   /**********************************************
   * free all hashtables for landuse computation *
   * delete used files   *                       *
   **********************************************/
   if ( inUSGSImpe.compare("YES") == 0 )
   {
     CPLFree (gridIMPE);
   }

   if ( inUSGSCano.compare("YES") == 0 )
   {   
      CPLFree (gridCANO);
   }

   if ( inNOAALand.compare("YES") == 0 ||  inUSGSLand.compare("YES") == 0 )
   {
        CPLFree (gridNLCD);
   }

   if ( inNASALand.compare("YES") == 0 )
   {
        CPLFree (gridMODIS);
   }

   CPLFree (gridIDS);

   //delete 30m domain grid image after processing
   poDrive = poGrid->GetDriver();
   GDALClose( (GDALDatasetH) poGrid );
 
   if ( poDrive->Delete( gridRasterFile.c_str() )  == CE_Failure )
   {
      printf( "Deleting the file failed: %s\n", gridRasterFile.c_str() );
      exit( 1 );
   }
   printf( "\tDeleted the modified file: %s\n", gridRasterFile.c_str() );

   /***************************************************************
   * Prepare data to write WRF landuse data file in netCDF format *
   ***************************************************************/
   //currently the program only suppots LCC WRF netcdf write out
   if (proj != LCC)
   {
       printf("\tWRF supports four projection types: LATLONG, LCC, UPS, and MERC.\n");
       printf("\tBut, this program currently only supports LCC projection in WRF NetCDF output format.\n");
       printf("\tWarning: the program ends without WRF NetCDF output file.\n");
       exit ( 1 );
   }


    /*****************************************************
    * Compute center and domain corner lat and long data *
    *****************************************************/
    string proj4Str = string(pszProj4);
    proj4DF = pj_init_plus (proj4Str.c_str() );
    if (proj4DF == NULL)
    {
        printf( "Initializing Proj4 projection string failed: .\n",pszProj4 );
        exit ( 1 );
    }

    //get the maximum x and y for the domain
    xmax = x0 + dx*cols;
    ymax = y0 + dy*rows;

    if ( pj_is_latlong(proj4DF) )
    {
       printf( "The grid domain is defined in the lat and long coordinates.\n" );
       cLon = x0 + (xmax - x0) / 2.0;
       cLat = y0 + (ymax - y0) / 2.0;

       //SW corner point
       swLon = x0 + dx/2.0;
       swLat = y0 + dy/2.0;

       //NW corner point
       nwLon = x0 + dx/2.0;
       nwLat = ymax - dy/2.0;

       //NE corner point
       neLon = xmax - dx/2.0;
       neLat = ymax - dy/2.0;

       //SE corner point
       seLon = xmax - dx/2.0;
       seLat = y0 + dy/2.0;
    }
    else
    {
       //get center point lat and long of WRF mass grid (cross)
       xx = x0 + (xmax - x0) / 2.0;
       yy = y0 + (ymax - y0) / 2.0;

       latlongP = computeLatLong(proj4DF,xx,yy);
       cLon = latlongP.u;
       cLat = latlongP.v;
       printf ( "Center point: xx = %lf   yy = %lf  cLon = %lf  cLat = %lf\n",xx,yy,cLon,cLat);

       //SW corner point
       xx = x0 + dx/2.0;
       yy = y0 + dy/2.0;
       latlongP = computeLatLong(proj4DF,xx,yy);
       swLon = latlongP.u;
       swLat = latlongP.v;
       printf ( "SW point: xx = %lf   yy = %lf  swLon = %lf  swLat = %lf\n",xx,yy,swLon,swLat);

       //NW corner point
       xx = x0 + dx/2.0;
       yy = ymax - dy/2.0;
       latlongP = computeLatLong(proj4DF,xx,yy);
       nwLon = latlongP.u;
       nwLat = latlongP.v;
       printf ( "NW point: xx = %lf   yy = %lf  nwLon = %lf  nwLat = %lf\n",xx,yy,nwLon,nwLat);

       //NE corner point
       xx = xmax - dx/2.0;
       yy = ymax - dy/2.0;
       latlongP = computeLatLong(proj4DF,xx,yy);
       neLon = latlongP.u;
       neLat = latlongP.v;
       printf ( "NE point: xx = %lf   yy = %lf  neLon = %lf  neLat = %lf\n",xx,yy,neLon,neLat);

       //SE corner point
       xx = xmax - dx/2.0;
       yy = y0 + dy/2.0;
       latlongP = computeLatLong(proj4DF,xx,yy);
       seLon = latlongP.u;
       seLat = latlongP.v;
       printf ( "SE point: xx = %lf   yy = %lf  seLon = %lf  seLat = %lf\n",xx,yy,seLon,seLat);
    }
   
   /*****************************************************
   * Create grid mass location arrays for netcdf output *
   ******************************************************/ 
   //x and y array 2d
   if ( (x = (float*) calloc (times*cols, sizeof(float)) ) == NULL)
   {
      printf( "Calloc x failed.\n");
      exit ( 1 );
   }
   if ( (y = (float*) calloc (times*rows, sizeof(float)) ) == NULL)
   {
      printf( "Calloc y failed.\n");
      exit ( 1 );
   }

   //mass x
   for (i=0; i<cols; i++)
   {
      x[i] = x0 + dx*i + dx/2.0;
   }
   //mass y
   for (j=0; j<rows; j++)
   {
      y[j] = y0 + dy*j + dy/2.0;
   }

   ///lat and long array 3d
   if ( (lat = (float*) calloc (times*rows*cols, sizeof(float)) ) == NULL)
   {
      printf( "Calloc lat failed.\n");
      exit ( 1 );
   }
   if ( (lon = (float*) calloc (times*rows*cols, sizeof(float)) ) == NULL)
   {
      printf( "Calloc lon failed.\n");
      exit ( 1 );
   }

   for(j=0; j<rows; j++)
   {
      yy = y0 + dy*j + dy/2.0;
      for (i=0; i<cols; i++)
      {
         xx = x0 + dx*i + dx/2.0;
         latlongP = computeLatLong(proj4DF,xx,yy);
         lon[j*cols+i] = latlongP.u;
         lat[j*cols+i] = latlongP.v;
      }
   }

   pj_free ( proj4DF );

   /***********************************************
   * Write WRF landuse data file in netCDF format *
   ***********************************************/

   /********************
   * Dimension lengths *
   ********************/
   size_t west_east_len   = cols;
   size_t south_north_len = rows;
   size_t time_len = TIMES;
   size_t dateStr_len = 19;
   /********************
   * Enter define mode *
   ********************/
   int ncid;
   anyErrors( nc_create(outNetcdfFile.c_str(), NC_CLOBBER, &ncid) );

   /********************
   * Define dimensions *
   ********************/
   int dateStr_dim;
   int time_dim;
   int west_east_dim;
   int south_north_dim;
   int land_cat_dim;

   printf( "Defining dimensions in output netcdf file...\n" );

   anyErrors( nc_def_dim(ncid, "Time", NC_UNLIMITED, &time_dim) );
   anyErrors( nc_def_dim(ncid, "DateStrLen", dateStr_len, &dateStr_dim) );
   anyErrors( nc_def_dim(ncid, "west_east", west_east_len, &west_east_dim) );
   anyErrors( nc_def_dim(ncid, "south_north", south_north_len, &south_north_dim) );
   anyErrors( nc_def_dim(ncid, "land_cat", land_cat_len, &land_cat_dim) );


   /*******************
   * Define variables *
   *******************/
   int times_id;
   int x_id;
   int y_id;
   int lon_id;
   int lat_id;
   int lu_list_id;
   int imperv_id;
   int canopy_id;
   int lum_id;
   int luf_id;
   int lud_id;

   int dims2_x[2],dims2_y[2],dims3[3], dims2_lu[2],dims4[4];  //dimention definition

   printf( "Defining variables in output netcdf file...\n" );

   dims2_x[0] = time_dim;
   dims2_x[1] = dateStr_dim;
   anyErrors( nc_def_var(ncid, "Times", NC_CHAR, 2, dims2_x, &times_id) );

   dims2_x[0] = time_dim;
   dims2_x[1] = west_east_dim;
   anyErrors( nc_def_var(ncid, "X_M", NC_FLOAT, 2, dims2_x, &x_id) );

   dims2_y[0] = time_dim;
   dims2_y[1] = south_north_dim;
   anyErrors( nc_def_var(ncid, "Y_M", NC_FLOAT, 2, dims2_y, &y_id) );

   dims3[0] = time_dim;
   dims3[1] = south_north_dim;
   dims3[2] = west_east_dim;
   anyErrors( nc_def_var(ncid, "XLONG_M", NC_FLOAT, 3, dims3, &lon_id) );
   anyErrors( nc_def_var(ncid, "XLAT_M", NC_FLOAT, 3, dims3, &lat_id) );

   if ( inUSGSImpe.compare("YES") == 0 )
   {
      anyErrors( nc_def_var(ncid, "IMPERV", NC_FLOAT, 3, dims3, &imperv_id) );
   }

   if ( inUSGSCano.compare("YES") == 0 )
   {
      anyErrors( nc_def_var(ncid, "CANOPY", NC_FLOAT, 3, dims3, &canopy_id) );
   }

   if (land_cat_len > 0 ) 
   {
      dims2_lu[0] = time_dim;
      dims2_lu[1] = land_cat_dim;
      anyErrors( nc_def_var(ncid, "LU_LIST", NC_INT, 2, dims2_lu, &lu_list_id) );

      dims4[0] = time_dim;
      dims4[1] = land_cat_dim;
      dims4[2] = south_north_dim;
      dims4[3] = west_east_dim;
      anyErrors( nc_def_var(ncid, "LANDUSEF", NC_FLOAT, 4, dims4, &luf_id) );
      anyErrors( nc_def_var(ncid, "LANDMASK", NC_FLOAT, 3, dims3, &lum_id) );
      anyErrors( nc_def_var(ncid, "LU_INDEX", NC_FLOAT, 3, dims3, &lud_id) );

   }

   /********************
   * Assign attributes *
   ********************/
   printf( "Assigning variable attributes in output netcdf file...\n" );
   int fieldtype[1];

   // x values
   fieldtype[0] = 104;
   anyErrors( nc_put_att_int(ncid, x_id, "FieldType", NC_INT, 1, fieldtype) );
   anyErrors( nc_put_att_text(ncid, x_id, "MemoryOrder", 2, "X ") );
   anyErrors( nc_put_att_text(ncid, x_id, "Description", 14, "X on mass grid") );
   anyErrors( nc_put_att_text(ncid, x_id, "Units", 1, "Meters") );
   anyErrors( nc_put_att_text(ncid, x_id, "stagger", 1, "M") );

   // y values
   anyErrors( nc_put_att_int(ncid, y_id, "FieldType", NC_INT, 1, fieldtype) );
   anyErrors( nc_put_att_text(ncid, y_id, "MemoryOorder", 2, "Y ") );
   anyErrors( nc_put_att_text(ncid, y_id, "Description", 14, "Y on mass grid") );
   anyErrors( nc_put_att_text(ncid, y_id, "Units", 1, "Meters") );
   anyErrors( nc_put_att_text(ncid, y_id, "stagger", 1, "M") );


   // Grid longitudes
   anyErrors( nc_put_att_int(ncid, lon_id, "FieldType", NC_INT, 1, fieldtype) );
   anyErrors( nc_put_att_text(ncid, lon_id, "MemoryOrder", 3, "XY ") );
   anyErrors( nc_put_att_text(ncid, lon_id, "Units", 17, "degrees longitude") );
   anyErrors( nc_put_att_text(ncid, lon_id, "Description", 22, "longitude on mass grid") );
   anyErrors( nc_put_att_text(ncid, lon_id, "Stagger", 1, "M") );

   // Grid latitudes
   anyErrors( nc_put_att_int(ncid, lat_id, "FieldType", NC_INT, 1, fieldtype) );
   anyErrors( nc_put_att_text(ncid, lat_id, "MemoryOrder", 3, "XY ") );
   anyErrors( nc_put_att_text(ncid, lat_id, "Units", 16, "degrees latitude") );
   anyErrors( nc_put_att_text(ncid, lat_id, "Description", 21, "latitude on mass grid") );
   anyErrors( nc_put_att_text(ncid, lat_id, "Stagger", 1, "M") );

   // Impervious surface coverage
   if ( inUSGSImpe.compare("YES") == 0 )
   {
      anyErrors( nc_put_att_int(ncid, imperv_id, "FieldType", NC_INT, 1, fieldtype) );
      anyErrors( nc_put_att_text(ncid, imperv_id, "MemoryOrder", 3, "XY ") );
      anyErrors( nc_put_att_text(ncid, imperv_id, "Description", 41, "percent of impervious surface in WRF grid") );
      anyErrors( nc_put_att_text(ncid, imperv_id, "Units", 7, "Percent") );
      anyErrors( nc_put_att_text(ncid, imperv_id, "Stagger", 1, "M") );
   }

   // Forest canopy
   if ( inUSGSCano.compare("YES") == 0 )
   {
      anyErrors( nc_put_att_int(ncid, canopy_id, "FieldType", NC_INT, 1, fieldtype) );
      anyErrors( nc_put_att_text(ncid, canopy_id, "MemoryOrder", 3, "XY ") );
      anyErrors( nc_put_att_text(ncid, canopy_id, "Description", 41, "percent of forest tree canopy in WRF grid") );
      anyErrors( nc_put_att_text(ncid, canopy_id, "Units", 7, "Percent") );
      anyErrors( nc_put_att_text(ncid, canopy_id, "stagger", 1, "M") );
   }
   
   
   // 2001 landuse
   if (land_cat_len > 0 ) 
   {
      // landuse classes list
      anyErrors( nc_put_att_int(ncid, lu_list_id, "FieldType", NC_INT, 1, fieldtype) );
      anyErrors( nc_put_att_text(ncid, lu_list_id, "MemoryOrder", 3, "0  ") );
      anyErrors( nc_put_att_text(ncid, lu_list_id, "Description", 42, "2001 USGS NLCD and MODIS IGBP class number") );
      anyErrors( nc_put_att_text(ncid, lu_list_id, "Units", 8, "Category") );
      anyErrors( nc_put_att_text(ncid, lu_list_id, "Stagger", 1, "") );

      // 2001 landuse fraction
      anyErrors( nc_put_att_int(ncid, luf_id, "FieldType", NC_INT, 1, fieldtype) );
      anyErrors( nc_put_att_text(ncid, luf_id, "MemoryOrder", 3, "XY ") );
      anyErrors( nc_put_att_text(ncid, luf_id, "Description", 31, "percent of land use in WRF grid") );
      anyErrors( nc_put_att_text(ncid, luf_id, "Units", 7, "Percent") );
      anyErrors( nc_put_att_text(ncid, luf_id, "stagger", 1, "M") );

      //land mask
      anyErrors( nc_put_att_int(ncid, lum_id, "FieldType", NC_INT, 1, fieldtype) );
      anyErrors( nc_put_att_text(ncid, lum_id, "MemoryOrder", 3, "XY ") );
      anyErrors( nc_put_att_text(ncid, lum_id, "Units", 17, "None") );
      anyErrors( nc_put_att_text(ncid, lum_id, "Description", 26, "Landmask : 1=land, 0=water") );
      anyErrors( nc_put_att_text(ncid, lum_id, "Stagger", 1, "M") );

      // dominant land class
      anyErrors( nc_put_att_int(ncid, lud_id, "FieldYype", NC_INT, 1, fieldtype) );
      anyErrors( nc_put_att_text(ncid, lud_id, "MemoryOrder", 3, "XY ") );
      anyErrors( nc_put_att_text(ncid, lud_id, "Units", 16, "Category") );
      anyErrors( nc_put_att_text(ncid, lud_id, "Description", 17, "Dominant category") );
      anyErrors( nc_put_att_text(ncid, lud_id, "Stagger", 1, "M") );
   }

   /**************************
   * Store global attributes *
   **************************/
   //global attributes
   int gatt[1];
   float gattf[1];
   float cdf_corner_lats[4];
   float cdf_corner_lons[4];

   printf( "Defining global attributes in output netcdf file...\n" );

   anyErrors( nc_put_att_text(ncid, NC_GLOBAL, "title", 23, "WRF GRID LANDUSE DATA FROM computeGridLandUse.exe") );
   anyErrors( nc_put_att_text(ncid, NC_GLOBAL, "SIMULATION_START_DATE", 19, "0000-00-00_00:00:00") );

   gatt[0] = cols+1;
   anyErrors( nc_put_att_int(ncid, NC_GLOBAL, "WEST-EAST_GRID_DIMENSION", NC_INT, 1, gatt) );
   gatt[0] = rows+1;
   anyErrors( nc_put_att_int(ncid, NC_GLOBAL, "SOUTH-NORTH_GRID_DIMENSION", NC_INT, 1, gatt) );
   gatt[0] = 0;
   anyErrors( nc_put_att_int(ncid, NC_GLOBAL, "BOTTOM-TOP_GRID_DIMENSION", NC_INT, 1, gatt) );
   anyErrors( nc_put_att_text(ncid, NC_GLOBAL, "GRIDTYPE", 1, "C") );

   gattf[0] = x0;
   anyErrors( nc_put_att_float(ncid, NC_GLOBAL, "XMIN", NC_FLOAT, 1, gattf) );
   gattf[0] = y0;
   anyErrors( nc_put_att_float(ncid, NC_GLOBAL, "YMIN", NC_FLOAT, 1, gattf) );
   gattf[0] = dx;
   anyErrors( nc_put_att_float(ncid, NC_GLOBAL, "DX", NC_FLOAT, 1, gattf) );
   gattf[0] = dy;
   anyErrors( nc_put_att_float(ncid, NC_GLOBAL, "DY", NC_FLOAT, 1, gattf) );

   gattf[0] = cLat;
   anyErrors( nc_put_att_float(ncid, NC_GLOBAL, "CEN_LAT", NC_FLOAT, 1, gattf) );
   gattf[0] = cLon;
   anyErrors( nc_put_att_float(ncid, NC_GLOBAL, "CEN_LON", NC_FLOAT, 1, gattf) );
   gattf[0] = trueLat1;
   anyErrors( nc_put_att_float(ncid, NC_GLOBAL, "TRUELAT1", NC_FLOAT, 1, gattf) );
   gattf[0] = trueLat2;
   anyErrors( nc_put_att_float(ncid, NC_GLOBAL, "TRUELAT2", NC_FLOAT, 1, gattf) );
   gattf[0] = cLat;
   anyErrors( nc_put_att_float(ncid, NC_GLOBAL, "MOAD_CEN_LAT", NC_FLOAT, 1, gattf) );
   gattf[0] = stdLon;
   anyErrors( nc_put_att_float(ncid, NC_GLOBAL, "STAND_LON", NC_FLOAT, 1, gattf) );
   gattf[0]= stdLat;
   anyErrors( nc_put_att_float(ncid, NC_GLOBAL, "STAND_LAT", NC_FLOAT, 1, gattf) );
   gattf[0] = poleLat;
   anyErrors( nc_put_att_float(ncid, NC_GLOBAL, "POLE_LAT", NC_FLOAT, 1, gattf) );
   gattf[0] = poleLon;
   anyErrors( nc_put_att_float(ncid, NC_GLOBAL, "POLE_LON", NC_FLOAT, 1, gattf) );

   cdf_corner_lats[0] = swLat;
   cdf_corner_lats[1] = nwLat;
   cdf_corner_lats[2] = neLat;
   cdf_corner_lats[3] = seLat;
   anyErrors( nc_put_att_float(ncid, NC_GLOBAL, "corner_lats", NC_FLOAT, 4, cdf_corner_lats) );
   cdf_corner_lons[0] = swLon;
   cdf_corner_lons[1] = nwLon;
   cdf_corner_lons[2] = neLon;
   cdf_corner_lons[3] = seLon;
   anyErrors( nc_put_att_float(ncid, NC_GLOBAL, "corner_lons", NC_FLOAT, 4, cdf_corner_lons) );

   gatt[0] = proj;
   anyErrors( nc_put_att_int(ncid, NC_GLOBAL, "MAP_PROJ", NC_INT, 1, gatt) );
   anyErrors( nc_put_att_text(ncid, NC_GLOBAL, "MMINLU", 4, "USGS") );

   /********************
   * Leave define mode *
   ********************/
   anyErrors( nc_enddef (ncid) );
        
        
   /*********************************
   * Write variables to netCDF file *
   *********************************/
   printf( "Writing variable data in output netcdf file...\n" );

   //     Store Times
   if ( (dateStr = (char *) calloc (1*dateStr_len, sizeof(char)) ) == NULL)
   {
       printf( "Calloc dateStr failed.\n");
       exit ( 1 );
   }
   long tx_start[2];
   long tx_count[2];
   tx_start[0]=0;
   tx_start[1]=0;
   tx_count[0]=time_len;
   tx_count[1]=dateStr_len;
   strcpy (dateStr,"0000-00-00_00:00:00");
   anyErrors( ncvarput(ncid, times_id, tx_start, tx_count, dateStr) );
   printf( "\tWrited Times\n" );
   free( dateStr );

   //      Store x
   anyErrors( nc_put_var_float(ncid, x_id, x) );
   printf( "Writed x\n" );
   free(x);

   //      Store y
   anyErrors( nc_put_var_float(ncid, y_id, y) );
   printf( "Writed y\n" );
   free(y);

   //      Store lon
   anyErrors( nc_put_var_float(ncid, lon_id, lon) );
   printf( "Writed lon\n" );
   free(lon);

   //      Store lat
   anyErrors( nc_put_var_float(ncid, lat_id, lat) );
   printf( "Writed lat\n" );
   free(lat);

   //      Store imperv
   if ( inUSGSImpe.compare("YES") == 0 )
   {
      anyErrors( nc_put_var_float(ncid, imperv_id, imperv) );
      printf( "Writed imperv\n" );
      free(imperv);
   }

   //      Store canopy
   if ( inUSGSCano.compare("YES") == 0 )
   {
      anyErrors( nc_put_var_float(ncid, canopy_id, canopy) );
      printf( "Writed canopy\n" );
      free(canopy);
   }

   //landuse
   if (land_cat_len > 0 )
   {
      //      Store lu_list
      anyErrors( nc_put_var_int(ncid, lu_list_id, lu_list) );
      printf( "Writed lu_list\n" );

      //   Store luf
      anyErrors( nc_put_var_float(ncid, luf_id, luf) );
      printf( "Writed luf\n" );

      //   Store lum
      anyErrors( nc_put_var_float(ncid, lum_id, lum) );
      printf( "Writed lum\n" );

      //      Store lud
      anyErrors( nc_put_var_float(ncid, lud_id, lud) );
      printf( "Writed lud\n" );

      free(lu_list);
      free(luf);
      free(lum);
      free(lud);
   }

   /********************
   * Close netCDF file *
   ********************/
   anyErrors( nc_close(ncid) );

   printf( "Finished writing output NetCDF file: %s\n\n", outNetcdfFile.c_str() );

   printf ("\n\nCompleted in computing and outputing all landuse data.\n");
}


/************************************************************************/
/*    countGridIDs()                                                    */
/************************************************************************/

void   countGridIDs(  )  
{

    long                  i,nPixelCount=0;
    long long             nPixelCount_grd = 0;
    int                   cCells, rCells;       //cells
    int                   gridID; 


     printf( "\nCount grid ID numbers in each domain grid cell...\n" );

           
/* -------------------------------------------------------------------- */
/*   Read one row at a time from top to down                            */
/* -------------------------------------------------------------------- */
     GUInt32 *poImage_grd = (GUInt32 *) CPLCalloc(sizeof(GUInt32),xCells_grd);
     int iLine;
     for( iLine = 0; iLine<yCells_grd; iLine++ )
     {
        if ( (poGridBand->RasterIO(GF_Read, 0, iLine,xCells_grd, 1,
                                   poImage_grd,xCells_grd,1,GDT_UInt32,0,0)) == CE_Failure)
        {
            printf( "\tError: Reading row = %d from domain grid image.\n",iLine+1);
            CPLFree (poImage_grd);
            exit( 1 );
        }

        for (i=0; i<xCells_grd; i++)
        {
           gridID = poImage_grd[i] ;
           if (gridID > 0 && gridID <= gridPixels)
           {
               gridIDS[gridID - 1] += 1;
           }
        }
     } 
     printf ( "Finished counting modeling domain grid ID numbers.\n\n" );

}  //end of countGridIDs


/************************************************************************/
/*    computeImpe_Cano(std::vector<string> imageFiles, string fileType) */
/************************************************************************/

void   computeImpe_Cano( std::vector<string> imageFiles, string fileType )  
{

    int                   i,j,k;
    int                   pixelIndex;
    string                fileName;             //file to be processed
    GDALDataset           *poRDataset;
    GDALRasterBand        *poBand;
    double                adfGeoTransform[6];
    double                xUL,yUL;
    int                   xCells, yCells;       //cells
    double                xCellSize, yCellSize; //cell size
    double                xmin,xmax,ymin,ymax;         //current image extent
    double                intpart; 

    const char            *pszWKT = NULL;
    char                  *pszWKT_nc = NULL;
    OGRSpatialReference   oSRS;
    char                  *pszProj4 = NULL;
   
    GByte                 NoDataValue = 127;                  //nodata value for i and j images
    int                   gridID; 


   printf( "\nCompute percentage of imperviousness or canopy in domain grid cells...\n" );

/* -------------------------------------------------------------------- */
/*      Process one image at a time for the image vector                */
/* -------------------------------------------------------------------- */
   //loop through all images in the vector
   for ( i = 0; i<imageFiles.size(); i++)
   {
      fileName = imageFiles.at(i);
      printf( "  %s\n",fileName.c_str() );
     
/* -------------------------------------------------------------------- */
/*      Open image i                                                    */
/* -------------------------------------------------------------------- */ 
      //open the current i image 
      poRDataset = (GDALDataset *) GDALOpen( fileName.c_str(), GA_ReadOnly );
      if( poRDataset == NULL )
      {
         printf( "  Error: Open raster file failed: %s.\n", fileName.c_str() );
         exit( 1 );
      }
/* -------------------------------------------------------------------- */
/*   get rows, columns, cell size, and image extent of image i in vector*/
/* -------------------------------------------------------------------- */
      // get rows and columns of the image
      xCells = poRDataset->GetRasterXSize();
      yCells = poRDataset->GetRasterYSize();
      printf( "  Image size is %dx%dx%d\n", xCells, yCells, poRDataset->GetRasterCount() );

      //get UL corner coordinates and grid size
      if( poRDataset->GetGeoTransform( adfGeoTransform ) == CE_None )
      {
         xUL = adfGeoTransform[0];
         yUL = adfGeoTransform[3];
         printf( "  Image UL Origin = (%.6lf,%.6lf)\n", xUL,yUL );

         xCellSize =  adfGeoTransform[1] ;
         yCellSize =  -1 * adfGeoTransform[5] ;  // Make it positive.  Negative due to Origin is UL corner point
         printf( "  Image Pixel Size = (%.3lf,%.3lf)\n", xCellSize, yCellSize );

         //check if UL corner coordinates are the standard NLCD grid corner
         if ( fabs( modf(xUL/xCellSize,&intpart) )!= 0.5 || fabs (modf(yUL/yCellSize,&intpart) ) != 0.5 )
         {
            printf ( "  Error: UL corner not have NLCD corner coordinates: modf(fabs(xUL/30))=0.5 for: %s\n",fileName.c_str() );
            printf ( "         Regrid using gdal_translate utility.\n" );
            exit ( 1 );
         }
      }

      //check consistent cell size
      if ( xCellSize_grd != xCellSize || yCellSize_grd != yCellSize )
      {
           printf( "  Error: This image's cell size is different from the domain grid image.\n" );
           printf( "         Resmaple it using gdal_translate utility.\n" );
           exit ( 1 );
      }
 
      //compute extent of band 1 image (only one band for all images)
      xmin = xUL;
      xmax = xUL + xCellSize * xCells;

      ymax = yUL;
      ymin = yUL - yCellSize * yCells;
      printf( "  Image extent:  minXY(%.3f,%.3f)   maxXY(%.3f,%.3f)\n", xmin,ymin,xmax,ymax ); 

/* -------------------------------------------------------------------- */
/*   Get image projection to make sure that all images have the same    */
/* -------------------------------------------------------------------- */
     //get projection from the image
     if( (pszWKT = poRDataset->GetProjectionRef())  == NULL )
     {
        printf( "  Error: Projection is not defined in the NLCD file: %s.\n", fileName.c_str() );
        printf( "         Define it using gdal_translate utility.\n");
        exit( 1 );
     }

     //convert it to no const char
     pszWKT_nc =strdup ( pszWKT ); 
     oSRS.importFromWkt( &pszWKT_nc );
     oSRS.exportToProj4( &pszProj4 );    
      
     //make sure that the image has the same projection as the first image's
     if (! oSRS.IsSame (&oSRS_grd) )
     {
         printf( "  Error: This image's projection is different from the domain grid image's.\n" );
         printf( "         Projection for this image = %s\n", pszProj4);
         printf( "         Project it using gdalwarp utility.\n" );
         exit ( 1 );
     }

/* -------------------------------------------------------------------- */
/*   Image data type has to be GByte                                    */
/* -------------------------------------------------------------------- */
     //get the band 1 image from the current image and make sure that it is GByte image
     poBand = poRDataset->GetRasterBand( 1 );  // band 1
     GDALDataType poBandType = poBand->GetRasterDataType();
     printf ("  Image Type = %d\n",poBandType);
     if ( poBandType !=GDT_Byte )
     {
         printf( "Error: Image data type is not GDT_Byte: %s.\n", fileName.c_str() );    
         exit ( 1 );
     }  

/* -------------------------------------------------------------------- */
/*   Compute overlapping box indexes in both image i and domain image   */
/* -------------------------------------------------------------------- */
     //check whether those two images are overlaying and calculate row and col in overlay areas from both images
     int row1 = 0, row2 = 0, col1=0, col2=0;
     int nXSize = 0 ,nYSize = 0;     //overlaypping array size for image i 

     int row1_grd = 0, row2_grd = 0, col1_grd=0, col2_grd=0;
     int nXSize_grd = 0 ,nYSize_grd = 0;     //overlaypping array size for domain gird image

     //if the two images intersecting
     if (! (xmin >= xMax_grd || xmax <= xMin_grd ||  ymin >= yMax_grd || ymax <= yMin_grd) )
     { 
         //Row and column start from LU corner
         //calculate intersection UL columns
         computeIntersectionBox(xCells,yCells,xmin,ymin,xmax,ymax,xMin_grd,yMin_grd,xMax_grd,yMax_grd,&col1,&row1,&col2,&row2,&col1_grd,&row1_grd,&col2_grd,&row2_grd);
         
         //compute x and y cells, min and max for QA checking in current image i
         printf ("\ti=%d image intesection: (col=%d row=%d)  (col=%d row=%d)\n",i,col1,row1,col2,row2);
         nXSize = col2 - col1 + 1;
         nYSize = row2 - row1 + 1; 
         printf ("\t\tnXSizei = %d  nYSize = %d\n",nXSize, nYSize);

         double oXmin = xmin + (col1 - 1) * xCellSize_grd;
         double oXmax = xmin + col2 * xCellSize_grd;
         double oYmax = ymax - (row1 - 1) * yCellSize_grd;
         double oYmin = ymax - row2 * yCellSize_grd;
         printf("\t\tOverlapping box: x: %lf - %lf    y: %lf - %lf\n",oXmin,oXmax,oYmin,oYmax); 

         //compute x and y cells and min and max for domain grid image
         printf ("\tdomain grid image intesection: (col=%d row=%d)  (col=%d row=%d)\n",col1_grd,row1_grd,col2_grd,row2_grd);
         nXSize_grd = col2_grd - col1_grd + 1;
         nYSize_grd = row2_grd - row1_grd + 1;
         printf ("\t\tnXSize = %d  nYSize = %d\n",nXSize_grd, nYSize_grd);
         //split grid domain intersection box into 4 blocks for reading because of 4 bytes data type
         int colm_grd = col1_grd + nXSize_grd / 2;
         int rowm_grd = row1_grd + nYSize_grd / 2;
         printf( "\t\tMiddle point in grid domain intersection box: col=%d   row=%d\n",colm_grd,rowm_grd);

         double oXmin_grd = xMin_grd + (col1_grd - 1) * xCellSize_grd;
         double oXmax_grd = xMin_grd + col2_grd * xCellSize_grd;
         double oYmax_grd = yMax_grd - (row1_grd - 1) * yCellSize_grd;
         double oYmin_grd = yMax_grd - row2_grd * yCellSize_grd;
         printf("\t\tOverlapping box: x: %lf - %lf    y: %lf - %lf\n",oXmin_grd,oXmax_grd,oYmin_grd,oYmax_grd);

         //make sure that overlapping box are the same in both image i and j
         if (nXSize != nXSize_grd || nYSize != nYSize_grd)
         {
            printf( "  Error: overlapping images x and y sizes are different for images i and domain grid image.\n" );
            exit ( 1 );
         }
         if (oXmin != oXmin_grd || oXmax != oXmax_grd || oYmax != oYmax_grd || oYmin != oYmin_grd)
         {
            printf( "  Error: overlapping box x and y are different for images i and domain grid image.\n" );
            exit ( 1 );
         }
 
/* -------------------------------------------------------------------- */
/*         get image in the intersecting box from image i               */
/* -------------------------------------------------------------------- */
        
         poBand = poRDataset->GetRasterBand( 1 );  // band 1
         GByte *poImage = (GByte *) CPLCalloc(sizeof(GByte),nXSize*nYSize);
         if ( (poBand->RasterIO(GF_Read, col1-1, row1-1, nXSize, nYSize,
               poImage,nXSize,nYSize,GDT_Byte,0,0)) == CE_Failure) 
         {
            printf( "  Error: reading band 1 data from image i: %s.\n", fileName.c_str() );
            CPLFree (poImage);
            exit( 1 );
         }
          
           
/* -------------------------------------------------------------------- */
/*     read one row at time from domain image due to the size           */
/* -------------------------------------------------------------------- */
        //domain grid image is too big to read once 
        GUInt32 *poImage_grd = (GUInt32 *) CPLCalloc(sizeof(GUInt32),nXSize_grd);

        //compute imperviousness percentage
        if ( fileType.find(impeType) !=string::npos )
        { 
          for (j=0; j<nYSize; j++)
          {
             if ( (poGridBand->RasterIO(GF_Read, col1_grd-1, row1_grd-1+j, nXSize_grd, 1,
                                        poImage_grd, nXSize_grd, 1, GDT_UInt32, 0, 0)) == CE_Failure)
             {
                printf( "\tError: reading line: row=%d  col=%d from domain grid image.\n",row1_grd+j,col1_grd);
                CPLFree (poImage_grd);
                exit( 1 );
             }
               
             for (k=0; k<nXSize; k++)
             {
                pixelIndex = nXSize * j + k;
                gridID = poImage_grd[k];
                if ( gridID > 0 && gridID <= gridPixels && poImage[pixelIndex] != NoDataValue)
                {
                   gridIMPE[gridID-1] += poImage[pixelIndex] * xCellSize_grd * yCellSize_grd / 100.00;  //compute imperviousness percent 
                }
             }  // end of k col 
          }  //end of j row
        }

        //compute canopy percentage
        if ( fileType.find(canoType) !=string::npos )
        {
          for (j=0; j<nYSize; j++)
          {
             if ( (poGridBand->RasterIO(GF_Read, col1_grd-1, row1_grd-1+j, nXSize_grd, 1,
                                        poImage_grd, nXSize_grd, 1, GDT_UInt32, 0, 0)) == CE_Failure)
             {
                printf( "\tError: reading line: row=%d  col=%d from domain grid image.\n",row1_grd+j,col1_grd);
                CPLFree (poImage_grd);
                exit( 1 );
             }

             for (k=0; k<nXSize; k++)
             {
                pixelIndex = nXSize * j + k;
                gridID = poImage_grd[k];
                if ( gridID > 0 && gridID <= gridPixels && poImage[pixelIndex] != NoDataValue)
                {
                   gridCANO[gridID-1] += poImage[pixelIndex] * xCellSize_grd * yCellSize_grd / 100.00;  //compute canopy percent
                }
             }  // end of k col
          }  //end of j row

        } 

        CPLFree (poImage);
        CPLFree (poImage_grd);
        GDALClose( (GDALDatasetH) poRDataset );

     }  //end of overlapping processing
     else
     {
         printf ("\ti=%d image does not intersect with domain grid image.\n",i);
     }

   }  //end of i

   printf ("Finished preprocessing images: %s\n",fileType.c_str());
}  //end of the function


/************************************************************************/
/*    computeLandUse(std::vector<string> imageFiles, string fileType) */
/************************************************************************/

void   computeLandUse( std::vector<string> imageFiles, string fileType )  
{

    int                   i,j,k,n,m;
    int                   pixelIndex;
    string                fileName;             //file to be processed
    GDALDataset           *poRDataset;
    GDALRasterBand        *poBand;
    double                adfGeoTransform[6];
    double                xUL,yUL;
    int                   xCells, yCells;       //cells
    double                xCellSize, yCellSize; //cell size
    double                xmin,xmax,ymin,ymax;         //current image extent
    double                intpart; 

    const char            *pszWKT = NULL;
    char                  *pszWKT_nc = NULL;
    OGRSpatialReference   oSRS;
    char                  *pszProj4 = NULL;
   
    GByte                 NoDataValue = 127;          //nodata value for i and j images
    int                   gridID, classID, idIndex; 
    int                   outIndex;


   printf( "\nCompute percentage of NLCD landuse classes in domain grid cells...\n" );

/* -------------------------------------------------------------------- */
/*      Set NODATA value for each USGS and NOAA NLCD                    */
/* -------------------------------------------------------------------- */
   //set no data value for each type images
   if ( fileType.find(cgapType) !=string::npos )
   {  
      //CGAP NLCD data have NODATA = 0 Unclassified = 1
      NoDataValue = 1;
   }
   else
   {  
      //USGS NLCD data have NODATA = 127
      NoDataValue = 127;
   }


/* -------------------------------------------------------------------- */
/*      Process one image at a time for the image vector                */
/* -------------------------------------------------------------------- */
   //loop through all images in the vector
   for ( i = 0; i<imageFiles.size(); i++)
   {
      fileName = imageFiles.at(i);
      printf( "  %s\n",fileName.c_str() );
     
/* -------------------------------------------------------------------- */
/*      Open image i                                                    */
/* -------------------------------------------------------------------- */ 
      //open the current i image 
      poRDataset = (GDALDataset *) GDALOpen( fileName.c_str(), GA_ReadOnly );
      if( poRDataset == NULL )
      {
         printf( "  Error: Open raster file failed: %s.\n", fileName.c_str() );
         exit( 1 );
      }
/* -------------------------------------------------------------------- */
/*   get rows, columns, cell size, and image extent of image i in vector*/
/* -------------------------------------------------------------------- */
      // get rows and columns of the image
      xCells = poRDataset->GetRasterXSize();
      yCells = poRDataset->GetRasterYSize();
      printf( "  Image size is %dx%dx%d\n", xCells, yCells, poRDataset->GetRasterCount() );

      //get UL corner coordinates and grid size
      if( poRDataset->GetGeoTransform( adfGeoTransform ) == CE_None )
      {
         xUL = adfGeoTransform[0];
         yUL = adfGeoTransform[3];
         printf( "  Image UL Origin = (%.6lf,%.6lf)\n", xUL,yUL );

         xCellSize =  adfGeoTransform[1] ;
         yCellSize =  -1 * adfGeoTransform[5] ;  // Make it positive.  Negative due to Origin is UL corner point
         printf( "  Image Pixel Size = (%.3lf,%.3lf)\n", xCellSize, yCellSize );

         //check if UL corner coordinates are the standard NLCD grid corner
         if ( fabs( modf(xUL/xCellSize,&intpart) )!= 0.5 || fabs (modf(yUL/yCellSize,&intpart) ) != 0.5 )
         {
            printf ( "  Error: UL corner not have NLCD corner coordinates: modf(fabs(xUL/30))=0.5 for: %s\n",fileName.c_str() );
            printf ( "         Regrid using gdal_translate utility.\n" );
            exit ( 1 );
         }
      }

      //check consistent cell size
      if ( xCellSize_grd != xCellSize || yCellSize_grd != yCellSize )
      {
           printf( "  Error: This image's cell size is different from the domain grid image.\n" );
           printf( "         Resmaple it using gdal_translate utility.\n" );
           exit ( 1 );
      }
 
      //compute extent of band 1 image (only one band for all images)
      xmin = xUL;
      xmax = xUL + xCellSize * xCells;

      ymax = yUL;
      ymin = yUL - yCellSize * yCells;
      printf( "  Image extent:  minXY(%.3f,%.3f)   maxXY(%.3f,%.3f)\n", xmin,ymin,xmax,ymax ); 

/* -------------------------------------------------------------------- */
/*   Get image projection to make sure that all images have the same    */
/* -------------------------------------------------------------------- */
     //get projection from the image
     if( (pszWKT = poRDataset->GetProjectionRef())  == NULL )
     {
        printf( "  Error: Projection is not defined in the NLCD file: %s.\n", fileName.c_str() );
        printf( "         Define it using gdal_translate utility.\n");
        exit( 1 );
     }

     //convert it to no const char
     pszWKT_nc =strdup ( pszWKT ); 
     oSRS.importFromWkt( &pszWKT_nc );
     oSRS.exportToProj4( &pszProj4 );    
      
     //make sure that the image has the same projection as the first image's
     if (! oSRS.IsSame (&oSRS_grd) )
     {
         printf( "  Error: This image's projection is different from the domain grid image's.\n" );
         printf( "         Projection for this image = %s\n", pszProj4);
         printf( "         Project it using gdalwarp utility.\n" );
         exit ( 1 );
     }

/* -------------------------------------------------------------------- */
/*   Image data type has to be GByte                                    */
/* -------------------------------------------------------------------- */
     //get the band 1 image from the current image and make sure that it is GByte image
     poBand = poRDataset->GetRasterBand( 1 );  // band 1
     GDALDataType poBandType = poBand->GetRasterDataType();
     printf ("  Image Type = %d\n",poBandType);
     if ( poBandType !=GDT_Byte )
     {
         printf( "Error: Image data type is not GDT_Byte: %s.\n", fileName.c_str() );    
         exit ( 1 );
     }  

/* -------------------------------------------------------------------- */
/*   Compute overlapping box indexes in both image i and domain image   */
/* -------------------------------------------------------------------- */
     //check whether those two images are overlaying and calculate row and col in overlay areas from both images
     int row1 = 0, row2 = 0, col1=0, col2=0;
     int nXSize = 0 ,nYSize = 0;     //overlaypping array size for image i 

     int row1_grd = 0, row2_grd = 0, col1_grd=0, col2_grd=0;
     int nXSize_grd = 0 ,nYSize_grd = 0;     //overlaypping array size for domain gird image

     //if the two images intersecting
     if (! (xmin >= xMax_grd || xmax <= xMin_grd ||  ymin >= yMax_grd || ymax <= yMin_grd) )
     { 
         //Row and column start from LU corner
         //calculate intersection UL columns
         computeIntersectionBox(xCells,yCells,xmin,ymin,xmax,ymax,xMin_grd,yMin_grd,xMax_grd,yMax_grd,&col1,&row1,&col2,&row2,&col1_grd,&row1_grd,&col2_grd,&row2_grd);
         
         //compute x and y cells, min and max for QA checking in current image i
         printf ("\ti=%d image intesection: (col=%d row=%d)  (col=%d row=%d)\n",i,col1,row1,col2,row2);
         nXSize = col2 - col1 + 1;
         nYSize = row2 - row1 + 1; 
         printf ("\t\tnXSizei = %d  nYSize = %d\n",nXSize, nYSize);

         double oXmin = xmin + (col1 - 1) * xCellSize_grd;
         double oXmax = xmin + col2 * xCellSize_grd;
         double oYmax = ymax - (row1 - 1) * yCellSize_grd;
         double oYmin = ymax - row2 * yCellSize_grd;
         printf("\t\tOverlapping box: x: %lf - %lf    y: %lf - %lf\n",oXmin,oXmax,oYmin,oYmax); 

         //compute x and y cells and min and max for domain grid image
         printf ("\tdomain grid image intesection: (col=%d row=%d)  (col=%d row=%d)\n",col1_grd,row1_grd,col2_grd,row2_grd);
         nXSize_grd = col2_grd - col1_grd + 1;
         nYSize_grd = row2_grd - row1_grd + 1;
         printf ("\t\tnXSize = %d  nYSize = %d\n",nXSize_grd, nYSize_grd);
         //split grid domain intersection box into 4 blocks for reading because of 4 bytes data type
         int colm_grd = col1_grd + nXSize_grd / 2;
         int rowm_grd = row1_grd + nYSize_grd / 2;
         printf( "\t\tMiddle point in grid domain intersection box: col=%d   row=%d\n",colm_grd,rowm_grd);

         double oXmin_grd = xMin_grd + (col1_grd - 1) * xCellSize_grd;
         double oXmax_grd = xMin_grd + col2_grd * xCellSize_grd;
         double oYmax_grd = yMax_grd - (row1_grd - 1) * yCellSize_grd;
         double oYmin_grd = yMax_grd - row2_grd * yCellSize_grd;
         printf("\t\tOverlapping box: x: %lf - %lf    y: %lf - %lf\n",oXmin_grd,oXmax_grd,oYmin_grd,oYmax_grd);

         //make sure that overlapping box are the same in both image i and j
         if (nXSize != nXSize_grd || nYSize != nYSize_grd)
         {
            printf( "  Error: overlapping images x and y sizes are different for images i and domain grid image.\n" );
            exit ( 1 );
         }
         if (oXmin != oXmin_grd || oXmax != oXmax_grd || oYmax != oYmax_grd || oYmin != oYmin_grd)
         {
            printf( "  Error: overlapping box x and y are different for images i and domain grid image.\n" );
            exit ( 1 );
         }
 
/* -------------------------------------------------------------------- */
/*         get image in the intersecting box from image i               */
/* -------------------------------------------------------------------- */
        
         poBand = poRDataset->GetRasterBand( 1 );  // band 1
         GByte *poImage = (GByte *) CPLCalloc(sizeof(GByte),nXSize*nYSize);
         if ( (poBand->RasterIO(GF_Read, col1-1,row1-1,nXSize,nYSize,
               poImage,nXSize,nYSize,GDT_Byte,0,0)) == CE_Failure) 
         {
            printf( "  Error: reading band 1 data from image i: %s.\n", fileName.c_str() );
            CPLFree (poImage);
            exit( 1 );
         }
          
/* -------------------------------------------------------------------- */
/*     read one row at time from domain image due to the size           */
/* -------------------------------------------------------------------- */
        //domain grid image is too big to read once 
        GUInt32 *poImage_grd = (GUInt32 *) CPLCalloc(sizeof(GUInt32),nXSize_grd);
             
        //count NOAA CGAP NLCD landuse classes
        if ( fileType.find(cgapType) !=string::npos )
        {       
          for (j=0; j<nYSize; j++)
          {
             if ( (poGridBand->RasterIO(GF_Read, col1_grd-1, row1_grd-1+j, nXSize_grd, 1,
                                        poImage_grd, nXSize_grd, 1, GDT_UInt32, 0, 0)) == CE_Failure)
             {  
                printf( "\tError: reading line: row=%d  col=%d from domain grid image.\n",row1_grd+j,col1_grd);
                CPLFree (poImage_grd);
                exit( 1 );
             }     
             int  writeIndicator = 0;   
             for (k=0; k<nXSize; k++)
             {
                pixelIndex = nXSize * j + k;
                gridID = poImage_grd[k];
                classID = poImage[pixelIndex];        
                if ( gridID > 0 && gridID <= gridPixels && classID > NoDataValue)
                {
                   idIndex = noaaIDS[classID];    //index in NOAA NLCD ID pointer 
                   outIndex = idIndex * gridPixels + gridID - 1;  //index in NLCD landuse pointer               
                   gridNLCD[outIndex] += 1;   // count NOAA landuse class in 2d array
                   
                   //set gridID to 0 in poImage_grd after reading NOAA NLCD data.  So, USGS NLCD will not count them again
                   poImage_grd[k] = 0;
                   writeIndicator = 1;
                }
             }  // end of k col
            
             //write changed doamin grid image line back to the image
             if ( writeIndicator == 1)
             {
                if ( (poGridBand->RasterIO(GF_Write, col1_grd-1, row1_grd-1+j, nXSize_grd, 1,
                                        poImage_grd, nXSize_grd, 1, GDT_UInt32, 0, 0)) == CE_Failure)
                {
                    printf( "\tError: writing line: row=%d  col=%d to domain grid image.\n",row1_grd+j,col1_grd);
                    CPLFree (poImage_grd);
                    exit( 1 );
                } 
             }
             
          }  //end of j row

        }   //NOAA NLCD files

        //count USGS NLCD landuse classes
        if ( fileType.find(landType) !=string::npos )
        {      
          for (j=0; j<nYSize; j++)
          {
             if ( (poGridBand->RasterIO(GF_Read, col1_grd-1, row1_grd-1+j, nXSize_grd, 1,
                                        poImage_grd, nXSize_grd, 1, GDT_UInt32, 0, 0)) == CE_Failure)
             { 
                printf( "\tError: reading line: row=%d  col=%d from domain grid image.\n",row1_grd+j,col1_grd);
                CPLFree (poImage_grd);
                exit( 1 );
             }    
             int  writeIndicator = 0;  
             for (k=0; k<nXSize; k++)
             {
                pixelIndex = nXSize * j + k;
                gridID = poImage_grd[k];
                classID = poImage[pixelIndex];
                if ( gridID > 0 && gridID <= gridPixels && classID != NoDataValue)
                {
                   idIndex = nlcdIDS[classID];    //index in USGS NLCD ID pointer
                   outIndex = idIndex * gridPixels + gridID - 1;  //index in NLCD landuse pointer              
                   gridNLCD[outIndex] += 1;   // count USGS NLCD landuse class in 2d array

                   //set gridID to 0 in poImage_grd after reading USGS NLCD data.  So, cells will not be counted again
                   poImage_grd[k] = 0;
                   writeIndicator = 1;
                }
             }  // end of k col

             //write changed doamin grid image line back to the image
             if ( writeIndicator == 1)
             {
                if ( (poGridBand->RasterIO(GF_Write, col1_grd-1, row1_grd-1+j, nXSize_grd, 1,
                                        poImage_grd, nXSize_grd, 1, GDT_UInt32, 0, 0)) == CE_Failure)
                {
                    printf( "\tError: writing line: row=%d  col=%d to domain grid image.\n",row1_grd+j,col1_grd);
                    CPLFree (poImage_grd);
                    exit( 1 );
                }
             }

          }  //end of j row

        }   //USGS NLCD files

        
        CPLFree (poImage);
        CPLFree (poImage_grd);
        GDALClose( (GDALDatasetH) poRDataset );

     }  //end of overlapping processing
     else
     {
         printf ("\ti=%d image does not intersect with domain grid image.\n",i);
     }

   }  //end of i

   printf ("Finished preprocessing images: %s\n",fileType.c_str());
}  //end of the function


/************************************************************************/
/*    computeMODIS( string modisFile )                                  */
/************************************************************************/

void   computeMODIS( string modisFile )  
{

    int                   i,j;
    int                   pixelIndex;
    GDALDataset           *poRDataset;
    GDALRasterBand        *poBand;
    double                adfGeoTransform[6];
    double                xUL,yUL;
    int                   xCells, yCells;       //cells
    double                xCellSize, yCellSize; //cell size
    double                xmin,xmax,ymin,ymax;         //current image extent
    double                intpart; 

    const char            *pszWKT = NULL;
    char                  *pszWKT_nc = NULL;
    OGRSpatialReference   oSRS;
    char                  *pszProj4 = NULL;
   
    GByte                 NoDataValue = 150;          // set MODIS image nodata value = 150
    int                   gridID, classID, idIndex; 
    int                   outIndex;


   printf( "\nCompute percentage of MODIS landuse classes in domain grid cells...\n" );


/* -------------------------------------------------------------------- */
/*      Open MODIS image                                                */
/* -------------------------------------------------------------------- */
   //open the current i image 
   poRDataset = (GDALDataset *) GDALOpen( modisFile.c_str(), GA_ReadOnly );
   if( poRDataset == NULL )
   {
      printf( "\tError: Open raster file failed: %s.\n", modisFile.c_str() );
      exit( 1 );
   }
/* -------------------------------------------------------------------- */
/*   get rows, columns, cell size, and image extent of MODIS data       */
/* -------------------------------------------------------------------- */
   // get rows and columns of the image
   xCells = poRDataset->GetRasterXSize();
   yCells = poRDataset->GetRasterYSize();
   printf( "  Image size is %dx%dx%d\n", xCells, yCells, poRDataset->GetRasterCount() );

   //get UL corner coordinates and grid size
   if( poRDataset->GetGeoTransform( adfGeoTransform ) == CE_None )
   {
      xUL = adfGeoTransform[0];
      yUL = adfGeoTransform[3];
      printf( "  Image UL Origin = (%.6lf,%.6lf)\n", xUL,yUL );

      xCellSize =  adfGeoTransform[1] ;
      yCellSize =  -1 * adfGeoTransform[5] ;  // Make it positive.  Negative due to Origin is UL corner point
      printf( "  Image Pixel Size = (%.3lf,%.3lf)\n", xCellSize, yCellSize );
   }

 
   //compute extent of MODIS image
   xmin = xUL;
   xmax = xUL + xCellSize * xCells;

   ymax = yUL;
   ymin = yUL - yCellSize * yCells;
   printf( "\tImage extent:  minXY(%.3f,%.3f)   maxXY(%.3f,%.3f)\n", xmin,ymin,xmax,ymax ); 

/* -------------------------------------------------------------------- */
/*   Get image projection to make sure that all images have the same    */
/* -------------------------------------------------------------------- */
   //get projection from the image
   if( (pszWKT = poRDataset->GetProjectionRef())  == NULL )
   {
        printf( "\tError: Projection is not defined in the NLCD file: %s.\n", modisFile.c_str() );
        printf( "\tDefine it using gdal_translate utility.\n");
        exit( 1 );
   }

   //convert it to no const char
   pszWKT_nc =strdup ( pszWKT ); 
   oSRS.importFromWkt( &pszWKT_nc );
   oSRS.exportToProj4( &pszProj4 );    
      
   //make sure that the image has the same projection as the first image's
   if (! oSRS.IsSame (&oSRS_grd) )
   {
       printf( "\tError: This image's projection is different from the domain grid image's.\n" );
       printf( "\tProjection for this image = %s\n", pszProj4);
       printf( "\tProject it using gdalwarp utility.\n" );
       exit ( 1 );
   }

/* -------------------------------------------------------------------- */
/*   Image data type has to be GByte                                    */
/* -------------------------------------------------------------------- */
     //get the band 1 image from the image and make sure that it is GByte image
     poBand = poRDataset->GetRasterBand( 1 );  // band 1
     GDALDataType poBandType = poBand->GetRasterDataType();
     printf ("\tImage Type = %d\n",poBandType);
     if ( poBandType !=GDT_Byte )
     {
         printf( "\tError: Image data type is not GDT_Byte: %s.\n", modisFile.c_str() );    
         exit ( 1 );
     }  

/* -------------------------------------------------------------------- */
/*         get image into array                                         */
/* -------------------------------------------------------------------- */
        
     poBand = poRDataset->GetRasterBand( 1 );  // band 1
     GByte *poImage = (GByte *) CPLCalloc(sizeof(GByte),xCells*yCells);
     if ( (poBand->RasterIO(GF_Read, 0,0,xCells,yCells,
           poImage,xCells,yCells,GDT_Byte,0,0)) == CE_Failure) 
     {
         printf( "\tError: reading band 1 data from image: %s.\n", modisFile.c_str() );
         CPLFree (poImage);
         exit( 1 );
     }
          
/* -------------------------------------------------------------------- */
/*     read one row at time from domain image due to the size           */
/* -------------------------------------------------------------------- */
     //domain grid image is too big to read once 
     GUInt32 *poImage_grd = (GUInt32 *) CPLCalloc(sizeof(GUInt32),xCells_grd);
             
     double x,y;
     int  col,row;
     for( i = 0; i<yCells_grd; i++ )
     {
        if ( (poGridBand->RasterIO(GF_Read, 0, i,xCells_grd, 1,
                                   poImage_grd,xCells_grd,1,GDT_UInt32,0,0)) == CE_Failure)
        {
            printf( "\tError: Reading row = %d from domain grid image.\n",i+1);
            CPLFree (poImage_grd);
            exit( 1 );
        }

         //get cell center point y and compute row in MODIS image
        y = yMax_grd - i * yCellSize_grd - yCellSize_grd / 2.0;
        row = (int) (floor ((ymax - y) / yCellSize));   //start from 0

        for (j=0; j<xCells_grd; j++)
        {
           gridID = poImage_grd[j] ;
           if (gridID > 0 && gridID <= gridPixels)
           {
               //get cell center point x and compute col in MODIS image
               x = xMin_grd + j * xCellSize_grd + xCellSize_grd / 2.0;
               col = (int) (floor ((x - xmin) / xCellSize));  //start from 0
              
               pixelIndex = xCells * row + col;
               classID = poImage[pixelIndex];       //get MODIS class ID
               if ( classID != NoDataValue)
               {
                  idIndex = modisIDS[classID];         //index in MODIS class ID pointer
                  outIndex = idIndex * gridPixels + gridID - 1;  //index in output MODIS landuse pointer
                  gridMODIS[outIndex] += 1;   // count MODIS landuse class in 2d array
               }
           }
        }
     }
     printf ( "Finished counting modeling domain grid ID numbers.\n\n" );

     CPLFree (poImage);
     CPLFree (poImage_grd);
     GDALClose( (GDALDatasetH) poRDataset );



   printf ("Finished preprocessing MODIS images.\n");
}  //end of the function


/************************************************************************/
/*    computeIntersectionBoxIndexes(...)                                */
/************************************************************************/
void computeIntersectionBox(int xCells, int yCells, double xmin, double ymin, double xmax, double ymax,
                            double xMin_grd, double yMin_grd, double xMax_grd, double yMax_grd,
                            int *col1, int *row1, int *col2, int *row2,
                            int *col1_grd, int *row1_grd, int *col2_grd, int *row2_grd)
{

         //compute indexes of intersection box in domain grid image and image i

         if ( xmin>xMin_grd )
         {
            *col1 = 1;
            *col1_grd =  (int) ( (xmin - xMin_grd) / xCellSize_grd + 1);
         }
         if ( xmin<xMin_grd )
         {
            *col1 = (int) ( (xMin_grd - xmin) / xCellSize_grd + 1);
            *col1_grd = 1;
         }
         if ( xmin==xMin_grd )
         {
            *col1 = 1;
            *col1_grd = 1;  
         }

         //calculate intersection LR columns
         if ( xmax<xMax_grd )
         {
            *col2 = xCells;
            *col2_grd = (int) ( (xmax - xMin_grd) / xCellSize_grd );
         }
         if ( xmax>xMax_grd )
         {
            *col2 = (int) ( (xMax_grd - xmin) / xCellSize_grd );
            *col2_grd = xCells_grd;
         }
         if ( xmax==xMax_grd )   
         {
            *col2 = xCells; 
            *col2_grd = xCells_grd; 
         }
     
         //calculate intersection LR rows 
         if ( ymin>yMin_grd )
         {
            *row2 = yCells;
            *row2_grd = (int) ( (yMax_grd - ymin) / yCellSize_grd ); 
         }
         if ( ymin<yMin_grd )
         {
            *row2 = (int) ( (ymax - yMin_grd) / yCellSize_grd );
            *row2_grd = yCells_grd;
         }
         if ( ymin==yMin_grd )
         {
            *row2 = yCells;
            *row2 = yCells_grd;
         }

         //calculate intersection UL row
         if ( ymax<yMax_grd )
         {
            *row1 = 1;
            *row1_grd = (int) ( (yMax_grd - ymax) / yCellSize_grd + 1); 
         }
         if ( ymax>yMax_grd )
         {
            *row1 = (int) ( (ymax - yMax_grd) / yCellSize_grd + 1);
            *row1_grd = 1;
         }
         if ( ymax==yMax_grd )
         {
            *row1 = 1;
            *row1_grd = 1; 
         }
        
}

/*******************************************
* Functions for outputiing netcdf file   ***
*******************************************/
/***************************************************
* Get the WRF map projection type from PROJ4 string*
***************************************************/
int getProjType(string proj4Str)
{
     int type;

     if(  proj4Str.find("+proj=latlong") != string::npos )
     {
        //latitude and longitude projection
        type = LATLONG;
     }
     else if(  proj4Str.find("+proj=lcc") != string::npos )
     {
        //Lambert Conformal Conic
        type = LCC;
     }
     else if(  proj4Str.find("+proj=ups") != string::npos )
     {
        //Universal Polar Stereographic
        type = UPS;
     }
     else if(  proj4Str.find("+proj=merc") != string::npos )
     {
        //Mercator
        type = MERC;
     }
     else
     {
        printf("WRF only had four projection types: LATLONG, LCC, UPS, and MERC.\n");
        printf("Check GRID_PROJ definition in the script file.\n");
        exit ( 1 );
     }

     if (type != LCC)
     {
        printf("WRF supports four projection types: LATLONG, LCC, UPS, and MERC.\n");
        printf("But, the program currently only supports LCC projection in WRF NetCDF output format.\n");
        exit ( 1 );
     }
   
     return type;
}


/*******************
* String tokenizer *
*******************/
void tokenize   (const string &str,
                                        vector<string> &tokens,
                                        const string &delimiters)
{
        // Skip delimiters at beginning.
        string::size_type lastPos = str.find_first_not_of(delimiters, 0);

        // Find first "non-delimiter".
        string::size_type pos = str.find_first_of(delimiters, lastPos);

        while (string::npos != pos || string::npos != lastPos)
        {
                // Found a token, add it to the vector.
                tokens.push_back(str.substr(lastPos, pos - lastPos));

                // Skip delimiters.  Note the "not_of"
                lastPos = str.find_first_not_of(delimiters, pos);

                // Find next "non-delimiter"
                pos = str.find_first_of(delimiters, lastPos);
        }
}


/*********************************************
* Handle errors by printing an error message *
* and exiting with a non-zero status.        *
*********************************************/
void anyErrors(int e)
{
        if (e != NC_NOERR)
        {
                printf("NetCDF error: %s\n", nc_strerror(e));
                exit ( 1 );
        }
}


/*********************************************
***   Project a point to lat/long point ******
*********************************************/
projUV   computeLatLong(projPJ proj4DF, double x, double y)
{
    projUV  latlongP, xyP;

    xyP.u = x;
    xyP.v = y;

    latlongP = pj_inv(xyP, proj4DF);
    if (latlongP.u == HUGE_VAL)
    {
       printf( "Error in computing lat and long for point: %lf,%lf\n",x,y);
       exit ( 1 );
    }

    latlongP.u *= RAD_TO_DEG;
    latlongP.v *= RAD_TO_DEG;

    return latlongP;
}

