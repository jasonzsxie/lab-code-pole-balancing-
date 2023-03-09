// ==========================================================================
//
// CartPole.cpp
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
#include <iostream>
#include <iomanip>
#include "CartPole.h"
#include "Utils.h"

#define ONE_DEGREE          0.0174532	/* 2pi/360 */
#define SIX_DEGREES         0.1047192
#define TWELVE_DEGREES      0.2094384
#define FIFTEEN_DEGREES     0.2617993
#define THIRTY_DEGREES      0.5235986
#define FIFTY_DEGREES       0.87266
#define NINETY_DEGREES      1.570788

#define MUP                 0.000002
#define MUC                 0.0005
#define GRAVITY             -9.8
#define MASSCART            1.0
#define MASSPOLE_1          0.1
#define LENGTH_1            0.5		  // actually half the pole's length
#define FORCE_MAG           10.0
#define TAU                 0.01	  // seconds between state updates
#define RANDOM_START        0

#define PI                  3.141592653589793

CCartPole::CCartPole(bool bTraining)
{
    m_pNeuralNet = new CNeuralNet(nNUM_INPUTS, nNUM_OUTPUTS, 
								  nNUM_HIDDEN_LAYERS, nNUM_NEURON_PER_HIDDEN_LAYER);
    m_bMarkov = true;

    m_pdInputs = new double[nNUM_INPUTS];
//    m_pdInputs = new double[CParams::nNumInputs];
//    m_pdActive = new double[nNUM_NEURON_PER_HIDDEN_LAYER];

	m_bTraining = (bTraining) ? true : false;
    InitState();
	
//	m_pFifo = fifo_new();
}

CCartPole::~CCartPole()
{
    if(m_pNeuralNet)
        delete m_pNeuralNet;
    delete [] m_pdInputs;
	
//	fifo_free(m_pFifo, free);
    //delete m_pdActive;
}

void CCartPole::InitState(void)
{
    double adTrainingState[] = { 0.0, 0.0, 0.0, 0.0, 0.01, 0.0, 0.01, 0.0 }; // original
    //double adTestState[]     = { 0.0, 0.0, 0.0, 0.0, 0.04, 0.0, 0.03, 0.0 }; // hard one
    double adTestState[]     = { 0.0, 0.0, 0.0, 0.0, 0.01, 0.0, 0.01, 0.0 }; // same init condition
    double *pdState;

	pdState = (m_bTraining) ? adTrainingState : adTestState;
	int nSize = sizeof(adTrainingState); // size of a state array

    memcpy(m_adState, pdState, nSize); 
    memset(m_adDyDx, 0, sizeof(m_adDyDx));

    m_dwSteps = 0;

    m_InQ.Init();
}


// actionX and actionY are the outputs of network
void CCartPole::Step(double actionX, double actionY, 
                              double state[], double *derivs)
{
    double forceX,forceY,costheta_X,costheta_Y,sintheta_X,sintheta_Y,
          gsintheta_X,gsintheta_Y,temp_X,temp_Y,ml,fi_X,fi_Y,mi_X,mi_Y;

    forceX =  (actionX - 0.5) * FORCE_MAG * 2;
    forceY =  (actionY - 0.5) * FORCE_MAG * 2;
    costheta_X = cos(state[4]);
    sintheta_X = sin(state[4]);
    gsintheta_X = GRAVITY * sintheta_X;
    costheta_Y = cos(state[6]);
    sintheta_Y = sin(state[6]);
    gsintheta_Y = GRAVITY * sintheta_Y;
    
    ml = LENGTH_1 * MASSPOLE_1;
    temp_X = MUP * state[5] / ml;
    temp_Y = MUP * state[7] / ml;
    fi_X = (ml * state[5] * state[5] * sintheta_X) +
           (0.75 * MASSPOLE_1 * costheta_X * (temp_X + gsintheta_X)) + forceX;
    fi_Y = (ml * state[7] * state[7] * sintheta_Y) +
           (0.75 * MASSPOLE_1 * costheta_Y * (temp_Y + gsintheta_Y)) + forceY;
    mi_X = MASSPOLE_1 * (1 - (0.75 * costheta_X * costheta_X));
    mi_Y = MASSPOLE_1 * (1 - (0.75 * costheta_Y * costheta_Y));
    
    derivs[1] = fi_X / (mi_X + MASSCART);
    derivs[3] = fi_Y / (mi_Y + MASSCART);
    derivs[5] = -0.75 * (derivs[1] * costheta_X + gsintheta_X + temp_X)
                 / LENGTH_1;
    derivs[7] = -0.75 * (derivs[3] * costheta_Y + gsintheta_Y + temp_Y)
                 / LENGTH_1;

}

