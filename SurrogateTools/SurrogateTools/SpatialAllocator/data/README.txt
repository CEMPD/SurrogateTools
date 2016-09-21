The tn_* files are extractions from the files described below
for the state of Tennessee.

cnty_tn is a lat-lon file that contains only TN county polygons.

-----------------------------------------------------
2002 US Surrogates
-----------------------------------------------------

Format:

Surrogate: Coverage name/Type

     Data file items and description
	
		Item definition (if needed)


Projection: Lambert
	Units: Meters
	Parameters:  Spheroid Sphere
	1st Standard Parallel: 33 0 0.000
	2nd Standard Parallel: 45 0 0.000
	Central Meridian:  -97 0 0.000
	Latitude of Projection’s Origin:  40 0 0.000
	False easting (meters): 0.00000
	False northing (meters):  0.00000

+proj=lcc,+lat_1=33,+lat_2=45,+lat_0=40,+lon_0=-97


-------------
-------------


Population, Housing, and Housing Density: POPHU2K / Polygon *
Source:  2000 US Census Bureau
   
    ITEM NAME      WIDTH OUTPUT TYPE NO.DEC	DESCRIPTION
	AREA	8	18	F	5	Polygon area (m2)
	PERIMETER	8   	18  	F   	5	Polygon perimeter	    
	POPHU2K#         	4   	5 	B   	-	Internal coverage #
	POPHU2K-ID       	4   	5  	B   	-	Internal coverage id
	STFID	12	12  	C   	-	Census ID
	STUSAB          	2 	2  	C   	-	State Abbreviation
	TOTAL_SAMPLE	8	20	F	5	Total Pop in Sample Pop (STF3)
	URBAN	8	20	F	5	Total Urban Pop (STF3)
	INS_URB_AR	8	20	F	5  	Total Pop inside census designated urban  area
	INS_URB_CL	8	20	F	5   	Total Pop inside census designated urban cluster
	RURAL	8	20	F	5	Total Rural Pop (STF3)
	FARM	8	20	F	5	Total Rural Pop Living on a Farm (STF3)
	NON-FARM	8	20	F	5	Total Rural Pop Not Living on a Farm (STF3)
	FIPSSTCO	5	5	C	-	State and County FIPS
	TRACT	6	6	C	-	Census Tract Number
	STATE	2	2	C	-	Census State Code
	COUNTY	3	3	C	-	Census County Code
	BLKGRP	1	1	C	-	Census Block Group Number
	WHITE	8	9	F	0	Total White Pop
	BLACK	8	9	F	0	Total Black Pop
	AMERI_ES	8	9	F	0	Total American Indian and Alaska Native Pop
	ASIAN	8	9	F	0	Total Asian Pop
	HAWN_PI	8	9	F	0	Total Native Hawaiian and Pacific Islander Pop
	OTHER	8	9	F	0	Total Other Race Pop
	MULT_RACE	8	9	F	0	Total Multi Race Pop
	HISPANIC	8	9	F	0	Total Hispanic Pop
	MALES	8	9	F	0	Total Male Pop
	FEMALES	8	9	F	0	Total Female Pop
	AGE_UNDER5	8	9	F	0	Total Pop under 5 years
	AGE_5_17	8	9	F 	0	Total Pop 5 to 17 years
	AGE_18_21	8	9	F	0 	Total Pop 18 to 21 years
	AGE_22_29	8	9	F	0	Total Pop 22 to 29 years
	AGE_30_39	8	9	F	0	Total Pop 30 to 39 years
	AGE_40_49	8	9	F	0	Total Pop 40 to 49 years
	AGE_50_64	8	9	F 	0 	Total Pop 50 to 64 years
	AGE_65_UP	8	9	F	0	Total Pop over 64 years
	MED_AGE	8	10	F	1	Median Age, Both Sexes
	MED_AGE_M	8	10	F	1	Median Age, Males
	MED_AGE_F	8	10	F	1	Median Age, Females
	HOUSEHOLDS	8	9	F	0	Number of Households
	AVE_HH_SZ	8	11	F	2	Average Household Size	
	HSEHLD_1_M	8	9	F	0	1-person household, male householder
	HSEHLD_1_F	8	9	F	0	1-person household, female householder
	MARHH_CHD	8	9	F	0	family households, married-couple 
							With children under 18
	MARHH_NO_C		8	9	F	0	family households, married-couple family, no own 
							children under 18 yrs
	MHH_CHILD		8	9	F	0	family households, other family, male							householder, no wife present w/ own children
							Under 18 years
	FHH_CHILD		8	9	F	0	family households, other family, female 
							Householder, no husband present, w/ own children
							Under 18 years
	FAMILIES		8	9	F	0	families
	AVE_FAM_SZ		8	11	F	2	average family size
	HSE_UNITS		8	9	F	0	housing units
	VACANT		8	9	F	0	housing units, vacant
	OWNER_OCC		8	9	F	0	housing units, owner occupied
	RENTER_OCC		8	9	F	0	housing units, renter occupied
	POP2000		8	19	F	11	Total Population from STF 1 File
	Area-H		4	12	F	3	Area in Hectares
	HDENS		4	12	F	3	HSE_UNITS / AREA-ACRE
	AREA-ACRE		4	12	F	3	Area in Acres
	STCTYTR		11	11	C		State, County, Tract ID

