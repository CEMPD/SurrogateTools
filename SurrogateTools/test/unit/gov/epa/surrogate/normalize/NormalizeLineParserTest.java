package gov.epa.surrogate.normalize;

import junit.framework.TestCase;

public class NormalizeLineParserTest extends TestCase {

	public void testShouldSplitLineAndCreateSurrogateRow() {
		String line = "  100	01001	  104	   38	0.03342011	!	1459.489783	43671.000443	1.000000";
		NormalizeLineParser lineParser = new NormalizeLineParser("\t", 5);
		NormalizeSurrogateRow row = lineParser.parse(line);
		assertEquals(row.getSurrogateID(), 100);
		assertEquals(row.getCountyID(), 1001);
		assertEquals(row.getRatio(), 0.03342011, 0.00000001);
		assertEquals(row.getComment(), "1459.489783	43671.000443	1.000000");
	}

}
