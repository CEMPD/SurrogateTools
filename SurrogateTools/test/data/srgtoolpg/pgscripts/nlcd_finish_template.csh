#!/bin/csh -fx

# gen_nlcd_finish_900921.sh shell script: 
# execute once for each surrogate to process from NLCD data
# change value assigned to surrogate_code to change the surrogate being processed
# Final script to run for a surrogate when processing NLCD data (run after 1st, top for each land cover category, and gather complete successfully)
# NOTE: taken from gen_forest scripts from QA 36-km scripts

# prerequisites
# - county shapefile, weight polygon shapefile, and grid cells geometries loaded
# - indices on all geometry columns, cluster, vacuum, & analyze performed (NOT cluster on county shapefile)
# - gen_nlcd_gather_900921.sh script completed successfully


VAR_DEFINITIONS


set grid_table=$schema.$grid		# name of table for grid
set geom_grid=$grid_table.gridcell			# field name of grid geometry (Polygon)

set cty_table=$schema.cb_2014_us_counties		# name of table for county
set geom_cty="$cty_table.geom_$grid_proj"		# field name of county geometry (MultiPolygon)
set cty_attr="geoid"				# unique ID field in county table

$PGBIN/psql -q $dbname << END
DROP TABLE IF EXISTS $schema.numer_${grid}_${surg_code};
DROP TABLE IF EXISTS $schema.surg_${grid}_${surg_code};
END

printf "CREATE TABLE $schema.numer_${grid}_${surg_code}\n" > create_numer_${surg_code}.sql
printf "\t($cty_attr varchar(6) not null,\n" >> create_numer_${surg_code}.sql
printf "\tcolnum integer not null,\n" >> create_numer_${surg_code}.sql
printf "\trownum integer not null,\n" >> create_numer_${surg_code}.sql
printf "\tnumer_${grid_proj} double precision,\n" >> create_numer_${surg_code}.sql
printf "\tprimary key ($cty_attr, colnum, rownum));\n" >> create_numer_${surg_code}.sql

echo "CREATE TABLE $schema.numer_${grid}_${surg_code}; primary key [numerator for surrogate calculation each county/grid cell]"
$PGBIN/psql -h $server -d $dbname -U $user -f create_numer_${surg_code}.sql

printf "CREATE TABLE $schema.surg_${grid}_${surg_code}\n" > create_surg_${surg_code}.sql
printf "\t(surg_code integer not null,\n" >> create_surg_${surg_code}.sql
printf "\t$cty_attr varchar(6) not null,\n" >> create_surg_${surg_code}.sql
printf "\tcolnum integer not null,\n" >> create_surg_${surg_code}.sql
printf "\trownum integer not null,\n" >> create_surg_${surg_code}.sql
printf "\tsurg_${grid_proj} double precision default 0.0,\n" >> create_surg_${surg_code}.sql
printf "\tnumer_${grid_proj} double precision default 0.0,\n" >> create_surg_${surg_code}.sql
printf "\tdenom_${grid_proj} double precision default 0.0,\n" >> create_surg_${surg_code}.sql
printf "\tprimary key ($cty_attr, colnum, rownum));" >> create_surg_${surg_code}.sql

echo "CREATE TABLE $schema.surg_${grid}_${surg_code}; add primary key"
$PGBIN/psql -h $server -d $dbname -U $user -f create_surg_${surg_code}.sql

printf "insert into $schema.numer_${grid}_${surg_code} ($cty_attr, colnum, rownum, numer_${grid_proj})\n" > fill_numer_${surg_code}.sql
printf "\tSELECT $cty_attr,\n" >> fill_numer_${surg_code}.sql
printf "\tcolnum,\n" >> fill_numer_${surg_code}.sql
printf "\trownum,\n" >> fill_numer._${surg_code}sql
printf "\tsum(area_wp_cty_cell_${grid_proj})\n" >> fill_numer_${surg_code}.sql
printf "  FROM $schema.wp_cty_cell_nlcd_${grid}\n" >> fill_numer_${surg_code}.sql
printf "  where $lc_function\n" >> fill_numer_${surg_code}.sql
printf "  group by $cty_attr, colnum, rownum;" >> fill_numer_${surg_code}.sql

echo "Populate $schema.numer_${grid}_${surg_code}"
$PGBIN/psql -h $server -d $dbname -U $user -f fill_numer_${surg_code}.sql

printf "insert into $schema.surg_${grid}_${surg_code} (surg_code, $cty_attr, colnum, rownum, numer_${grid_proj})\n" > fill_surg_${surg_code}.sql
printf "\tSELECT $surg_code,\n" >> fill_surg_${surg_code}.sql
printf "\t$cty_attr,\n" >> fill_surg_${surg_code}.sql
printf "\tcolnum,\n" >> fill_surg_${surg_code}.sql
printf "\trownum,\n" >> fill_surg_${surg_code}.sql
printf "\tnumer_${grid_proj}\n" >> fill_surg_${surg_code}.sql
printf "  FROM $schema.numer_${grid}_${surg_code};" >> fill_surg_${surg_code}.sql

echo "Populate $schema.surg_${grid}_${surg_code}"
$PGBIN/psql -h $server -d $dbname -U $user -f fill_surg_${surg_code}.sql

printf "update $schema.surg_${grid}_${surg_code}\n" > finalize_surg_${surg_code}.sql
printf "\tset denom_${grid_proj} = \n" >> finalize_surg_${surg_code}.sql
printf "\t\t(select sum(denom_${grid_proj}) from $schema.denom_nlcd_${grid_proj}\n" >> finalize_surg_${surg_code}.sql # start here
printf "\t\twhere denom_nlcd_${grid_proj}.$cty_attr = surg_${grid}_${surg_code}.$cty_attr \n" >> finalize_surg_${surg_code}.sql
printf "\t\tand denom_nlcd_${grid_proj}.${lc_function}\n" >> finalize_surg_${surg_code}.sql
printf "\t\tgroup by $cty_attr)\n" >> finalize_surg_${surg_code}.sql
printf "\tfrom $schema.denom_nlcd_${grid_proj}\n" >> finalize_surg_${surg_code}.sql
printf "\twhere $schema.surg_${grid}_${surg_code}.$cty_attr = $schema.denom_nlcd_${grid_proj}.$cty_attr;\n" >> finalize_surg_${surg_code}.sql
printf "update $schema.surg_${grid}_${surg_code}\n" >> finalize_surg_${surg_code}.sql
printf "\tset surg_${grid_proj} = numer_${grid_proj} / denom_${grid_proj};" >> finalize_surg_${surg_code}.sql

echo "Finalize surg"
$PGBIN/psql -h $server -d $dbname -U $user -f finalize_surg_${surg_code}.sql

echo "Exporting surrogates srg_${grid}_${surg_code}"
if ( ! -d output/$grid ) mkdir -p output/$grid
echo "#GRID" > output/${grid}/USA_${surg_code}_NOFILL.txt

$PGBIN/psql --field-separator '	' -t -a --no-align ${dbname} << END >> output/${grid}/USA_${surg_code}_NOFILL.txt 
SELECT *
  FROM $schema.surg_${grid}_${surg_code} order by $cty_attr, colnum, rownum;
END

echo "Surrogatee $surg_code complete";
