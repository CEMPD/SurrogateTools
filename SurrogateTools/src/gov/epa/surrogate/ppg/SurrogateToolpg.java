package gov.epa.surrogate.ppg;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
//import java.io.InputStream;
//import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Date;
import java.util.Hashtable;
import java.util.Iterator;
import java.util.List;
import java.util.Vector;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

//import gov.epa.surrogate.CSV;

/**
 * This program is used to generate surrogate ratios by calling srgcreate.exe and srgmerger.exe or java
 * merging/gapfilling tools based on csv and txt files.
 * 
 * Usage: java SurrogateTool control_variables_ppg.csv Requirement: srgcreate.exe, srgmerge.exe, SurrogateTools.jar and
 * java 2 Platform Standard Edition (J2SE) software Development Date: Feb-Mar 2017: added egrid
 * computation Developed by: IE/CEMPD at UNC
 */
public class SurrogateToolpg  {

	long      startTime, endTime;     // time in milliseconds
	
	Hashtable controls; // main control variables

	Hashtable shapefiles; // shapefile information

	Hashtable surrogates; // surrogate generation information

	Hashtable generations; // surrogates to be generated

	Hashtable surrogateIDs; // surrogate code information

	Hashtable srgDesc = new Hashtable();; // surrogate description information

	Hashtable runLog = new Hashtable(); // run execution status for each surrogate

	String logFile = null; // run log file name with format: srgRun_"date"_"time".log

	public static final int runFinish = 0; // run finish without error

	public static final int runContinue = 1; // run continue without error

	public static final int runError = 2; // run continue with error

	public static final int runStop = 3; // run stop due to error

	int prunStatus = 0; // overall status of the program run, initial value = 0 = runFinish

	String USER; // User who is running the program

	String CS, OS, FS, LS; // system related information: computer system, operating type, file separator, line
							// separator

	String COMMENT, CMD, EQ; // batch file or bash/csh file controls: comment sign, set environment variable command,
								// equal sign

	String TIME;
	String LIBPATH; // LD_LIBRARY_PATH for unix system

	String CONTROL_VARIABLE_FILE; // main control variable file for headers

	String SCRIPT_HEADER; // header for scripts .bat or .csh/.sh file

	String GRIDPOLY_HEADER; // GRID header for surrogates

	String[] COMMAND = new String[3]; // command to run external exe

	Vector mainEnv = new Vector(); // array to hold main environment variables
	Vector cmainEnv = new Vector(); // hold variables for srgcreate
	public static final String NOFILL_TYPE = "_NOFILL"; // srg file type without gapfills

	public static final String tDir = "temp_files"; // temp file directory name
	public static final String rowKey = "ROWKEY"; // key for list containg all keys in the csv
	

	boolean  run_create = false; // indicator to create, merge, and gapfill
																		// surrogates
    boolean  hasSrgMerge = true;
	
	// method launch log file and to set system related variables
	public void getSystemInfo() {

		//get the current time in  milliseconds
		startTime = System.currentTimeMillis(); 
		
		USER = System.getProperty("user.name");
		CS = System.getProperty("os.name").toLowerCase();
		OS = CS.toLowerCase();
		FS = System.getProperty("file.separator");
		LS = System.getProperty("line.separator");
		System.out.println("Your Operating system is:  " + OS + LS);

//		if (OS.indexOf("windows 9") > -1 || OS.indexOf("nt") > -1 || OS.indexOf("windows 20") > -1
//				|| OS.indexOf("windows xp") > -1) {
//			OS = "window";
//			COMMENT = ":: ";
//			CMD = "set";
//			EQ = "=";
//			TIME = "time /t";
//			SCRIPT_HEADER = "@echo off";
//			COMMAND[0] = "cmd.exe";
//			COMMAND[1] = "/c";
//			if (OS.indexOf("windows 9") > -1) {
//				COMMAND[0] = "command.com";
//			}
//		} else { 
			// assume Unix
			OS = "unix";
			COMMENT = "# ";
			CMD = "export";
			EQ = "=";
			TIME = " ";
			LIBPATH = System.getenv("LD_LIBRARY_PATH");
			SCRIPT_HEADER = "#!/bin/csh ";
			COMMAND[0] = "/bin/csh";
			COMMAND[1] = "-c";
//		}
	}

	public void createLogFile(String logFileName) {		
		
		// get running date and old date log file
		// SimpleDateFormat fmt = new SimpleDateFormat("yyyyMMdd_HHmmss");
		Date now = new Date();
		// String dt = fmt.format(now);
		// String logFileName = "srgRun_"+dt+".log";

		File cfile = new File(logFileName);
		File pfile = cfile.getParentFile();
		if (pfile == null) {
			// only specify name, so use the current directory for log file
			logFile = "." + FS + logFileName; // current directory
		} else {
			// check and make the directory of specified log file
			if (!pfile.exists()) {
				if (!pfile.mkdirs()) {
					System.out.println("Error: Creating the log file directory failed -- " + logFileName);
					System.exit(runStop);
				}
			}
			logFile = logFileName;
		}

		// move and set logFile
		int i = 1;
		String tempFile = logFile;
		while (checkFile(tempFile, "NONE", runError)) {
			tempFile = logFile + "." + Integer.toString(i);
			i++;
		}

		// rename existing log file to logFile.i
		if (!tempFile.equals(logFile)) {
			if (!renameFile(logFile, tempFile, runContinue)) {
				checkFile(logFile, "YES", runError); // delete the existing log file if renaming failed
			}
		}

		writeLogFile("\nRun Date: " + now.toString() + LS + LS, runContinue);
		System.out.println("\nRun Date: " + now.toString() + LS + LS);
	}

