// ==========================================================================
//
// NeuralNet.cpp
//
// Author: jrkwon
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
#endif
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "NeuralNet.h"
#include "Utils.h"
#include "Params.h"

//---------------------------------------------------------------------------
// neuron
//---------------------------------------------------------------------------
CNeuron::CNeuron(int nNumInputs): m_nNumInputs(nNumInputs+1)											
{
	// actually, this initialization part will not be used in the PoleBalance
	// project, because GenAlg generates random chromosomes and put them into
	// the NeuralNet's weights and faciliation rates
	srand( (unsigned)time( NULL ) );

	//we need an additional weight for the bias hence the +1
	for(int i = 0; i < nNumInputs+1; i++)
	{
		//set up the weights with an initial random value
		m_vdWeight.push_back(RandomClamped());
	}
	m_dDynActRate = RandFloat();
	m_dActivation = 0.0;
}

//---------------------------------------------------------------------------
//	ctor creates a layer of neurons of the required size by calling the 
//	CNeuron ctor the rqd number of times
//---------------------------------------------------------------------------
CNeuronLayer::CNeuronLayer(int nNumNeurons, int nNumInputsPerNeuron)
                           : m_nNumNeurons(nNumNeurons)
{
	for(int i = 0; i < nNumNeurons; i++)
		m_vNeurons.push_back(CNeuron(nNumInputsPerNeuron));
}

//---------------------------------------------------------------------------
//	creates a ANN based on the default values in params.ini
//---------------------------------------------------------------------------
CNeuralNet::CNeuralNet(int nNumInputs, int nNumOutputs, int nNumHiddenLayers,
					   int nNeuronsPerHiddenLayer) 
{
    m_nNumInputs = nNumInputs; // nNUM_INPUTS;
    m_nNumOutputs = nNumOutputs;//nNUM_OUTPUTS;
	m_nNumHiddenLayers = nNumHiddenLayers;//nNUM_HIDDEN_LAYERS;
	m_nNeuronsPerHiddenLayer = nNeuronsPerHiddenLayer;//nNUM_NEURON_PER_HIDDEN_LAYER;

	CreateNet();
}

//---------------------------------------------------------------------------
//	this method builds the ANN. The weights are all initially set to 
//	random values -1 < w < 1
//---------------------------------------------------------------------------
void CNeuralNet::CreateNet()
{
    //create the layers of the network
    if(m_nNumHiddenLayers > 0)
    {
        //#ifdef FULLY_RECURRENT_NETWORK
        if(g_pCmdArg->m_bRecurrentNet)
        {
		//m_nNumInputs = nNUM_INPUTS+nNUM_NEURON_PER_HIDDEN_LAYER;//change the # of inputs
            m_nNumInputs += nNUM_NEURON_PER_HIDDEN_LAYER;//change the # of inputs
        }
//#endif
        //create first hidden layer
        m_vLayers.push_back(CNeuronLayer(m_nNeuronsPerHiddenLayer, 
                                         m_nNumInputs));

        for(int i = 0; i < m_nNumHiddenLayers-1; i++)
        {
            m_vLayers.push_back(CNeuronLayer(m_nNeuronsPerHiddenLayer,
                                             m_nNeuronsPerHiddenLayer));
        }
        //create output layer
        m_vLayers.push_back(CNeuronLayer(m_nNumOutputs, m_nNeuronsPerHiddenLayer));
    }
    else
    {
        //create output layer
        m_vLayers.push_back(CNeuronLayer(m_nNumOutputs, m_nNumInputs));
    }
}

//---------------------------------------------------------------------------
//	returns a vector containing the weights
//---------------------------------------------------------------------------

/* VectorDouble CNeuralNet::GetWeights() const
{
	//this will hold the weights
	VectorDouble vdWeights;
	
	//for each layer
	for(int i=0; i<m_nNumHiddenLayers + 1; ++i)
	{
		//for each neuron
		for(int j = 0; j < m_vLayers[i].m_nNumNeurons; ++j)
		{
			//for each weight
			for(int k = 0; k < m_vLayers[i].m_vNeurons[j].m_nNumInputs; ++k)
			{
				vdWeights.push_back(m_vLayers[i].m_vNeurons[j].m_vdWeight[k]);
			}
		}
	}

	return vdWeights;
} */

