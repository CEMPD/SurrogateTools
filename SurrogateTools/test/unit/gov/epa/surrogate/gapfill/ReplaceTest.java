package gov.epa.surrogate.gapfill;

import junit.framework.TestCase;

public class ReplaceTest extends TestCase {
	
	public void testPrimaryReplace(){
		Replace replace = new PrimaryReplace();
		String line="890	4012	31	41	0.01288622	!	1350589.074743	104808819.151381	0.012886";
		int rowSrgId = 890;
		int outputSurrogateID = 100;
		String result = replace.replace(line,rowSrgId,outputSurrogateID);
		String expLine = "100	4012	31	41	0.01288622	!	1350589.074743	104808819.151381	0.012886";
		assertEquals(expLine,result);
	}
	
	public void testNonPrimaryReplace(){
		Replace replace = new NonPrimaryReplace();
		String line="890	4012	31	41	0.01288622	!	1350589.074743	104808819.151381	0.012886";
		int rowSrgId = 890;
		int outputSurrogateID = 100;
		String result = replace.replace(line,rowSrgId,outputSurrogateID);
		String expLine = "100	4012	31	41	0.01288622	!	1350589.074743	104808819.151381	0.012886 GF: "+rowSrgId;
		assertEquals(expLine,result);
	}

}
