#ifdef WIN32
#include "windows.h"
#include "stdafx.h"
#endif
#include "NeuGen.h"
#include "Params.h"
#include "CmdArg.h"
#include "Curvature.h"
#include "Utils.h"

#define strLINE   "--------------------------------------------------------------------------------"
#define strTITLE  "            PoleBalance: Computer Science Dept. Texas A&M University"
#define strAUTHOR "                                                               2006 Jaerock Kwon"

#ifndef WIN32
#include <sys/times.h>

long GetTickCount()
{
    struct tms tm;
    return times(&tm);
}

#endif

void PrintUsage(char * const argv[])
{
    printf("%s\n", strLINE);
    printf("%s\n%s\n", strTITLE, strAUTHOR);
    printf("%s\n", strLINE);
    printf("PoleBalance -m:t|r [-i:InFile] [-v -t -d:### -b:### -bs:### -da:#.## -mt]\n");
    printf("options: -m:t|r    mode:training|runing\n");
    printf("         -v        verbose\n");
    printf("         -t        the trajectory of the pole cart\n");
    printf("         -d:###    delay:steps (1 step = 10ms)\n");
    printf("         -b:###    blackout:steps\n");
    printf("         -bs:###   blackout start:steps (default: 500 (5 sec))\n");
    printf("         -da:fo    dynamic activation used (dr:  0 ~ 1)\n");
    printf("         -da:all   dynamic activation used (dr :-1 ~ 1)\n");
    printf("         -da:#.##  dynamic activation used (one value for all neurons\n");
    printf("         -mt       use new value when 'da' is mutated\n");
    printf("         -f:s|m    sensory:motor to which the facilitation apply\n");
    printf("         -n:f|r    network (feedforward and recurrent)\n");
	printf("         -noise:## add noise to input (## %%)\n");
}

void PrintElapsedTime(DWORD dwStart)
{
    DWORD dwHour, dwMin, dwSec;
    dwHour = dwMin = dwSec = 0;
    dwSec = (GetTickCount()-dwStart)/1000;
    if(dwSec >= 3600)
    {
        dwHour = (dwSec/3600);
        dwSec = (dwSec%3600);
        dwMin = (dwSec/60);
        dwSec = (dwSec%60);
    }
    if(dwSec >= 60)
    {
        dwMin = (dwSec/60);
        dwSec = (dwSec%60);
    }
    printf("Elpased Time: %02d:%02d:%02d\n", dwHour, dwMin, dwSec);
}


CCmdArg *g_pCmdArg;


// it returns 0 when the problem is solved, otherwise returns -1
int main (int argc, char * const argv[]) {
    CNeuGen *m_pNeuGen;
	
    if(argc == 1)
    {
        PrintUsage(argv);
        return -1;
    }
    g_pCmdArg = new CCmdArg((char**)argv);

    if(g_pCmdArg->m_Mode == CCmdArg::MODE_RUNNING)
    {
        if(g_pCmdArg->m_szInFileName[0] == '\0')
        {
            PrintUsage(argv);
            return -1;
        }
    }
		
    m_pNeuGen = new CNeuGen(g_pCmdArg->m_szInFileName);
	// set input weight file but fail to open
	if(m_pNeuGen->m_bFailToOpen)
	{
		printf("Error: could not open %s\n", g_pCmdArg->m_szInFileName);
	}
	else // no inut weight file or succeed to open it
	{
		SYSTEMTIME st;
		GetLocalTime(&st);
		printf("Start at %04d-%02d-%02d %02d:%02d:%02d\n", 
			   st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
		
		DWORD dwStart = GetTickCount();
		
		m_pNeuGen->Run();
		
		GetLocalTime(&st);
		printf("\nStop at %04d-%02d-%02d %02d:%02d:%02d\n", 
			   st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
		
		PrintElapsedTime(dwStart);
		printf("\n");
	}
	
    int nRet = (m_pNeuGen->m_bSolved) ? 0 : -1;
	
    delete m_pNeuGen;
    delete g_pCmdArg;
	
    return nRet;
}
