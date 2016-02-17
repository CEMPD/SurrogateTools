package gov.epa.surrogate.qa;

import gov.epa.surrogate.SurrogateDescription;
import junit.framework.TestCase;

public class SurrogateFileReaderTest extends TestCase {

	public void testReadSmallSurrogateFile() throws Exception {
		String fileName = "test/data/small_USA_890_FILL.txt";
		Counties counties = new Counties();
		SurrogateDescription srgDescription = srgDescription(fileName);
		SurrogateFileReader reader = new SurrogateFileReader(srgDescription, "\t", 5, counties);
		reader.read(new Threshold(0.3));
		int[] codes = counties.allCountyCodes();
		assertEquals(codes.length, 3);
		assertEquals(6021, codes[0]);
		assertEquals(6023, codes[1]);
		assertEquals(6025, codes[2]);
	}

	private SurrogateDescription srgDescription(String fileName) {
		String[] tokens = { "USA", "890", "Commercial Timber", fileName };
		return new SurrogateDescription(tokens);
	}
	
	public void testReadSmallSurrogateFileWithMultipleSurrogates_shouldSkipSurrogatesExcept_890() throws Exception {
		String fileName = "test/data/small_USA_mulitipleSurrogates_FILL.txt";
		Counties counties = new Counties();
		SurrogateDescription srgDescription = srgDescription(fileName);
		SurrogateFileReader reader = new SurrogateFileReader(srgDescription, "\t", 5, counties);
		reader.read(new Threshold(0.3));
		int[] codes = counties.allCountyCodes();
		assertEquals(codes.length, 3);
		assertEquals(6021, codes[0]);
		assertEquals(6023, codes[1]);
		assertEquals(6025, codes[2]);
	}


}
