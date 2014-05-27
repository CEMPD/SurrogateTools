package gov.epa.surrogate.qa;

import gov.epa.surrogate.SystemInfo;

public class Main {
	
	public static void main(String[] arguments) {
		
		if (arguments.length < 1) {
			System.out.println("Usage: ava gov.epa.surrogate.qa.Main SRGDESC.txt");
			System.exit(1);
		}
		new SystemInfo().print();
		try {
			QAReports ss = new QAReports(arguments[0]);
			ss.execute();
		} catch (Exception e) {
			System.out.println(e.getMessage());
			System.exit(1);
		}
		
	}

}