	// methods to track errors
	public void writeLogFile(String message, int runStatus) {

		// update run status
		if (runStatus != runFinish && runStatus != runContinue) {
			prunStatus = runStatus;
		}

		if (logFile == null) {
			System.out.println(message);
		} else {
			try {
				FileWriter fw = new FileWriter(logFile, true); // open logFile to append
				BufferedWriter logOut = new BufferedWriter(fw); // output file writer for log file
				logOut.write(message);

				if (runStatus == runFinish || runStatus == runStop) {
					String srgRunInfo = writeSrgRunLog();
					if (srgRunInfo != null) {
						logOut.write(srgRunInfo);
					}

					Date endDate = new Date();
					logOut.write(LS + "End Date: " + endDate.toString() + LS);
					System.out.println(LS + "End Date: " + endDate.toString() + LS);
					
					endTime = System.currentTimeMillis(); 
					double elapsed_time_minutes = (endTime - startTime)/(1000.0*60.0);
					logOut.write("Elapsed time in minutes: " + Double.toString(elapsed_time_minutes) + LS);
					System.out.println(LS + "Elapsed time in minutes: " + Double.toString(elapsed_time_minutes) + LS);
					
					if (runStatus == runStop) {
						logOut.write(LS + "ERROR -- The Program Run Stopped" + LS);
						System.out.println(LS + "ERROR -- The Program Run Stopped. See log file for details." + LS);
					}
					if (runStatus == runFinish && prunStatus == runFinish) {
						logOut.write(LS + "SUCCESS -- The Program Run Completed" + LS);
						System.out.println(LS + "SUCCESS -- The Program Run Completed. See log file for details." + LS);
					}
					if (runStatus == runFinish && prunStatus != runFinish) {
						logOut.write(LS + "FINISH -- The Program Run Finished with Some Errors" + LS);
						System.out.println(LS + "FINISH -- The Program Run Finished with Some Errors. See log file for details." + LS);
					}					
					
				}
				logOut.close();
			} catch (IOException e) {
				System.out.println("Error -- " + e.toString());
			}
		}

		if (runStatus == runFinish || runStatus == runStop) {
			System.exit(prunStatus); // program exit with prunStatus code
		}
	}
	
	 
	public void readControls(String fileName) {
		String[] items = { "VARIABLE", "VALUE" }; // first field--key field
		String[] gridVars = { "GENERATION CONTROL FILE", "SURROGATE SPECIFICATION FILE", 
				"SHAPEFILE CATALOG", "SURROGATE CODE FILE",  
				"OUTPUT_FORMAT", "OUTPUT_FILE_TYPE",
				"OUTPUT_GRID_NAME", "GRIDDESC",
				"OUTPUT_FILE_ELLIPSOID", "OUTPUT DIRECTORY", "PGSCRIPT DIRECTORY", 
				"OUTPUT SRGDESC FILE", "OVERWRITE OUTPUT FILES", "LOG FILE NAME",
				"SRID_FINAL", "DBNAME", 
				"PGBIN", "PG_SERVER", "PG_USER", "COMPUTE SURROGATES" };  

		int keyItems = 1; // 1 = first item is the key item
		ArrayList list = new ArrayList();
		int FIRST_ITEM = 0;
		//boolean checkStatus;
		int j;

		if (!checkFile(fileName, "NO", runContinue)) {
			System.out.println("Error: Control File does not exist -- " + fileName);
			System.exit(runStop);
		}

		// get full path control variable file
		File cf = new File(fileName);
		try {
			CONTROL_VARIABLE_FILE = cf.getCanonicalPath(); // save for header
			System.out.println("File name:"+CONTROL_VARIABLE_FILE);
		} catch (Exception e) {
			e.printStackTrace();
			System.exit(runStop);
		}

		ProcessCSV file = new ProcessCSV(fileName);
		controls = file.readCSV(items, keyItems);
		checkCSVError(controls, runStop); // check errors in reading csv file
		
		// first check and set log file path
		if (!controls.containsKey("LOG FILE NAME")) {
			writeLogFile("Error: Main control CSV file does not contain variable -- LOG FILE NAME" + LS, runStop);
		}
		list = (ArrayList) controls.get("LOG FILE NAME");
		logFile = (String) list.get(FIRST_ITEM);
		if (logFile.length() == 0 || logFile.matches("^\\s*$")) {
			logFile = null;
			writeLogFile("Error: Main control variable value for LOG FILE NAME is missing... " + LS, runStop);
		}
		createLogFile(logFile); // create log file name

		writeLogFile(LS + "\t\t" + "Main Control CSV File" + LS + LS, runContinue);
		// System.out.println("\t\tMain Control CSV File"+LS+LS);
		ArrayList rows = (ArrayList) controls.get(rowKey);
		for (j = 0; j < rows.size(); j++) {
			String key = (String) rows.get(j);
			list = (ArrayList) controls.get(key);
			String value = (String) list.get(0);
			//System.out.println("key: "+key+"\t"+value);
			
			writeLogFile(key + "\t" + value + LS, runContinue);
			if (value.length() == 0 || value.matches("^\\s*$")) {
				writeLogFile("Error: " + key + " does not have a value in " + CONTROL_VARIABLE_FILE + "." + LS, runStop);
			}

		}
		writeLogFile(LS, runContinue);
		 
		// check all needed variables exist
		String outputFType = getControls("OUTPUT_FILE_TYPE");
		if (outputFType.equals("RegularGrid")) {
			for (j = 0; j < gridVars.length - 2; j++) {
				if (!controls.containsKey(gridVars[j])) {
					System.out.println("key: "+gridVars[j]+"\t"+getControls(gridVars[j]));
					writeLogFile("Error: " + gridVars[j] + " does not exist in " + CONTROL_VARIABLE_FILE + "." + LS,
							runStop);
				}
			}
		}
		
		// check variables not needed
		// check varaibles settings
		
		if (!getControls("OUTPUT_FORMAT").equals("SMOKE")) {
			writeLogFile("Error: OUTPUT_FORMAT value has to be SMOKE" + LS, runStop);
		}
		 
		if (!outputFType.equals("RegularGrid") && !outputFType.equals("Polygon") && !outputFType.equals("EGrid")) {
			writeLogFile("Error: OUTPUT_FILE_TYPE value has to be RegularGrid, EGrid or Polygon" + LS, runStop);
		}
		 
		//check two output files
		String outputSrgFile = getControls("OUTPUT SURROGATE FILE");
		if (checkIsDirectory(outputSrgFile) ){
			writeLogFile("Error: OUTPUT SURROGATE FILE - " + outputSrgFile + " is an existing directory." + LS, runStop);
		}
			
		outputSrgFile = getControls("OUTPUT SRGDESC FILE");
		if (checkIsDirectory(outputSrgFile)){
			writeLogFile("Error: OUTPUT SRGDESC FILE - " + outputSrgFile + " is an existing directory." + LS, runStop);
		}
		
		String ow = getControls("OVERWRITE OUTPUT FILES");
		String OW = ow.toUpperCase();
		if (!OW.equals("YES") && (!OW.equals("NO"))) {
			writeLogFile("Error: Invalid OVERWRITE OUTPUT FILES entry in the main control file" + LS, runStop);
		}

		ow = getControls("COMPUTE SURROGATES");
		OW = ow.toUpperCase();
		if (!OW.equals("YES") && (!OW.equals("NO"))) {
			writeLogFile("Error: Invalid COMPUTE SURROGATES FROM SHAPEFILES entry in the main control file" + LS,
					runStop);
		}
		if (OW.equals("YES")) {
			run_create = true;
		}
	}

	// method to get control value for an item in control table
	public String getVarValue(String varName, Hashtable stable, String tname) {
		String value;
		ArrayList list = new ArrayList();
		int FIRST_ITEM = 0;
		
		list = (ArrayList) stable.get(varName);
		value = (String) list.get(FIRST_ITEM);
		if (value.length() == 0 || value.matches("^\\s*$")) {
			writeLogFile("Error: " + tname + " table is missing -- here" + varName + LS, runStop);
		}
		if (!stable.containsKey(varName)) {
			System.out.println("Error: " + tname +" does not contain variable name -- " + varName + "" + LS);
			writeLogFile("Error: " + tname +" does not contain variable name -- " + varName + "" + LS, runStop);
		}
		
		return value;
	}

	public void readSurrogateSpec() {
		String[] items = { "REGION", "SURROGATE CODE", "SURROGATE", "DATA SHAPEFILE", "DATA ATTRIBUTE",
				"WEIGHT SHAPEFILE", "WEIGHT ATTRIBUTE", "WEIGHT FUNCTION", "FILTER FUNCTION", "MERGE FUNCTION",
				"SECONDARY SURROGATE", "TERTIARY SURROGATE", "QUARTERNARY SURROGATE"}; // REGION+SURROGATE CODE = key
		//"SURROGATE FILE HEADER PATH", "SURROGATE FILE HEADER FILE",																				// field
		int keyItems = 2; // 2 = combined first two items is the key
//		ArrayList list = new ArrayList();
//		Iterator it;

		/** get Surrogate Specification File Name from control Hashtable* */
		String fileName = getControls("SURROGATE SPECIFICATION FILE");

		checkFile(fileName, "NO", runStop); // check CSV fileName exists

		ProcessCSV file = new ProcessCSV(fileName);
		surrogates = file.readCSV(items, keyItems);
		checkCSVError(surrogates, runStop);

		
//		 System.out.println(LS+"\t\t"+"Surrogate Specification CSV File"+LS+LS); ArrayList rows = (ArrayList)
//		 surrogates.get(rowKey); for (int j=0;j<rows.size();j++) { String key = (String) rows.get(j);
//		 System.out.println("key: "+key); list = (ArrayList) surrogates.get(key); it = list.iterator(); while
//		 (it.hasNext()) { System.out.println(it.next()); } } 
		 
	}