Agriculture:  US_AG2K / Polygon *
Source:  NLCD – National Land Cover Database (USGS)

    ITEM NAME      WIDTH OUTPUT TYPE NO.DEC	DESCRIPTION
	AREA	4	12	F	3	Polygon area (m2)
	PERIMETER	4	12	F	3	Polygon perimeter
	US_AG2K#	4	5	B		Internal coverage #
	US_AG2K-ID	4	5	B		Internal coverage id
	FIPSSTCO	5	5	C		State and County FIPS ID
	STATE	66	66	C		State Name
	COUNTY	66	66	C		County Name
	GRID-CODE	4	8	B
	
     				2 = Non-Agriculture
     				61 = Orchards/Vineyards
     				81 = Pasture/Hay
					82 = Row Crops
					83 = Small Grains
					84 = Fallow Land

Forest: US_FOR2K / Polygon *
Source:  NLCD – National Land Cover Database (USGS)

    ITEM NAME      WIDTH OUTPUT TYPE NO.DEC	DESCRIPTION
	AREA	4	12	F	3	Polygon area (m2)
	PERIMETER	4	12	F	3	Polygon perimeter
	US_FOR2K#	4	5	B		Internal coverage #
	US_FOR2K-ID	4	5	B		Internal coverage id
	FIPSSTCO	5	5	C		State and County FIPS ID
	STATE	66	66	C		State Name
	COUNTY	66	66	C		County Name
	GRID-CODE	4	8	B
					2 = Non-Forest
					41 = Deciduous Forest
					42 = Evergreen Forest
					43 = Mixed Forest
					91 = Woody Wetlands

Land/Water: US_LW2K / Polygon *
Source:  NLCD – National Land Cover Database (USGS)

    ITEM NAME      WIDTH OUTPUT TYPE NO.DEC	DESCRIPTION
	AREA	4	12	F	3	Polygon area (m2)
	PERIMETER	4	12	F	3	Polygon perimeter
	US_LW2K#	4	5	B		Internal coverage #
	US_LW2K-ID	4	5	B		Internal coverage ID
	FIPSSTCO	5	5	C		State and County FIPS ID
	STATE	66	66	C		State Name
	COUNTY	66	66	C		County Name
	GRID-CODE	4	8	B		
						-9999 = Land
						0 = Coastal Water
						11 = Inland Water
	H20-CODE	1	1	I		
						1 = Inland Water
						2 = Land
						3 = Coastal Water

	STATECTY	5	5	I		State and County FIPS ID

Landuse: US_LU2K / Polygon *
Source:  FEMA – HAZUS data
·	Square Footage is in Thousands, so 3.000 = 3000 square feet

    ITEM NAME      WIDTH OUTPUT TYPE NO.DEC	DESCRIPTION
	AREA	4	12	F	3	Polygon area (m2)
	PERIMETER	4	12	F	3	Polygon perimeter
	US_LU2K#	4	5	B		Internal coverage #
	US_LU2K-ID	4	5	B		Internal coverage ID
	TRACT90	11	11	C		1990 Tract ID
	RES1	8	19	F	3	Total Square Footage of Single Family 
						Dwellings
	RES2	8	19	F	3	Total Square Footage of Mobile Home 
						Dwellings
	RES3	8	19	F	3	Total Square Footage of Multi Family
						Dwellings
	RES4	8	19	F	3	Total Square Footage of Temporary 
						Lodging 
	RES5	8	19	F	3	Total Square Footage of Institutional 
						Dormitories
	RES6	8	19	F	3	Total Square Footage of Nursing Homes
	COM1	8	19	F	3	Total Square Footage of Retail Trade
	COM2	8	19	F	3	Total Square Footage of Wholesale 
						Trade
	COM3	8	19	F	3	Total Square Footage of 
						Personal/Repair Services
	COM4	8	19	F	3	Total Square Footage of 
						Professional/Technical Services
	COM5	8	19	F	3	Total Square Footage of Banks	
	COM6	8	19	F	3	Total Square Footage of Hospitals
	COM7	8	19	F	3	Total Square Footage of 
						Medical Offices/Clinics
	COM8	8	19	F	3	Total Square Footage of
						Entertainment and Recreation
	COM9 	8	19	F	3	Total Square Footage of Theaters
	COM10	8	19	F	3	Total Square Footage of Parking
	IND1	8	19	F	3	Total Square Footage of Heavy 
						Industrial
	IND2	8	19	F	3	Total Square Footage of Light
						Industrial
	IND3	8	19	F	3	Total Square Footage of 
						Food/Drugs/Chemicals
	IND4	8	19	F	3	Total Square Footage of 
						Metals/Minerals Processes
	IND5	8	19	F	3	Total Square Footage of 
						High Technology
	IND6	8	19	F	3	Total Square Footage of Construction
	AGR1	8	19	F	3	Total Square Footage of Agriculture
	REL1	8	19	F	3	Total Square Footage of Church and
						Nonprofits
	GOV1	8	19	F	3	Total Square Footage of General 
						Government Services
	GOV2	8	19	F	3	Total Square Footage of 
						Emergency Response
	EDU1	8	19	F	3	Total Square footage of Schools
	EDU2	8	19	F	3	Total Square footage of colleges
						And universities
	TR90-AREA-H	4	12	F	3	1990 Tract Area in Hectares
	
	
