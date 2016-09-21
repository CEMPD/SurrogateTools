package gov.epa.surrogate.gapfill;

import junit.framework.TestCase;

public class GapfillInputReaderTest extends TestCase {

	public void testShouldReadACorrectInputFile() throws Exception {
		String inputFileName = "test/data/gapfill/gapfill_input.txt";
		GapfillInputFileReader reader = new GapfillInputFileReader(inputFileName);
		reader.read();
		assertEquals("test/data/gapfill/gapfilled_surrogates.txt", reader.getOutputFileName());
		assertEquals("test/data/gapfill/srg_xref.txt", reader.getCrossReferenceFileName());
		GapfillCommand[] commands = reader.getCommands();
		assertEquals(3, commands.length);
	}

	public void testShouldGiveMessageForOutputFileNotSpecified() throws Exception {
		String inputFileName = "test/data/gapfill/wrong_gapfill_input_output.txt";
		GapfillInputFileReader reader = new GapfillInputFileReader(inputFileName);
		try {
			reader.read();
		} catch (Exception e) {
			assertEquals("The output surrogate file name is not specified", e.getMessage());
			return;
		}
		assertFalse("Output file is not specified", true);
	}
	
	public void testShouldGiveMessageForCrossReferenceFileNotSpecified() throws Exception {
		String inputFileName = "test/data/gapfill/wrong_gapfill_input_crossReference.txt";
		GapfillInputFileReader reader = new GapfillInputFileReader(inputFileName);
		try {
			reader.read();
		} catch (Exception e) {
			assertEquals("The cross reference file name is not specified", e.getMessage());
			return;
		}
		assertFalse("Cross reference file is not specified", true);
	}
	
	
	public void testShouldGiveMessageForNoGapfillCommandsNotSpecified() throws Exception {
		String inputFileName = "test/data/gapfill/gapfill_input_noGapfillCommands.txt";
		GapfillInputFileReader reader = new GapfillInputFileReader(inputFileName);
		try {
			reader.read();
		} catch (Exception e) {
			assertEquals("Atleast one gapfill command should be specified", e.getMessage());
			return;
		}
		assertFalse("Atleast one gapfill command should be specified", true);
	}
}