	// method to get surrogate list from a hashtable
	public ArrayList getSurrogateList(String key, int runStatus) {
		ArrayList list = new ArrayList();

		if (!surrogates.containsKey(key)) {
			writeLogFile("Error: Surrogate specification csv file does not contain surrogate -- " + key + LS, runStatus);
			return null;
		}
		list = (ArrayList) surrogates.get(key);
		return list;
	}
	
	public ArrayList getShapefilesList(String key, int runStatus) {
		ArrayList list = new ArrayList();

		if (!shapefiles.containsKey(key)) {
			writeLogFile("Error: Catalog csv file does not contain surrogate -- " + key + LS, runStatus);
			return null;
		}
		list = (ArrayList) shapefiles.get(key);
		return list;
	}

	public void readShapefiles() {
		String[] items = { "SHAPEFILE NAME", "DIRECTORY", "ELLIPSOID", "PROJECTION"}; // first field--key
		int keyItems = 1; // 1 = first item is the key
		// ArrayList list = new ArrayList();
		// Iterator it;

		/** get shpaefile information from shapefile catalog CSV file* */
		String fileName = getControls("SHAPEFILE CATALOG");
		checkFile(fileName, "NO", runStop); // check CSV fileName exists

		ProcessCSV file = new ProcessCSV(fileName);
		shapefiles = file.readCSV(items, keyItems);
		checkCSVError(shapefiles, runStop);

		/**
		 * System.out.println(LS+"\t\t"+"Shapefile Catalog CSV File"+LS+LS); ArrayList rows = (ArrayList)
		 * shapefiles.get(rowKey); for (int j=0;j<rows.size();j++) { String key = (String) rows.get(j);
		 * System.out.println("key: "+key); list = (ArrayList) shapefiles.get(key); it = list.iterator(); while
		 * (it.hasNext()) { System.out.println(it.next()); } }
		 */
	}

	// set item index in shapefile catalog
//	public static final int DIRECTORY_INDEX = 0, ELLIPSOID_INDEX = 1, PROJECTION_INDEX = 2; // shapefile name is the key
//																							// in the hashtable
	// read data in from generation file and compare entries with specification file
	public void readGenerations() {
		String[] items = { "REGION", "SURROGATE CODE", "SURROGATE", "GENERATE", 
				"QUALITY ASSURANCE", "PG SCRIPT", }; // REGION+SURROGATE	
		                                // CODE = key																								// field
		int keyItems = 2; // 2 = combined first two items is the key
		ArrayList list = new ArrayList();
		ArrayList srgList = new ArrayList();

		String fileName = getControls("GENERATION CONTROL FILE");
		checkFile(fileName, "NO", runStop); // check CSV fileName exists

		ProcessCSV file = new ProcessCSV(fileName);
		generations = file.readCSV(items, keyItems);
		checkCSVError(generations, runStop);

		// compare entries with specification file and error checking YES and NO columns
		int checkStatus = 0;
		ArrayList rows = (ArrayList) generations.get(rowKey);
		for (int j = 0; j < rows.size(); j++) {
			String key = (String) rows.get(j);
			list = (ArrayList) generations.get(key);

			// check surrogate names
			if ((srgList = getSurrogateList(key, runError)) == null) {
				checkStatus = runError;
				continue;
			}

			String gSrgName = (String) list.get(SG_SURROGATE_INDEX); //1
			String sSrgName = (String) srgList.get(SS_SURROGATE_INDEX); //1
			if (!gSrgName.equals(sSrgName)) {
				writeLogFile("Error: Surrogate Names are different in the generation and specification files -- " + key
						+ LS, runError);
				checkStatus = runError;
			}

			// check YES and NO entries
			String generate = (String) list.get(SG_GENERATE_INDEX);
			String g = generate.toUpperCase();
			if (!g.equals("YES") && (!g.equals("NO"))) {
				writeLogFile("Error: Wrong GENERATE entry in the generation file -- " + key + LS, runError);
				checkStatus = runError;
			}

			String quality = (String) list.get(SG_QUALITY_ASSURANCE_INDEX);
			String q = quality.toUpperCase();
			if (!q.equals("YES") && (!q.equals("NO"))) {
				writeLogFile("Error: Wrong QUALITY ASSURANCE entry in the generation file -- " + key + LS, runError);
				checkStatus = runError;
			}
		}

		if (checkStatus == runError) {
			writeLogFile(LS, runStop); // if the file has any error, exit the run
		}
	}

	public void readSurrogateCodes() {
		String[] items = { "NAME", "#CODE" }; // first field--key field
		int keyItems = 1; // first item is the key
		// ArrayList list = new ArrayList();
		// Iterator it;

		String fileName = getControls("SURROGATE CODE FILE");
		checkFile(fileName, "NO", runStop); // check CSV fileName exists

		ProcessCSV file = new ProcessCSV(fileName);
		surrogateIDs = file.readCSV(items, keyItems);
		checkCSVError(surrogateIDs, runStop);

		/**
		 * System.out.println(LS+"\t\t"+"Surrogate IDs CSV File"+LS+LS); ArrayList rows = (ArrayList)
		 * surrogateIDs.get(rowKey); for (int j=0;j<rows.size();j++) { String key = (String) rows.get(j);
		 * System.out.println("key: "+key); list = (ArrayList) surrogateIDs.get(key); it = list.iterator(); while
		 * (it.hasNext()) { System.out.println(it.next()); } }
		 */
	}

	// method to get surrogate code from a hashtable
	public String getSurrogateCodes(String varName, int runStatus) {
		String value;
		ArrayList list = new ArrayList();
		int FIRST_ITEM = 0;
		int CODE_INDEX = 1;

		if (!surrogateIDs.containsKey(varName)) {
			writeLogFile("Error: Surrogate Code File does not contain name -- " + varName + LS, runStatus);
			return null;
		}
		list = (ArrayList) surrogateIDs.get(varName);
		value = (String) list.get(FIRST_ITEM);
		String[] code = value.split("=");
		if (code[CODE_INDEX].matches("^\\s*$")) {
			writeLogFile("Error: Surrogate Code File is missing the code for -- " + varName + " --" + LS, runStatus);
			return null;
		}
		return code[CODE_INDEX];
	}

	// set indexes for output poly shapefile information for headers
	public static final int OUTPUT_POLY_FILE_INDEX = 4, OUTPUT_POLY_ATTR_INDEX = 5, OUTPUT_FILE_ELLIPSOID_INDEX = 6,
			OUTPUT_FILE_MAP_PRJN_INDEX = 7; // output file info for polygon output type

	public void setGentVariables() {
        String[] env_gitems = { "surrogate_path", "dbname", "srid_final", "grid", "PGBIN", "server", 
        		"user", "srgdesc_file"};
	    // names used in inputs files 
        String[] env_fitems = { "OUTPUT DIRECTORY", "DBNAME", "SRID_FINAL", "OUTPUT_GRID_NAME",
        		"PGBIN", "PG_SERVER", "PG_USER", "OUTPUT SRGDESC FILE"};
//        String[] env_ftypes = {"surrogates", "shapefiles", "surrogates", "surrogates",
//        		"shapefiles", "shapefiles", "surrogates", "controls", "controls"};
 
        		//SS, SC, SS, SS, SC, SC, SS, CONTROL, CONTROL
 
		// create environment variable Vector for grid output format
		if (run_create) {
			for (int i = 0; i < env_gitems.length; i++) {
				mainEnv.add(env_gitems[i] + "=" + getControls(env_fitems[i]));
			}
		}
		// add hard coded environment variables

		System.out.println(LS + "\t\t" + "Main Environment Variables for SRGCREATE" + LS + LS);
		for (int i = 0; i < mainEnv.size(); i++) {
			System.out.println((String) mainEnv.get(i)); // print out main environment variables
		}
	}

