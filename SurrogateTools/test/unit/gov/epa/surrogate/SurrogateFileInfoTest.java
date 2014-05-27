package gov.epa.surrogate;

import gov.epa.surrogate.SurrogateFileInfo;
import junit.framework.TestCase;

public class SurrogateFileInfoTest extends TestCase{
	
	public void testShouldSplitFillNameAndSurrogateName() throws Exception{
		String line = "OUTSRG=Gapfilled Airports; GAPFILL=test/data/100_NOFILL.txt|Airports;test/data/srg_nash_ref.txt|Population";
		String infoToken = "test/data/100_NOFILL.txt|Airports";
		SurrogateFileInfo info = new SurrogateFileInfo(infoToken,line);
		assertEquals(info.getSurrogateFileName(),"test/data/100_NOFILL.txt");
		assertEquals(info.getSurrogateName(),"Airports");
	}
	
	public void testShouldSplitFillNameAndSurrogateNameAndFileNotExist() throws Exception{
		String line = "OUTSRG=Gapfilled Airports; GAPFILL=test/data/100_FILL.txt|Airports;test/data/srg_nash_ref.txt|Population";
		String infoToken = "test/data/100_FILL.txt|Airports";
		try {
			new SurrogateFileInfo(infoToken,line);
		} catch (Exception e) {
			String expMsg = "The file '" + "test\\data\\100_FILL.txt" + "' does not exist";
			assertEquals(expMsg,e.getMessage());
			return;
		}
		assertFalse("The file not exist",true);
	}
	
	public void testShouldGiveWrongFormatExceptionMoreThanTwoTokens(){
		String line = "OUTSRG=Gapfilled Airports; GAPFILL=test/data|/100_NOFILL.txt|Airports;test/data/srg_nash_ref.txt|Population";
		String infoToken = "test/data|/100_NOFILL.txt|Airports";
		try {
			new SurrogateFileInfo(infoToken,line);
		} catch (Exception e) {
			assertEquals("The line '" + line + "' is not in the correct format",e.getMessage());
			return;
		}
		assertFalse("Wrong Format",true);
	}
	
	public void testShouldGiveWrongFormatExceptionLessThanTwoTokens(){
		String line = "OUTSRG=Gapfilled Airports; GAPFILL=test/data/100_NOFILL.txtAirports;test/data/srg_nash_ref.txt|Population";
		String infoToken = "test/data/100_NOFILL.txtAirports";
		try {
			new SurrogateFileInfo(infoToken,line);
		} catch (Exception e) {
			assertEquals("The line '" + line + "' is not in the correct format",e.getMessage());
			return;
		}
		assertFalse("Wrong Format",true);
	}
}
