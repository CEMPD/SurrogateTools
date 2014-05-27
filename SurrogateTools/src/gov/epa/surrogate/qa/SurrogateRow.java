package gov.epa.surrogate.qa;

public class SurrogateRow {

	private int surrogateCode;

	private double ratio;

	private int gapFillCode;

	private boolean gapFilled;

	private int countyCode;

	public SurrogateRow(String[] tokens, int minTokens, boolean gapFilled) {
		// FIXME: validation
		surrogateCode = new Integer(tokens[0].trim()).intValue();
		countyCode = new Integer(tokens[1].trim()).intValue();
		ratio = new Double(tokens[minTokens - 1]).doubleValue();
		this.gapFilled = gapFilled;
		gapFillCode = gapFillCode(tokens, gapFilled);

	}

	private int gapFillCode(String[] tokens, boolean gapFilled) {
		if (!gapFilled) {
			return -1;
		}
		String lastToken = tokens[tokens.length - 1];
		if(!lastToken.contains("GF:")){
			this.gapFilled = false;
			return -1;
		}
		String[] gfTokens = lastToken.split(":");
		return Integer.parseInt(gfTokens[1].trim());

	}

	public int getSurrogateCode() {
		return surrogateCode;
	}

	public double getRatio() {
		return ratio;
	}

	public int getCountyCode() {
		return countyCode;
	}

	public int getGapFillCode() {
		return gapFillCode;
	}

	public boolean isGapFilled() {
		return gapFilled;
	}

}
