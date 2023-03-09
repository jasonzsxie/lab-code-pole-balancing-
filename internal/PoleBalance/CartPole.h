// ==========================================================================
//
// CartPole.h
//
// Writer: jrkwon
//
// Hisotry:
// 12/20/05 Start!
//
// ==========================================================================

#ifndef CARTPOLE_H
#define CARTPOLE_H

#include "Params.h"
#include "Types.h"
#include "NeuralNet.h"
#include "Utils.h"
#include "Queue.h"

#define nNUM_STATES     nNUM_INPUTS   //8  // same as nNUM_INPUTS

//
// Implements the pole balancing dynamics using the Runge-Kutta 4th-order 
// integration method.
//
class CCartPole
{
public:
    CCartPole(bool bTraining);
    ~CCartPole();

    void InitState(void);
    void Step(double dActionX, double dActionY, 
              double *pdState, double *pdDerivs);

    void RungeKutta4thOrder(double dF1, double dF2, double *pdY, 
                            double *pdDydx, double *pdYout);
    void PerformAction(VectorDouble &Output);

    double Fitness(void);
    void SetupInput();

    bool Update(void);
    //void PutWeightsAndFRates(VectorDouble vdWeights);
    //int GetNumberOfWeights(void);
	void AddNoiseToInput(void);
	
public:
    CNeuralNet* m_pNeuralNet;
    double m_dFitness;
    
    double m_adState[nNUM_STATES];
    double m_adDyDx[nNUM_STATES];
    double *m_pdInputs;//[nNUM_INPUTS];
    //double *m_pdActives;//[nNUM_NEURON_PER_HIDDEN_LAYER];
    
    bool m_bMarkov;
    DWORD m_dwSteps;
   
    double m_dCartMass;
    double m_dPoleMass;
    double m_dPoleLength;
    double m_dForceMag;

    double m_dCartPosX;
    double m_dCartPosY;
    double m_dPoleAngleX;
    double m_dPoleAngleY;

	bool   m_bTraining;		// training or test a network
	//CCircularQ m_InQ;
	CQueue m_InQ;
	//fifo_t* m_pFifo;
	
};

#endif // CARTPOLE_H
