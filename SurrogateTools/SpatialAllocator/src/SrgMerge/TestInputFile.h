#include "GapfillInputFile.h"
#include "ReadSrgInfo.h"
#include "SyntaxError.h"
#include "FileErrors.h"

#ifndef __TEST_INPUT_FILE_H
#define __TEST_INPUT_FILE_H

int allTest_testInputFile();

/** Syntax Error: Information file does not have output file*/
void testSE_outfile();
/** Syntax Error: Information file does not have ref file*/
void testSE_refFile();
/** Syntax Error: Not Specifying Surrogate information with "OUTSRG=" */
void testSE_cmdLine();
/** Syntax Error: Incorrect delimiter */
void testSE_delim();
/**File Not Exist Error: ref file */ 
void testFNF_refFile();
/** File Not Exist Error: file for a surrogate */
void testFNF_srgFile();
/**Syntax Error: Division is not allowed for Merging */
void testSE_div();
/**Syntax Error: Subtraction is not allowed for Merging */
void testSE_subtraction();
/**Syntax Error: Duplicate input surrogates entries are not allowed for GAPFILL"*/
void testSE_dupSrgEntries();
/**Syntax Error: Duplicate output surrogates entries in the xref file"*/
void testSE_xrefDupEntries();

void result(string expMsg,string resMsg,string testName);
#endif
