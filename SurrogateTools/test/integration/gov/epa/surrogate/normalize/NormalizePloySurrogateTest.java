package gov.epa.surrogate.normalize;

import gov.epa.surrogate.DiffTwoFiles;
import gov.epa.surrogate.Precision;
import junit.framework.TestCase;

public class NormalizePloySurrogateTest extends TestCase {

	public void testShouldNormalizePolySurrogatesInTheFile() throws Exception {
		String fileName = "test/data/small_polysurrogate_240.txt";
		int[] excludeCounties = { 1011, 1022 };
		NormalizeSurrogate ns = new NormalizeSurrogate(fileName, 4, new Precision(1e-6), excludeCounties);
		ns.normalize();
		compareWithSampleFile("test/data/expected_small_polysurrogate_240_NORM.txt", "test/data/small_polysurrogate_240_NORM.txt");
	}

	private void compareWithSampleFile(String referenceFile, String testResultFile) throws Exception {
		new DiffTwoFiles(referenceFile, testResultFile).compare();
	}

//	protected void tearDown() throws Exception {
//		new File("test/data/small_surrogate_240_NORM.txt").delete();
//	}

}
