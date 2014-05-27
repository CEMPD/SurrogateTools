package gov.epa.surrogate.merge;

import gov.epa.surrogate.Surrogates;

import java.util.List;

import junit.framework.TestCase;

public class MergeCountiesTest extends TestCase {

	public void testShouldReadMergeCounties() throws Exception {
		Surrogates surrogates = new Surrogates("");
		surrogates.addSurrogate(2, "Airports");
		surrogates.addSurrogate(7, "Population");
		surrogates.addSurrogate(207, "Merge Airports");

		MergeCommand command = command();
		MergeCommandReader reader = new MergeCommandReader(surrogates, command);
		reader.read();

		List<Counties> countiesColl = reader.getCountiesColl();
		assertMergeCommandRead(countiesColl);
		int outputSurrogateID = surrogates.getSurrogateID(command.getOutputSurrogate());
		MergeCounties mergeCounties = new MergeCounties(countiesColl,command.getEquation(),outputSurrogateID);
		Counties counties = mergeCounties.doMerge();
		List<Integer> mergedCodes = counties.allCountyCodes();
		int[] expIDs = { 1001, 1002, 1003, 1004, 1005, 1007, 1009,1010, 1011, 1013, 1022,1033 };
		assertEquals(12,mergedCodes.size());
		for (int i = 0; i < mergedCodes.size(); i++) {
			assertEquals(expIDs[i], counties.getCounty(mergedCodes.get(i)).getCountyCode());
		}
		
	}

	private void assertMergeCommandRead(List<Counties> countiesColl) throws Exception {
		assertEquals(2, countiesColl.size());
		Counties counties1 = countiesColl.get(0);
		List<Integer> allCodes1 = counties1.allCountyCodes();
		assertEquals(9, allCodes1.size());
		int[] expIDs1 = { 1001, 1002, 1004, 1005, 1007, 1010, 1011, 1013, 1022 };
		for (int i = 0; i < allCodes1.size(); i++) {
			assertEquals(expIDs1[i], counties1.getCounty(allCodes1.get(i)).getCountyCode());
		}

		Counties counties2 = countiesColl.get(1);
		List<Integer> allCodes2 = counties2.allCountyCodes();
		assertEquals(8, allCodes2.size());
		int[] expIDs2 = { 1001, 1003, 1005, 1007, 1009, 1011, 1022, 1033 };
		for (int i = 0; i < allCodes2.size(); i++) {
			assertEquals(expIDs2[i], counties2.getCounty(allCodes2.get(i)).getCountyCode());
		}
	}

	private MergeCommand command() throws Exception {
		String line = "OUTSRG=Merge Airports; 0.5*({test/data/merge/2_NOFILL.txt|Airports})+0.5*({test/data/merge/7_NOFILL.txt|Population});";
		MergeCommand command = new MergeCommand(line);
		return command;
	}

}
