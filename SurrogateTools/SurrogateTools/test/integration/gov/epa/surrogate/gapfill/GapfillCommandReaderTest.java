package gov.epa.surrogate.gapfill;

import gov.epa.surrogate.Surrogates;

import java.util.List;

import junit.framework.TestCase;

public class GapfillCommandReaderTest extends TestCase {

	public void testShouldReadGapfillCommand() throws Exception {
		Surrogates surrogates = new Surrogates("");
		surrogates.addSurrogate(2, "Airports");
		surrogates.addSurrogate(7, "Population");
		GapfillCommand command = command();
		GapfillCommandReader reader = new GapfillCommandReader(surrogates, command);
		reader.read();
		List<String> primarySrgFileComments = reader.getPrimarySrgFileComments();
		assertEquals(3, primarySrgFileComments.size());

		Counties countiesColl = reader.getCounties();
		County[] counties = countiesColl.getCounties();
		assertEquals(9, counties.length);
		int[] expIDs = { 1001, 1002, 1003, 1005, 1007, 1009, 1010, 1011, 1022 };
		for (int i = 0; i < counties.length; i++) {
			assertEquals(expIDs[i], counties[i].getCountyID());
		}
	}

	private GapfillCommand command() throws Exception {
		String line = "OUTSRG=Airports; GAPFILL=test/data/gapfill/2_NOFILL.txt|Airports;test/data/gapfill/7_NOFILL.txt|Population";
		GapfillCommand command = new GapfillCommand(line);
		return command;
	}

}
