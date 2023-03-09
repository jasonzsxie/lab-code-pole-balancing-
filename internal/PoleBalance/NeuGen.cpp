// ==========================================================================
//
// NeuGen.cpp
//
// Writer: jrkwon
//
// 
//
// Hisotry:
// 12/19/05 Start!
//
// ==========================================================================

#ifdef WIN32
#include "stdafx.h"
#endif
#include "NeuGen.h"
#include "GenAlg.h"
#include "Utils.h"
#include "Curvature.h"
#ifdef WIN32
#include "Windows.h" // SYSTEMTIME
#endif
#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <stdio.h>
#ifdef WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#include <sys/types.h>

inline void _mkdir(const char *pDir)
{
	mkdir(pDir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
}

#define _chdir chdir

#endif

#define lnDELAY_WIDTH				3
#define lnGEN_WIDTH					5
#define lnFIT_WIDTH					5
#define lnBLACKOUT_WIDTH			3
#define lnBLACKOUT_START_WIDTH		5


using namespace std;

// for training
/****
CNeuGen::CNeuGen() 
{
    m_dwTicks = 0;

    m_pCartPole = new CCartPole(true);
	// the sum of weight values + the facilitation rate for all the neurons
    m_nChromo1Length = m_pCartPole->m_pNeuralNet->GetNumberOfWeights();
	m_nChromo2Length = m_pCartPole->m_pNeuralNet->GetNumberOfNeurons();

    m_pGenAlg = new CGenAlg(nPOPULATION_SIZE, dMUTATION_RATE, 
                            dCROSSOVER_RATE,
                            m_nChromo1Length, m_nChromo2Length);
	//Get the weights from the GA and insert into the sweepers brains
	m_vgPopulation = m_pGenAlg->GetChromos();
    m_nPopulation = 0;
	
	// put weight and the facilitation rate into the neural network
	// (initial random weights and the facilitation rate should be made in
	// GenAlg
    m_pCartPole->m_pNeuralNet->PutWeightsAndFRates(m_vgPopulation[0].m_vdChromo1,
												   m_vgPopulation[0].m_vdChromo2);
    
	m_bNewGeneration = false;
    m_dwGenerations = 0;
    m_bSolved = m_bFinished = false;

    m_pCurvature = new CCurvature();
}
****/

// for testing/running
CNeuGen::CNeuGen(char *szWeightFileName) 
{
    m_dwTicks = 0;

    m_pCartPole = new CCartPole(false);
    m_nChromo1Length = m_pCartPole->m_pNeuralNet->GetNumberOfWeights();
	m_nChromo2Length = m_pCartPole->m_pNeuralNet->GetNumberOfNeurons();
	
    m_pGenAlg = new CGenAlg(nPOPULATION_SIZE, dMUTATION_RATE, 
                            dCROSSOVER_RATE,
                            m_nChromo1Length, m_nChromo2Length);
	//Get the weights from the GA and insert into the sweepers brains
	m_vgPopulation = m_pGenAlg->GetChromos();
    m_nPopulation = 0;
    
    if(szWeightFileName[0] != '\0')
    {    
		std::ifstream grab(szWeightFileName);
		if(grab.is_open())
		{
			m_bFailToOpen = false;
			for(int i = 0; i < m_vgPopulation[0].m_vdChromo1.size(); i++)
				grab >> m_vgPopulation[0].m_vdChromo1[i];
			
			for(int i = 0; i < m_vgPopulation[0].m_vdChromo2.size(); i++)
				grab >> m_vgPopulation[0].m_vdChromo2[i];
		}
		else
		{
			m_bFailToOpen = true;
			return;
		}
	}
	// get facilitation rate from the file name
	//char szFileName[MAX_PATH];
	//strcpy(szFileName, szWeightFileName);
	//char *pszFRate = strrchr(szFileName, '.');  /* ~~ .txt */
	//if(pszFRate != 0) 
	//{
	//	*pszFRate = '\0';
	//    pszFRate = strrchr(szFileName, '.'); /* ~~ 0.906632 */
	//}
	//double dFRate = atof(pszFRate);
	
    m_pCartPole->m_pNeuralNet->PutWeightsAndFRates(m_vgPopulation[0].m_vdChromo1,
												   m_vgPopulation[0].m_vdChromo2);
	
	//printf("NeuGen::NeuGen: %.6f\n", m_vgPopulation[0].m_dFacilitationRate);
    m_bNewGeneration = false;
    m_dwGenerations = 0;
    m_bSolved = m_bFinished = false;

    m_pCurvature = new CCurvature();
}

CNeuGen::~CNeuGen()
{
    if(m_pGenAlg)
        delete m_pGenAlg;
    if(m_pCartPole)
        delete m_pCartPole;
    if(m_pCurvature)
        delete m_pCurvature;
}

void CNeuGen::Run()
{
    while(!m_bFinished)
    {
        if(g_pCmdArg->m_Mode == CCmdArg::MODE_TRAINING)
        {
            Update();
        }
        else 
		{
			if(UpdateBest() == false)
			   break;
		}
    }
}

void CNeuGen::PutNewWeights()
{
	// start with next pop
	m_pCartPole->InitState();
	m_pCartPole->m_dwSteps = 0;
	m_nPopulation++;

    if(m_nPopulation < nPOPULATION_SIZE)
    {
        m_pCartPole->m_pNeuralNet->PutWeightsAndFRates(
                                    m_vgPopulation[m_nPopulation].m_vdChromo1,
                                    m_vgPopulation[m_nPopulation].m_vdChromo2);
    }
	//printf("NeuGen::Update: %.6f\n", m_vgPopulation[m_nPopulation].m_dFacilitationRate);
	
	m_dwTicks = 0;
}

// empirical value
#define dALPHA		1.5
#define dBETA		1.25

void CNeuGen::PutNewWeightsOrNewGeneration()
{
	double dComplexity = 0;
	
	if(g_pCmdArg->m_bCheckComplexity)
		dComplexity = GetComplexity();
	m_vgPopulation[m_nPopulation].m_dComplexity = dComplexity;
	
	m_vgPopulation[m_nPopulation].m_dFitness 
		= m_vgPopulation[m_nPopulation].m_dSteps = m_pCartPole->m_dFitness;
	if(dComplexity != 0)
		m_vgPopulation[m_nPopulation].m_dFitness -= dALPHA*pow(dComplexity, dBETA);
	
	OutputDebugStringF("%4d: %.2f %f %f\n", m_dwGenerations, 
					   m_vgPopulation[m_nPopulation].m_dSteps, dComplexity, 
					   m_vgPopulation[m_nPopulation].m_dFitness);
	
	m_vgPopulation[m_nPopulation].m_vdActs = m_vdActs;
	m_vgPopulation[m_nPopulation].m_vdOrgActs = m_vdOrgActs;

	// clear activation value array
	m_vdActs.erase(m_vdActs.begin(), m_vdActs.end());
	m_vdOrgActs.erase(m_vdOrgActs.begin(), m_vdOrgActs.end());
	
	if(m_nPopulation < nPOPULATION_SIZE-1)//CParams::nPopulationSize-1)//
	{
		PutNewWeights();
	}
	else
	{
		// start new generation
		m_bNewGeneration = true;// goto Epoch
		//OutputDebugStringF("%d Generation -------\n", m_dwGenerations);
	}	
}

void CNeuGen::SaveActivationValue(int nBestGenome, bool bOrg /* = false */)
{
	ofstream fileActValue;
	
	std::stringstream strTemp;
	strTemp << setfill('0');

 	string strTail;
	if(bOrg)
		strTail = "_act_org.txt";
	else
		strTail = "_act.txt";

//	if(g_pCmdArg->m_dDynActRate != -1)
	if(g_pCmdArg->m_bDynamicAct)
		strTemp << m_strCurDirName << "_tg" << setw(lnGEN_WIDTH) << m_dwGenerations << "f"
				<< setw(lnFIT_WIDTH) << m_pGenAlg->BestFitness() 
				/*<< "_fr" << setiosflags(ios::fixed) 
				<< setprecision(2)*/ << g_pCmdArg->m_dDynActRate << strTail;
	else
		strTemp << m_strCurDirName << "_tg" << setw(lnGEN_WIDTH) << m_dwGenerations << "f"
				<< setw(lnFIT_WIDTH) << m_pGenAlg->BestFitness() << strTail;
	
	std::string strFileName = strTemp.str();
	fileActValue.open(strFileName.c_str());
	
	VectorDouble vdActivations;
	for(unsigned int i = 0; i < m_vgPopulation[nBestGenome].m_vdActs.size(); i++)
	{
		if(bOrg)
			vdActivations = m_vgPopulation[nBestGenome].m_vdOrgActs[i];
		else
			vdActivations = m_vgPopulation[nBestGenome].m_vdActs[i];
		for(int j = 0; j < nNUM_NEURON_PER_HIDDEN_LAYER; j++)
		{
			OutputDebugStringF("%f\t", vdActivations[j]);
			fileActValue << setiosflags(ios::fixed) 
						 << setprecision(6) << vdActivations[j] << "\t";
		}
		OutputDebugStringF("\n\r");
		fileActValue << endl;
	}
	fileActValue.close();	
}

void CNeuGen::SaveFacilitationRates()
{
	string strFinalFileName; // all the facilitation rates
	ofstream fileFinalDRate; 
	
	// all the facilitation rates
	strFinalFileName = m_strCurDirName + "_FinalDRate.txt";
	fileFinalDRate.open(strFinalFileName.c_str());
	
	for(int i = 0; i < nPOPULATION_SIZE; i++)
	{
		for(int j = 0; j < m_vgPopulation[i].m_vdChromo2.size(); j++)
			fileFinalDRate << setiosflags(ios::fixed) 
							<< setprecision(6) 
							<< m_vgPopulation[i].m_vdChromo2[j] 
							<< "\t";//endl;
		fileFinalDRate << setw(6) << (int)m_vgPopulation[i].m_dFitness;
		fileFinalDRate << endl;
	}
	fileFinalDRate.close();
}

void CNeuGen::TrainingSuccess(int nBestGenome)
{
	m_bSolved = m_bFinished = true;
	
	OutputDebugStringF("The best fitness: %.2f\n", m_pGenAlg->BestFitness());
	
	ofstream fileWeight;
	stringstream streamWeight;
	
	streamWeight << setfill('0');
#ifdef WIN32
	streamWeight << "..\\";
#else
	streamWeight << "../";		
#endif
	if(g_pCmdArg->m_dDynActRate != -1)
		streamWeight << m_strCurDirName << "_tg" << setw(lnGEN_WIDTH) 
					<< m_dwGenerations << "f" << setw(lnFIT_WIDTH) 
					<< m_pGenAlg->BestFitness() 
					/*<< " fr" << setiosflags(ios::fixed) 
					<< setprecision(6) << g_pCmdArg->m_dDynActRate*/ << ".txt";
	else
		streamWeight << m_strCurDirName << "_tg" << setw(lnGEN_WIDTH) 
					<< m_dwGenerations << "f" << setw(lnFIT_WIDTH) 
					<< m_pGenAlg->BestFitness() << ".txt";
	
	
	string strWeightFile = streamWeight.str();
	
	fileWeight.open(strWeightFile.c_str()); //"weight.txt");
	for(int i = 0; i < m_vgPopulation[nBestGenome].m_vdChromo1.size(); i++)
	{
		OutputDebugStringF("Weight[%2d]: %f\n", 
						   i, m_vgPopulation[nBestGenome].m_vdChromo1[i]);
		fileWeight << setiosflags(ios::fixed) 
					<< setprecision(6) 
					<< m_vgPopulation[nBestGenome].m_vdChromo1[i] 
					<< endl;
	}
	for(int i = 0; i < m_vgPopulation[nBestGenome].m_vdChromo2.size(); i++)
	{
		OutputDebugStringF("Facilitation rate[%2d]: %f\n", 
						   i, m_vgPopulation[nBestGenome].m_vdChromo2[i]);
		fileWeight << setiosflags(ios::fixed) 
					<< setprecision(6) 
					<< m_vgPopulation[nBestGenome].m_vdChromo2[i] 
					<< endl;
	}
	// let's see the facilitation rate values distribution for all
	// the population
	SaveFacilitationRates();
}

void CNeuGen::NewGeneration()
{
	m_dwGenerations++;
	m_dwTicks = 0;
	m_nPopulation = 0;
				
	//run the GA to create a new population
	m_vgPopulation = m_pGenAlg->Epoch(m_vgPopulation);
				
	m_pCartPole->m_pNeuralNet->PutWeightsAndFRates(
												m_vgPopulation[0].m_vdChromo1,
												m_vgPopulation[0].m_vdChromo2);
	
	m_pCartPole->InitState();
				
}

void CNeuGen::SaveGenerationAndFitnessValue()
{
	ofstream fileGenFit;
	fileGenFit.open("GenFit.txt", ofstream::out | ofstream::app);
	
	fileGenFit << setw(lnGEN_WIDTH) <<  m_dwGenerations << "\t" 
				<< setw(lnFIT_WIDTH) << m_pGenAlg->BestFitness() << "\n";
	fileGenFit.close();	
}

void CNeuGen::SaveInitFRates(void)
{
	ofstream FileInitFRates;
	string strFileName;
	strFileName = m_strCurDirName + "_InitDRates.txt";
	FileInitFRates.open(strFileName.c_str());
	
	for(int i = 0; i < nPOPULATION_SIZE; ++i)
	{
		for(int j = 0; j < m_nChromo2Length; j++)
		{
			FileInitFRates << setiosflags(ios::fixed) << setprecision(6) 
						   << m_vgPopulation[i].m_vdChromo2[j] << "\t";
		}
		FileInitFRates << endl;		
	}
	FileInitFRates.close();		
}

// training
bool CNeuGen::Update(void)
{
	static bool s_fSuccess = false;
	static bool s_fCreatedDir = false;

	if(s_fCreatedDir == false)
	{
		MakeDataDir();
		s_fCreatedDir = true;
		SaveInitFRates();
	}

	if (!m_bNewGeneration)
	{
        if(m_pCartPole->Update() == false) // finish balancing
        {
			PutNewWeightsOrNewGeneration();
            return false;
        }
		else
        {
            // update fitness value (# of steps)
            m_vgPopulation[m_nPopulation].m_dFitness = m_pCartPole->m_dFitness;

            // save activation value of hidden neuron
            VectorDouble vdAct = m_pCartPole->m_pNeuralNet->GetActivations();
            m_vdActs.push_back(vdAct);

	    // save org activation value of hidden neuron
            VectorDouble vdOrgAct = m_pCartPole->m_pNeuralNet->GetOrgActivations();
            m_vdOrgActs.push_back(vdOrgAct);


			if(m_pCartPole->m_dFitness >= dwNUM_TICKS)
			{
				s_fSuccess = true;
				// goto Epoch and finish training in the next turn
                //m_bNewGeneration = true;

				m_vgPopulation[m_nPopulation].m_dComplexity = 0; // not used
				m_vgPopulation[m_nPopulation].m_dSteps = m_pCartPole->m_dFitness;
				m_vgPopulation[m_nPopulation].m_vdActs = m_vdActs;
				m_vgPopulation[m_nPopulation].m_vdOrgActs = m_vdOrgActs;
				
				PutNewWeights();
				
				return false;
			}
			if(s_fSuccess)
			{
				if(m_nPopulation == nPOPULATION_SIZE)
					m_bNewGeneration = true; // goto Epoch and finish it
			}
        }
	}
	else
	{
		m_bNewGeneration = false;
        
		m_pGenAlg->SetChromos(m_vgPopulation);
		m_pGenAlg->CalculateBestWorstAvTot();
		int nBestGenome = m_pGenAlg->BestGenome();

        OutputDebugStringF("\n-----------------------\n");
		OutputDebugStringF("%4d: %.2f\n", m_dwGenerations, m_pGenAlg->BestFitness());
        OutputDebugStringF("-----------------------\n");
		
		SaveGenerationAndFitnessValue();
		
		//if(m_vgPopulation[nBestGenome].m_vdActs.size() >= nMAX_SCALE_DATA)
		//{
		//	SaveActivationValue();
		//}

		if(m_vgPopulation[nBestGenome].m_dFitness >= dwNUM_TICKS)
		{
			SaveActivationValue(nBestGenome);
			SaveActivationValue(nBestGenome, true);

			TrainingSuccess(nBestGenome);
		}
		else
		{
			if(m_dwGenerations < nMAX_GENERATION)
			{
				NewGeneration();
			}
			else
			{
				m_bSolved = false; // not solved!!!! fail to evolve the network
				m_bFinished = true;
				SaveFacilitationRates();
			}
		}
	}

	return true;
}

void CNeuGen::MakeDataDir()
{
	SYSTEMTIME SysTime;
	
	GetLocalTime(&SysTime);
	
	std::stringstream strDirStream;
	//char chMode;
	//chMode = (g_pCmdArg->m_Mode == CCmdArg::MODE_TRAINING) ? 't' : 'r';
	
	strDirStream << setfill('0');
	
    if(g_pCmdArg->m_szInFileName[0] != '\0')
	{
		char szInputFileNmae[MAX_PATH];
		strcpy(szInputFileNmae, g_pCmdArg->m_szInFileName);
		char* pPtr = strstr(szInputFileNmae, ".txt");
		if(pPtr != NULL)
		{
			*pPtr = '\0';
		}
        strDirStream << szInputFileNmae;
	}
    
	if(g_pCmdArg->m_bDynamicAct)
	{
		strDirStream /*<< chMode */
            << (g_pCmdArg->m_bNdpia ? "_n" : "_f")
            << (g_pCmdArg->m_bRecurrentNet ? "_rc" : "_ff")
            << (g_pCmdArg->m_bFacilMotor ? "_fm" : "" )
            << (g_pCmdArg->m_bFacilSensor ? "_fs" : "")
            << (g_pCmdArg->m_bBigMutate ? "_bmt_" : "_smt_")
            << setw(4) << SysTime.wYear  
            << setw(2) << SysTime.wMonth
            << setw(2) << SysTime.wDay 
            << setw(2) << SysTime.wHour 
            << setw(2) << SysTime.wMinute << setw(2) 
			<< SysTime.wSecond;
		if(g_pCmdArg->m_nDelay != 0)
			strDirStream << "_d" << setw(lnDELAY_WIDTH) << g_pCmdArg->m_nDelay;
        strDirStream << "_fr" << setiosflags(ios::fixed) 
            << setprecision(2) << g_pCmdArg->m_dDynActRate;
	}
	else
	{
		strDirStream  /*<< chMode 
            << (g_pCmdArg->m_bNdpia ? "_n" : "_f")*/
            << (g_pCmdArg->m_bRecurrentNet ? "rc" : "ff")
            /*<< (g_pCmdArg->m_bFacilMotor ? "_fm" : "" )
            << (g_pCmdArg->m_bFacilSensor ? "_fs" : "") */
            << (g_pCmdArg->m_bBigMutate ? "_bmt_" : "_smt_")
            << setw(4) << SysTime.wYear 
            << setw(2) << SysTime.wMonth
            << setw(2) << SysTime.wDay
            << setw(2) << SysTime.wHour
            << setw(2) << SysTime.wMinute 
			<< setw(2) << SysTime.wSecond;
		if(g_pCmdArg->m_nDelay != 0)
		   strDirStream << "_d" << setw(lnDELAY_WIDTH) << g_pCmdArg->m_nDelay;
	}
	
    
	/*	
		if(g_pCmdArg->m_dDynActRate != -1)
	 strDirStream << chMode << 'd' << setw(4) << SysTime.wYear << "." 
					<< setw(2) << SysTime.wMonth
					<< "." << setw(2) << SysTime.wDay << "_" 
					<< setw(2) << SysTime.wHour << "." 
					<< setw(2) << SysTime.wMinute << "." << setw(2) 
					<< SysTime.wSecond << "_d" << setw(lnDELAY_WIDTH) 
					<< g_pCmdArg->m_nDelay
					<< "_fr" << setiosflags(ios::fixed) 
					<< setprecision(2) << g_pCmdArg->m_dDynActRate;
	 else
	 strDirStream << chMode << 'd' << setw(4) << SysTime.wYear << "." 
					<< setw(2) << SysTime.wMonth
					<< "." << setw(2) << SysTime.wDay << "_" 
					<< setw(2) << SysTime.wHour << "." 
					<< setw(2) << SysTime.wMinute << "." << setw(2) 
					<< SysTime.wSecond << "_d" << setw(lnDELAY_WIDTH) 
					<< g_pCmdArg->m_nDelay;
	 */	
	g_pCmdArg->m_strCurDirName = m_strCurDirName = strDirStream.str();
	
	_mkdir(m_strCurDirName.c_str());
	_chdir(m_strCurDirName.c_str());	
}

void CNeuGen::MakeDataDirBest(char *pszFileName)
{
	SYSTEMTIME SysTime;
	stringstream strDirStream;
	stringstream strTemp;
	
	GetLocalTime(&SysTime);
	
	strDirStream << setfill('0');
	strDirStream << pszFileName;
	if(g_pCmdArg->m_nDelay != 0)
		strDirStream << "_d" << setw(lnDELAY_WIDTH) << g_pCmdArg->m_nDelay;

	if(g_pCmdArg->m_nBlackOut != 0)
	{
		strDirStream << "_b" << setw(lnBLACKOUT_WIDTH) << g_pCmdArg->m_nBlackOut
		<< "_bs" << setw(lnBLACKOUT_START_WIDTH);
	}
	if(g_pCmdArg->m_bDynamicAct)
	{
		strDirStream << "_fr" << setiosflags(ios::fixed)
		<< setprecision(2) << g_pCmdArg->m_dDynActRate;
	}
	if(g_pCmdArg->m_nNoise != 0)
		strDirStream << "_nz" << setw(3) << g_pCmdArg->m_nNoise;
	
	/*
	 strDirStream << pszFileName << "_d" 
				<< setw(lnDELAY_WIDTH) << g_pCmdArg->m_nDelay
				<< "_b" << setw(lnBLACKOUT_WIDTH) << g_pCmdArg->m_nBlackOut
				<< "_bs" << setw(lnBLACKOUT_START_WIDTH) 
				<< g_pCmdArg->m_nBlackOutStart
                     << "_fr" << setiosflags(ios::fixed)
                     << setprecision(2) << g_pCmdArg->m_dDynActRate;
	 */
	
	g_pCmdArg->m_strCurDirName = m_strCurDirName = strDirStream.str();
	
	_mkdir(m_strCurDirName.c_str());
	_chdir(m_strCurDirName.c_str());
	strDirStream.flush();	
}

void CNeuGen::SuccessToBalance()
{
	ofstream fileResult;
	stringstream streamFileName;
	string strFileName;
	
	m_bSolved = m_bFinished = true;
	OutputDebugStringVF("\nSuccess to balance %d steps\n", m_dwTicks);
	streamFileName <<  m_strCurDirName  
                       //<< "_fr" << setiosflags(ios::fixed)
                       //<< setprecision(2) << g_pCmdArg->m_dDynActRate
                       << "_success." << m_dwTicks << ".txt";
	
	strFileName = streamFileName.str();
	fileResult.open(strFileName.c_str());
	fileResult.close();
	
	// copy this to the parent dir for my convenience
#ifdef WIN32
	string strCopyFile = "..\\" + strFileName;
#else
	string strCopyFile = "../" + strFileName;
#endif
	fileResult.open(strCopyFile.c_str());
	fileResult.close();
}

void CNeuGen::FailToBalance()
{
	ofstream fileResult;
	stringstream streamFileName;
	string strFileName;
	
	m_bSolved = false;
	m_bFinished = true;
	
	m_vdActs.erase(m_vdActs.begin(), m_vdActs.end());
	m_vdOrgActs.erase(m_vdOrgActs.begin(), m_vdOrgActs.end());

	OutputDebugStringVF("\nFailed to balance at %d step\n", m_dwTicks);
	streamFileName << m_strCurDirName 
                       //<< "_fr" << setiosflags(ios::fixed)
                       //<< setprecision(2) << g_pCmdArg->m_dDynActRate
                       << "_failure." << m_dwTicks << ".txt" ;
		
	strFileName = streamFileName.str();
	fileResult.open(strFileName.c_str());
	fileResult.close();
	// copy this to the parent dir for my convenience
#ifdef WIN32
	string strCopyFile = "..\\" + strFileName;
#else
	string strCopyFile = "../" + strFileName;
#endif
	fileResult.open(strCopyFile.c_str());
	fileResult.close();
}

void CNeuGen::SaveActivationValueBest(char *pszFileName, bool bOrg /* = false */)
{
	VectorDouble vdActivations;
	string strFileName;
	
	if(bOrg)	
	{
		vdActivations = m_pCartPole->m_pNeuralNet->GetOrgActivations();
		m_vdOrgActs.push_back(vdActivations);
	}
	else
	{
		vdActivations = m_pCartPole->m_pNeuralNet->GetActivations();
		m_vdActs.push_back(vdActivations);
	}
	
	ofstream fileActValue;
	stringstream strTemp;
	string strTail;
	if(bOrg)
		strTail = "_act_org.txt";
	else
		strTail = "_act.txt";

	//strTemp << pszFileName << strTail;
	strTemp << m_strCurDirName << strTail;
	
	strFileName = strTemp.str();
	fileActValue.open(strFileName.c_str(), ofstream::out | ofstream::app);
	
    for(int i = 0; i < nNUM_NEURON_PER_HIDDEN_LAYER; i++)
    {
        OutputDebugStringF("%f\t", vdActivations[i]);
		fileActValue << setiosflags(ios::fixed) << setprecision(6) << vdActivations[i] << "\t";
    }
    OutputDebugStringF("\n");
	fileActValue << endl;	
}

void CNeuGen::SaveXYForceBest(char *pszFileName, bool bOrg /* = false */)
{
	ofstream fileXYValue;
	stringstream strTemp2;
	VectorDouble vdActivations;
	string strFileName;

	string strTail;
	if(bOrg)
	{
		vdActivations = m_pCartPole->m_pNeuralNet->GetOrgActivations();
		strTail = "_xy_force_org.txt";
	}
	else
	{
		vdActivations = m_pCartPole->m_pNeuralNet->GetActivations();
		strTail = "_xy_force.txt";
	}
	

	strTemp2 << m_strCurDirName << strTail;
	strFileName = strTemp2.str();
	fileXYValue.open(strFileName.c_str(), ofstream::out | ofstream::app);
	
	for(int j = nNUM_NEURON_PER_HIDDEN_LAYER; j < nNUM_NEURON_PER_HIDDEN_LAYER+nNUM_OUTPUTS; j++)
	{
        OutputDebugStringF("%f\t", vdActivations[j]);
		//fileActValue << setfill('0');
		fileXYValue << setiosflags(ios::fixed) << setprecision(6) << vdActivations[j] << "\t";
	}
    OutputDebugStringF("\n");
	fileXYValue << endl;	
}

// running
// using weights value from an input file
bool CNeuGen::UpdateBest()
{
	static bool fCreatedDir = false;
	char *pszFileName;
	char *pszTemp;
	char szFileName[MAX_PATH];

	// trim '.txt'
	strcpy(szFileName, g_pCmdArg->m_szInFileName); 
	pszFileName = szFileName;
	if(pszTemp = strrchr(pszFileName, '.'))
		*pszTemp = '\0';

	if(fCreatedDir == false)
	{
		MakeDataDirBest(pszFileName);		
		fCreatedDir = true;
	}

	if (m_dwTicks == dwNUM_TICKS)
	{
		SuccessToBalance();
		return true;
	}
	m_dwTicks++;

    if(m_pCartPole->Update() == false)
	{
		FailToBalance();
		return false;
    }
        
	SaveActivationValueBest(pszFileName);
	SaveActivationValueBest(pszFileName, true);
	
	SaveXYForceBest(pszFileName);
	SaveXYForceBest(pszFileName, true);
	 
 	return true;
}

double CNeuGen::GetComplexity()
{
    //SCurvData CurvData; 
	SCircleData CircleData;
    double *pdSrc[3];

    // make src data from m_vdActs
    size_t nRow = m_vdActs.size();

    if(nRow < nMAX_SCALE_DATA)
        return 0;
    
    for(int i = 0; i < nNUM_NEURON_PER_HIDDEN_LAYER; i++)
    {
        pdSrc[i] = new double [nRow];
    }

    for(int i = 0; i < nNUM_NEURON_PER_HIDDEN_LAYER; i++)
    {
        for(size_t j = 0; j < nRow; j++)
        {
            pdSrc[i][j] = m_vdActs[j][i];
        }
    }

    m_pCurvature->GetCircleCountInfo(&CircleData, pdSrc, (int)nRow);
	return CircleData.m_nAverage;
}