	// copy main environment into a vector for a surrogate computation
	public Vector copyMainVar() {
		Vector all = new Vector();

		for (int i = 0; i < mainEnv.size(); i++) {
			all.add(mainEnv.get(i));
		}
		return all;
	}
	
	public Vector copyCMainVar() {
		Vector all = new Vector();

		for (int i = 0; i < cmainEnv.size(); i++) {
			all.add(cmainEnv.get(i));
		}
		return all;
	}

	// output environment variables to a batch or bash/csh file
	public String writeFile(String dir, String tfile, String pgf, Vector env) {
		String line, key, value, setline;
		String outFile;

		if (!checkDir(dir, runError)) {
			return null;
		}
		
		if (OS.equals("window")) {
			outFile = dir + FS + tfile + ".bat";
		} else {
			// assume unix
			outFile = dir + FS + tfile + ".csh";
		}
		
		try {
			
			FileWriter fw = new FileWriter(outFile);
			BufferedWriter out = new BufferedWriter(fw);
			
			//String templateFile = "gov/epa/surrogate/ppg/sql/gen_" + ckey +"_noHead.txt" ;
			String templateFile = pgf;
			System.out.println(templateFile);
			
//			InputStream fr = this.getClass().getClassLoader().getResourceAsStream(templateFile);		
//			String templateFile = "test/data/srgtoolpg" + FS + "gen_" + ckey +"_noHead.txt" ;
//			FileReader fr = new FileReader(templateFile);

			 

			FileReader file = new FileReader(templateFile);
			BufferedReader in = new BufferedReader(file);
			
			while ( (line = in.readLine()) != null) {
				//System.out.println("CMD line:" + line);
				if (line.contains("VAR_DEFINITIONS") ) {
					for (int i = 0; i < env.size(); i++) {
						String all = (String) env.get(i);
						String[] var = all.split("=", 2);
						//key = var[0].toLowerCase();
						key = var[0];
						value = var[1];
						setline = "set " + key + EQ + value + LS;
						if ( key.contains("srid_final") ) // grid_proj=srid_final
							setline += "set grid_proj" + EQ + value + LS;
						if ( key.contains("schema_name") ) // grid_proj=srid_final
							setline += "set schema" + EQ + value + LS;
						out.write(setline);
					}
				}
				else
					out.write(line + LS);
			}
			// run the program
			line = TIME + LS;
			out.write(line);
			//line = exe + LS;
			out.write(line);
			//fr.close();
			out.close();
			File efile = new File(outFile);
			efile.setExecutable(true);

		} catch (IOException e) {
			e.printStackTrace();
			writeLogFile("Error -- " + e.toString() + LS, 7);
			return null;
		}
		return outFile;
	}


	public boolean checkRunMessage(String message) {

		Pattern p = Pattern.compile("ERROR IN RUNNING THE");
		Matcher m = p.matcher(message);
		if (m.find()) {
			writeLogFile(message + LS, runError);
			return true;
		}
		writeLogFile(message + LS, runContinue);
		return false;
	}


	private boolean renameFile(String oldFile, String newFile, int runStatus) {

		// File (or directory) with old name
		File file = new File(oldFile);

		// File (or directory) with new name
		File file2 = new File(newFile);

		// Rename file (or directory)
		boolean success = file.renameTo(file2);
		if (!success) {
			writeLogFile("Error: In Renaming File: " + oldFile + " to " + newFile + LS, runStatus);
			return false;
		}

		return success;
	}


	// add output surrogate file information into a hashtable
	private void addSrgDesc(String key, String srgName, String fileName) {

		// split key into region and code
		String[] rc = key.split("_"); // split partial function
		String region = rc[SS_REGION_INDEX];
		String code = rc[SS_SURROGATE_CODE_INDEX];

		// delete the existing entry
		if (srgDesc.containsKey(key)) {
			srgDesc.remove(key);
		}

		// add new entry
		String line = region + "," + code + ",\"" + srgName + "\"," + fileName;
		System.out.println("srgDesc Line = " + line);
		srgDesc.put(key, line);
	}



	// set item index in surrogate specification file and surrogate generation file
	// for generations: REGION+SURROGATE CODE = key
	// shapefiles: SHAPEFILE NAME
	// surrgates: SURROGATES
	public static final int SS_REGION_INDEX = 0, SS_SURROGATE_CODE_INDEX = 1, SS_SURROGATE_INDEX = 2, 
			SS_DATA_SHAPEFILE_INDEX = 3, SS_DATA_ATTRIBUTE_INDEX = 4, SS_WEIGHT_SHAPEFILE_INDEX = 5, 
			SS_WEIGHT_ATTRIBUTE_INDEX = 6, SS_WEIGHT_FUNCTION_INDEX = 7, SS_FILTER_FUNCTION_INDEX = 8, 
			SS_MERGE_FUNCTION_INDEX = 9,SS_SECONDARY_SURROGATE_INDEX = 10, SS_TERTIARY_SURROGATE_INDEX = 11, 
			QUARTERNARY_SURROGATE_INDEX = 12;

	// From surrogate generation
	public static final int SG_REGION_INDEX = 0, SG_SURROGATE_CODE_INDEX = 1, SG_SURROGATE_INDEX = 2, 
			SG_GENERATE_INDEX = 3, SG_QUALITY_ASSURANCE_INDEX = 4, SG_PGSCRIPT_INDEX=5;
	
	// From shapefile catelog
	public static final int SC_DBNAME_INDEX = 0; // Directory

	public static final int CREATE_STATUS_INDEX = 5, MERGE_STATUS_INDEX = 6, GAPFILL_STATUS_INDEX = 7; // processing
																										// status stored
																										// in																							// generation																					// table

	// set item index in surrogate code file
	public static final int CODE_INDEX = 0; // name is the key in the hashtable

	public ArrayList srgList;
	//public ArrayList specList;
	public ArrayList catalogList;

