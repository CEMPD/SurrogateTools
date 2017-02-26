package gov.epa.surrogate.ppg;

import junit.framework.TestCase;


public class PpgSurrogateTest extends TestCase {

	public void testPGSA() throws Exception {
		System.out.println("begin test. ");
		String fileName = "test/data/srgtoolpg/control_variables.csv";
		SurrogateToolpg sc = new SurrogateToolpg();
		sc.getSystemInfo();
		sc.readControls(fileName);
		System.out.println("finished reading controls. ");
		sc.readSurrogateSpec();
		System.out.println("finished reading specification. ");
		sc.readShapefiles();
		System.out.println("finished reading shape files. ");
		sc.readGenerations();
		System.out.println("finished reading generation. ");
		sc.readSurrogateCodes();
		System.out.println("finished reading surrogate codes. ");
		sc.setGentVariables();
		System.out.println("finished generate main vars. ");
		//sc.getGridPolyHeader();
		if (sc.run_create)
			sc.generatePGSurrogates();
				
		sc.writeLogFile(sc.LS, SurrogateToolpg.runFinish); // exit the program run
		sc = null;

	}
}