VectorDouble CNeuralNet::GetActivations() const
{
	//this will hold the weights
	VectorDouble vdActivations;
	
	//for each layer
	for(int i=0; i<m_nNumHiddenLayers + 1; ++i)
	{
		//for each neuron
		for(int j = 0; j < m_vLayers[i].m_nNumNeurons; ++j)
		{
            vdActivations.push_back(m_vLayers[i].m_vNeurons[j].m_dActivation);
		}
	}

	return vdActivations;
}

VectorDouble CNeuralNet::GetOrgActivations() const
{
	//this will hold the weights
	VectorDouble vdActivations;
	
	//for each layer
	for(int i=0; i<m_nNumHiddenLayers + 1; ++i)
	{
		//for each neuron
		for(int j = 0; j < m_vLayers[i].m_nNumNeurons; ++j)
		{
            vdActivations.push_back(m_vLayers[i].m_vNeurons[j].m_dOrgActivation);
		}
	}

	return vdActivations;
}


//---------------------------------------------------------------------------
//	given a vector of doubles this function replaces the weights in the NN
//  with the new values
//---------------------------------------------------------------------------
void CNeuralNet::PutWeightsAndFRates(VectorDouble &vdWeights, 
									 VectorDouble &vdFRates)
{
	int cWeight = 0;
	int cFRate = 0;
	
	//for each layer
	for(int i = 0; i < m_nNumHiddenLayers + 1; ++i)
	{
		//for each neuron
		for(int j = 0; j < m_vLayers[i].m_nNumNeurons; ++j)
		{
			//for each weight
			for(int k = 0; k < m_vLayers[i].m_vNeurons[j].m_nNumInputs; ++k)
			{
				m_vLayers[i].m_vNeurons[j].m_vdWeight[k] 
												= vdWeights[cWeight++];
			}
			// the last element is the facilitation rate of the neuron for each 
			// pair of weights and the facilitation rate value.
			// the structure of vdWeightsAndFRates is like
			// ------+|------+....
			// wghts f wghts f
			m_vLayers[i].m_vNeurons[j].m_dDynActRate = vdFRates[cFRate++];
		}
	}
}

//---------------------------------------------------------------------------
//	returns the total number of weights needed for the net
//---------------------------------------------------------------------------
int CNeuralNet::GetNumberOfWeights() const
{
	int nNumWeights = 0;
	
	//for each layer
	for(int i = 0; i < m_nNumHiddenLayers + 1; ++i)
	{
		//for each neuron
		for(int j = 0; j < m_vLayers[i].m_nNumNeurons; ++j)
		{
			//for each weight
			nNumWeights += m_vLayers[i].m_vNeurons[j].m_nNumInputs;
			//for(int k = 0; k < m_vLayers[i].m_vNeurons[j].m_nNumInputs; ++k)
			//	nNumWeights++;
		}
	}

	return nNumWeights;
}

//---------------------------------------------------------------------------
//	returns the total number of weights needed for the net
//---------------------------------------------------------------------------
int CNeuralNet::GetNumberOfNeurons() const
{
	int nNumNeurons = 0;
	
	//for each layer
	for(int i = 0; i < m_nNumHiddenLayers + 1; ++i)
	{
		nNumNeurons += m_vLayers[i].m_nNumNeurons;
	}
	
	return nNumNeurons;
}

