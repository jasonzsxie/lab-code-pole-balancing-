// ==========================================================================
//
// Params.h
//
// Writer: jrkwon
//
// 
//
// Hisotry:
// 12/19/05 Start!
//
// ==========================================================================
#ifndef PARAMS_H
#define PARAMS_H

#include <fstream>
#include "Types.h"
#include "CmdArg.h"

#define bINPUT_DELAY

#define nMAX_SCALE_DATA                 50//100 //50
#define nMIN_SCALE_FACTOR               0.001 //0.0001
#define nMIN_COMPLEXITY                 200 //100

#define nMAX_GENERATION					50000 //10000 //5000 //1000

// fail if the # of generation is nMIN_GENERATION 
// and the balance steps is less than nMIN_STEPS
//#define nMIN_GENERATION					500
#define nMIN_STEPS						50 

// if this is comment out, it means print out the activation values
// no more use
//#define TRAIN_NET     
//#define PRINT_POLE_STATUS   // print x,y and angle of x and y
extern CCmdArg *g_pCmdArg;

#define nNUM_INPUTS                     8
#define nNUM_OUTPUTS                    2     // force to x and force to y
#define nNUM_HIDDEN_LAYERS              1
#define nNUM_NEURON_PER_HIDDEN_LAYER    3
#define dACTIVATION_RESPONSE            1.0
#define dBIAS                           0.5   //-1.0

// genetic algorithm
#define nPOPULATION_SIZE                50    // 600
#define dMUTATION_RATE                  0.2
#define dCROSSOVER_RATE                 0.7
//#define nCROMOSOME_LENGTH               (nNUM_INPUTS + nNUM_NEURON_PER_HIDDEN_LAYER)
#define dMAX_PERTURBATION               0.3
#define nNUM_ELITE                      3
#define nNUM_COPIES_ELITE               1

#define dFACILITATION_RATE				0.7
#define nMAX_DELAY						10 // 1 step = 10 ms
#define nBLACKOUT_START_STEP			500

#define dwNUM_TICKS                      5000 // 1000  // 5000  //180000

// pole balancing
//#define nNUM_STATES                     8

/*class CParams
{
public:
    // used for the neural network
    static int nNumInputs;
    static int nNumOutputs;
    static int nNumHiddenLayers;
    static int nNeuronsPerHiddenLayer;

    //for tweeking the sigmoid function
    static double dActivationResponse;
    //bias value
    static double dBias;

    // GA parameters
    static int nPopulationSize;
    static double dMutationRate;
    static double dCrossoverRate;

    //the maximum amount the ga may mutate each weight by
    static double dMaxPerturbation;

    //used for elitism
    static int    nNumElite;
    static int    nNumCopiesElite;

    // cart pole
    static DWORD  dwNumTicks;

    //ctor
    CParams(char* szFileName)
    {
        LoadInParameters(szFileName);
    }

    bool LoadInParameters(char* szFileName);
};
*/
#endif // PARAMS_H