	public void generatePGSurrogates() {
		/**
		 * loop through and compute the surrogates through postgresSQL/postGIS
		 */
		ArrayList list = new ArrayList();
		 
		Vector allVar = new Vector();
		//String header;
		String outDir;
		String pgSoutDir;
		String srgFile;
		String pgTemFile;
		 
		//System.out.print("under construction");
		
		if (!checkDir(outDir = getControls("OUTPUT DIRECTORY"), runError)) {
			System.out.print("Error: output directory -- " + outDir  + " Does Not Exist");
			return; // srgcreate ends
		}
		
		if (!checkDir(pgSoutDir = getControls("PGSCRIPT DIRECTORY"), runError)) {
			System.out.print("Error: output directory -- " + pgSoutDir  + " Does Not Exist");
			return; // srgcreate ends
		}
		
		String ow = getControls("OVERWRITE OUTPUT FILES");
		String OW = ow.toUpperCase();
		 
		ArrayList rows = (ArrayList) generations.get(rowKey);
		for (int j = 0; j < rows.size(); j++) {
			String key = (String) rows.get(j);
			list = (ArrayList) generations.get(key);
			String generate = (String) list.get(SG_GENERATE_INDEX);
			String g = generate.toUpperCase();
			String pgf = (String) list.get(SG_PGSCRIPT_INDEX);

			ArrayList logList = getSrgLogList(key, (String) list.get(SG_SURROGATE_INDEX));
			//System.out.println("KEY: "+ key + " Generate: "+g);

			if ((srgList = getSurrogateList(key, runError)) == null) {
				putSrgRunLog(key, logList, "Failed: No specification data");
				continue;
			}
			
			if (g.equals("YES")) {
				System.out.println(LS + "Run PostgreSQL/PostgreGIS to generate surrogate ratios for " + key );
				writeLogFile(LS + "Run PostgreSQL/PostgreGIS to generate surrogate ratios for " + key + 
						LS, runContinue);
				
				if (checkFile(srgFile = outDir + FS + key + NOFILL_TYPE + ".txt", "NONE", runContinue)
						&& OW.equals("NO")) {
					// the output file exists and skip the surrogate
					writeLogFile(LS + "Surrogate ratio file exists for " + key + ": "
							+ (String) list.get(SG_SURROGATE_INDEX) + LS, runContinue);
					putSrgRunLog(key, logList, "SRGCREATE Skipped");
					continue;
				}
				
				checkFile(pgTemFile = pgSoutDir + FS + pgf, "NO", runStop);
					 
				
				// extract variable from SG
				allVar = copyMainVar(); // copy main env to the current surrogate
				allVar.add("surrogate_file=" + srgFile);
				allVar.add("surg_code=" + (String) list.get(SG_SURROGATE_CODE_INDEX));
				allVar.add("region=" + (String) list.get(SG_REGION_INDEX));
				allVar.add("data_shape=" + (String) srgList.get(SS_DATA_SHAPEFILE_INDEX));
				allVar.add("data_attribute=" + (String) srgList.get(SS_DATA_ATTRIBUTE_INDEX));
				allVar.add("weight_shape=" + (String) srgList.get(SS_WEIGHT_SHAPEFILE_INDEX));
				allVar.add("weight_attribute=" + (String) srgList.get(SS_WEIGHT_ATTRIBUTE_INDEX));
				allVar.add("weight_function=\"" + (String) srgList.get(SS_WEIGHT_FUNCTION_INDEX)+'"');
				allVar.add("filter_function=\"" + (String) srgList.get(SS_FILTER_FUNCTION_INDEX) + '"');
				
				//extract variables from calalog file: dbname,schema_name
				String ckey = (String) srgList.get(SS_DATA_SHAPEFILE_INDEX);
				if ((catalogList = getShapefilesList(ckey, runError)) == null) {
					putSrgRunLog(ckey, logList, "Failed: No calalog data. ");
					continue;
				}
				allVar.add("schema_name="+(String)catalogList.get(SC_DBNAME_INDEX));
				allVar.add("output_dir="+outDir);
				
				//add pg log file definition
				//allVar.add("logfile=generate_"+(String) list.get(SG_SURROGATE_CODE_INDEX)+".log");
				//System.out.println(LS + catalogList.size() + " "+catalogList.toString());
				
				String scripts = writeFile(outDir + FS + tDir, key + NOFILL_TYPE, pgTemFile, allVar);
				 
				if (scripts == null) {
					logList.add("Warning: failed to create bat or bash/csh file");
				}
				
				String[] env = new String[allVar.size()]; // put all environment variables in a String array
				System.out.println("\t\t" + "Environment Variables Settings for SRGCREATE" + LS);
				for (int i = 0; i < allVar.size(); i++) {
					env[i] = (String) allVar.get(i);
					System.out.println((String) allVar.get(i) + "\n");
				}
				

				COMMAND[2] = COMMAND[0]+ " " + scripts;
				//System.out.println("command: " + COMMAND[0] + COMMAND[1] + " " + COMMAND[2]);
				RunScripts rs = new RunScripts("PGSRGCREATE script", COMMAND, env);
				String runMessage = rs.run();
				 
				rs = null; // free all memory used by rs
				System.out.println(LS + "checkRunMessage: " + runMessage + LS);
				if (checkRunMessage(runMessage)) {
					putSrgRunLog(key, logList, "PGSRGCREATE Failed");
					System.out.println(key + "   SRGCREATE Failed");
				} else {
					putSrgRunLog(key, logList, "PGSRGCREATE Success");
					System.out.println(LS + key + "   PGSRGCREATE Success" + LS);

					if (!addSrgHeaders(key, srgFile, CREATE_STATUS_INDEX)) {
						putSrgRunLog(key, logList, "PGSRGCREATE Headers Failed");
						System.out.println(LS + key + "   PGSRGCREATE Headers Failed" + LS);
					} else {
						addSrgDesc(key, (String) list.get(SS_SURROGATE_INDEX), srgFile);
						writeSrgDesc();
					}
				}
			}
		}
	}
	
	private boolean addSrgHeaders(String key, String fileName, int runExe) {

		StringBuffer headers = new StringBuffer();
		ArrayList list = new ArrayList();
		String line;

		headers.append(GRIDPOLY_HEADER);

		if ((list = getSurrogateList(key, runError)) == null) {
			writeLogFile("Error: No surrogate specification data for " + key + LS, runError);
			return false;
		}

		headers.append("#SRGDESC=" + (String) list.get(SS_SURROGATE_CODE_INDEX) + ","
		+ (String) list.get(SS_SURROGATE_INDEX)
				+ LS);
		headers.append("#" + LS);
		headers.append("#SURROGATE REGION = " + (String) list.get(SS_REGION_INDEX) + LS);
		headers.append("#SURROGATE CODE = " + (String) list.get(SS_SURROGATE_CODE_INDEX) + LS);
		headers.append("#SURROGATE NAME = " + (String) list.get(SS_SURROGATE_INDEX) + LS);

		if (runExe == CREATE_STATUS_INDEX || runExe == GAPFILL_STATUS_INDEX) {
			headers.append("#DATA SHAPEFILE = " + (String) list.get(SS_DATA_SHAPEFILE_INDEX) + LS);
			headers.append("#DATA ATTRIBUTE = " + (String) list.get(SS_DATA_ATTRIBUTE_INDEX) + LS);
			headers.append("#WEIGHT SHAPEFILE = " + (String) list.get(SS_WEIGHT_SHAPEFILE_INDEX) + LS);
			headers.append("#WEIGHT ATTRIBUTE = " + (String) list.get(SS_WEIGHT_ATTRIBUTE_INDEX) + LS);
			headers.append("#WEIGHT FUNCTION = " + (String) list.get(SS_WEIGHT_FUNCTION_INDEX) + LS);
			headers.append("#FILTER FUNCTION = " + (String) list.get(SS_FILTER_FUNCTION_INDEX) + LS);
		}
		headers.append("#" + LS);
		headers.append("#USER = " + USER + LS);
		headers.append("#COMPUTER SYSTEM = " + CS + LS);
		Date now = new Date();
		headers.append("#DATE = " + now.toString() + LS);

		// temp file
		String newFile = fileName + ".tmp";

		checkFile(fileName, "NO", runStop);
		if (!renameFile(fileName, newFile, runError)) {
			return false;
		}
		
		try {
			// open input file
			FileReader fr = new FileReader(newFile);
			BufferedReader buff = new BufferedReader(fr);

			// open output file
			FileWriter fw = new FileWriter(fileName);
			BufferedWriter out = new BufferedWriter(fw);
			// write headers
			out.write(headers.toString());

			// read the input file
			while ((line = buff.readLine()) != null) {
				// skip lines starting with # or empty lines
				if (line.matches("^\\#GRID.*") || line.matches("^\\#GRID\\t.*") ||
					line.matches("^\\#POLYGON.*") || line.matches("^\\#POLYGON\\t.*") || 	
					line.matches("^\\s*$") || line.matches("^\\#SRGDESC.*") ||
					line.matches("^\\#.*")) {
					continue;
				}
				out.write(line + LS);
			}
			out.close();
			buff.close();

		} catch (IOException e) {
			writeLogFile("Error -- " + e.toString() + LS, runError);
			return false;
		}

		// delete the temp file
		boolean success = (new File(newFile)).delete();
		if (!success) {
			writeLogFile("Warning: In Deleting Temp File: " + newFile + LS, runError);
		}

		return true;
	}
	
