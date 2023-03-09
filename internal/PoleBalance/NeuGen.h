// ==========================================================================
//
// NeuGen.h
//
// Writer: jrkwon
//
// 
//
// Hisotry:
// 12/19/05 Start!
//
// ==========================================================================

#ifndef NEUGEN_H
#define NEUGEN_H

#include "GenAlg.h"
#include "CartPole.h"
#include "Curvature.h"

using namespace std;

class CNeuGen
{
public:
    /*CNeuGen();*/
    CNeuGen(char *szWeightFileName);
    ~CNeuGen();

	void Run();
    bool Update();
    bool UpdateBest();
	
private:
	void SaveInitFRates();
	void PutNewWeightsOrNewGeneration();
	void PutNewWeights();
	void MakeDataDir();
	void SaveActivationValue(int nBestGenome, bool bOrg = false);
	void TrainingSuccess(int nBestGenome);
	void SaveFacilitationRates();
	void SaveGenerationAndFitnessValue();
	void NewGeneration();
	
	void MakeDataDirBest(char *pszFileName);
	void SuccessToBalance();
	void FailToBalance();
	void SaveActivationValueBest(char *pszFileName, bool bOrg = false);
	void SaveXYForceBest(char *pszFilenName, bool bOrg = false);
	
    double GetComplexity();

public:
    CGenAlg *m_pGenAlg;
    CCartPole *m_pCartPole;
    
    VectorGenome m_vgPopulation;
    //CGenome m_Population;
	//stores the average fitness per generation for use in graphing.
	//VectorDouble m_vdAvFitness;
	//stores the best fitness per generation
	//VectorDouble m_vdBestFitness;

    //DWORD m_dwPopulationSize;
    //double m_dMutationRate;
    //double m_dCrossoverRate;
    int m_nChromo1Length;
    int m_nChromo2Length;

    DWORD m_dwTicks;
    DWORD m_nPopulation;
    DWORD m_dwGenerations;

    bool m_bNewGeneration;
    bool m_bSolved;
	bool m_bFinished;
	bool m_bFailToOpen; // failure to read the weight file

    //VectorDouble m_vdAct; // activation value for one execution
    std::vector<VectorDouble> m_vdActs; // the group of m_vdAct; kind of 2d array 
    std::vector<VectorDouble> m_vdOrgActs;
	//std::vector<VectorDouble> m_vdBestActs; // save the best activations

    CCurvature *m_pCurvature;
	
	string m_strCurDirName;
};

#endif // NEUGEN_H
