#include <math.h>
#include <stdlib.h>

#define INPUT_NEURONS		4
#define HIDDEN_NEURONS		3
#define OUTPUT_NEURONS		1

#define sqr(x)	((x) * (x))

#define LEARN_RATE		0.2

#define RAND_WEIGHT	( ((float)rand() / (float)RAND_MAX) - 0.5)

#define getSRand()	((float)rand() / (float)RAND_MAX)
#define getRand(x)      (int)((float)x*rand()/(RAND_MAX+1.0))


/* Weight Structures */

/* Input to Hidden Weights (with Biases) */
extern double wih[INPUT_NEURONS+1][HIDDEN_NEURONS];

/* Hidden to Output Weights (with Biases) */
extern double who[HIDDEN_NEURONS+1][OUTPUT_NEURONS];

/* Activations */
extern double inputs[INPUT_NEURONS];
extern double hidden[HIDDEN_NEURONS];
extern double target[OUTPUT_NEURONS];
extern double actual[OUTPUT_NEURONS];

/* Unit Errors */
extern double erro[OUTPUT_NEURONS];
extern double errh[HIDDEN_NEURONS];
