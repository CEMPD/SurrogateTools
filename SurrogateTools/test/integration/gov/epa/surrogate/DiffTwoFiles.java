package gov.epa.surrogate;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;

public class DiffTwoFiles {

	private BufferedReader referenceReader;

	private BufferedReader resultReader;

	public DiffTwoFiles(String referenceFile, String testResultFile) throws FileNotFoundException {
		referenceReader = new BufferedReader(new FileReader(new File(referenceFile)));
		resultReader = new BufferedReader(new FileReader(new File(testResultFile)));
	}

	public void compare() throws Exception {
		try {
			int lineNumber = 0;
			String referenceLine = null;
			String readLine = resultReader.readLine();
			while ((referenceLine = referenceReader.readLine()) != null) {
				lineNumber++;
				if (!(referenceLine.equals(readLine))) {
					throw new Exception("Two files are not equal at line " + lineNumber+"\n"+
							"expect line: '"+referenceLine+"'"+"\n"+
							"result line: '"+ readLine+ "'");
					
				}
				readLine = resultReader.readLine();
			}
			if (readLine != null) {
				throw new Exception("Result file is longer than reference file");
			}
		} finally {
			referenceReader.close();
			resultReader.close();
		}
	}
}
