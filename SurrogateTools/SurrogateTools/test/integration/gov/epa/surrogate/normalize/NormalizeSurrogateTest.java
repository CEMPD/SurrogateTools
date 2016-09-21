package gov.epa.surrogate.normalize;

import java.io.File;

import gov.epa.surrogate.DiffTwoFiles;
import gov.epa.surrogate.Precision;
import junit.framework.TestCase;

public class NormalizeSurrogateTest extends TestCase {

	public void testShouldNormalizeSurrogatesInTheFile() throws Exception {
		String fileName = "test/data/small_surrogate_550.txt";
		int[] excludeCounties = { 1011, 1022 };
		NormalizeSurrogate ns = new NormalizeSurrogate(fileName, 5, new Precision(1e-6), excludeCounties);
		ns.normalize();
		compareWithSampleFile("test/data/expected_normalized.txt", "test/data/small_surrogate_550_NORM.txt");
	}

	private void compareWithSampleFile(String referenceFile, String testResultFile) throws Exception {
		new DiffTwoFiles(referenceFile, testResultFile).compare();
	}

	protected void tearDown() throws Exception {
		new File("test/data/small_surrogate_550_NORM.txt").delete();
	}

}
