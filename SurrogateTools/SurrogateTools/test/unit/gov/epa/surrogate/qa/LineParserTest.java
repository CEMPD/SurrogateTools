package gov.epa.surrogate.qa;

import junit.framework.TestCase;

public class LineParserTest extends TestCase {
	
	public void testParsingLineWithoutGapFilled(){
		String line = "890	1001	103	37	0.53061224	!	26.000000	49.000000	0.530612";
		
		LineParser parser = new LineParser("\\t",5);
		SurrogateRow surrogateRow = parser.parse(line,false);
		
		assertEquals(surrogateRow.getSurrogateCode(),890);
		assertEquals(surrogateRow.getCountyCode(), 1001);
		assertEquals(surrogateRow.getRatio(),0.53061224,0.0000001);
		assertEquals(surrogateRow.getGapFillCode(), -1);
		assertFalse(surrogateRow.isGapFilled());
	}
	
	public void testParsingLineWithGapFilled(){
		String line = "890	4012	31	41	0.01288622	!	1350589.074743	104808819.151381	0.012886   GF: 320";
		
		LineParser parser = new LineParser("\\t",5);
		SurrogateRow surrogateRow = parser.parse(line,true);
		
		assertEquals(surrogateRow.getSurrogateCode(),890);
		assertEquals(surrogateRow.getCountyCode(), 4012);
		assertEquals(surrogateRow.getRatio(),0.01288622,0.0000001);
		assertEquals(surrogateRow.getGapFillCode(), 320);
		assertTrue(surrogateRow.isGapFilled());
	}

}