	public String getControls(String varName) {
		String value;
		ArrayList list = new ArrayList();
		int FIRST_ITEM = 0;

		if (!controls.containsKey(varName)) {
			writeLogFile("Error: Main control CSV file does not contain variable name -- " + varName + "" + LS, runStop);
		}
		list = (ArrayList) controls.get(varName);
		value = (String) list.get(FIRST_ITEM);
		if (value.length() == 0 || value.matches("^\\s*$")) {
			writeLogFile("Error: Main control variable value is missing -- here" + varName + LS, runStop);
		}
		return value;
	}
	
	public ArrayList getSrgLogList(String key, String srgName) {
		ArrayList logList = new ArrayList();

		// System.out.println("get SrgLogList key: "+key+" srgName: "+srgName+LS);
		if (runLog.containsKey(key)) {
			logList = (ArrayList) runLog.get(key);
		} else {
			logList.add(srgName);
		}
		return logList;
	}
	
	public String writeSrgRunLog() {
		ArrayList list = new ArrayList();
		ArrayList rows = new ArrayList(); // all keys in the CSV file sequence

		if (runLog.isEmpty()) {
			return null;
		}

		StringBuffer runInfo = new StringBuffer();
		runInfo.append(LS + "\t\t" + "Surrogate Generation Summary" + LS + LS);
		rows = (ArrayList) generations.get(rowKey);
		for (int j = 0; j < rows.size(); j++) {
			String key = (String) rows.get(j);
			list = (ArrayList) generations.get(key);
			String generate = (String) list.get(SG_GENERATE_INDEX);
			String g = generate.toUpperCase();
			if (g.equals("YES") && runLog.containsKey(key)) {
				runInfo.append(key + "\t");
				ArrayList runList = (ArrayList) runLog.get(key);
				for (int i = 0; i < runList.size() - 1; i++) {
					runInfo.append((String) runList.get(i) + "\t");
				}
				runInfo.append((String) runList.get(runList.size() - 1) + LS);
			}
		}
		return runInfo.toString(); // return srgRunlog information
	}
	
	private void writeSrgDesc() {
		String line; // one record from csv file
		List list = new ArrayList();
		int ITEMS = 4;
		int SRGFILE_INDEX = 3;
		String outSrgTotal;

		// return if no new surrogate files created
		if (srgDesc.isEmpty()) {
			return;
		}

		// get SRGDESC file and check the existence
		String fileName = getControls("OUTPUT SRGDESC FILE");
		ProcessCSV csv = new ProcessCSV(); // for parsing input csv file line

		if (checkFile(fileName, "NONE", runContinue)) {
			try {
				FileReader file = new FileReader(fileName);
				BufferedReader buff = new BufferedReader(file);

				while ((line = buff.readLine()) != null) {
					// get rid of headers
					if (line.matches("^\\#.*") || line.matches("^\\s*$")) {
						continue;
					}
					list = csv.parse(line.trim());
					// check total items in each line
					if (list.size() != ITEMS) {
						writeLogFile("Warning: Wrong format line in " + fileName + ": " + line + LS, runError);
					} else {
						String region = (String) list.get(SS_REGION_INDEX);
						String code = (String) list.get(SS_SURROGATE_CODE_INDEX);
						String srgName = (String) list.get(SS_SURROGATE_INDEX);
						String srgFile = (String) list.get(SRGFILE_INDEX);

						String key = region + "_" + code;
						if (srgDesc.containsKey(key)) {
							continue; // skip old file and replace it with the new file
						}
						String newline = region + "," + code + ",\"" + srgName + "\"," + srgFile;
						srgDesc.put(key, newline);
					}
				}
				buff.close();
			} catch (IOException e) {
				writeLogFile("Error: Reading existing OUTPUT SRGDESC FILE --" + e.toString() + LS, runError);
			}

			// delete the old srgdesc file
			if (!checkFile(fileName, "YES", runError)) {
				writeLogFile("Error: Deleting existing OUTPUT SRGDESC FILE -- " + fileName + LS, runError);
			}
			System.out.println("Finished checking the SRGDESC file");
		}

		// write out srgdesc information to the file and append individual srg files based on the srgdesc information
		// check the existence of appened srg output file
		String ow = getControls("OVERWRITE OUTPUT FILES");
		String OW = ow.toUpperCase();
		outSrgTotal = getControls("OUTPUT SURROGATE FILE");
		if (!outSrgTotal.equals("NONE")) {
			if (OW.equals("YES")) {
				if (!checkFile(outSrgTotal, "YES", runError)) {
					writeLogFile("Error: Deleting existing OUTPUT SURROGATE FILE -- " + outSrgTotal + LS, runError);
					OW = "NO"; // no new OUTPUT SURROGATE FILE
				}
			} else {
				if (!checkFile(outSrgTotal, "NONE", runContinue)) {
					OW = "YES"; // output total srg file if no existing old file
				}
			}
		} else {
			OW = "NO";
		}

		try {
			// open output SRGDESC file
			FileWriter fw = new FileWriter(fileName); // overwrite the existing file whether or not it is deleted
			BufferedWriter out = new BufferedWriter(fw);

			// write grid headers
			System.out.println("#GRID" + GRIDPOLY_HEADER );
			out.write(GRIDPOLY_HEADER);

			// output srgdesc data from the sorted hashtable
			Vector v = new Vector(srgDesc.keySet());
			Collections.sort(v);
			Iterator it = v.iterator();
			while (it.hasNext()) {
				String key = (String) it.next();
				line = (String) srgDesc.get(key);
				// System.out.println("srgDesc Line = "+line);
				out.write(line + LS);

				list = csv.parse(line.trim());
				// String region = (String) list.get(REGION_INDEX);
				// String code = (String) list.get(SURROGATE_CODE_INDEX);
				// String srgName = (String) list.get(SURROGATE_INDEX);
				String srgFile = (String) list.get(SRGFILE_INDEX);

				if (OW.equals("YES") && checkFile(srgFile, "NONE", runContinue)) {
					try {
						// open total srg file for writing
						FileWriter fwSrg = new FileWriter(outSrgTotal, true); // append all files together
						BufferedWriter outSrg = new BufferedWriter(fwSrg);

						// open individual srg file for reading
						FileReader file = new FileReader(srgFile);
						BufferedReader buff = new BufferedReader(file);

						while ((line = buff.readLine()) != null) {
							outSrg.write(line + LS);
						}
						outSrg.close();
						buff.close();
					} catch (IOException e) {
						writeLogFile("Error: In reading surrogate file for appending --" + line + LS, runError);
					}
				} else if (!checkFile(srgFile, "NONE", runContinue)) {
					writeLogFile("Error: Surrogate file does not exist in SRGDESC file: " + line + LS, runError);
				}
			}
			out.close();
		} catch (IOException e) {
			writeLogFile("Error: Writing OUTPUT SRGDESC FILE --" + e.toString() + LS, runError);
		}
		System.out.println("Finished writing srgDesc file and total surrogate file");
	}

	
	// method to check the existence of a file
	public boolean checkFile(String fileName, String delete, int runStatus) {

		String ow = delete.toUpperCase();
		String tfile = fileName;
		File tempfile = new File(tfile);

		// check the file, if it exists, delete it
		if (tempfile.exists() && ow.equals("YES")) {
			boolean success = tempfile.delete();
			if (!success) {
				writeLogFile("Error: Deleting " + tfile + " file" + LS, runStatus);
				return false;
			}
		}

		// the file has to be existing, otherwise error message
		if (!tempfile.exists() && ow.equals("NO")) {
			writeLogFile("Error: File - " + tfile + " - Does Not Exist " + ow + LS, runStatus);
			return false;
		}

		// check the file existence, no message
		if (!tempfile.exists() && ow.equals("NONE")) {
			return false;
		}

		return true;
	}
	
