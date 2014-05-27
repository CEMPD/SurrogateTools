#! /bin/csh -f
#******************* Overlay Run Script **************************************
#
#  Example of Overlay mode for MIMS Spatial Allocator
#  This example uses a shapefile overlaying a Regular Grid
#  Created 3/30/2005 by Ben Brunk, Carolina Environmental Program
#
#  Updated:  March 2006 provided support for egrid.  LR
#  Dec. 2007, LR -- projection specification changes
#*****************************************************************************

# Set debug output
setenv DEBUG_OUTPUT Y

# Set executable
setenv MIMSDIR ..
setenv EXE "$MIMSDIR/allocator.exe"

# Set Input Directory
setenv DATADIR $MIMSDIR/data
setenv OUTPUT $MIMSDIR/output

# Select method of spatial analysis

setenv MIMS_PROCESSING OVERLAY

setenv TIME time

setenv OVERLAY_SHAPE US36KM_148X112
setenv OVERLAY_TYPE RegularGrid
setenv OVERLAY_MAP_PRJN US36KM_148X112
setenv OVERLAY_ELLIPSOID "+a=6370997.0,+b=6370997.0"
setenv GRIDDESC $DATADIR/GRIDDESC.txt
setenv OVERLAY_OUT_TYPE DelimitedFile
setenv OVERLAY_OUT_NAME $OUTPUT/grid_point.csv
setenv OVERLAY_OUT_DELIM COMMA
setenv OVERLAY_OUT_CELLID YES
# will print row and col
# problem with this method is that ROW and COL are not avaiable to pass through
#setenv OVERLAY_ATTRS Site_ID
setenv OVERLAY_ATTRS ALL
setenv INPUT_FILE_NAME $DATADIR/CASTNET_locations.txt
setenv INPUT_FILE_TYPE PointFile
setenv INPUT_FILE_XCOL Longitude
setenv INPUT_FILE_YCOL Latitude
setenv INPUT_FILE_DELIM SEMICOLON
setenv INPUT_FILE_MAP_PRJN "+proj=latlong"
setenv INPUT_FILE_ELLIPSOID "+a=6370997.0,+b=6370997.0"
setenv WRITE_HEADER Y

echo "Overlaying egrid with PointFile"
$TIME $EXE
