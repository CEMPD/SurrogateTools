package gov.epa.surrogate.ppg;

/** -------------------------MAIN--------------------------------------* */
public class Main {

	public static void main(String[] arguments) {

		try {
			SurrogateToolpg sc = new SurrogateToolpg();

			if (arguments.length < 1) {
				System.out.println("Usge: java SrgTool controlFile");
				System.exit(SurrogateToolpg.runStop);
			}

			sc.getSystemInfo();

			sc.readControls(arguments[0]);
			sc.readSurrogateSpec();
			System.out.println("finished reading specification. ");
			sc.readShapefiles();
			System.out.println("finished reading shape files. ");
			sc.readGenerations();
			System.out.println("finished reading generation. ");
			sc.readSurrogateCodes();
			System.out.println("finished reading surrogate codes. ");

			sc.setGentVariables();
			//	sc.getGridPolyHeader();

			//	if (sc.run_load)
			//		sc.setLoadVariables();
			//		sc.loadShapefile();
			//	if (sc.run_grid)
			//		sc.gridShapefile();
			if (sc.run_create)
				sc.generatePGSurrogates();

			sc.writeLogFile(sc.LS, SurrogateToolpg.runFinish); // exit the program run
			sc = null;
		} catch (Exception e) {
			System.out.println(e.getMessage());
			System.exit(1);
		}
	}
}

/** -------------------------END of THE MAIN CLASS----------------------- */

