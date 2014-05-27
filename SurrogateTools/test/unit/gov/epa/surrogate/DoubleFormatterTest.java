package gov.epa.surrogate;

import gov.epa.surrogate.qa.DoubleFormatter;
import junit.framework.TestCase;

public class DoubleFormatterTest extends TestCase {
	
	public void testFormatOneDecimal(){
		double d = 1.0;
		DoubleFormatter formatter = new DoubleFormatter();
		assertEquals("1.00000000",formatter.format(d));
		
	}
	
	public void testFormatMoreThanSixDecimal(){
		double d = 1.234567890;
		DoubleFormatter formatter = new DoubleFormatter();
		assertEquals("1.23456789",formatter.format(d));
		
	}
	
	public void testFormatBigNumber(){
		double d = 1098888.234;
		DoubleFormatter formatter = new DoubleFormatter();
		assertEquals("1098888.23400000",formatter.format(d));
	}
	
	public void testFormatNegativeNumber(){
		double d = -1098888.234;
		DoubleFormatter formatter = new DoubleFormatter();
		assertEquals("-1098888.23400000",formatter.format(d));
	}
	
	public void testFormatScientificNumber(){
		double d = -109.2e-4;
		DoubleFormatter formatter = new DoubleFormatter();
		assertEquals("-0.01092000",formatter.format(d));
	}

}