Roads:  US_RDS2K / Line *
Source:  US Census Bureau – TIGER data

    ITEM NAME      WIDTH OUTPUT TYPE NO.DEC	DESCRIPTION
	FNODE#	4	5	B			From node #
	TNODE#	4	5	B			To node #
	LPOLY#	4	5	B			Left polygon #
	RPOLY#	4	5	B			Right polygon #
	LENGTH	4	12	F	3		Length (meters)
	US_RDS2K#	4	5	B			Internal coverage #
	US_RDS2K-ID	4	5	B			Internal coverage ID
	CFCC	3	3	C			Census Bureau Road Classification
	UA	5	5	C			urban area ID (Census Bureau)
	NAME	90	90	C			Name of urban area
	UA_TYPE	2	2	C			Urban Area Type
	FRDCLASS	2	2	I			Road Class derived from CFCC code 
							And urban areas
							5 = Urban Primary
							6 = Rural Primary
							10 = Urban Secondary
							11 = Rural Secondary
	FIPSSTCO	5	5	C			State and County FIPS
	STATE	66	66	C			State Name
	COUNTY	66	66	C			County Name
	
Railroads:  US_RAIL2K / Line	 *
Source:  Bureau of Transportation Statistics – Federal Railroad Administration
									
    ITEM NAME      WIDTH OUTPUT TYPE NO.DEC	DESCRIPTION
	FNODE#	4	5	B			From node #
	TNODE#	4	5	B			To node #
	LPOLY#	4	5	B			Left polygon #
	RPOLY#	4	5	B			Right polygon #
	LENGTH	4	12	F	3		Length (meters)
	US_RAIL2K#	4	5	B			Internal coverage #
	US_RAIL2K-ID	4	5	B			Internal coverage ID
	ABANDONED	1	1	C			Abandoned Railroad Flag	
							A = Abandoned
	PASS	4	4	C			Type of Passenger Rail Flag
							V = VIA Line
							A = Amtrak Line
							C = Commuter Line
							T = Tourist Line
							R = Rapid Transit Line
							X = Previous Passenger Line
							Y = Undesignated Service
	MILITARY	1	1	C			Type of Military Rail Flag
							S = line on the STRACNET system
							C = connector to STRACNET system
	STATE	2	2	C			State Abbreviation
	RR_CLASS	1	1	C			0 AND U = Unknown class
							1 = Class 1 is defined by railroads 
							having more than 250 million 
							dollars in revenues
							2 = Class 2 is defined by railroads
							having 10 – 250 million dollars in
							revenues
							3 = Class 3 is defined by railroads 
							having less than 10 million dollars
							in revenue
							A = Abandoned railroads
	RAILROAD	31	31	C			Railroad Name
	CLASS1-LENGTH	4	12	F	3		Length of Railroad defined as Class 1
	CLASS2-LENGTH	4	12	F	3		Length of Railroad defined as Class 2
	CLASS3-LENGTH	4	12	F	3		Length of Railroad defined as Class 3
	CLASSA-LENGTH	4	12	F	3		Length of Railroad defined as Class A
	CLASSU-LENGTH	4	12	F	3		Length of Railroad defined as Class 0/U
	FIPSSTCO	5	5	C			State and County FIPS
	COUNTY	66	66	C			County Name
	STFID	8	19	F	11		State and County FIPS

