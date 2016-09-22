/********************************************************************
 * This program is used to create a grid polygon shapefile with user's specifiied
 * modeling grid domain and projection using GDAL library.
 *
 * Written by the Institute for the Environment at UNC, Chapel Hill
 * in support of the EPA NOAA CMAS Modeling, 2007-2008.
 * 
 * By Limei Ran, Feb, 2008
 *
 * Usage: create_gridPolygon "PROJ4 project_definitation" xmin ymin xcell_size ycell_size row col out_shapefile
 *        For instance:
 *        create_gridPolygon.exe "+proj=lcc +a=6370000.0 +b=6370000.0 +lat_1=33 +lat_2=45 +lat_0=40 +lon_0=-97" -1080000.0 -1692000.0  12000 12000 250 289 wrf12km.shp
 *        or
 *        create_gridPolygon.exe
 *
 *        Environment Variables needed without arguments:  
 *            GRID_PROJ -- PROJ4 projection definition 
 *            GRID_XMIN -- grid x min
 *            GRID_YMIN -- grid y min
 *            GRID_XCELLSIZE -- grid x cell size
 *            GRID_YCELLSIZE -- grid y cell sixe
 *            GRID_ROWS -- grid rows
 *            GRID_COLUMNS -- grid columns
 *            GRID_SHAPEFILE_NAME -- output grid shapefile name
 */
#include <dirent.h>
#include "ogrsf_frmts.h"
#include "ogr_geometry.h"
#include "cpl_conv.h"
#include "cpl_string.h"
#include "ogr_api.h"
#include "commontools.h"


static void Usage();

/************************************************************************/
/*                                main()                                */
/************************************************************************/

