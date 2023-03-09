// ==========================================================================
//
// CCmdArg.cpp
//
// Writer: jrkwon
//
//      The original version was written by Mat Buckland.
//
// 
//
// Hisotry:
//      12/19/05 Start!
//
// ==========================================================================

#ifdef WIN32
#include "stdafx.h"
#else
#include <string.h>
#include <stdlib.h>
#endif
#include <time.h>
#include "CmdArg.h"
#include "Params.h"

// output1: fitness & weight
// output2: x, y position

// input1: weights 
// output1: activation value

// training 
// -o:best fitness in each generation
// -o1: x, y, angle x, angle y trajectory
// -o2: weight
// -o3: activation value in training
// -o5: see complexity value 
// -m:t  -o:fw01.txt -o2:w01.txt
// -m:tc -o:fw01.txt -o2:w01.txt  <-- check complexity
// -m:tc -o:fw01.txt -02:w01.txt -o3:ta01.txt

// running
// -i: weight
// -o: activation value
// -o1:curvature = avg curv / # of data
// -o4:all info of curv
// -m:r -i:w01.txt -o:a01.txt -o1:c01.txt
// -m:r -i:w01.txt -o:a01.txt -o1:c01.txt -o4:curv01.txt


// examples
// -m:r -i:h3ff\w02.txt -o:h3ff\a02.txt -o1:h3ff\c02.txt -o4:h3ff\co02.txt

// display track!!
// -m:r -i:weight.txt -t

CCmdArg::CCmdArg(char *argv[])
{
    /* set default value */
    m_Mode = MODE_TRAINING;
	memset(m_szInFileName, 0, nMAX_FILENAME_LENGTH);
	m_fpIn = NULL;

    Parse(argv);
}

CCmdArg::~CCmdArg()
{
	if(m_fpIn)
		fclose(m_fpIn);
}

bool CCmdArg::CheckAndCopy(char* argv[], char *pszToken, char *pszStr)
{
    char *pszPtr;

    for(int i = 1; argv[i]; i++)
    {
        pszPtr = strstr(argv[i], pszToken);
        if(pszPtr)
        {
            pszPtr += strlen(pszToken);
            while(*pszPtr)
            {
                *pszStr++ = *(pszPtr++);
            }
            *pszStr = '\0';
            return true;
        }
    }
    return false;
}

bool CCmdArg::CheckAndCopy(char* argv[], char *pszToken, char &ch)
{
    char *pszPtr;

    for(int i = 1; argv[i]; i++)
    {
        pszPtr = strstr(argv[i], pszToken);
        if(pszPtr)
        {
            pszPtr += strlen(pszToken);
            ch = *pszPtr;
            return true;
        }
    }
    return false;
}

void CCmdArg::WriteTime(FILE *fp)
{
    time_t rawtime;
    struct tm * pTime;

    time ( &rawtime );
    pTime = localtime ( &rawtime );
    fprintf(fp, "%4d-%02d-%02d %02d:%02d:%02d\n", pTime->tm_year, pTime->tm_mon,
                                                  pTime->tm_mday,
                                                  pTime->tm_hour, pTime->tm_min,
                                                  pTime->tm_sec);
}

