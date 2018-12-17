#!/bin/csh

# 100 - Population gen_acs_100.sh
# prerequisites
# - indices on all geometry columns
# - all files in same (final) projection
# - already cut by geographic boundaries & grid cells

set PGBIN = /proj/ie/apps/SA/Spatial-Allocator/pg_srgcreate/postgresql-9.5.3/local/bin
set weight_shape = ACS_2014_5YR_PopHousing
setenv surg_code 100					# SG.csv: SURROGATE_CODE

setenv dbname NEI2014      				# SC.csv: DBNAME
setenv data_attribute geoid				# SS.csv: DATA ATTRIBUTE
setenv weight_attribute pop2014				# SS.csv: WEIGHT ATTRIBUTE
setenv schema_name public    				# SC.csv: SCHEMA_NAME
setenv server localhost
setenv user zeadelma
setenv shp_tbl `echo $weight_shape | tr "[:upper:]" "[:lower:]"`                # SC.csv: SHP_TABLEI
setenv srid_final 900921				# SC.csv: SRID_FINAL
setenv region USA					# SS.csv: REGION
setenv grid conus12km_444x336
setenv surrogate_path /proj/ie/proj/EMAQ/Platform/Surrogates/2014/Spatial-Allocator/pg_srgcreate/output/ 	#SScsv: SURROGATE_PATH
setenv logfile gen_acs_100.log

echo "Surrogate 100 begins." > ${logfile}

printf "DROP TABLE IF EXISTS ${schema_name}.denom_${surg_code};\n" > ${surg_code}_denom.sql
printf "CREATE TABLE ${schema_name}.denom_${surg_code} (${data_attribute} varchar (6) not null,\n" >> ${surg_code}_denom.sql
printf "\tdenom double precision default 0.0,\n" >> ${surg_code}_denom.sql
printf "\tprimary key (${data_attribute}));\n" >> ${surg_code}_denom.sql

printf "insert into ${schema_name}.denom_${surg_code}\n" >> ${surg_code}_denom.sql
printf "SELECT ${data_attribute},\n" >> ${surg_code}_denom.sql
printf "\t${weight_attribute} AS denom\n" >> ${surg_code}_denom.sql
printf "\t FROM ${schema_name}.${shp_tbl}_denom_${srid_final};" >> ${surg_code}_denom.sql
  
echo "Creating denominator table"
$PGBIN/psql -h $server -d $dbname -U $user -f ${surg_code}_denom.sql

setenv vac_dbname ${dbname}
setenv vac_schema ${schema_name}
setenv vac_table denom_${surg_code}

../util/vacanalyze.sh

if ( -e ${surg_code}_numer.sql ) rm -f ${surg_code}_numer.sql
printf "DROP TABLE IF EXISTS ${schema_name}.numer_${surg_code};\n" > ${surg_code}_numer.sql
printf "CREATE TABLE ${schema_name}.numer_${surg_code} (${data_attribute} varchar (6) not null,\n" >> ${surg_code}_numer.sql
printf "\tcolnum integer not null,\n" >> ${surg_code}_numer.sql
printf "\trownum integer not null,\n" >> ${surg_code}_numer.sql
printf "\tnumer double precision default 0.0,\n" >> ${surg_code}_numer.sql
printf "\tprimary key (${data_attribute}, colnum, rownum));\n" >> ${surg_code}_numer.sql

printf "insert into ${schema_name}.numer_${surg_code}\n" >> ${surg_code}_numer.sql
printf "SELECT ${data_attribute},\n" >> ${surg_code}_numer.sql
printf "\tcolnum,\n" >> ${surg_code}_numer.sql
printf "\trownum,\n" >> ${surg_code}_numer.sql
printf "SUM(${weight_attribute}) AS numer\n" >> ${surg_code}_numer.sql
printf "FROM ${schema_name}.cty_${shp_tbl}_${grid}\n" >> ${surg_code}_numer.sql
printf "GROUP BY ${data_attribute}, colnum, rownum;\n" >> ${surg_code}_numer.sql

echo "Creating numerator table"
$PGBIN/psql -h $server -d $dbname -U $user -f ${surg_code}_numer.sql

setenv vac_dbname ${dbname}
setenv vac_schema ${schema_name}
setenv vac_table numer_${surg_code}

../util/vacanalyze.sh

if ( -e ${surg_code}_srg.sql ) rm -f ${surg_code}_srg.sql 
printf "DROP TABLE IF EXISTS ${schema_name}.surg_${surg_code};\n" > ${surg_code}_srg.sql
printf "CREATE TABLE ${schema_name}.surg_${surg_code} (surg_code integer not null,\n" >> ${surg_code}_srg.sql
printf "\t${data_attribute} varchar (6) not null,\n" >> ${surg_code}_srg.sql
printf "\tcolnum integer not null,\n" >> ${surg_code}_srg.sql
printf "\trownum integer not null,\n" >> ${surg_code}_srg.sql
printf "\tsurg real default 0.0,\n" >> ${surg_code}_srg.sql
printf "\tnumer real,\n" >> ${surg_code}_srg.sql
printf "\tdenom real,\n" >> ${surg_code}_srg.sql
printf "\tprimary key (${data_attribute}, colnum, rownum));\n" >> ${surg_code}_srg.sql

printf "insert into ${schema_name}.surg_${surg_code}\n" >> ${surg_code}_srg.sql
printf "SELECT CAST('$surg_code' AS INTEGER) AS surg_code,\n" >> ${surg_code}_srg.sql
printf "\td.${data_attribute},\n" >> ${surg_code}_srg.sql
printf "\tcolnum,\n" >> ${surg_code}_srg.sql
printf "\trownum,\n" >> ${surg_code}_srg.sql
printf "\tCAST( numer as double precision) / CAST( denom as double precision) AS surg,\n" >> ${surg_code}_srg.sql
printf "\tnumer,\n" >> ${surg_code}_srg.sql
printf "\tdenom\n" >> ${surg_code}_srg.sql
printf "FROM ${schema_name}.numer_${surg_code} n\n" >> ${surg_code}_srg.sql
printf "JOIN ${schema_name}.denom_${surg_code} d\n" >> ${surg_code}_srg.sql
printf "USING (${data_attribute})\n" >> ${surg_code}_srg.sql
printf "WHERE numer != 0\n" >> ${surg_code}_srg.sql
printf "AND denom != 0\n" >> ${surg_code}_srg.sql
printf "GROUP BY d.${data_attribute}, colnum, rownum, numer, denom\n" >> ${surg_code}_srg.sql
printf "ORDER BY d.${data_attribute}, colnum, rownum;\n" >> ${surg_code}_srg.sql

echo "Creating surrogate table"
$PGBIN/psql -h $server -d $dbname -U $user -f ${surg_code}_srg.sql
 
setenv vac_dbname ${dbname}
setenv vac_schema ${schema_name}
setenv vac_table surg_${surg_code}

../util/vacanalyze.sh
set echo
echo "ready to write surrogate file" >> ${logfile}
echo "#GRID" > ${surrogate_path}${region}_${surg_code}_NOFILL.txt 		# NEED TO PUT HEADER HERE

$PGBIN/psql --field-separator '	' -t -a --no-align ${dbname} << ENDS >> ${surrogate_path}${region}_${surg_code}_NOFILL.txt 

select surg_code, ${data_attribute}, colnum, rownum, surg, '!', numer, denom
	from ${schema_name}.surg_${surg_code}
	order by geoid, colnum, rownum;
ENDS
echo "Surrogate 100 ends." >> ${logfile}
