package gov.epa.surrogate.merge;

import gov.epa.surrogate.DiffTwoFiles;

import java.io.File;

import junit.framework.TestCase;

public class MergingTest extends TestCase {

	public void testShouldReadMergeInputFile() throws Exception {
		String inputFile = "test/data/merge/merge_input.txt";
		Merging mg = new Merging(inputFile);
		mg.doMerging();
		compareWithSampleFile("test/data/merge/merged_surrogates.txt","test/data/merge/expected_merged_output.txt");
		
	}

	private void compareWithSampleFile(String referenceFile, String testResultFile) throws Exception {
		new DiffTwoFiles(referenceFile, testResultFile).compare();
	}

	protected void tearDown() throws Exception {
		new File("test/data/merge/merged_surrogates.txt").delete();
	}


}
