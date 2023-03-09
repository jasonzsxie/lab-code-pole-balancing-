// ==========================================================================
//
// NeuralNet.h
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

#ifndef NEURALNET_H
#define NEURALNET_H

#include <vector>
#include <math.h>
#include "Types.h"

//#define FULLY_RECURRENT_NETWORK

//#define FACILITATE_ACTIVATION
//#define EVOLVING_FACILITATION_RATE

//---------------------------------------------------------------------------
//	Neuron
//---------------------------------------------------------------------------

class CNeuron
{
public:
    CNeuron(int nNumInputs);

public:
    int	m_nNumInputs;           // the number of inputs into the neuron
    
	// these two values which are weights and the facilitation rate will be
	// dealt as a part of a chromosome in genetic algorithm.
	// actually, a chromosome has the weights and the facilitation rate of all
	// the neuron consisting a network
	VectorDouble m_vdWeight;    // the weights for each input
	double m_dDynActRate; // dynamic act rate

    double m_dActivation;    // after applying activation rates
    double m_dOrgActivation; // original activation value (before  
                             // applying activation rates)
};

typedef std::vector<CNeuron> VectorNeuron;

//---------------------------------------------------------------------
//	a layer of neurons
//---------------------------------------------------------------------

class CNeuronLayer
{
public:
    CNeuronLayer(int nNumNeurons, int nNumInputsPerNeuron);

public:	
    int	m_nNumNeurons;          // the number of neurons in this layer
    VectorNeuron m_vNeurons;	// the layer of neurons
};

typedef std::vector<CNeuronLayer> VectorNeuronLayer;


//----------------------------------------------------------------------
//	neural net class
//----------------------------------------------------------------------

class CNeuralNet
{
public:
    CNeuralNet(int nNumInputs, int nNumOutputs, int nNumHiddenLayers,
			   int nNeuronsPerHiddenLayer);

    void CreateNet();
    //VectorDouble GetWeights() const;
    VectorDouble GetActivations() const;
    VectorDouble GetOrgActivations() const;
	double GetFacilitationRate() const;
	//void PutFacilitationRate(dFacilitationRate);

    // returns total number of weights in net
    int GetNumberOfWeights() const;
	int GetNumberOfNeurons() const;

    // replaces the weights and the facilitation rates with new ones
    void PutWeightsAndFRates(VectorDouble &vdWeights, VectorDouble &FRates);

    // calculates the outputs from a set of inputs
    VectorDouble Update(VectorDouble &vdInputs);

    // sigmoid response curve
    inline double Sigmoid(double dActivation, double dResponse);

private:
    int	m_nNumInputs;
    int	m_nNumOutputs;
    int	m_nNumHiddenLayers;
    int	m_nNeuronsPerHiddenLayer;
    // storage for each layer of neurons including the output layer
    VectorNeuronLayer	m_vLayers;
};
				
#endif // NEURALNET_H