Low Residential Housing:  US_LOWRES / Polygon *
Source:  NLCD – National Land Cover Database

    ITEM NAME      WIDTH OUTPUT TYPE NO.DEC	DESCRIPTION
	AREA	4	12	F	3		Polygon area (m2)
	PERIMETER	4	12	F	3		Polygon perimeter
	US_LOWRES#	4	5	B			Internal coverage #
	US_LOWRES-ID	4	5	B			Internal coverage ID
	GRID-CODE	4	8	B			Low residential code
							21 = Low intensity residential
	FIPSSTCO	5	5	C			State and County FIPS
	STATE	66	66	C			State Name
	COUNTY	66	66	C			County Name
	STFID	8	19	F	11		State and County FIPS

Ports:  US_PORTS2K / Point *
Source:  US Army Corps of Engineers

    ITEM NAME      WIDTH OUTPUT TYPE NO.DEC	DESCRIPTION
	AREA	4	12	F	3		Internal item
	PERIMETER	4	12	F	3		Internal item
	US_PORTS2K#	4	5	B			Internal coverage #
	US_PORTS2K-ID	4	5	B			Internal coverage ID
	NAME	30	30	C			Name of Port
	ADDRESS	40	40	C			Port Address
	CITY	30	30	C			City Name
	STATE	2	2	C			State Abbreviation
	ZIPCODE	10	10	C			Zipcode
	OWNER	25	25	C			Port Owner
	FUNCTION	10	10	C			Main Function (Type of Goods)										of Port
	BERTHS	4	3	B			Number of Berths
	LAT	8	10	F	6		Latitude
	LONG	8	11	F	6		Longitude

Dry Cleaners:  US_DRYCLEAN / Polygon *
Source:  US Census Bureau – Zip Code Statistics

    ITEM NAME      WIDTH OUTPUT TYPE NO.DEC	DESCRIPTION
	AREA	4	12	F	3		Polygon Area (m2)
	PERIMETER	4	12	F	3		Polygon perimeter
	US_DRYCLEAN#	4	5	B			Internal coverage #
	US_DRYCLEAN-ID	4	5	B			Internal coverage ID
	ZCTA	5	5	C			Census designated Zip Code
	FREQUENCY	4	5	B			Frequency of the same Zip Code
	SUM-AREA-H	8	18	F	6		Area of Zip Code in hectares
	NAME	90	90	C			Zip Code
	NO_EST	8	19	F	11		Total Number of Drycleaners in Zip Code
	EMP1_4	8	19	F	11		Total Number of Drycleaners in Zip Code	
							With 1 – 4 employees
	EMP5_9	8	19	F	11		Total Number of Drycleaners in Zip Code
							With 5 – 9 employees
	EMP10_19	8	19	F	11		Total Number of Drycleaners in Zip Code
							With 10 – 19 employees
	EMP20_49	8	19	F	11		Total Number of Drycleaners in Zip Code
							With 20 – 49 employees
	EMP50_99	8	19	F	11		Total Number of Drycleaners in Zip Code
							With 50 – 99 employees
	EMP100_249	8	19	F	11		Total Number of Drycleaners in Zip Code
							With 100 – 249 employees
	EMP250_499	8	19	F	11		Total Number of Drycleaners in Zip Code
							With 250 – 499 employees
	EMP500_999	8	19	F	11		Total Number of Drycleaners in Zip Code
							With 500 – 999 employees
	EMP1000_	8	19	F	11		Total Number of Drycleaners in Zip Code
							With more than 1000 employees
	PO_NAME	28	28	C			Name of Post Office serving zip code
	STATE	2	2	C			State Abbreviation
	AREA-H	4	12	F	3		Area of polygon in hectares
	RATIO	8	8	N	6		% of polygon that is part of zipcode
							(many zipcodes have more than 
								one polygon)
	NO_EST_RECAL	8	8	F	6		Number of drycleaners in polygon

Gas Stations (SIC 5541):  US_GAS_STA / Polygon *
Source:  Business Counts data

    ITEM NAME      WIDTH OUTPUT TYPE NO.DEC	DESCRIPTION
	AREA	4	12	F	3		Polygon Area (m2)
	PERIMETER	4	12	F	3		Polygon perimeter
	US_GAS_STA#	4	5	B			Internal coverage #
	US_GAS_STA-ID	4	5	B			Internal coverage ID
	STFID	12	12	C			State, County, Tract, Block Group ID
	COUNTYNAME	20	20	C			County Name
	STATENAME	25	25	C			State Name
	NUM_OF_GAS	8	9	F	0		Number of Gas Stations in Census Blk Group
	AREA-H	4	12	F	3		Area of polygon (block group) in hectares
	STCTYTR	11	11	C			State, County, Tract ID

