# for Linux or any gcc/g++ compiler
CC = gcc
CPP = g++

# for IBM VisualAge compiler on AIX
#CC  = c89
#CPP = xlC_r

#Optional flags - uncomment one OPTFLAGS line
OPTFLAGS = -O
#OPTFLAGS = -O -DDEBUG 

LIBS = $(LIBPATH) -lspatial
LIBPATH=-L. -L..
LDFLAGS = -static 
#On AIX, add -bnoquiet to LDFLAGS for more useful linker output
CFLAGS = -g $(OPTFLAGS) -DGAPFILL__MAIN -DEVAL_UNIT -I/usr/vacpp/include 

#.cpp.o:
##	$(CPP) $(CFLAGS) -c $<
#
#.c.o:
#	$(CC) $(CFLAGS) -c $<

CPP_GAP_SRC_OBJS = GapfillInputFile.o ReadSrgInfo.o MergeFunction.o MergeOutput.o \
	GapfillOutput.o SrgMerge.o

all: srgmerge.exe

srgmerge.exe:  $(CPP_GAP_SRC_OBJS)
	$(CPP) -o $@ $(CPP_GAP_SRC_OBJS) $(LIBS) $(LDFLAGS)

clean:
	 -rm *.o srgmerge.exe

GapfillInputFile.o: GapfillInputFile.cpp GapfillInputFile.h
	$(CPP) $(CFLAGS) -c GapfillInputFile.cpp
ReadSrgInfo.o: ReadSrgInfo.cpp ReadSrgInfo.h
	$(CPP) $(CFLAGS) -c ReadSrgInfo.cpp
GapfillOutput.o: GapfillOutput.cpp GapfillOutput.h Surrogates.h
	$(CPP) $(CFLAGS) -c GapfillOutput.cpp
MergeOutput.o: MergeOutput.cpp MergeOutput.h Surrogates.h
	$(CPP) $(CFLAGS) -c MergeOutput.cpp
MergeFunction.o: MergeFunction.cpp FileTags.h MergeFunction.h 
	$(CPP) $(CFLAGS) -c MergeFunction.cpp
SrgMerge.o: SrgMerge.cpp GapfillInputFile.h ReadSrgInfo.h GapfillOutput.h \
	MergeOutput.h SyntaxError.h FileErrors.h Errors.h
	$(CPP) $(CFLAGS) -c SrgMerge.cpp