void CCartPole::RungeKutta4thOrder(double f1, double f2, double y[], 
                                        double dydx[], double yout[])
{
    int i;
    double hh,h6,dym[8],dyt[8],yt[8];

    hh=TAU*0.5;
    h6=TAU/6.0;

    for (i = 0; i < nNUM_STATES; i++) 
        yt[i] = y[i]+hh*dydx[i];
    
    Step(f1,f2,yt,dyt);
    
    dyt[0] = yt[1];
    dyt[2] = yt[3];
    dyt[4] = yt[5];
    dyt[6] = yt[7];

    for (i = 0; i < nNUM_STATES; i++) 
        yt[i] = y[i]+hh*dyt[i];
    
    Step(f1,f2,yt,dym);

    dym[0] = yt[1];
    dym[2] = yt[3];
    dym[4] = yt[5];
    dym[6] = yt[7];

    for (i = 0; i < nNUM_STATES; i++) 
    {
        yt[i]=y[i]+TAU*dym[i];
        dym[i] += dyt[i];
    }

    Step(f1,f2,yt,dyt);
    
    dyt[0] = yt[1];
    dyt[2] = yt[3];
    dyt[4] = yt[5];
    dyt[6] = yt[7];

    for (i = 0; i < nNUM_STATES; i++)
        yout[i]=y[i]+h6*(dydx[i]+dyt[i]+2.0*dym[i]);
}
 
double CCartPole::Fitness(void)
{
    return (double)m_dwSteps;
}

void CCartPole::AddNoiseToInput(void)
{
	double dNoise;
	ofstream fileInputs, fileNoiseInputs, fileNoise;
	string strFileName, strNoiseFileName, strNoise;
	

	if(g_pCmdArg->m_Mode == CCmdArg::MODE_RUNNING)
	{
		strFileName = g_pCmdArg->m_strCurDirName + "_inputs_org.txt";
		strNoiseFileName = g_pCmdArg->m_strCurDirName + "_inputs_nz.txt";
		strNoise = g_pCmdArg->m_strCurDirName + "_inputs_nz_ratio.txt";
		
		fileInputs.open(strFileName.c_str(), ofstream::out | ofstream::app);
		fileNoiseInputs.open(strNoiseFileName.c_str(), ofstream::out | ofstream::app);
		fileNoise.open(strNoise.c_str(), ofstream::out | ofstream::app);
	}
	
	for(int i = 0; i < nNUM_INPUTS; i++)
	{
		dNoise = RandInt(-100, 100)/(10000.0/g_pCmdArg->m_nNoise);
		//printf("%f\t%f\t", dNoise, m_pdInputs[i]);
		if(g_pCmdArg->m_Mode == CCmdArg::MODE_RUNNING)
		{
			fileInputs << setiosflags(ios::fixed) << setprecision(6) << m_pdInputs[i] << "\t";
			fileNoiseInputs << setiosflags(ios::fixed) << setprecision(6) << m_pdInputs[i]+m_pdInputs[i]*dNoise << "\t";
			fileNoise << setiosflags(ios::fixed) << setprecision(6) << dNoise << "\t";
		}
		
		m_pdInputs[i] += (m_pdInputs[i]*dNoise);
		//printf("%f\n", m_pdInputs[i]);
	}
	fileInputs << endl;
	fileNoiseInputs << endl;
	fileNoise << endl;
	
	fileInputs.close();
	fileNoiseInputs.close();
	fileNoise.close();
}