int main( int nArgc,  char* papszArgv[] )
{
    struct stat stFileInfo;
    const int argsN = 8;  //number of input arguments required
    const char  *pszDataSource = NULL;
    const char *pszDriverName = "ESRI Shapefile";
    OGRSFDriver *poDriver = NULL;
    OGRDataSource *poDS = NULL;
    OGRSpatialReference oSRS;
    OGRLayer *poLayer = NULL;
    char szName[33];
    OGRFeature *poFeature = NULL;
    OGRLinearRing  oRing;
    OGRPolygon oPoly;
    double xmin,xmax;
    double ymin,ymax;
    int row, col, i,j;
    int gid = 0;
    double x0, y0;
    double xcell, ycell;
    char *pszProj4;

    string test;

    //print program version
    printf ("\nUsing: %s\n", prog_version);
/* -------------------------------------------------------------------- */
/*      Processing command line arguments.                              */
/* -------------------------------------------------------------------- */
   
    printf("\nSetting variables to run the program:\n\n"); 
    if( nArgc == argsN + 1)
    {
      for( int iArg = 1; iArg < nArgc; iArg++ )
      {
        if( iArg  == 1 )
        {
            pszProj4 = papszArgv[iArg];
            if ( strstr(pszProj4, "+proj=") == NULL )
            {
               Usage();
            }
        }
        else if( iArg == 2  )
        {
            x0 = atof( papszArgv[iArg] );
        }
        else if( iArg == 3) 
        {
            y0 = atof( papszArgv[iArg] );
        }
        else if( iArg == 4 )
        {
           xcell = atof( papszArgv[iArg] );
        }
        else if( iArg == 5 )
        {
            ycell = atof( papszArgv[iArg] );
        }
        else if( iArg == 6 )
        {
            row = atoi( papszArgv[iArg] );
        }
        else if( iArg == 7)
        {
           col = atoi( papszArgv[iArg] ); 
        }
        else
        {
           pszDataSource = papszArgv[iArg]; 
           if( pszDataSource == NULL )
             Usage();
        }
      }
    }
    else if ( nArgc == 1)
    {
      pszProj4 = getEnviVariable("GRID_PROJ");
      x0 = atof( getEnviVariable("GRID_XMIN") );
      y0 = atof( getEnviVariable("GRID_YMIN") );
      xcell = atof( getEnviVariable("GRID_XCELLSIZE") );
      ycell = atof( getEnviVariable("GRID_YCELLSIZE") );
      row = atoi( getEnviVariable("GRID_ROWS") );
      col = atoi( getEnviVariable("GRID_COLUMNS") );
      pszDataSource = getEnviVariable("GRID_SHAPEFILE_NAME"); 
    }
    else
    {
        printf("\nError in input arguments.  Missing %d args.\n",argsN + 1 - nArgc);
        Usage();
        exit( -nArgc );
    }

   printf("PROJ = %s\n",pszProj4);
   printf("Domain Lower Left Corner:  x = %f  y = %lf\n",x0,y0);
   printf("Domain Grid Size:  xcell = %f  ycell = %f\n",xcell,ycell);
   printf("Domain Gird Number:  row = %d  col = %d\n",row,col);
   printf("Output Shapefile = %s\n\n",pszDataSource);

   //Error checking grid cell size
   if (xcell <= 0 || ycell <= 0)
   {
      printf( "  Error: cell size has to > 0\n",xcell,ycell);
     exit ( 1 );
   }

   //Error checking rows and columns
   if ( row < 1 || col < 1 )
   {
      printf( "  Error: rows and columns have to >= 1\n",row,col);
      exit ( 1 );
   }

/* -------------------------------------------------------------------- */
/*      Register format(s).                                             */
/* -------------------------------------------------------------------- */
    OGRRegisterAll();

   //check the output shapefile.  If it exists delete it
   if (stat(pszDataSource, &stFileInfo) == 0)
   {
       printf( "Shapefile exists and delete it: %s\n", pszDataSource );
       poDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName( pszDriverName );
       if( poDriver == NULL )
       {
          printf( "%s driver not available.\n", pszDriverName );
          exit( 1 );
       }
       if ( (poDriver->DeleteDataSource( pszDataSource )) != OGRERR_NONE )
       {
          printf( "\tError in deleting shapefile: %s\n\n", pszDataSource );
       }
    }

    printf("\nGenerating grid shapefile for the given domain\n\n"); 

    poDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(
                pszDriverName );
    if( poDriver == NULL )
    {
        printf( "%s driver not available.\n", pszDriverName );
        exit( 1 );
    }

/* -------------------------------------------------------------------- */
/*      Open output shapefile.                                          */
/* -------------------------------------------------------------------- */
    poDS = poDriver->CreateDataSource( pszDataSource, NULL );
    if( poDS == NULL )
    {
        printf( "Creation of output file failed.\n" );
        exit( 1 );
    }

/* -------------------------------------------------------------------- */
/*      Create output Polygon feature.                                  */
/* -------------------------------------------------------------------- */
    string proj4Str = string(pszProj4);
    oSRS.importFromProj4( proj4Str.c_str() );
    
    poLayer = poDS->CreateLayer( pszDataSource, &oSRS, wkbPolygon, NULL );
 
    printf ("Created shapefile.\n");
    if( poLayer == NULL )
    {
        printf( "Layer creation failed.\n" );
        exit( 1 );
    }

/* -------------------------------------------------------------------- */
/*      Create Polygon Attribute Name.                                  */
/* -------------------------------------------------------------------- */
    
    OGRFieldDefn oField( "GRIDID", OFTInteger);
    printf ("Created item.\n");

    if( poLayer->CreateField( &oField ) != OGRERR_NONE )
    {
        printf( "Creating Name field failed.\n" );
        exit( 1 );
    }

/* -------------------------------------------------------------------- */
/*      Assign each polygon attribute and geometry.                     */
/* -------------------------------------------------------------------- */
    
    for ( i = 0; i < row; i++ )
    {
      ymin = y0 + i * ycell;
      ymax = ymin + ycell;

      for( j = 0; j < col; j++ )
      {
         gid ++;        
         
         poFeature = OGRFeature::CreateFeature( poLayer->GetLayerDefn() );
         poFeature->SetField( "GRIDID", gid );
       
         xmin = x0 + j * xcell;
         xmax = xmin + xcell;

         oRing.addPoint( xmin, ymin );
         oRing.addPoint( xmin, ymax );
         oRing.addPoint( xmax, ymax );
         oRing.addPoint( xmax, ymin );
         oRing.addPoint( xmin, ymin );
         //printf("ID = %d   x: %lf   %lf, y: %lf   %lf\n",gid,xmin,xmax,ymin,ymax);

         oPoly.addRing( &oRing );
   
         if ( poFeature->SetGeometry( &oPoly ) != OGRERR_NONE )
         {
            printf( "Setting a polygon feature failed.\n" );
            exit( 1 );
         }

         if( poLayer->CreateFeature( poFeature ) != OGRERR_NONE )
         {
            printf( "Failed to create feature in shapefile.\n" );
            exit( 1 );
         }

         oRing.empty();
         oPoly.empty();
         OGRFeature::DestroyFeature( poFeature ); 
      } 
    }

   
    OGRDataSource::DestroyDataSource( poDS );
    
    
    printf ("Completed in creating the grid shapefile.\n\n");
}

/************************************************************************/
/*                               Usage()                                */
/************************************************************************/

static void Usage()

{
    printf( "\nError in input arguments.\n");
    printf( "Usage: create_gridPolygon \"PROJ4 project_definitation\" xmin ymin xcell_size ycell_size row col out_shapefile\n");
    exit( 1 );
}

