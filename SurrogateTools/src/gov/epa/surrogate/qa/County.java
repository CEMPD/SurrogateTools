package gov.epa.surrogate.qa;

import java.util.Hashtable;
import java.util.Map;

public class County {

	private int countyCode;
	
	private Map<Integer,CountySurrogate> surrogates;

	public County(int countyCode) {
		this.countyCode = countyCode;
		surrogates = new Hashtable<Integer,CountySurrogate>();
	}

	public int getCountyCode(){
		return countyCode;
	}
	
	public void addRow(SurrogateRow row){
		int surrogateCode = row.getSurrogateCode();
		Integer id = new Integer(surrogateCode);
		CountySurrogate srg = surrogates.get(id);
		if(srg == null){
			srg = new CountySurrogate(surrogateCode, row.getGapFillCode());
			surrogates.put(id,srg);
		}
		srg.addRatio(row.getRatio());
	}

	public CountySurrogate surrogate(int code) {
		return surrogates.get(new Integer(code));
	}
	
	

}
