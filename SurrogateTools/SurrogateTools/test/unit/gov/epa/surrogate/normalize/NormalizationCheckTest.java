package gov.epa.surrogate.normalize;

import gov.epa.surrogate.Precision;

import java.io.FileNotFoundException;

import junit.framework.TestCase;

public class NormalizationCheckTest extends TestCase {

	public void testShouldRequireNormalizationFile() throws FileNotFoundException {
		String fileName = "test/data/small_surrogate_550.txt";
		NormalizationCheck check = new NormalizationCheck(fileName, 5, new Precision(1e-6), "\t");
		try {
			check.required();
		} catch (Exception e) {
			assertFalse("Should not have thrown an exception", true);
			return;
		}
		assertTrue("Should not have thrown an exception", true);
	}

	public void testShouldNotRequireNormalizationFile() throws FileNotFoundException {
		String fileName = "test/data/100_NOFILL.txt";
		NormalizationCheck check = new NormalizationCheck(fileName, 5, new Precision(1e-6), "\t");
		try {
			check.required();
		} catch (Exception e) {
			String expMsg = "Normalization is not required for the surrogate file '" + fileName + "'";
			assertEquals(expMsg, e.getMessage());
			return;
		}
		assertTrue("Should have thrown an exception", false);
	}

}