void CCartPole::SetupInput(void)
{
	static double s_adPrevInputs[nNUM_INPUTS];

#ifdef bINPUT_DELAY
	if(m_bMarkov)
    {
        m_pdInputs[0] = m_adState[0] / 3.0;
        m_pdInputs[1] = m_adState[1] / 1.5;
        m_pdInputs[2] = m_adState[2] / 3.0;
        m_pdInputs[3] = m_adState[3] / 1.5;
        m_pdInputs[4] = m_adState[4] / 0.52;
        m_pdInputs[5] = m_adState[5] / 2.0;
        m_pdInputs[6] = m_adState[6] / 0.52;
        m_pdInputs[7] = m_adState[7] / 2.0;
        //m_pdInputs[8] = .5;
    }
    else
    {
        m_pdInputs[0] = m_adState[0] / 3.0;
        m_pdInputs[1] = 0;
        m_pdInputs[2] = m_adState[2] / 3.0;
        m_pdInputs[3] = 0;
        m_pdInputs[4] = m_adState[4] / 0.52;
        m_pdInputs[5] = 0;
        m_pdInputs[6] = m_adState[6] / 0.52;
        m_pdInputs[7] = 0;
        //m_pdInputs[8] = .5;
    }
	
	if(g_pCmdArg->m_nNoise > 0)
	{
		AddNoiseToInput();
	}

	if(g_pCmdArg->m_nDelay != 0)
	{
		double *pdInputs;
		m_InQ.Put(m_pdInputs, sizeof(double)*nNUM_INPUTS);

		if(m_InQ.Size() == g_pCmdArg->m_nDelay+1)
		{
			pdInputs = m_InQ.Get();
			memcpy(m_pdInputs, pdInputs, sizeof(double)*nNUM_INPUTS);
		}
	}

	if(g_pCmdArg->m_nBlackOut != 0)
	{
		if(m_dwSteps >= g_pCmdArg->m_nBlackOutStart && ((m_dwSteps - g_pCmdArg->m_nBlackOutStart) < g_pCmdArg->m_nBlackOut))
			memcpy(m_pdInputs, s_adPrevInputs, sizeof(s_adPrevInputs));
		else
            memcpy(s_adPrevInputs, m_pdInputs, sizeof(s_adPrevInputs));
	}

#else
    if(m_bMarkov)
    {
        m_pdInputs[0] = m_adState[0] / 3.0;
        m_pdInputs[1] = m_adState[1] / 1.5;
        m_pdInputs[2] = m_adState[2] / 3.0;
        m_pdInputs[3] = m_adState[3] / 1.5;
        m_pdInputs[4] = m_adState[4] / 0.52;
        m_pdInputs[5] = m_adState[5] / 2.0;
        m_pdInputs[6] = m_adState[6] / 0.52;
        m_pdInputs[7] = m_adState[7] / 2.0;
        //m_pdInputs[8] = .5;
    }
    else
    {
        m_pdInputs[0] = m_adState[0] / 3.0;
        m_pdInputs[1] = 0;
        m_pdInputs[2] = m_adState[2] / 3.0;
        m_pdInputs[3] = 0;
        m_pdInputs[4] = m_adState[4] / 0.52;
        m_pdInputs[5] = 0;
        m_pdInputs[6] = m_adState[6] / 0.52;
        m_pdInputs[7] = 0;
        //m_pdInputs[8] = .5;
    }
#endif
}

