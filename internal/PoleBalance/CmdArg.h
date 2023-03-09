// ==========================================================================
//
// CmdArg.h
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
#ifndef CMDARG_H
#define CMDARG_H

#include <stdio.h>
#include <string>

#ifndef MAX_PATH
#define MAX_PATH 255
#endif

#define nMAX_FILENAME_LENGTH     MAX_PATH

class CCmdArg
{
public:
    CCmdArg(char *argv[]);
    ~CCmdArg();
	
    void Parse(char *argv[]);
    void WriteTime(FILE *fp);

private:
    bool CheckAndCopy(char* argv[], char *pszToken, char *pszStr);
    bool CheckAndCopy(char* argv[], char *pszToken, char &ch);

public:
	// only for global variable
	std::string m_strCurDirName; 	
	
	// command arg
    enum eMode {
        MODE_TRAINING, /* m:t get weight of networks */
        MODE_RUNNING,  /* m:r get activation values */
    };

    eMode m_Mode;
    bool m_bCheckComplexity;
    char m_szInFileName[nMAX_FILENAME_LENGTH];
    //char m_aszOutFileName[nMAX_OUTFILE][nMAX_FILENAME_LENGTH];

    FILE* m_fpIn;
    //FILE* m_afpOut[nMAX_OUTFILE];

    bool m_bVerbose;
	bool m_bCartTrack;
	int m_nDelay;    // unit: step (10ms)
	unsigned int m_nBlackOut; // unit: step (10ms)
	unsigned int m_nBlackOutStart;
	double m_dDynActRate; // dynamic act rate for the all neurons
	bool m_bDynamicAct; // 
	bool m_bFRateOnly; //  
	bool m_bBigMutate; // -mt
    bool m_bNdpia; // NDPIA
    bool m_bFacilMotor; // f:m
    bool m_bFacilSensor; // f:s
    bool m_bRecurrentNet;
	int m_nNoise; // %
};

#endif // CMDARG_H

