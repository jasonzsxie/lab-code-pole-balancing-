/*
 *  utils.c
 *  bp
 *
 *  Created by Jaerock Kwon on 5/30/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */
#include <sys/time.h>
#include "utils.h"

void GetLocalTime(SYSTEMTIME *pSystemTime)
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