	public void checkCSVError(Hashtable hTable, int runStatus) {
		ArrayList list = new ArrayList();
		int ERROR_INDEX = 0;

		if (hTable.containsKey("CSV_ERROR")) {
			list = (ArrayList) hTable.get("CSV_ERROR");
			String message = (String) list.get(ERROR_INDEX); // get error message
			writeLogFile(message + LS, runStatus);
		}
	}
	
	public boolean checkDir(String dirName, int runStatus) {
		String tdir;

		tdir = dirName;
		File tempdir = new File(tdir);
		if (!tempdir.exists()) {
			boolean success = tempdir.mkdirs(); // create all directories
			if (!success) {
				writeLogFile("Error: Making the output directory -- " + dirName + LS, runStatus);
				return false;
			}
		}
		return true;
	}
	
	public void putSrgRunLog(String key, ArrayList list, String message) {

		if (runLog.containsKey(key)) {
			runLog.remove(key);
		}

		// System.out.println(LS+"Put SrgLogList key: "+key+" message: "+message+LS);
		list.add(message);
		runLog.put(key, list);
	}
	
	//check a file is directory or not
	public boolean checkIsDirectory( String fileName) {
		
		String tfile = fileName;	
		File tempfile = new File(tfile);
		
        //check whether it exists and it is a directory
		if ( tempfile.isDirectory() ) {
			return true;			
		}	
		
		return false;
		
	}
	
	public void getGridPolyHeader() {
		if (getControls("OUTPUT_FILE_TYPE").equals("RegularGrid") || getControls("OUTPUT_FILE_TYPE").equals("EGrid")) { // get
																														// grid																													// information
			System.out.println("\t\t" + "Get Grid Header For Surrogate Files");
			writeLogFile(LS + "\t\t" + "Get Grid Header For Surrogate Files" + LS, runContinue);
			
			String header = getHeaderFromGridDescFile();
			if (!header.isEmpty()) {
				GRIDPOLY_HEADER = header + LS; // get header with the end of line
				System.out.println(GRIDPOLY_HEADER);
			} else {
				writeLogFile("Error: Getting Grid Header Failed, check grid description file." + LS, runStop);
				System.out.println("Error: Getting grid header failed, check grid description file." + LS);
			}
		} else if (getControls("OUTPUT_FILE_TYPE").equals("Polygon")) { // set output polygon information
			GRIDPOLY_HEADER = "#POLYGON\t" + mainEnv.get(OUTPUT_POLY_FILE_INDEX) + "\t"
					+ mainEnv.get(OUTPUT_POLY_ATTR_INDEX) + "\t" + mainEnv.get(OUTPUT_FILE_ELLIPSOID_INDEX) + "\t"
					+ mainEnv.get(OUTPUT_FILE_MAP_PRJN_INDEX) + LS;
			writeLogFile(LS + "Header of The Surrogate Ratio File" + LS + GRIDPOLY_HEADER + LS, runContinue);
			System.out.println(GRIDPOLY_HEADER);
		}
	}
	
	private enum GridDescSection { COORD, GRID }
	private String getHeaderFromGridDescFile() {
		// check that GRIDDESC file exists
		String gridDescFileName = getControls("GRIDDESC");
		checkFile(gridDescFileName, "NO", runStop);
		
		String header = "";
		
		Hashtable<String, String> systems = new Hashtable<>(); // coordinate systems by name
		Hashtable<String, String> coordTypes = new Hashtable<>(); // mapping of coordinate types
		coordTypes.put("1", "LAT-LON");
		coordTypes.put("2", "LAMBERT");
		coordTypes.put("3", "MERCATOR");
		coordTypes.put("4", "STEREOGRAPHIC");
		coordTypes.put("5", "UTM");
		coordTypes.put("6", "POLGRD3");
		coordTypes.put("7", "EquatorialMERCATOR");
		coordTypes.put("8", "TransverseMERCATOR");
		
		// GRIDDESC file format
		// ' '
		// COORD-NAME (in quotes)
		// COORDTYPE (int), P_ALP, P_BET, P_GAM, XCENT, YCENT
		// ' '
		// GRID-NAME (in quotes)
		// COORD-NAME (in quotes), XORIG, YORIG, XCELL, YCELL, NCOLS (int), NROWS (int), NTHIK (int)
		// ' '
		
		// regex to match coordinate and grid names
		Pattern namePattern = Pattern.compile("^['\"].*?['\"]");
		Matcher matcher;
		
		// regex to split lines on whitespace or commas
		String splitPattern = "\\s*[\\s,]\\s*";
		
		try {
			// open the GRIDDESC file for reading
			FileReader file = new FileReader(gridDescFileName);
			BufferedReader buff = new BufferedReader(file);
			
			String line;
			int lineNum = 0;
			GridDescSection section = GridDescSection.COORD;
			boolean isDataLine = false;
			String name = "";
			while ((line = buff.readLine()) != null) {
				lineNum++;
				
				// skip header line
				if (lineNum == 1) continue;
				
				// skip blank lines
				if (line.isEmpty()) continue;
				
				if (section == GridDescSection.COORD) {
					// read coordinate system description entries
					
					// check for last line of section
					if (line.startsWith("' '") || line.startsWith("\" \"")) {
						section = GridDescSection.GRID;
						isDataLine = false;
						continue;
					}
					
					// extract coordinate system name
					if (!isDataLine) {
						matcher = namePattern.matcher(line);
						if (matcher.find()) {
							// save coordinate system name and read data values
							name = matcher.group();
							isDataLine = true;
						} else {
							writeLogFile("Error: Unknown format at line " + lineNum + " in GRIDDESC file" + LS, runStop);
						}
					} else {
						String[] pieces = line.trim().split(splitPattern);
						if (pieces.length != 6) {
							writeLogFile("Error: Unknown format at line " + lineNum + " in GRIDDESC file" + LS, runStop);
						}
						// save the data values associated with the current coordinate system
						systems.put(name, line.trim());
						isDataLine = false;
					}
				}
				
				if (section == GridDescSection.GRID) {
					// read grid description entries
					
					// check for last line of section
					if (line.startsWith("' '") || line.startsWith("\" \"")) {
						break;
					}
					
					// extract grid name
					if (!isDataLine) {
						matcher = namePattern.matcher(line);
						if (matcher.find()) {
							// save grid name (minus quotes) and read data values
							name = matcher.group().replace("'", "").replace("\"", "");
							isDataLine = true;
						} else {
							writeLogFile("Error: Unknown format at line " + lineNum + " in GRIDDESC file" + LS, runStop);
						}
					} else {
						// check if current grid name matches OUTPUT_GRID_NAME
						if (name.equals(getControls("OUTPUT_GRID_NAME"))) {
							// extract coordinate system name
							matcher = namePattern.matcher(line);
							if (matcher.find()) {
								String coordName = matcher.group();
								
								if (!systems.containsKey(coordName)) {
									writeLogFile("Error: Unknown coordinate system " + coordName + " at line " + lineNum + " in GRIDDESC file" + LS, runStop);
								}
								
								// remove coordinate system name from line
								line = line.replace(coordName, "").replaceFirst(splitPattern, "");
								
								// convert rest of values to numbers
								String[] pieces = line.split(splitPattern);
								if (pieces.length < 7) {
									writeLogFile("Error: Unknown format at line " + lineNum + " in GRIDDESC file" + LS, runStop);
								}
								try {
									Double xorig = Double.valueOf(pieces[0].replace("D", "E"));
									Double yorig = Double.valueOf(pieces[1].replace("D", "E"));
									Double xcell = Double.valueOf(pieces[2].replace("D", "E"));
									Double ycell = Double.valueOf(pieces[3].replace("D", "E"));
									Integer ncols = Integer.valueOf(pieces[4]);
									Integer nrows = Integer.valueOf(pieces[5]);
									Integer nthik = Integer.valueOf(pieces[6]);
									
									pieces = systems.get(coordName).split(splitPattern);
									String coordType = pieces[0];
									Double p_alp = Double.valueOf(pieces[1].replace("D", "E"));
									Double p_bet = Double.valueOf(pieces[2].replace("D", "E"));
									Double p_gam = Double.valueOf(pieces[3].replace("D", "E"));
									Double xcent = Double.valueOf(pieces[4].replace("D", "E"));
									Double ycent = Double.valueOf(pieces[5].replace("D", "E"));
									
									// build header line
									header = name + "\t";
									header += String.format("%f", xorig) + "\t";
									header += String.format("%f", yorig) + "\t";
									header += String.format("%f", xcell) + "\t";
									header += String.format("%f", ycell) + "\t";
									header += String.format("%d", ncols) + "\t";
									header += String.format("%d", nrows) + "\t";
									header += String.format("%d", nthik) + "\t";
									header += coordTypes.get(coordType) + "\t";
									if (coordType.equals("1")) {
										header += "degrees\t";
									} else {
										header += "meters\t";
									}
									if (coordType.equals("6")) {
										header += String.format("%f", p_gam) + "\t";
										header += String.format("%f", p_alp) + "\t";
										header += String.format("%f", p_bet) + "\t";
									} else {
										header += String.format("%f", p_alp) + "\t";
										header += String.format("%f", p_bet) + "\t";
										header += String.format("%f", p_gam) + "\t";
									}
									header += String.format("%f", xcent) + "\t";
									header += String.format("%f", ycent) + "\t";
									header += LS;
								} catch (NumberFormatException e) {
									writeLogFile("Error: Bad number format at line " + lineNum + " in GRIDDESC file" + LS, runStop);
								}
								break;
							} else {
								writeLogFile("Error: Unknown format at line " + lineNum + " in GRIDDESC file" + LS, runStop);
							}
						} else {
							// go on to next grid
							isDataLine = false;
						}
					}
				}
			}
			
			buff.close();
		} catch (IOException e) {
			writeLogFile("Error: Reading GRIDDESC FILE --" + e.toString() + LS, runError);
		}
		
		if (header.isEmpty()) {
			writeLogFile("Error: Could not find grid " + getControls("OUTPUT_GRID_NAME") + " in GRIDDESC file" + LS, runError);
		}
		
		return header;
	}
	
