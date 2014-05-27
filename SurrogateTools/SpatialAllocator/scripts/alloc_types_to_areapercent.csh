#! /bin/csh -f
#******************* Allocate Shapefiles Run Script **************************
# Allocates a polygon shapefile's data to an I/O API gridded file
# Oct. 2006, LR 
#*****************************************************************************


setenv DEBUG_OUTPUT Y

# Set executable
setenv MIMSDIR ..
setenv EXE "$MIMSDIR/src/allocator.exe"

# Set Input Directory
setenv DATADIR $MIMSDIR/data
setenv OUTPUT $MIMSDIR/output

# Select method of spatial analysis

setenv MIMS_PROCESSING ALLOCATE

setenv TIME time

#set "data" shapefile parameters
setenv GRIDDESC $DATADIR/GRIDDESC.txt

#set parameters for file being allocated
setenv INPUT_FILE_NAME $DATADIR/surfzone_poly
setenv INPUT_FILE_TYPE ShapeFile
setenv INPUT_FILE_MAP_PRJN "+proj=lcc,+lat_1=33,+lat_2=45,+lat_0=40,+lon_0=-97"
setenv INPUT_FILE_ELLIPSOID SPHERE
setenv ALLOCATE_ATTRS TYPE
setenv ALLOC_MODE_FILE ALL_AREAPERCENT

# Set name and path of resulting shapefile
setenv OUTPUT_FILE_TYPE IoapiFile
setenv OUTPUT_GRID_NAME US36
setenv OUTPUT_FILE_MAP_PRJN US36
setenv OUTPUT_FILE_ELLIPSOID SPHERE
setenv OUTPUT_FILE_NAME $OUTPUT/surf_land_${OUTPUT_GRID_NAME}.ncf

#echo "Allocating surf zone data by land use type"
$TIME $EXE
