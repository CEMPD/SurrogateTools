package gov.epa.surrogate.normalize;

import java.io.File;

import gov.epa.surrogate.DiffTwoFiles;
import junit.framework.TestCase;

public class NormalizeTest extends TestCase {

	public void testRunNormalizeSurrogates() throws Exception {
		String srgDescFile = "test/data/small_SRGDESC.txt";
		String excludeListFile = "test/data/exclude_counties.txt";
		NormalizeSurrogates ns = new NormalizeSurrogates(srgDescFile, excludeListFile, "1e-6");
		ns.normalize();
		compareWithSampleFile("test/data/expected_normalize_small_SRGDESC.txt", "test/data/small_SRGDESC_NORM.txt");
		compareWithSampleFile("test/data/expected_small_surrogate_550_NORM.txt", "test/data/small_surrogate_550_NORM.txt");
	}

	private void compareWithSampleFile(String referenceFile, String testResultFile) throws Exception {
		new DiffTwoFiles(referenceFile, testResultFile).compare();
	}

	protected void tearDown() throws Exception {
		new File("test/data/small_surrogate_550_NORM.txt").delete();
		new File("test/data/small_SRGDESC_NORM.txt").delete();
	}

}
