package gov.epa.surrogate.qa;

import junit.framework.TestCase;

public class OutputFileNamesTest extends TestCase{
	
	public void testOutputFileNamesBasedOnSRGDESFileNameWithAExtension() throws Exception{
		String srgDescFileName = "test/data/srgDesc103X102.txt";
		OutputFileNames outputNames = new OutputFileNames(srgDescFileName,"usa");
		assertEquals("test\\data\\srgDesc103X102_usa_summary.csv",outputNames.summaryFileName());
		assertEquals("test\\data\\srgDesc103X102_usa_gapfill.csv",outputNames.gapFillFileName());
		assertEquals("test\\data\\srgDesc103X102_usa_not1.csv",outputNames.not1FileName());
		assertEquals("test\\data\\srgDesc103X102_usa_nodata.csv",outputNames.noDataFileName());
	}

	public void testOutputFileNamesBasedOnSRGDESFileNameWithMultipleDots() throws Exception{
		String srgDescFileName = "test/data/srgDesc.103X102.txt";
		OutputFileNames outputNames = new OutputFileNames(srgDescFileName,"usa");
		assertEquals("test\\data\\srgDesc.103X102_usa_summary.csv",outputNames.summaryFileName());
		assertEquals("test\\data\\srgDesc.103X102_usa_gapfill.csv",outputNames.gapFillFileName());
		assertEquals("test\\data\\srgDesc.103X102_usa_not1.csv",outputNames.not1FileName());
		assertEquals("test\\data\\srgDesc.103X102_usa_nodata.csv",outputNames.noDataFileName());
	}
	
	public void testOutputFileNamesBasedOnSRGDESFileNameWithNoDots() throws Exception{
		String srgDescFileName = "test/data/srgDesc";
		OutputFileNames outputNames = new OutputFileNames(srgDescFileName,"usa");
		assertEquals("test\\data\\srgDesc_usa_summary.csv",outputNames.summaryFileName());
		assertEquals("test\\data\\srgDesc_usa_gapfill.csv",outputNames.gapFillFileName());
		assertEquals("test\\data\\srgDesc_usa_not1.csv",outputNames.not1FileName());
		assertEquals("test\\data\\srgDesc_usa_nodata.csv",outputNames.noDataFileName());
	}
	
	public void testOutputFileNamesWhichAlreadyExist() throws Exception{
		String srgDescFileName = "test/data/expected";
		OutputFileNames outputNames = new OutputFileNames(srgDescFileName, "usa");
		try{
			outputNames.summaryFileName();
		}catch (Exception e) {
			assertTrue(e.getMessage().contains("_usa_summary.csv" + " already exists"));
			return;
		}
		
		assertFalse(srgDescFileName+"_usa_summary.csv already exists",true);
	}
}
