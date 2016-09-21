package gov.epa.surrogate.gapfill;

import gov.epa.surrogate.Surrogates;
import junit.framework.TestCase;

public class CrossReferenceFileReaderTest extends TestCase {
	
	/**
	 * @throws Exception
	 */
	public void testReadACrossReferenceFile() throws Exception{
		String fileName = "test/data/gapfill/srg_xref.txt";
		CrossReferenceFileReader reader = new CrossReferenceFileReader(fileName);
		reader.read();
		Surrogates surrogates = reader.getSurrogates();
		assertEquals(2, surrogates.getSurrogateID("Airports"));
		assertEquals(3, surrogates.getSurrogateID("Area"));
		assertEquals(4, surrogates.getSurrogateID("Ports"));
		assertEquals(5, surrogates.getSurrogateID("Navigable H20"));
		assertEquals(6, surrogates.getSurrogateID("Highways"));
		assertEquals(7, surrogates.getSurrogateID("Population"));
		assertEquals(8, surrogates.getSurrogateID("Housing"));
		assertEquals(202, surrogates.getSurrogateID("Gapfilled Airports"));
		assertEquals(206, surrogates.getSurrogateID("Gapfilled Highways"));
		assertEquals(300, surrogates.getSurrogateID("Half Pop Half Housing"));
		assertEquals(303, surrogates.getSurrogateID("Half Pop Half Housing, Area"));
		
		assertEquals("Airports", surrogates.getSurrogateName(2));
		assertEquals("Area", surrogates.getSurrogateName(3));
		assertEquals("Ports", surrogates.getSurrogateName(4));
		assertEquals("Navigable H20", surrogates.getSurrogateName(5));
		assertEquals("Highways", surrogates.getSurrogateName(6));
		assertEquals("Population", surrogates.getSurrogateName(7));
		assertEquals("Housing", surrogates.getSurrogateName(8));
		assertEquals("Gapfilled Airports", surrogates.getSurrogateName(202));
		assertEquals("Gapfilled Highways", surrogates.getSurrogateName(206));
		assertEquals("Half Pop Half Housing", surrogates.getSurrogateName(300));
		assertEquals("Half Pop Half Housing, Area", surrogates.getSurrogateName(303));
		
	}
	

}
