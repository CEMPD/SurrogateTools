package gov.epa.surrogate.gapfill;

import gov.epa.surrogate.DiffTwoFiles;

import java.io.File;

import junit.framework.TestCase;

public class GapfillingTest extends TestCase {
	
	public void testShouldGapfillUsingAGapfillInputFile() throws Exception{
		String inputFile = "test/data/gapfill/gapfill_input.txt";
		Gapfilling gf = new Gapfilling(inputFile);
		gf.doGapfill();
		compareWithSampleFile("test/data/gapfill/expected_gapfilled_output.txt","test/data/gapfill/gapfilled_surrogates.txt");
	}
	
	private void compareWithSampleFile(String referenceFile, String testResultFile) throws Exception {
		new DiffTwoFiles(referenceFile, testResultFile).compare();
	}

	protected void tearDown() throws Exception {
		new File("test/data/gapfill/gapfilled_surrogates.txt").delete();
	}

}