Navigable Waterways:  US_NAV_H20 / Line *
Source:  US Army Corps of Engineers

    ITEM NAME      WIDTH OUTPUT TYPE NO.DEC	DESCRIPTION
	FNODE#	4	5	B			From node #
	TNODE#	4	5	B			To node #
	LPOLY#	4	5	B			Left polygon #
	RPOLY#	4	5	B			Right polygon #
	LENGTH	4	12	F	3		Length in meters
	US_NAV_H20#	4	5	B			Internal coverage #
	US_NAV_H20-ID	4	5	B			Internal coverage ID
	DESCRIPT	35	35	C			Description of Location
	LINKNAME	35	35	C			Description of waterway
	GEO	1	1	C			Geographic Class	
							G = Great Lakes
							O = Ocean
							I = Inland
	FUNC	2	2	C			Functional Class
							N = No traffic
							S = Shallow draft
							D = Deep draft
							B = Both
	WTYPE	4	6	B			Waterway Type		
							1 = Harbor
							2 = Intracoastal Waterway
							3 = Sealane
							4 = Sealane with separation zone
							5 = Open Water
							6 = River, creek, thoroughfare,lake
							7 = Estuary
							8 = Channel
							9 = Canal
							10 = Great Lakes direct link
							11 = Great Lakes indirect link
							12 = Corp of Engineers Lock
	CHART	15	15	C			4 or 5 digit NOAA chart number or 15
							Character 1:100K USGS Quad Map ID
	STATE	2	2	C			State Abbreviation
	FIPSSTCO	5	5	C			State and County FIPS code
	COUNTY	66	66	C			County Name
	STFID	8	19	F	11		State and County FIPS code

USGS Mines:  MINES_USGS / Point *
Source:  USGS

    ITEM NAME      WIDTH OUTPUT TYPE NO.DEC	DESCRIPTION
	AREA	4	12	F	3		Internal item
	PERIMETER	4	12	F	3		Internal item
	MINES_USGS#	4	5	B			Internal coverage #
	MINES_USGS-ID	4	5	B			Internal coverage ID
	COMP_NAME	73	73	C			Company Name
	OPER_NAME	43	43	C			Operator Name
	LATITUDE	8	11	F	8		Latitude
	LONGITUDE	8	12	F	8		Longitude
	OPER_TYPE	30	30	C			Type of Operation
	COMMODITY	13	13	C			Type of Commodity
	FIPSSTCO	5	5	C			State and County FIPS
	STFID	8	19	F	11		State and County FIPS

NLCD Mines:  MINES_NLCD / Polygon *
Source: NLCD – National Land Cover Dataset

    ITEM NAME      WIDTH OUTPUT TYPE NO.DEC	DESCRIPTION
	AREA	4	12	F	3		Polygon area (m2)
	PERIMETER	4	12	F	3		Polygon perimeter
	MINES_NLCD#	4	5	B			Internal coverage #
	MINES_NLCD-ID	4	5	B			Internal coverage ID
	MINE-CODE	4	8	B			32 = Mine Area
							99 = Non Mine Area
	FIPSSTCO	5	5	C			State and County FIPS
	STATE	66	66	C			State Name
	COUNTY	66	66	C			County Name
	STFID	8	19	F	11		State and County FIPS

Natural Gas Facilities:  US_NAT_GAS / Point *
Source: FEMA – HAZUS data

    ITEM NAME      WIDTH OUTPUT TYPE NO.DEC	DESCRIPTION
	AREA	4	12	F	3		Polygon area (m2)
	PERIMETER	4	12	F	3		Polygon perimeter
	US_NAT_GAS#	4	5	B			Internal coverage #
	US_NAT_GAS-ID	4	5	B			Internal coverage ID
	NAME	30	30	C			Name of Facility
	CITY	30	30	C			Name of City
	STATE	2	2	C			State abbreviation
	ZIPCODE	10	10	C			Zipcode
	FUNCTION	10	10	C			Main function of facility
	LAT	8	10	F	6		Latitude
	LONG	8	11	F	6		Longitude
	FIPSSTCO	5	5	C			State and County FIPS

Wastewater Treatment Plants:  US_WWTP / Point *
Source:  FEMA – HAZUS data

    ITEM NAME      WIDTH OUTPUT TYPE NO.DEC	DESCRIPTION
	AREA	4	12	F	3		Internal item
	PERIMETER	4	12	F	3		Internal item
	US_WWTP#	4	5	B			Internal coverage #
	US_WWTP-ID	4	5	B			Internal coverage ID
	NAME	30	30	C			Name of facility
	CITY	30	30	C			Name of City
	STATE	2	2	C			State abbreviation
	ZIPCODE	10	10	C			Zipcode
	LAT	8	10	F	6		Latitude
	LONG	8	11	F	6		Longitude
	FIPSSTCO	5	5	C			State and County FIPS