void CCartPole::PerformAction(VectorDouble &Output)
{
    double dForceX, dForceY;

    dForceX = Output[0];
    dForceY = Output[1];
    
    m_adDyDx[0] = m_adState[1];
    m_adDyDx[2] = m_adState[3];
    m_adDyDx[4] = m_adState[5];
    m_adDyDx[6] = m_adState[7];
    
    Step(dForceX, dForceY, m_adState, m_adDyDx);
    RungeKutta4thOrder(dForceX, dForceY, m_adState, m_adDyDx, m_adState);

    double adTmp[nNUM_STATES];

    if(m_bMarkov)
        memcpy(adTmp, m_adState, sizeof(m_adState));

    m_adDyDx[0] = m_adState[1];
    m_adDyDx[2] = m_adState[3];
    m_adDyDx[4] = m_adState[5];
    m_adDyDx[6] = m_adState[7];
    
    Step(dForceX, dForceY, m_adState, m_adDyDx);
    RungeKutta4thOrder(dForceX, dForceY, m_adState, m_adDyDx, m_adState);

    if(m_bMarkov)
    {
        m_adDyDx[1] = (m_adState[1] - adTmp[1])/TAU;
        m_adDyDx[3] = (m_adState[3] - adTmp[3])/TAU; 
        m_adDyDx[5] = (m_adState[5] - adTmp[5])/TAU;
        m_adDyDx[7] = (m_adState[7] - adTmp[7])/TAU;
    }
}

// evaluate network, go cart!
bool CCartPole::Update(void)
{
    m_dwSteps++;

	SetupInput();

    // get x, y
    m_dCartPosX = m_adState[0];
    m_dCartPosY = m_adState[2];
    // get angles
    m_dPoleAngleX = (m_adState[4] * 360) / (2 * PI);
    m_dPoleAngleY = (m_adState[6] * 360) / (2 * PI);

    //if(g_pCmdArg->m_Mode == CCmdArg::MODE_TRAINING && g_pCmdArg->m_afpOut[1])
	if(g_pCmdArg->m_Mode == CCmdArg::MODE_RUNNING && g_pCmdArg->m_bCartTrack)
    {
        OutputDebugStringF("%f\t%f\t%f\t%f\n", m_dCartPosX, m_dCartPosY, 
						   m_dPoleAngleX, m_dPoleAngleY);
		ofstream fileGenFit;
		string strFileName;
		strFileName = g_pCmdArg->m_strCurDirName + "_xy_track.txt";
		fileGenFit.open(strFileName.c_str(), ofstream::out | ofstream::app);
		fileGenFit << setiosflags(ios::fixed) << setprecision(6) << m_dCartPosX 
			<< "\t" 
			<< setiosflags(ios::fixed) << setprecision(6) << m_dCartPosY
			<< "\t" 
			<< setiosflags(ios::fixed) << setprecision(6) << m_dPoleAngleX 
			<< "\t" 
			<< setiosflags(ios::fixed) << setprecision(6) << m_dPoleAngleY << endl;
		fileGenFit.close();
    }

    VectorDouble vdInput(m_pdInputs, m_pdInputs+nNUM_INPUTS);
    //VectorDouble vdInput(m_pdInputs, m_pdInputs+CParams::nNumInputs);
    VectorDouble vdOutput = m_pNeuralNet->Update(vdInput);
	//OutputDebugStringF("%d %d %d\n", vdOutput.size(), vdOutput[0], vdOutput[1]);
    PerformAction(vdOutput);

    double dFailureAngle;

    if(g_pCmdArg->m_Mode == CCmdArg::MODE_TRAINING)
        dFailureAngle = FIFTEEN_DEGREES;
    else
        dFailureAngle = NINETY_DEGREES;


    //double dFailureAngle = NINETY_DEGREES; //THIRTY_DEGREES;
        // check for failure
    if (m_adState[0] < -1.5 || m_adState[0] > 1.5  ||
        m_adState[2] < -1.5 || m_adState[2] > 1.5  ||
        m_adState[4] < -dFailureAngle || m_adState[4] > dFailureAngle || 
        m_adState[6] < -dFailureAngle || m_adState[6] > dFailureAngle)
        return false;

    m_dFitness = (double)m_dwSteps;
    return true;
}

/* void CCartPole::PutWeightsAndFRates(VectorDouble vdWeights)
{
	m_pNeuralNet->PutWeightsAndFRates(vdWeights);
}
 */

/*int CCartPole::GetNumberOfWeights(void)
{
    // get total number of weights in the network
    return m_pNeuralNet->GetNumberOfWeights();
}
*/

