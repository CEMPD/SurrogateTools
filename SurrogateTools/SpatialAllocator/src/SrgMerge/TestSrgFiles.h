
/* This file will have test for checking whether srg file specified are have expected informations
 */
#include "Errors.h"
#include "SyntaxError.h"
#include "FileErrors.h"
#include "ReadSrgInfo.h"
#include "GapfillInputFile.h"
#include "GapfillOutput.h"

#ifndef __TEST_SRG_FILE_H
#define __TEST_SRG_FILE_H
int allTest_testSrgFiles();
extern void result(string expMsg,string resMsg,string testName);


/**Test whether all surrogate files belong to the same grid info */
void test_sameGridInfo();
/**Test whether output surrogates specified is exist in xref file */
void test_srgExistXRefFile();
/**Test whether the input surrogates is exist in the surrogates file*/
void test_srgExistSrgFile();


#endif
