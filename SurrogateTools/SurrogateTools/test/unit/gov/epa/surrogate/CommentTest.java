package gov.epa.surrogate;

import junit.framework.TestCase;

public class CommentTest extends TestCase {

	public void testCommentStartWithSpaces() {
		String line = " #This is a comment";
		assertTrue(line + " is a comment line ", new Comment().isComment(line));
	}
	
	public void testNot_A_CommentLineStartWithSpaces() {
		String line = " This is not a comment";
		assertFalse(line + " is not a comment line ", new Comment().isComment(line));
	}
	
	public void testEmptyLine(){
		String line = "       ";
		assertTrue("Empty line is a comment", new Comment().isComment(line));
	}
	
}
