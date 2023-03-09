// ==========================================================================
//
// GenAlg.cpp
//
// Author: jrkwon
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
#endif
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <vector>
#include <algorithm>

#include "GenAlg.h"
#include "Params.h"
#include "Utils.h"

//-----------------------------------constructor-------------------------
//
//	sets up the population with random floats
//
//-----------------------------------------------------------------------
CGenAlg::CGenAlg(int nPopSize, double dMutationRate, 
                 double dCrossoverRate, int	nChromo1Length, int nChromo2Length) 
                  : m_nPopSize(nPopSize),
                    m_dMutationRate(dMutationRate),
                    m_dCrossoverRate(dCrossoverRate),
                    m_nChromo1Length(nChromo1Length),
					m_nChromo2Length(nChromo2Length),
                    m_dTotalFitness(0),
                    m_dwGeneration(0),
                    m_nFittestGenome(0),
                    m_dBestFitness(0),
                    m_dWorstFitness(99999999),
                    m_dAverageFitness(0)
{
	srand( (unsigned)time( NULL ) );

	// initialise population with chromosomes consisting of random
	// weights and all fitnesses set to zero
						
	for(int i = 0; i < m_nPopSize; ++i)
	{
		m_vgPopulation.push_back(CGenome());

		for(int j = 0; j < m_nChromo1Length; ++j)
		{
			m_vgPopulation[i].m_vdChromo1.push_back(RandomClamped());
		}
		
		for(int j = 0; j < m_nChromo2Length; j++)
		{
			if(g_pCmdArg->m_bDynamicAct)
			{
				if(g_pCmdArg->m_bFRateOnly)
//#ifdef sFACILIATION_ONLY
					m_vgPopulation[i].m_vdChromo2.push_back(RandFloat());
				else
//#else
					m_vgPopulation[i].m_vdChromo2.push_back(RandomClamped());
//#endif
			}
			else // actually unused
			{
				m_vgPopulation[i].m_vdChromo2.push_back(RandomClamped());
			}
		}
	}
}


//---------------------------------Mutate--------------------------------
//
//	mutates a chromosome by perturbing its weights by an amount not 
//	greater than dMAX_PERTURBATION
//-----------------------------------------------------------------------
void CGenAlg::Mutate1(VectorDouble &vdChromo1)
{
	//traverse the chromosome and mutate each weight dependent
	//on the mutation rate
	for (unsigned int i=0; i<vdChromo1.size(); ++i)
	{
		//do we perturb this weight?
		if (RandFloat() < m_dMutationRate)
		{
			//add or subtract a small value to the weight
            vdChromo1[i] += (RandomClamped() * dMAX_PERTURBATION);//CParams::dMaxPerturbation);//
		}
	}
}

void CGenAlg::Mutate2(VectorDouble &vdChromo2)
{
	double dPerturb;
	
	//traverse the chromosome and mutate each weight dependent
	//on the mutation rate
	for (unsigned int i=0; i<vdChromo2.size(); ++i)
	{
		//do we perturb this weight?
		if (RandFloat() < m_dMutationRate)
		{
			if(g_pCmdArg->m_bBigMutate)
			{
				// actually new value instead of perturbed values
				vdChromo2[i] = RandomClamped();
				continue;
			}
		
			dPerturb = (RandomClamped() * dMAX_PERTURBATION);
			
			// add or subtract a small value to the weight
			vdChromo2[i] += dPerturb;
			
			// but it should not be below zero and over one
			if(g_pCmdArg->m_bDynamicAct)
			{
				if(g_pCmdArg->m_bFRateOnly)
				{
					if(vdChromo2[i] < 0)
						vdChromo2[i] = 0;
					else if((g_pCmdArg->m_bNdpia == false) && (vdChromo2[i] > 1))
						vdChromo2[i] = 1;//RandFloat();
				}
				else
				{
					if((g_pCmdArg->m_bNdpia == false) && (vdChromo2[i] < -1))
						vdChromo2[i] = -1;//RandomClamped(); //-1;
					else if((g_pCmdArg->m_bNdpia == false) && (vdChromo2[i] > 1))
						vdChromo2[i] = 1;//RandomClamped(); //1;
				}
			}
		}
	}
}

