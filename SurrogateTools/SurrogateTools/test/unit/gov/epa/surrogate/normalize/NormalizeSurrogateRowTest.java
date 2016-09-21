package gov.epa.surrogate.normalize;

import junit.framework.TestCase;

public class NormalizeSurrogateRowTest extends TestCase {

	public void testShouldCheckArrayOfTokensAssignedToCorrectAttributes() {
		String[] tokens = { "100", "01001", "103", "36", "0.00173117", "75.601977	43671.000443	0.001731" };
		NormalizeSurrogateRow row = new NormalizeSurrogateRow(tokens, 5, "\t");
		assertEquals(row.getSurrogateID(), 100);
		assertEquals(row.getCountyID(), 1001);
		assertEquals(row.getRatio(), 0.00173117, 0.00000001);
		assertEquals(row.getComment(), tokens[5]);
	}

}
