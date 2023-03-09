// ==========================================================================
//
// Utils.h
//
// Writer: jrkwon
//
// 
//
// Hisotry:
// 12/19/05 Start!
//
// ==========================================================================

#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#ifndef WIN32
#include <sys/time.h>
#endif
#include <stdarg.h>
#include <stdlib.h>
#include <math.h>
#include <sstream>
#include <string>
#include <iostream>
#include <vector>
#include "CmdArg.h"
#include "Params.h"

using namespace std;

//----------------------------------------------------------------------------
//	some random number functions.
//----------------------------------------------------------------------------

//returns a random integer between x and y
inline int	  RandInt(int x,int y) {return rand()%(y-x+1)+x;}

//returns a random float between zero and 1
inline double RandFloat()		   {return (rand())/(RAND_MAX+1.0);}

//returns a random bool
inline bool   RandBool()
{
	if (RandInt(0,1)) return true;

	else return false;
}

//returns a random float in the range -1 < n < 1
inline double RandomClamped()	   {return RandFloat() - RandFloat();}

extern CCmdArg *g_pCmdArg;

#ifndef WIN32
#define _vsnprintf vsnprintf
#endif

inline void /*__cdecl*/ OutputDebugStringF (const char* pchFormat, ...) 
{ 
	static int nCnt = 0;
	
    if(g_pCmdArg->m_bVerbose == false)
	{
		if((++nCnt % 100) == 0)
		{
			nCnt = 0;
			printf(".");
		}
		return;
    }

    //assert (pchFormat != 0); 
    va_list vaParams; 
    va_start (vaParams, pchFormat); 
    char pchMessage[256]; 
    _vsnprintf (pchMessage, sizeof (pchMessage), pchFormat, vaParams); 
    pchMessage[sizeof(pchMessage)-1] = '\0'; 
    //OutputDebugStringA (pchMessage); 
    printf(pchMessage);
    va_end (vaParams); 
}

// do not care 'verbose' option 
inline void /*__cdecl*/ OutputDebugStringVF (const char* pchFormat, ...) 
{ 
    //assert (pchFormat != 0); 
    va_list vaParams; 
    va_start (vaParams, pchFormat); 
    char pchMessage[256]; 
    _vsnprintf (pchMessage, sizeof (pchMessage), pchFormat, vaParams); 
    pchMessage[sizeof(pchMessage)-1] = '\0'; 
    //OutputDebugStringA (pchMessage); 
    printf(pchMessage);
    va_end (vaParams); 
}

inline void /*__cdecl*/ DebugF (const char* pchFormat, ...) 
{ 
#ifdef DEBUG
    //assert (pchFormat != 0); 
    va_list vaParams; 
    va_start (vaParams, pchFormat); 
    char pchMessage[256]; 
    _vsnprintf (pchMessage, sizeof (pchMessage), pchFormat, vaParams); 
    pchMessage[sizeof(pchMessage)-1] = '\0'; 
    //OutputDebugStringA (pchMessage); 
    printf(pchMessage);
    va_end (vaParams);
#endif
}

#ifndef WIN32
typedef unsigned int WORD;

typedef struct _SYSTEMTIME {
  WORD wYear;
  WORD wMonth;
  WORD wDayOfWeek;
  WORD wDay;
  WORD wHour;
  WORD wMinute;
  WORD wSecond;
  WORD wMilliseconds;
} SYSTEMTIME, *PSYSTEMTIME;
 
inline void GetLocalTime(SYSTEMTIME *pSystemTime)
{
  time_t t;
  struct tm       *tms;
  struct timeval  tv;
  struct timezone tz;

  time(&t);
  tms = localtime(&t);
  gettimeofday(&tv, &tz);

  /* adjust year and month */
  tms->tm_year += 1900;
  tms->tm_mon  += 1;

  pSystemTime->wMilliseconds = (int)tv.tv_usec / 1000;
  pSystemTime->wSecond      = (int)tms->tm_sec;
  pSystemTime->wMinute      = (int)tms->tm_min;
  pSystemTime->wHour        = (int)tms->tm_hour;
  pSystemTime->wDay         = (int)tms->tm_mday;
  pSystemTime->wMonth       = (int)tms->tm_mon;
  pSystemTime->wYear        = (int)tms->tm_year;
}
#endif

#endif // UTILS_H