	public void setCMainVariables() {
		String[] env_gitems = { "DEBUG_OUTPUT", "OUTPUT_FORMAT", "OUTPUT_FILE_TYPE", "OUTPUT_GRID_NAME", "GRIDDESC",
				"OUTPUT_FILE_ELLIPSOID" }; // envs for grid output
		String[] env_pitems = { "DEBUG_OUTPUT", "OUTPUT_FORMAT", "OUTPUT_FILE_TYPE", "OUTPUT_POLY_FILE",
				"OUTPUT_POLY_ATTR" }; // envs for polygon output
		String[] env_egitems = { "DEBUG_OUTPUT", "OUTPUT_FORMAT", "OUTPUT_FILE_TYPE", "OUTPUT_GRID_NAME", "GRIDDESC",
				"OUTPUT_FILE_ELLIPSOID", "OUTPUT_POLY_FILE" }; // envs for grid output
		String[] shape = new String[3]; // store output polygon shapefile information

		// add library path to the execution
		if (OS.equals("unix"))
			cmainEnv.add("LD_LIBRARY_PATH=" + LIBPATH);

		// create environment variable Vector for grid output format
		if (getControls("OUTPUT_FILE_TYPE").equals("RegularGrid")) {
			for (int i = 0; i < env_gitems.length; i++) {
				cmainEnv.add(env_gitems[i] + "=" + getControls(env_gitems[i]));
			}
		} else if (getControls("OUTPUT_FILE_TYPE").equals("EGrid")) {
			for (int i = 0; i < env_egitems.length; i++) {
				cmainEnv.add(env_egitems[i] + "=" + getControls(env_egitems[i]));
			}
		} else if (getControls("OUTPUT_FILE_TYPE").equals("Polygon")) { // /set POLYGON output format environmenta
																		// variables except last two
			for (int i = 0; i < env_pitems.length; i++) {
				cmainEnv.add(env_pitems[i] + "=" + getControls(env_pitems[i]));
			}

			String oshape = getControls("OUTPUT_POLY_FILE"); // output polygon shapefile
			shape = getShapeInfo(oshape, runStop);
			String oPolyFile = "OUTPUT_POLY_FILE=" + shape[DIRECTORY_INDEX] + FS + oshape; // put polygon file with dir
																							// into array
			cmainEnv.set(OUTPUT_POLY_FILE_INDEX, oPolyFile);
			cmainEnv.add("OUTPUT_FILE_ELLIPSOID=" + shape[ELLIPSOID_INDEX]); // put output polygon ellipsoid in array
			cmainEnv.add("OUTPUT_FILE_MAP_PRJN=" + "+" + shape[PROJECTION_INDEX]); // put polygon projection in array
		}

		// add hard coded environment variables
		cmainEnv.add("DATA_FILE_NAME_TYPE=ShapeFile");
		cmainEnv.add("WEIGHT_FILE_TYPE=ShapeFile");

		// set DENOMINATOR_THRESHOLD
		if (!controls.containsKey("DENOMINATOR_THRESHOLD")) {
			writeLogFile("Note: Main control CSV file does not contain variable -- DENOMINATOR_THRESHOLD" + LS,
					runContinue);
			writeLogFile("Note: DENOMINATOR_THRESHOLD will be set to the default threshold in srgcreate" + LS,
					runContinue);
		} else {
			cmainEnv.add("DENOMINATOR_THRESHOLD=" + getControls("DENOMINATOR_THRESHOLD"));
		}

		System.out.println(LS + "\t\t" + "Main Environment Variables for SRGCREATE" + LS + LS);
		for (int i = 0; i < cmainEnv.size(); i++) {
			System.out.println((String) cmainEnv.get(i)); // print out main environment variables
		}
	}
	
	// set item index in shapefile catalog
	public static final int DIRECTORY_INDEX = 0, ELLIPSOID_INDEX = 1, PROJECTION_INDEX = 2; // shapefile name is the key
																							// in the hashtable

	public String[] getShapeInfo(String shapeName, int runStatus) {
		ArrayList list = new ArrayList();
		String[] shapeData = new String[3]; // 0=DIRECTORY,1=ELLIPSOID,2=PROJECTION

		if (!shapefiles.containsKey(shapeName)) {
			writeLogFile("Error: Shapefile catalog CSV file does not contain shapefile -- " + shapeName + LS, runStatus);
			return null;
		}

		list = (ArrayList) shapefiles.get(shapeName);

		for (int i = DIRECTORY_INDEX; i <= PROJECTION_INDEX; i++) {
			shapeData[i] = (String) list.get(i); // put dir into array
			if ((shapeData[i].matches("^\\s*$")) || (shapeData[i].equals("NONE"))) {
				writeLogFile("Error: Shapefile catalog CSV file has missing value for " + shapeName + LS, runStatus);
				return null;
			}

		}

		// check main control file has SHAPEFILE DIRECTORY, if it does, use that directory instead
		if (controls.contains("SHAPEFILE DIRECTORY")) {
			shapeData[DIRECTORY_INDEX] = getControls("SHAPEFILE DIRECTORY");
		}

		// check exists of the shapefile
		if (!checkFile(shapeData[DIRECTORY_INDEX] + FS + shapeName + ".shp", "NO", runStatus)) {
			return null;
		}

		return shapeData;
	}


}