//----------------------------------GetChromoRoulette()------------------
//
//	returns a chromo based on roulette wheel sampling
//
//-----------------------------------------------------------------------
CGenome CGenAlg::GetChromoRoulette()
{
	//generate a random number between 0 & total fitness count
	double Slice = (double)(RandFloat() * m_dTotalFitness);

	//this will be set to the chosen chromosome
	CGenome TheChosenOne;
	
	//go through the chromosones adding up the fitness so far
	double FitnessSoFar = 0;
	
	for (int i=0; i<m_nPopSize; ++i)
	{
		FitnessSoFar += m_vgPopulation[i].m_dFitness;
		
		//if the fitness so far > random number return the chromo at 
		//this point
		if (FitnessSoFar >= Slice)
		{
			TheChosenOne = m_vgPopulation[i];
			break;
		}
	}

	return TheChosenOne;
}
	
//-------------------------------------Crossover()-----------------------
//	
//  given parents and storage for the offspring this method performs
//	crossover according to the GAs crossover rate
//-----------------------------------------------------------------------
bool CGenAlg::Crossover1(const VectorDouble &mum,
                        const VectorDouble &dad,
                        VectorDouble       &baby1,
                        VectorDouble       &baby2)
{
	//just return parents as offspring dependent on the rate
	//or if parents are the same
	if ( (RandFloat() > m_dCrossoverRate) || (mum == dad)) 
	{
		baby1 = mum;
		baby2 = dad;
		
		return false;
	}
	
	//determine a crossover point
	//unsigned int ;
	m_nCrossoverPoint = RandInt(0, m_nChromo1Length - 1);
	
	//create the offspring
	for (unsigned int i = 0; i < m_nCrossoverPoint; ++i)
	{
		baby1.push_back(mum[i]);
		baby2.push_back(dad[i]);
	}
	
	for (unsigned int i = m_nCrossoverPoint; i<mum.size(); ++i)
	{
		baby1.push_back(dad[i]);
		baby2.push_back(mum[i]);
	}
	
	
	return true;
}
 
// Crossover2 is affected by Crossover1, because Chromo2 should follow Chromo1's
// crossover rate. Actually Chromo2 values are linked with Chromo1
void CGenAlg::Crossover2(const VectorDouble &mum,
						 const VectorDouble &dad,
						 VectorDouble       &baby1,
						 VectorDouble       &baby2)
{	
	// determine a crossover point based on m_nCrossoverPoint of Crossover1
	double dRatio = (double)m_nCrossoverPoint / (double)m_nChromo1Length;
	unsigned int cp = (unsigned int) (m_nChromo2Length * dRatio);
	
	//create the offspring
	for (unsigned int i = 0; i < cp; ++i)
	{
		baby1.push_back(mum[i]);
		baby2.push_back(dad[i]);
	}
	
	for (unsigned i = cp; i < mum.size(); ++i)
	{
		baby1.push_back(dad[i]);
		baby2.push_back(mum[i]);
	}
	
	
	return;
}

/* void CGenAlg::Crossover(const VectorDouble &mum,
						const VectorDouble &dad,
						VectorDouble       &baby1,
						VectorDouble       &baby2)
{
	//just return parents as offspring dependent on the rate
	//or if parents are the same
	if ( (RandFloat() > m_dCrossoverRate) || (mum == dad)) 
	{
		baby1 = mum;
		baby2 = dad;

		return;
	}

	//determine a crossover point
	unsigned int cp = RandInt(0, m_nChromoLength - 1);

	//create the offspring
	for (unsigned int i=0; i<cp; ++i)
	{
		baby1.push_back(mum[i]);
		baby2.push_back(dad[i]);
	}

	for (unsigned i=cp; i<mum.size(); ++i)
	{
		baby1.push_back(dad[i]);
		baby2.push_back(mum[i]);
	}
	
	
	return;
}
 */

