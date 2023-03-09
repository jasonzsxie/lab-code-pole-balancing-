#include "backprop.h"

/* Weight Structures */

/* Input to Hidden Weights (with Biases) */
double wih[INPUT_NEURONS+1][HIDDEN_NEURONS];

/* Hidden to Output Weights (with Biases) */
double who[HIDDEN_NEURONS+1][OUTPUT_NEURONS];

/* Activations */
double inputs[INPUT_NEURONS];
double hidden[HIDDEN_NEURONS];
double target[OUTPUT_NEURONS];
double actual[OUTPUT_NEURONS];

/* Unit Errors */
double erro[OUTPUT_NEURONS];
double errh[HIDDEN_NEURONS];

/*
 *  assignRandomWeights()
 *
 *  Assign a set of random weights to the network.
 *
 */

void assignRandomWeights( void )
{
  int hid, inp, out;

  for (inp = 0 ; inp < INPUT_NEURONS+1 ; inp++) {
    for (hid = 0 ; hid < HIDDEN_NEURONS ; hid++) {
      wih[inp][hid] = RAND_WEIGHT;
    }
  }

  for (hid = 0 ; hid < HIDDEN_NEURONS+1 ; hid++) {
    for (out = 0 ; out < OUTPUT_NEURONS ; out++) {
      who[hid][out] = RAND_WEIGHT;
    }
  }

}


/*
 *  sigmoid()
 *
 *  Calculate and return the sigmoid of the val argument.
 *
 */

double sigmoid( double val )
{
  return (1.0 / (1.0 + exp(-val)));
}


/*
 *  sigmoidDerivative()
 *
 *  Calculate and return the derivative of the sigmoid for the val argument.
 *
 */

double sigmoidDerivative( double val )
{
  return ( val * (1.0 - val) );
}


/*
 *  feedForward()
 *
 *  Feedforward the inputs of the neural network to the outputs.
 *
 */

void feedForward( )
{
  int inp, hid, out;
  double sum;

  /* Calculate input to hidden layer */
  for (hid = 0 ; hid < HIDDEN_NEURONS ; hid++) {

    sum = 0.0;
    for (inp = 0 ; inp < INPUT_NEURONS ; inp++) {
      sum += inputs[inp] * wih[inp][hid];
    }

    /* Add in Bias */
    sum += wih[INPUT_NEURONS][hid];

    hidden[hid] = sigmoid( sum );

  }

  /* Calculate the hidden to output layer */
  for (out = 0 ; out < OUTPUT_NEURONS ; out++) {

    sum = 0.0;
    for (hid = 0 ; hid < HIDDEN_NEURONS ; hid++) {
      sum += hidden[hid] * who[hid][out];
    }

    /* Add in Bias */
    sum += who[HIDDEN_NEURONS][out];

    actual[out] = sigmoid( sum );

  }

}


/*
 *  backPropagate()
 *
 *  Backpropagate the error through the network.
 *
 */

void backPropagate( void )
{
  int inp, hid, out;

  /* Calculate the output layer error (step 3 for output cell) */
  for (out = 0 ; out < OUTPUT_NEURONS ; out++) {
    erro[out] = (target[out] - actual[out]) * sigmoidDerivative( actual[out] );
  }

  /* Calculate the hidden layer error (step 3 for hidden cell) */
  for (hid = 0 ; hid < HIDDEN_NEURONS ; hid++) {

    errh[hid] = 0.0;
    for (out = 0 ; out < OUTPUT_NEURONS ; out++) {
      errh[hid] += erro[out] * who[hid][out];
    }

    errh[hid] *= sigmoidDerivative( hidden[hid] );

  }

  /* Update the weights for the output layer (step 4 for output cell) */
  for (out = 0 ; out < OUTPUT_NEURONS ; out++) {

    for (hid = 0 ; hid < HIDDEN_NEURONS ; hid++) {
      who[hid][out] += (LEARN_RATE * erro[out] * hidden[hid]);
    }

    /* Update the Bias */
    who[HIDDEN_NEURONS][out] += (LEARN_RATE * erro[out]);

  }

  /* Update the weights for the hidden layer (step 4 for hidden cell) */
  for (hid = 0 ; hid < HIDDEN_NEURONS ; hid++) {

    for (inp = 0 ; inp < INPUT_NEURONS ; inp++) {
      wih[inp][hid] += (LEARN_RATE * errh[hid] * inputs[inp]);
    }

    /* Update the Bias */
    wih[INPUT_NEURONS][hid] += (LEARN_RATE * errh[hid]);

  }

}