Urban Areas:  US_URBAN / Polygon *
Source:  US Census Bureau Designated Urban Areas

    ITEM NAME      WIDTH OUTPUT TYPE NO.DEC	DESCRIPTION
	AREA	4	12	F	3		Polygon area
	PERIMETER	4	12	F	3		Polygon perimeter
	US_URBAN#	4	5	B			Internal coverage #
	US_URBAN-ID	4	5	B			Internal coverage ID
	UA	5	5	C			UA ID # from Census Bureau
	NAME	90	90	C			Name of Urban Area
	UA_TYPE	2	2	C			Census Bureau designation
							UA = Urban Area
							UC = Urban Cluster
	FIPSSTCO	5	5	C			State and County FIPS
	STATE	66	66	C			State Name
	COUNTY	66	66	C			County Name
	STFID	8	19	F	11		State and County FIPS

Oil Facilities:  US_OIL / Point *
Source:  FEMA – HAZUS data

    ITEM NAME      WIDTH OUTPUT TYPE NO.DEC	DESCRIPTION
	AREA	4	12	F	3		Internal item
	PERIMETER	4	12	F	3		Internal item
	US_OIL#	4	5	B			Internal coverage #
	US_OIL-ID	4	5	B			Internal coverage ID
	NAME	30	30	C			Facility Name
	CITY	30	30	C			City Name
	STATE	2	2	C			State abbreviation
	ZIPCODE	10	10	C			Zip code
	OWNER	25	25	C			Facility Owner
	FUNCTION	10	10	C			Facility function
							(Tank farm or production plant)
	LAT	8	10	F	6		Latitude
	LONG	8	11	F	6		Longitude
	FIPSSTCO	5	5	C			State and County FIPS
	STFID	8	19	F	11		State and County FIPS

Airports:  US_AIR-PT / Point *
Source:  Bureau of Transportation Statistics and FAA

    ITEM NAME      WIDTH OUTPUT TYPE NO.DEC	DESCRIPTION
	AREA	4	12	F	3		Internal item
	PERIMETER	4	12	F	3		Internal item
	US_AIR-PT#	4	5	B			Internal coverage #
	US_AIR-PT-ID	4	5	B			Internal coverage ID
	SITE_NO	11	11	C			Site Number
	LOCID	4	4	C			FAA Location ID
	FAC_TYPE	13	13	C			Facility Type
	FAA_REGION	3	3	C			FAA Region
	FAA_DIST	4	4	C			FAA District
	STFIPS	2	2	C			State FIPS
	ST_NAME	20	20	C			State Name
	COUNTY	21	21	C			County Name
	CITY	26	26	C			City Name
	FULL_NAME	42	42	C			Airport Name
	FAC_USE	2	2	C			Facility Use
							(Public or Private)
	OWN_TYPE	2	2	C			Type of Owner	
							MA = Air Force
							MN = Navy
							MR = Army
							PR = Private
							PU = Public
	LONGITUDE	8	19	F	7		Longitude
	LATITUDE	8	19	F	7		Latitude
	ELEV	8	11	F	0		Elevation
	CNTL_TWR	1	1	C			Control Tower Status
	AIRPORT_NA	33	33	C			Airport Name
	LOCID_1	6	6	C			Location ID
	LARGE_CERT	17	17	C			Total commercial enplanements (year 2000)
	COMMUTER_	18	18	C			Total commuter enplanements (year 2000)
	AIR_TAXI_	11	11	C			Total Air Taxi enplanements (year 2000)
	FOREIGN_FL	9	9	C			Total Foreign Flight enplanements(2000)
	TOTAL	10	10	C			Total enplanements (year 2000)
	U_S_TOTAL	6	6	C			% of total U.S. enplanements (year 2000)
	FIPSSTCO	5	5	C			State and County FIPS
	STATE	66	66	C			State Name
	STFID	8	19	F	11		State and County FIPS