//-----------------------------------Epoch()-----------------------------
//
//	takes a population of chromosones and runs the algorithm through one
//	 cycle.
//	Returns a new population of chromosones.
//
//-----------------------------------------------------------------------
vector<CGenome> CGenAlg::Epoch(vector<CGenome> &old_pop)
{
    //assign the given population to the classes population
    m_vgPopulation = old_pop;

    //reset the appropriate variables
    Reset();

    //sort the population (for scaling and elitism)
    sort(m_vgPopulation.begin(), m_vgPopulation.end());

    //calculate best, worst, average and total fitness
    CalculateBestWorstAvTot();

    //create a temporary vector to store new chromosones
    vector <CGenome> vecNewPop;

	//Now to add a little elitism we shall add in some copies of the
	//fittest genomes. Make sure we add an EVEN number or the roulette
  //wheel sampling will crash
	if (!(nNUM_COPIES_ELITE * nNUM_ELITE % 2))
    //if (!(CParams::nNumCopiesElite * CParams::nNumElite % 2)) //
	{
		GrabNBest(nNUM_ELITE, nNUM_COPIES_ELITE, vecNewPop);
        //GrabNBest(CParams::nNumElite, CParams::nNumCopiesElite, vecNewPop);
	}
	

	//now we enter the GA loop
	
	//repeat until a new population is generated
	while (vecNewPop.size() < (unsigned int)m_nPopSize)
	{
		//grab two chromosones
		CGenome mum = GetChromoRoulette();
		CGenome dad = GetChromoRoulette();

		//create some offspring via crossover
		VectorDouble		baby11, baby12, baby21, baby22;

		if(Crossover1(mum.m_vdChromo1, dad.m_vdChromo1, baby11, baby12))
		{
			// only for Dynamic Activation case
			Crossover2(mum.m_vdChromo2, dad.m_vdChromo2, baby21, baby22);
		}
		else // if no crossover occurred, just copy it 
		{
			baby21 = mum.m_vdChromo2;
			baby22 = dad.m_vdChromo2;
		}

		//now we mutate
		Mutate1(baby11);
		Mutate1(baby12);
		// only for Dynamic Activation case
		
		Mutate2(baby21);
		Mutate2(baby22);
			
		//double dNewFacilitationRate = (mum.m_dFacilitationRate + dad.m_dFacilitationRate)/2;
		
		//now copy into vecNewPop population
		vecNewPop.push_back(CGenome(baby11, baby21, 0));
		vecNewPop.push_back(CGenome(baby12, baby22, 0));
	}

	//finished so assign new pop back into m_vgPopulation
	m_vgPopulation = vecNewPop;

	return m_vgPopulation;
}


//-------------------------GrabNBest----------------------------------
//
//	This works like an advanced form of elitism by inserting NumCopies
//  copies of the NBest most fittest genomes into a population vector
//--------------------------------------------------------------------
void CGenAlg::GrabNBest(int	            NBest,
                        const int	      NumCopies,
                        vector<CGenome>	&Pop)
{
  //add the required amount of copies of the n most fittest 
	//to the supplied vector
	while(NBest--)
	{
		for (int i=0; i<NumCopies; ++i)
		{
			Pop.push_back(m_vgPopulation[(m_nPopSize - 1) - NBest]);
	  }
	}
}

//-----------------------CalculateBestWorstAvTot-----------------------	
//
//	calculates the fittest and weakest genome and the average/total 
//	fitness scores
//---------------------------------------------------------------------
void CGenAlg::CalculateBestWorstAvTot()
{
	m_dTotalFitness = 0;
	
	double HighestSoFar = 0;
	double LowestSoFar  = 9999999;
	
	for (int i=0; i<m_nPopSize; ++i)
	{
		//update fittest if necessary
		if (m_vgPopulation[i].m_dFitness > HighestSoFar)
		{
			HighestSoFar	 = m_vgPopulation[i].m_dFitness;
			
			m_nFittestGenome = i;

			m_dBestFitness	 = HighestSoFar;
		}
		
		//update worst if necessary
		if (m_vgPopulation[i].m_dFitness < LowestSoFar)
		{
			LowestSoFar = m_vgPopulation[i].m_dFitness;
			
			m_dWorstFitness = LowestSoFar;
		}
		
		m_dTotalFitness	+= m_vgPopulation[i].m_dFitness;
		
		
	}//next chromo
	
	m_dAverageFitness = m_dTotalFitness / m_nPopSize;
}

//-------------------------Reset()------------------------------
//
//	resets all the relevant variables ready for a new generation
//--------------------------------------------------------------
void CGenAlg::Reset()
{
	m_dTotalFitness		= 0;
	m_dBestFitness		= 0;
	m_dWorstFitness		= 9999999;
	m_dAverageFitness	= 0;
}