//---------------------------------------------------------------------------
//	given an input vector this function calculates the output vector
//---------------------------------------------------------------------------
VectorDouble CNeuralNet::Update(VectorDouble &vdInputs)
{
//#ifdef FULLY_RECURRENT_NETWORK
	static VectorDouble s_vdPrevOutputs; // store previous output
//#endif
	//static double s_dPrevAct;
    //stores the resultant outputs from each layer
    VectorDouble vdOutputs;

    //first check that we have the correct amount of inputs
    //if(vdInputs.size() != m_nNumInputs)
    //{
    //    //just return an empty vector if incorrect.
    //    return vdOutputs;
    //}

    //For each layer....
    for(int i = 0; i < m_nNumHiddenLayers + 1; ++i)
    {		
        if ( i > 0 )
        {
            vdInputs = vdOutputs;
        }
//#ifdef FULLY_RECURRENT_NETWORK 
		//else // only 1 hidden layer can be used
        else if(g_pCmdArg->m_bRecurrentNet)
		{
			if(s_vdPrevOutputs.size() == 0)
			{
				for(int j = 0; j < nNUM_NEURON_PER_HIDDEN_LAYER; j++)
					vdInputs.push_back(0.0);
			}
			else
			{
				for(unsigned int j = 0; j < s_vdPrevOutputs.size(); j++)
				{
					vdInputs.push_back(s_vdPrevOutputs[j]);
					//OutputDebugStringF("%f\t", m_vdPrevOutputs[j]);
				}
			}
			//OutputDebugStringF("\n");
		}
//#endif
		//OutputDebugStringF("vdInputs size %d\n", vdInputs.size());
		//for(int j = 0; j < vdInputs.size(); j++)
		//{
		//	OutputDebugStringF("%2d: %f\n", j, vdInputs[j]);
		//}

        vdOutputs.clear();

        //for each neuron sum the (inputs * corresponding weights).Throw 
        //the total at our sigmoid function to get the output.
        for(int j = 0; j < m_vLayers[i].m_nNumNeurons; ++j)
        {
            double dNetInput = 0;
            double dActivation;
            int	nNumInputs = m_vLayers[i].m_vNeurons[j].m_nNumInputs;
            int cWeight = 0;

            //for each weight
            for(int k = 0; k < nNumInputs - 1; ++k)
            {
                //sum the weights x inputs
                dNetInput += m_vLayers[i].m_vNeurons[j].m_vdWeight[k]
                             * vdInputs[cWeight++];
            }

            //add in the bias
            dNetInput += m_vLayers[i].m_vNeurons[j].m_vdWeight[nNumInputs-1]
                         * dBIAS;//CParams::dBias;//

            // we can store the outputs from each layer as we generate them. 
            // The combined activation is first filtered through the sigmoid 
            // function

            double dAct;
            dAct = Sigmoid(dNetInput, dACTIVATION_RESPONSE);

			if((g_pCmdArg->m_bDynamicAct) 
                // the first hidden layer only
                && ((g_pCmdArg->m_bFacilSensor && i == 0) 
                   || (g_pCmdArg->m_bFacilMotor && i == m_nNumHiddenLayers)))
            {
                    //double dAct;
                    double dActRate;
                    double dPrevAct;
                    
                    //dAct = Sigmoid(dNetInput, dACTIVATION_RESPONSE);
                    
                    if(g_pCmdArg->m_bNdpia)
                        dPrevAct = m_vLayers[i].m_vNeurons[j].m_dOrgActivation;
                    else
                        dPrevAct = m_vLayers[i].m_vNeurons[j].m_dActivation;                    
                    
                    if(g_pCmdArg->m_dDynActRate != -1)
                        dActRate = g_pCmdArg->m_dDynActRate;
                    else
                        dActRate = m_vLayers[i].m_vNeurons[j].m_dDynActRate;
                    
                    //dFRate = -0.7;
                    dActivation = dAct + dActRate*(dAct - dPrevAct);
            }
			else
			{
			//	dActivation = Sigmoid(dNetInput, dACTIVATION_RESPONSE);
                dActivation = dAct; // just copy the activation value
			}
			//s_dPrevAct = dActivation;

            m_vLayers[i].m_vNeurons[j].m_dActivation = dActivation;
            m_vLayers[i].m_vNeurons[j].m_dOrgActivation = dAct;

            vdOutputs.push_back(dActivation);//CParams::dActivationResponse));

//#ifdef FULLY_RECURRENT_NETWORK
			if((g_pCmdArg->m_bRecurrentNet) && (i == m_nNumHiddenLayers-1))
			{
				s_vdPrevOutputs = vdOutputs;//.push_back(dActivation);// = vdOutputs; // store previous output
			}
//#endif
        }
    }

    return vdOutputs;
}

//---------------------------------------------------------------------------
//
//------------------------------------------------------------------------
double CNeuralNet::Sigmoid(double dNetinput, double dResponse)
{
	return ( 1 / ( 1 + exp(-dNetinput / dResponse)));
}