Possible Timber Locations:  US_TIMB / Point
Source:  US Forest Service – Forest inventory and analysis survey

    ITEM NAME      WIDTH OUTPUT TYPE NO.DEC	DESCRIPTION
	AREA	4	12	F	3		Internal item
	PERIMETER	4	12	F	3		Internal item
	US_TIMB#	4	5	B			Internal coverage #
	US_TIMB-ID	4	5	B			Internal coverage ID
	X-COORD	4	12	F	3		X coordinate
	Y-COORD	4	12	F	3		Y coordinate
	RECNO	6	6	I			Record Number
	ID	12	12	I			ID Number
	EXP-FACTOR	6	6	I			represents how big the timber site is
	STCK-PCT	3	3	I			
	FOR-TYPE	2	2	I			Forest Type
							00 = White – Red – Jack Pine
							01 = Jack Pine
							02 = Red Pine
							03 = White Pine
							04 = White Pine – Hemlock
							05 = Hemlock
							06 = Scotch Pine
							07 = Ponderosa Pine
							10 = Spruce – Fir
							11 = Balsam Fir
							12 = Black Spruce
							13 = Red Spruce – balsam fir
							14 = Northern white cedar
							15 = Tamarack
							16 = White Spruce
							17 = Norway Spruce
							18 = Larch
							19 = Red Spruce
							20 = Longleaf – Slash Pine
							21 = Longleaf Pine
							22 = Slash Pine
							30 = Loblolly – Shortleaf Pine
							31 = Loblolly Pine
							32 = Shortleaf Pine
							33 = Virginia Pine
							34 = Sand Pine
							35 = Eastern redcedar
							36 = Pond Pine
							37 = Spruce Pine
							38 = Pitch Pine
							39 = Table-mountain pine
							40 = Oak-Pine
							41 = White Pine – northern red oak – wash
							42 = Eastern redcedar – hardwood
							43 = Longleaf pine – scrub oak
							44 = Shortleaf pine – oak
							45 = Virginia pine – southern red oak
							46 = Loblolly pine – hardwood
							47 = Slash pine – hardwood
							49 = Other oak – pine
							50 = Oak – Hickory
							51 = Post oak, black oak or bear oak
							52 = Chestnut oak
							53 = White oak – red oak – hickory
							54 = White oak
							55 = Northern red oak
							56 = Yellow-poplar-white oak-no.red oak
							57 = Southern scrub oak
							58 = Sweetgum – yellow-polar
							59 = Mixed central hardwoods
							60 = Oak-Gum-Cypress
							61 = Swamp chestnut oak – cherrybark oak
							62 = Sweetgum – Nuttall oak – willow oak
							63 = Sugarberry-American elm-green ash
							65 = Overcup oak – water hickory
							66 = Atlantic white cedar
							67 = Baldcypress – water tupelo
							68 = Sweetbay – swamp tupelo –red maple
							69 = Palm-mangrove-other tropical
							70 = Elm – Ash – Cottonwood
							71 = Black ash – American elm – red maple
							72 = River birch – sycamore
							73 = cottonwood
							74 = Willow
							75 = Sycamore – pecan – American elm
							76 = Red maple – lowland
							79 = Mixed lowland hardwoods
							80 = Maple-Beech-Birch
							81 = Sugar maple –beech- yellow birch
							82 = Black cherry
							83 = Black walnut
							84 = Red maple – northern hardwood
							87 = Red maple – upland
							88 = Northern hardwood – reverting field
							89 = Mixed northern hardwoods
							90 = Aspen – Birch
							91 = Aspen
							92 = Paper Birch
							93 = Gray Birch
							94 = Balsam poplar
							99 = Nonstocked
	OWNER	2	2	I			Owner Type
							11 = National Forest
							12 = BLM Lands
							13 = Indian Lands
							14 = Other Federal Lands
							15 = State owned lands
							16 = County/Municipal lands
							20 = Forest Industry
							40 = Farmer owned
							50 = Farmer owned – leased
							60 = Other Private – Corporate
							70 = Other Private – Individual
							80 = Other Private-Corporate—Leased
							90 = Other Private-Individual—Leased
	FLAG	3	3	I			Everything is flagged 1 meaning
							These areas are possible sites of 
							Timber harvesting

US Population Change (based on 2000 census block groups):  USPOPCHG / Polygon
1990 versus 2000 population
Source:  US Census Bureau

    ITEM NAME      WIDTH OUTPUT TYPE NO.DEC	DESCRIPTION
	AREA	4	12	F	3		Area of polygon (m2)
	PERIMETER	4	12	F	3		Perimeter of polygon
	USPOPCHG#	4	5	B			Internal coverage #
	USPOPCHG-ID	4	5	B			Internal coverage ID
	STFID	12	12	C			State, County, Census Tract, 
							Census Block Group ID
	SUM-IDEN-POP90	8	18	F	6		Population of 1990 calculated to 
							2000 block group boundaries
	POP2000	8	19	F	11		2000 Population
	AREA-H	4	12	F	3		Area in hectares of polygon
	POPCHG	8	19	F	6		Calculated change in population
	STCTYTR	11	11	C			State, County, Census Tract ID

American Indian Reservations:  US_IR / Polygon
Source:  US Census Bureau boundaries

    ITEM NAME      WIDTH OUTPUT TYPE NO.DEC	DESCRIPTION
	AREA	4	12	F	3		Polygon area (m2)
	PERIMETER	4	12	F	3		Polygon perimeter
	US_IR#	4	5	B			Internal coverage #
	US_IR-ID	4	5	B			Internal coverage ID
	NAME	90	90	C			Name of reservation
	FIPSSTCO	5	5	C			State and County FIPS code
	STATE	66	66	C			State Name
	COUNTY	66	66	C			County Name
	STFID	8	19	F	11		State and County FIPS

