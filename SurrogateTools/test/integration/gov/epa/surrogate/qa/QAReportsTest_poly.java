package gov.epa.surrogate.qa;

import gov.epa.surrogate.DiffTwoFiles;
import junit.framework.TestCase;

public class QAReportsTest_poly extends TestCase {

	public void testRunSurrgateQA() throws Exception {
		String srgDescFile = "test/data/small_SRGDESC_poly.txt";
		QAReports qaReport = new QAReports(srgDescFile);
		qaReport.execute(new Threshold(0.3));
		 
		compareWithSampleFile("test/data/expected_canada_threshold.csv", "test/data/SRGDESC_canada_threshold.csv");
		compareWithSampleFile("test/data/expected_usa_threshold.csv", "test/data/SRGDESC_poly_usa_threshold.csv");
	}
	
	private void compareWithSampleFile(String referenceFile, String testResultFile) throws Exception {
		new DiffTwoFiles(referenceFile, testResultFile).compare();
	}

	protected void tearDown() throws Exception {
		 
		//new File("test/data/SRGDESC_canada_threshold.csv").delete();
		//new File("test/data/SRGDESC_usa_threshold.csv").delete();
	}

}
