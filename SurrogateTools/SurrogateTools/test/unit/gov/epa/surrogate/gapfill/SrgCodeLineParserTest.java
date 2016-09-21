package gov.epa.surrogate.gapfill;

import gov.epa.surrogate.SrgCodeLineParser;
import gov.epa.surrogate.Surrogates;
import junit.framework.TestCase;

public class SrgCodeLineParserTest extends TestCase {

	public void testParseSrgLineWithTag() throws Exception{
		String line = "#SRGDESC=7,Population";
		Surrogates surrogates = new Surrogates("");
		SrgCodeLineParser parser = new SrgCodeLineParser(surrogates);
		parser.parse(line);
		assertEquals(7,surrogates.getSurrogateID("Population"));
	}
	
	public void testParseSurrgateNamesWithDelimiter_SrgLineWithTag() throws Exception{
		String line = "#SRGDESC=7, \"Population, Housing\"";
		Surrogates surrogates = new Surrogates("");
		SrgCodeLineParser parser = new SrgCodeLineParser(surrogates);
		parser.parse(line);
		assertEquals(7,surrogates.getSurrogateID("Population, Housing"));
	}
	
	public void testParseLineWithoutTag() throws Exception{
		String line = "Population";
		Surrogates surrogates = new Surrogates("");
		SrgCodeLineParser parser = new SrgCodeLineParser(surrogates);
		parser.parse(line);
		try {
			surrogates.getSurrogateID("Population");
		} catch (Exception e) {
			assertEquals("Could not find an id for the surrogate named 'Population'",e.getMessage());
			return;
		}
		assertFalse("Could not find id for population",true);
	}
	
	public void testParseLineEmptyString() throws Exception{
		String line = "  ";
		Surrogates surrogates = new Surrogates("");
		SrgCodeLineParser parser = new SrgCodeLineParser(surrogates);
		parser.parse(line);
		try {
			surrogates.getSurrogateID("Population");
		} catch (Exception e) {
			assertEquals("Could not find an id for the surrogate named 'Population'",e.getMessage());
			return;
		}
		assertFalse("Could not find id  for population",true);
	}
}