National Forest Areas:  US_NF / Polygon
Source:  US Forest Service

	ITEM NAME      WIDTH OUTPUT TYPE NO.DEC	DESCRIPTION	
	AREA	4	12	F	3		Polygon area (m2)
	PERIMETER	4	12	F	3		Polygon perimeter
	US_NF#	4	5	B			Internal coverage #
	US_NF-ID	4	5	B			Internal coverage ID
	ADMIN_FOREST	50	50	C			National Forest Name
	FIPSSTCO	5	5	C			State and County FIPS
	STATE	66	66	C			State Name
	COUNTY	66	66	C			County Name
	STFID	8	19	F	11		State and County FIPS

Golf Courses:  US_GOLF / Point
Source:  Geographic Name Inventory System (USGS)

	ITEM NAME      WIDTH OUTPUT TYPE NO.DEC	DESCRIPTION	
	AREA	4	12	F	3		Internal item
	PERIMETER	4	12	F	3		Internal item
	US_GOLF#	4	5	B			Internal coverage #
	US_GOLF-ID	4	5	B			Internal coverage ID
	NAME	70	70	C			Golf course name
	FIPSSTCO	5	5	C			State and County FIPS
	STATE	66	66	C			State Name
	COUNTY	66	66	C			County Name
	STFID	8	19	F	11		State and County FIPS

Home Heating Fuel Used:  US_HEAT / Polygon
Source:  US Census Bureau STF3 File

	ITEM NAME      WIDTH OUTPUT TYPE NO.DEC	DESCRIPTION	
	AREA	4	12	F	3		Polygon area (m2)
	PERIMETER	4	12	F	3		Polygon perimeter
	US_HEAT#	4	5	B			Internal coverage #
	US_HEAT-ID	4	5	B			Internal coverage ID
	STFID	12	12	C			State, County, Census Tract, Block Group 
	STUSAB	2	2	C			State Abbreviation
	AREALAND	14	14	C			Area of Land in Block Group
							As calculated by Census Bureau
	AREAWATR	14	14	C			Area of Water in block group
							As calculated by census bureau
	NAME	90	90	C			Block Group Name
	SAMPLE_TOT_UNITS	8	20	F	5		Total sample of houses in
							Block group
	UTIL_GAS	8	20	F	5		Number of houses using utility gas as
							The primary heating fuel
	LP_GAS	8	20	F	5		Number of houses using bottled, tank
							Or LP gas as the primary heating fuel
	ELEC	8	20	F	5		Number of houses using electricity
							As the primary heating fuel
	FUEL_OIL	8	20	F	5		Number of houses using fuel oil or
							Kerosene as the primary heating fuel
	COAL	8	20	F	5		Number of houses using coal or coke
							As the primary heating fuel
	WOOD	8	20	F	5		Number of houses using wood as the
							Primary heating fuel
	SOLAR	8	20	F	5		Number of houses using solar power as
							The primary heating fuel
	OTHER	8	20	F	5		Number of houses using other types of
							Fuel as the primary heating fuel
	NO_FUEL	8	20	F	5		Number of houses using no fuel for heating
	FIPSSTCO	5	5	C			State and County FIPS code
	TRACT	6	6	C			Census Tract Number
	GROUP	1	1	C			Census Block Group Number
	STATE	2	2	C			State FIPS code
	COUNTY	3	3	C			County FIPS code 
	BLKGRP	1	1	C			Block Group Number
	AREA-H	4	12	F	3		Polygon area in hectares
	STCTYTR	11	11	C			State, County and Tract Number
	AREALANDA	14	14	I			Area of land as calculated by Census Bureau
	AREAWATRA	14	14	I			Area of water as calculated by Census 
		

Four Mile Urban Buffer:  US_BUFF / Polygon
Source:  Buffer created around Census Bureau’s designated urban areas

	ITEM NAME      WIDTH OUTPUT TYPE NO.DEC	DESCRIPTION
	AREA	4	12	F	3		Polygon area (m2)
	PERIMETER	4	12	F	3		Polygon perimeter
	US_BUFF#	4	5	B			Internal coverage #
	US_BUFF-ID	4	5	B			Internal coverage ID
	BUFFERDIST	8	19	F	3		Buffer Distance
							4 = 4 Miles
	BUFFERCODE	4	10	B			1 = Urban Buffer
							2 = Urban Area
							0 =  Non Urban area and non urban buffer
								area
	UA	5	5	C			Census Bureau ID Number
	NAME	90	90	C			Name of Urban Area
	UA_TYPE	2	2	C			Census Designation
							UA = Urban Area
							UC = Urban Cluster
	BUFFAREA-H	4	12	F	3		Polygon area in hectares
	FIPSSTCO	5	5	C			State and County FIPS
	STATE	66	66	C			State Name
	COUNTY	66	66	C			County Name
	STFID	8	19	F	11		State and County FIPS









