package gov.epa.surrogate.qa;

import java.io.File;

import gov.epa.surrogate.DiffTwoFiles;
import junit.framework.TestCase;

public class QAReportsTest extends TestCase {

	public void testRunSurrgateQA() throws Exception {
		String srgDescFile = "test/data/SRGDESC.txt";
		QAReports qaReport = new QAReports(srgDescFile);
		qaReport.execute();
		compareWithSampleFile("test/data/expected_usa_summary.csv", "test/data/SRGDESC_usa_summary.csv");
		compareWithSampleFile("test/data/expected_usa_gapfill.csv", "test/data/SRGDESC_usa_gapfill.csv");
		compareWithSampleFile("test/data/expected_usa_nodata.csv", "test/data/SRGDESC_usa_nodata.csv");
		compareWithSampleFile("test/data/expected_usa_not1.csv", "test/data/SRGDESC_usa_not1.csv");
		compareWithSampleFile("test/data/expected_canada_summary.csv", "test/data/SRGDESC_canada_summary.csv");
		compareWithSampleFile("test/data/expected_canada_gapfill.csv", "test/data/SRGDESC_canada_gapfill.csv");
		compareWithSampleFile("test/data/expected_canada_nodata.csv", "test/data/SRGDESC_canada_nodata.csv");
		compareWithSampleFile("test/data/expected_canada_not1.csv", "test/data/SRGDESC_canada_not1.csv");
	}
	
	private void compareWithSampleFile(String referenceFile, String testResultFile) throws Exception {
		new DiffTwoFiles(referenceFile, testResultFile).compare();
	}

	protected void tearDown() throws Exception {
		new File("test/data/SRGDESC_usa_summary.csv").delete();
		new File("test/data/SRGDESC_usa_gapfill.csv").delete();
		new File("test/data/SRGDESC_usa_nodata.csv").delete();
		new File("test/data/SRGDESC_usa_not1.csv").delete();
		new File("test/data/SRGDESC_canada_summary.csv").delete();
		new File("test/data/SRGDESC_canada_gapfill.csv").delete();
		new File("test/data/SRGDESC_canada_nodata.csv").delete();
		new File("test/data/SRGDESC_canada_not1.csv").delete();
	}

}
