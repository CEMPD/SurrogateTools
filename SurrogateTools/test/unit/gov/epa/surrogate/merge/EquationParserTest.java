package gov.epa.surrogate.merge;

import gov.epa.surrogate.SurrogateFileInfo;

import java.util.List;

import junit.framework.TestCase;

public class EquationParserTest extends TestCase {

	public void testParseAMergeEqualtionWithTwoSurrogateInfos() throws Exception {
		String equation = " 0.5 * ({test/data/100_NOFILL.txt|Population})+0.5*({test/data/550_FILL.txt|Housing})";
		String line = "OUTSRG=Half Pop Half Housing;  0.5 * ({test/data/100_NOFILL.txt|Population})+0.5*({test/data/550_FILL.txt|Housing})";
		EquationParser parser = new EquationParser(equation, line);
		List<SurrogateFileInfo> infos = parser.parse();
		assertEquals(2, infos.size());
		assertEquals("test/data/100_NOFILL.txt", infos.get(0).getSurrogateFileName());
		assertEquals("Population", infos.get(0).getSurrogateName());
		assertEquals("test/data/550_FILL.txt", infos.get(1).getSurrogateFileName());
		assertEquals("Housing", infos.get(1).getSurrogateName());
		assertEquals(5.0, parser.getEquation().evaluate(5, 5), 0.00000000001);
	}
	
	public void testParseAMergeEqualtionWithTwoSurrogateInfos_WithUnEqualWeights() throws Exception {
		String equation = " 0.75 * ({test/data/100_NOFILL.txt|Population})+0.25*({test/data/550_FILL.txt|Housing})";
		String line = "OUTSRG=Half Pop Half Housing;  0.75 * ({test/data/100_NOFILL.txt|Population})+0.25*({test/data/550_FILL.txt|Housing})";
		EquationParser parser = new EquationParser(equation, line);
		List<SurrogateFileInfo> infos = parser.parse();
		assertEquals(2, infos.size());
		assertEquals("test/data/100_NOFILL.txt", infos.get(0).getSurrogateFileName());
		assertEquals("Population", infos.get(0).getSurrogateName());
		assertEquals("test/data/550_FILL.txt", infos.get(1).getSurrogateFileName());
		assertEquals("Housing", infos.get(1).getSurrogateName());
		assertEquals(3.0, parser.getEquation().evaluate(4, 0), 0.00000000001);
	}


	public void testParseAMergeEqualtionWithOneSurrogateInfos() throws Exception {
		String equation = " 1 * ({test/data/100_NOFILL.txt|Population}) ";
		String line = "OUTSRG=Population; 1 * ({test/data/100_NOFILL.txt | Population}) ";
		EquationParser parser = new EquationParser(equation, line);
		List<SurrogateFileInfo> infos = parser.parse();
		assertEquals(1, infos.size());
		assertEquals("test/data/100_NOFILL.txt", infos.get(0).getSurrogateFileName());
		assertEquals("Population", infos.get(0).getSurrogateName());
		assertEquals(2.0, parser.getEquation().evaluate(2), 0.00000000001);
	}
	
	
}
