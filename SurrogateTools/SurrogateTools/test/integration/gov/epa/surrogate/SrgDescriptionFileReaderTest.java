package gov.epa.surrogate;

import junit.framework.TestCase;

public class SrgDescriptionFileReaderTest extends TestCase {

	public void testShouldReadSmallDescriptionFile() throws Exception {
		String srgDescFile = "test/data/small_SRGDESC.txt";
		SrgDescriptionFileReader reader = new SrgDescriptionFileReader(srgDescFile);
		reader.read();
		SurrogateDescription[] surrogateDescriptions = reader.getSurrogateDescriptions();
		assertEquals(2,surrogateDescriptions.length);
	}

}
