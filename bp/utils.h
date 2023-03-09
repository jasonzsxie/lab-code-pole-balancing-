/*
 *  utils.h
 *  bp
 *
 *  Created by Jaerock Kwon on 5/30/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

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
#endif
