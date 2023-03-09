// ==========================================================================
//
// GenAlg.h
//
// Writer: jrkwon
//      The original version was written by Mat Buckland.
//
// 
//
// Hisotry:
//      12/19/05 Start!
//
// ==========================================================================

#ifndef GENALG_H
#define GENALG_H

#include "Types.h"

//---------------------------------------------------------------------------
//	genome
//---------------------------------------------------------------------------
class CGenome
{
public:
    CGenome() : m_dFitness(0) { }
    CGenome(VectorDouble vdChromo1, VectorDouble vdChromo2, double f)
			: m_vdChromo1(vdChromo1), m_vdChromo2(vdChromo2), m_dFitness(f) { }

    //overload '<' used for sorting
    friend bool operator<(const CGenome& lhs, const CGenome& rhs)
    {
        return (lhs.m_dFitness < rhs.m_dFitness);
    }

public:
    VectorDouble m_vdChromo1; // for weights
	VectorDouble m_vdChromo2; // for facilitation rates
	
    double m_dFitness;
	double m_dSteps;
	double m_dComplexity;
	std::vector<VectorDouble> m_vdActs; // save the best activations
	std::vector<VectorDouble> m_vdOrgActs;
};

typedef std::vector<CGenome> VectorGenome;

//---------------------------------------------------------------------------
//	the genetic algorithm class
//---------------------------------------------------------------------------
class CGenAlg
{
public:
    CGenAlg(int nPopSize, double dMutationRate, 
            double dCrossoverRate, int nChromo1Length, int nChromo2Length);

	//this runs the GA for one generation.
	VectorGenome Epoch(VectorGenome &vgOldPop);
	//CGenome Epoch(CGenome &OldPop);

	VectorGenome GetChromos() const { return m_vgPopulation; }
	void SetChromos(VectorGenome& vgPop) { m_vgPopulation = vgPop; }
	double AverageFitness() const { return m_dTotalFitness / m_nPopSize;}
	double BestFitness() const { return m_dBestFitness;}
	int BestGenome() const { return m_nFittestGenome; }

public:
    //VectorDouble m_vdChromosomes;

private:
    // genetic operation
	bool Crossover1(const VectorDouble &vdMum, const VectorDouble &vdDad,
						VectorDouble &vdBaby1, VectorDouble &vdBaby2);
    void Crossover2(const VectorDouble &vdMum, const VectorDouble &vdDad,
					VectorDouble &vdBaby1, VectorDouble &vdBaby2);

    void Mutate1(VectorDouble &vdChromo1);
    void Mutate2(VectorDouble &vdChromo2);
    CGenome	GetChromoRoulette();

    //use to introduce elitism
    void GrabNBest(int nNBest, const int nNumCopies, VectorGenome &vgPop);
public:
	void CalculateBestWorstAvTot(); // only for test
private:
    void Reset();

private:
    VectorGenome m_vgPopulation;// the entire population of chromosomes
    int m_nPopSize;             // size of population
    int m_nChromo1Length;        // amount of weights per chromo
	int m_nChromo2Length;
	
    double m_dTotalFitness;     // total fitness of population
    double m_dBestFitness;      // best fitness this population
    double m_dAverageFitness;   // average fitness
    double m_dWorstFitness;     // worst

    int	m_nFittestGenome;       // keeps track of the best genome

    // probability that a chromosones bits will mutate.
    // Try figures around 0.05 to 0.3 ish
    double m_dMutationRate;

    // probability of chromosones crossing over bits
    // 0.7 is pretty good
    double m_dCrossoverRate;

    //generation counter
    DWORD m_dwGeneration;
	
	// crossover point for Crossover1
	int m_nCrossoverPoint;
};

#endif // GENALG_H

