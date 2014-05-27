package gov.epa.surrogate.qa;

//represnts a cell in the county surrogate table
public class CountySurrogate {

	private int surrogateID;

	private double ratio;

	private int gapFillCode;

	public CountySurrogate(int surrogateID, int gapFillCode) {
		this.surrogateID = surrogateID;
		this.gapFillCode = gapFillCode;
		this.ratio = 0.0;
	}

	public int getSurrogateID() {
		return surrogateID;
	}

	public void addRatio(double ratio) {
		this.ratio += ratio;
	}

	public double getSum() {
		return ratio;
	}

	public boolean isGapFillted() {
		return !(gapFillCode == -1);
	}

	public int gapFillCode() {
		return gapFillCode;
	}
}
