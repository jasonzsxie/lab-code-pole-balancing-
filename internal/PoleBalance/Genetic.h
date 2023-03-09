// ==========================================================================
//
// Genetic.h
//
// Writer: jrkwon
//
// 
//
// Hisotry:
// 12/19/05 Start!
//
// ==========================================================================

#ifndef GENETIC_H
#define GENETIC_H

#include "Types.h"

class CGenome
{
public:
    CGenome(int nSize);
    ~CGenome() { };

    void Init(void);

public:
    VectorDouble m_vdChromosome;
    int m_nSize;

    double m_dFintness;
};

typedef std::vector<CGenome> VectorGenome;

class CGenetic
{
public:
    CGenetic::CGenetic(DWORD dwPopulationSize, double dMutationRate, 
                       double dCrossoverRate, int nChromosoneSize);
    ~CGenetic();
    
    void Init();

    void NewPopulation(void);
    void InitPopulation(void);
    void Evolve(void);
    VectorGenome Epoch(VectorGenome &vgOldPopulation);
    static int CompareFitness(const void * v1, const void * v2);

public:
    double m_dSumFitness;
    double m_dBestFitness;
    double m_dAverageFitness;

    VectorDouble m_vdWeights;
    int m_nChromosomeSize;

    VectorGenome m_vgPopulation;
    DWORD m_dwPopulationSize;
    DWORD m_dwGeneration;

    double m_dMutationRate;
    double m_dCrossoverRate;
};

#endif // GENETIC_H
