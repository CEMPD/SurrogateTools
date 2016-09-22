
        BLOCK DATA  INITBLK3

C***********************************************************************
C Version "@(#)$Header$"
C EDSS/Models-3 I/O API.
C Copyright (C) 1992-2002 MCNC and Carlie J. Coats, Jr.,
C (C) 2003-2010 by Baron Advanced Meteorological Systems.
C Distributed under the GNU LESSER GENERAL PUBLIC LICENSE version 2.1
C See file "LGPL.txt" for conditions of use.
C.........................................................................
C
C  FUNCTION:  initialize I/O state for STATE3 common, Models-3 I/O API
C
C  REVISION  HISTORY:  prototype 3/92 by CJC
C
C***********************************************************************

        IMPLICIT NONE

        INCLUDE  'PARMS3.EXT'
        INCLUDE  'STATE3.EXT'

        DATA  COUNT3 / 0 /
        DATA  LOGDEV / IMISS3  /
        DATA  FINIT3 / .FALSE. /

        END

