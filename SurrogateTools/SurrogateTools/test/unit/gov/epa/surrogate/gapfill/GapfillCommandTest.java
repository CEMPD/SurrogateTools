package gov.epa.surrogate.gapfill;

import gov.epa.surrogate.SurrogateFileInfo;
import junit.framework.TestCase;

public class GapfillCommandTest extends TestCase {
	
	public void testShouldParseACorrectFormatCommandLine() throws Exception{
		String line = "OUTSRG = Gapfilled Airports; GAPFILL = test/data/100_NOFILL.txt|Airports;test/data/550_FILL.txt|Population";
		GapfillCommand command = new GapfillCommand(line);
		assertEquals("Gapfilled Airports",command.getOutputSurrogate());
		SurrogateFileInfo[] infos = command.getSurrogateInfos();
		assertEquals(2,infos.length);
		assertEquals("test/data/100_NOFILL.txt",infos[0].getSurrogateFileName());
		assertEquals("test/data/550_FILL.txt",infos[1].getSurrogateFileName());
		assertEquals("Airports",infos[0].getSurrogateName());
		assertEquals("Population",infos[1].getSurrogateName());
	}
	
	public void testMissingStartTag_WrongFormat(){
		String line = "TSRG=Gapfilled Airports; GAPFILL=test/data/100_NOFILL.txt|Airports;test/data/550_FILL.txt|Population";
		try {
			new GapfillCommand(line);
		} catch (Exception e) {
			assertEquals("'OUTSRG' tag not found.\nThe line '" + line + "' is not in the correct format",e.getMessage());
			return;
		}
		assertFalse("Wrong format",true);
	}
	
	public void testMissingGapfillTag_WrongFormat(){
		String line = "OUTSRG=Gapfilled Airports; =test/data/100_NOFILL.txt|Airports;test/data/550_FILL.txt|Population";
		try {
			new GapfillCommand(line);
		} catch (Exception e) {
			assertEquals("'GAPFILL' tag not found.\nThe line '" + line + "' is not in the correct format",e.getMessage());
			return;
		}
		assertFalse("Wrong format",true);
	}
	
	public void testCouldnotIdentifyOutputSurrogatName_WrongFormat(){
		String line = "OUTSRGGapfilled Airports; GAPFILL=test/data/100_NOFILL.txt|Airports;test/data/550_FILL.txt|Population";
		try {
			new GapfillCommand(line);
		} catch (Exception e) {
			assertEquals("The line '" + line + "' is not in the correct format",e.getMessage());
			return;
		}
		assertFalse("Wrong format",true);
	}
	
	public void testCouldnotIdentifyGapfillInfos_WrongFormat(){
		String line = "OUTSRG=Gapfilled Airports; GAPFILLtest/data/100_NOFILL.txt|Airports;test/data/550_FILL.txt|Population";
		try {
			new GapfillCommand(line);
		} catch (Exception e) {
			assertEquals("Missing '=' after GAPFILL tag.\nThe line '" + line + "' is not in the correct format",e.getMessage());
			return;
		}
		assertFalse("Wrong format",true);
	}
	
	

}
