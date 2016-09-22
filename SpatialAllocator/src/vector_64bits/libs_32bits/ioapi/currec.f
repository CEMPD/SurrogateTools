
        INTEGER FUNCTION CURREC ( JDATE, JTIME, 
     &                            SDATE, STIME, TSTEP, 
     &                            CDATE, CTIME )

C***********************************************************************
C Version "@(#)$Header$"
C EDSS/Models-3 I/O API.
C Copyright (C) 1992-2002 MCNC and Carlie J. Coats, Jr., and
C (C) 2003-2010 Baron Advanced Meteorological Systems
C Distributed under the GNU LESSER GENERAL PUBLIC LICENSE version 2.1
C See file "LGPL.txt" for conditions of use.
C.........................................................................
C  subroutine body starts at line  61
C
C  FUNCTION:  Return the record number the time step in the time step 
C             sequence starting at SDATE:STIME and having time step TSTEP 
C             and compute its  date&time  CDATE:CTIME
C             In particular, this is the largest time step in the sequence 
C             having the property:
C
C                 CDATE:CTIME <= JDATE:JTIME
C
C             If JDATE:JTIME is out-of-range, return -1
C
C  PRECONDITIONS REQUIRED:  Dates represented YYYYDDD, 
C                           times represented HHMMSS.
C
C  SUBROUTINES AND FUNCTIONS CALLED:  NEXTIME, SEC2TIME, SECSDIFF, TIME2SEC
C
C  REVISION  HISTORY:
C       Adapted 2/99 by CJC from I/O API routine CURREC()
C
C       Version 1/2007 by CJC:  simplification; handle negative 
C       *DATE arguments correctly
C***********************************************************************

      IMPLICIT NONE

C...........   ARGUMENTS and their descriptions:

        INTEGER, INTENT(IN   ) :: SDATE, STIME    !  starting d&t for the sequence
        INTEGER, INTENT(IN   ) :: TSTEP           !  time step for the sequence
        INTEGER, INTENT(IN   ) :: JDATE, JTIME    !  d&t requested
        INTEGER, INTENT(  OUT) :: CDATE, CTIME    !  d&t for timestep of JDATE:JTIME

C...........   EXTERNAL FUNCTIONS and their descriptions:

        INTEGER, EXTERNAL :: SECSDIFF, SEC2TIME, TIME2SEC


C...........   SCRATCH LOCAL VARIABLES and their descriptions:

        INTEGER       SECS, STEP, IREC


C***********************************************************************
C   begin body of subroutine  CURREC

        IF ( TSTEP .EQ. 0 ) THEN   !  time-independent case:

            CURREC = 1
            CDATE  = SDATE
            CTIME  = STIME
            RETURN

        END  IF

        IF ( JDATE .LT. -10000000  .OR.         !  out-of-range
     &       JDATE .GT.  10000000  ) THEN       !  probable-error cases 

                CURREC = -1
                RETURN

        END  IF

        SECS = SECSDIFF( SDATE, STIME, JDATE, JTIME )

        IF ( SECS .LT. 0 ) THEN     !  before start of time step sequence

            CURREC = -1

        ELSE                        !  usual case:  integer arithmetic

            CDATE    = SDATE        !  sec2time to find offset from start,
            CTIME    = STIME        !  nextime() to compute date&time.
            STEP     = TIME2SEC( ABS( TSTEP ) ) 
            IREC     = SECS / STEP
            CALL NEXTIME( CDATE, CTIME, 
     &                    SEC2TIME( IREC * STEP ) )
            CURREC   = 1 + IREC

        END IF

        RETURN

        END FUNCTION CURREC

