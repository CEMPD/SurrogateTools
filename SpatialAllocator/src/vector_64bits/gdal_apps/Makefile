# for Linux or any gcc/g++ compiler
CC = gcc
CPP = g++

#Optional flags - uncomment one OPTFLAGS line
#OPTFLAGS = -O2
OPTFLAGS = -O -DDEBUG 

#set GDAL library and include directory compiled using gcc
GDAL_HOME = ../libs/gdal-1.5.2/local
GDAL_LIB = ${GDAL_HOME}/lib
GDAL_INCLUDE = ${GDAL_HOME}/include

# Set netCDF library and include directory compiled using gcc
NETCDF = ../libs/netcdf-4.0/local
NETCDF_LIB = ${NETCDF}/lib 
NETCDF_INC = ${NETCDF}/include

#set PROJ4.6 libary and share data directories compiled using gcc
PROJDIR = ../libs/proj-4.6.0/local
PROJ_LIBRARY = ${PROJDIR}/lib
PROJ_INCLUDE = ${PROJDIR}/include

# set the following environment variable in .cshrc file or control file
#PROJ_LIB = ../libs/proj-4.6.0/local/share/proj

# Library paths
LIBPATH = -L$(GDAL_LIB) 
LIBPATH_NCF = -L$(NETCDF_LIB)
LIBPATH_PROJ = -L$(PROJ_LIBRARY)

LIBS = -lm $(LIBPATH_PROJ) -lproj $(LIBPATH_NCF) -lnetcdf_c++ -lnetcdf $(LIBPATH) -lgdal


LDFLAGS = -static 

#on Linux
CPPFLAGS = -g $(OPTFLAGS) -I${PROJ_INCLUDE} -I${NETCDF_INC} -I${GDAL_INCLUDE}

#.cpp.o:
##	$(CPP) $(CFLAGS) -c $<
#
#.c.o:
#	$(CC) $(CFLAGS) -c $<


all: create_gridPolygon.exe toNLCDRaster.exe preProcessNLCD.exe computeGridLandUse.exe rasterWtoPolygons.exe txt2ncf.exe

create_gridPolygon.exe:  create_gridPolygon.o utilities.o
	$(CPP) -o $@  create_gridPolygon.o utilities.o $(LIBS)

toNLCDRaster.exe:  toNLCDRaster.o utilities.o
	$(CPP) -o $@ toNLCDRaster.o utilities.o $(LIBS)

preProcessNLCD.exe:  preProcessNLCD.o utilities.o
	$(CPP) -o $@ preProcessNLCD.o utilities.o $(LIBS)

computeGridLandUse.exe:  computeGridLandUse.o utilities.o
	$(CPP) -o $@ computeGridLandUse.o utilities.o $(LIBS)

rasterWtoPolygons.exe:  rasterWtoPolygons.o utilities.o
	$(CPP) -o $@ rasterWtoPolygons.o utilities.o $(LIBS)

txt2ncf.exe:  txt2ncf.o utilities.o
	$(CPP) -o $@ txt2ncf.o utilities.o $(LIBS)

install:
	if test ! -d ../../bin; then mkdir ../../bin; fi
	cp -f *.exe ../../bin

clean:
	 -rm *.o *.exe

create_gridPolygon.o: create_gridPolygon.cpp
	$(CPP) $(CPPFLAGS) -c create_gridPolygon.cpp
toNLCDRaster.o: toNLCDRaster.cpp
	$(CPP) $(CPPFLAGS) -c toNLCDRaster.cpp
preProcessNLCD.o: preProcessNLCD.cpp
	$(CPP) $(CPPFLAGS) -c preProcessNLCD.cpp
computeGridLandUse.o: computeGridLandUse.cpp
	$(CPP) $(CPPFLAGS) -c computeGridLandUse.cpp
rasterWtoPolygons.o: rasterWtoPolygons.cpp
	$(CPP) $(CPPFLAGS) -c rasterWtoPolygons.cpp
utilities.o: utilities.cpp
	$(CPP) -c utilities.cpp
txt2ncf.o: txt2ncf.cpp
	$(CPP) $(CPPFLAGS) -c txt2ncf.cpp