void CCmdArg::Parse(char *argv[])
{
    char szValue[80];
    char szFmt[80];

    CheckAndCopy(argv, "-m:", szValue);
    if(szValue[0] == 'r')
	{
        m_Mode = MODE_RUNNING;
		m_bCheckComplexity = false;
	}
    else if(szValue[0] == 't')
    {
        m_Mode = MODE_TRAINING;
		m_bCheckComplexity = (szValue[1] == 'c') ? true : false;
    }
    
    if(CheckAndCopy(argv, "-v", szValue))
        m_bVerbose = true;
    else
        m_bVerbose = false;

    if(CheckAndCopy(argv, "-t", szValue))
        m_bCartTrack = true;
    else
        m_bCartTrack = false;

	if(CheckAndCopy(argv, "-d:", szValue))
		m_nDelay = atoi(szValue);
	else
		m_nDelay = 0;

	if(CheckAndCopy(argv, "-b:", szValue))
		m_nBlackOut = atoi(szValue); 
	else
		m_nBlackOut = 0;

	if(CheckAndCopy(argv, "-bs:", szValue))
		m_nBlackOutStart = atoi(szValue); 
	else
		m_nBlackOutStart = 0; //nBLACKOUT_START_STEP;
		
	//if(CheckAndCopy(argv, "-f:", szValue))
	//	m_dDynActRate = atof(szValue);
	//else
	//	m_dDynActRate = -1.0; // no dynamic act rate, then evolve it
	
	if(CheckAndCopy(argv, "-da:", szValue))
	{
		m_bDynamicAct = true;
		m_dDynActRate = -1.0; // no dynamic act rate, then evolve it

		if(strcmp(szValue, "fo") == 0)
			m_bFRateOnly = true;
		else if(strcmp(szValue, "all") == 0)
			m_bFRateOnly = false;
		else
		{
			m_dDynActRate = atof(szValue);
		}
	}
	else
	{
		m_bDynamicAct = false;
	}

	if(CheckAndCopy(argv, "-mt", szValue))
	{
		m_bBigMutate = true;
	}
	else
	{
		m_bBigMutate = false;
	}
	
    m_bNdpia = false;
    if(CheckAndCopy(argv, "-dm:", szValue))
    {
		if(strcmp(szValue, "n") == 0)
			m_bNdpia = true;
        
        if(m_bDynamicAct == false)
        {
            m_bDynamicAct = true;
            m_bFRateOnly = false;
            m_dDynActRate = -1.0; // no dynamic act rate, then evolve it
        }
    }
    
    // if m_bDynamicAct
    m_bFacilMotor = m_bFacilSensor = true;
    if(CheckAndCopy(argv, "-f:", szValue))
	{        
		if(strcmp(szValue, "m") == 0)
			m_bFacilMotor = true, m_bFacilSensor = false;
		else
			m_bFacilSensor = true, m_bFacilMotor = false;
	}
    
	
    //CheckAndCopy(argv, "-i:", m_aszInFileName[0]);
	strcpy(szFmt, "-i:");
	if(CheckAndCopy(argv, szFmt, m_szInFileName))
	{
		m_fpIn = fopen(m_szInFileName, "rt");
	//WriteTime(m_afpIn[i]);
	}
    
    m_bRecurrentNet = true;
    if(CheckAndCopy(argv, "-n:", szValue))
	{        
		if(strcmp(szValue, "f") == 0) // feedforward
			m_bRecurrentNet = false;
    }
    
	m_nNoise = 0;
	if(CheckAndCopy(argv, "-noise:", szValue))
	{
		m_nNoise = atoi(szValue);
		//m_bAddNoise = true;
	}
    
    // print out paramter settings
    printf("----------------------------------------------------------------------------\n");
    printf("Settings\n");
    printf("----------------------------------------------------------------------------\n");
	printf("Mode:               %s\n", (m_Mode == MODE_TRAINING) ? "Training" : "Running");
    printf("Verbose:            %s\n", (m_bVerbose) ? "True" : "False");
    printf("Cart trajectory:    %s\n", (m_bCartTrack) ? "True" : "False");
    printf("Delay:              %d\n", m_nDelay);
    printf("Blackout Duration:  %d\n", m_nBlackOut);
    printf("Blackout StartStep: %d\n", m_nBlackOutStart);
    printf("Dynamic Act:        %s\t", (m_bDynamicAct) ? "True" : "False");
	if(m_bDynamicAct)
	{
		if(m_dDynActRate == -1)
			printf(" %s\n", m_bFRateOnly ? "F-Only" : "All");
		else
			printf(" %f\n", m_dDynActRate);
		printf("Dynamic Model:      %s\n",  m_bNdpia ? "NDPIA" : "FAN");
		printf("Which Neuron:       S:%s M:%s\n", m_bFacilSensor ? "Yes" : "No", m_bFacilMotor ? "Yes" : "No");
	}
	else
		printf("\n");
	printf("Mutation:           %s\n", ( m_bBigMutate) ? "New values used" : "Small change");
	printf("Recurrent:          %s\n", m_bRecurrentNet ? "Yes" : "No");
	printf("Add Noise:          %d%%\n", m_nNoise);
    printf("----------------------------------------------------------------------------\n");
}
