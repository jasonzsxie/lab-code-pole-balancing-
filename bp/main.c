#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#ifdef WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#include <sys/types.h>

void _mkdir(const char *pDir)
{
	mkdir(pDir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
}
#define _chdir chdir
#endif

#include "backprop.h"
#include "types.h"
#include "utils.h"

#define MAX_TRAINING_SAMPLES	3000
#define MAX_TEST_SAMPLES		1000
#define MAX_RESIDUE_SAMPLES		1000
#define MAX_SAMPLES				(MAX_TRAINING_SAMPLES+MAX_TEST_SAMPLES\
								 +MAX_RESIDUE_SAMPLES)

double hiddens[HIDDEN_NEURONS][MAX_SAMPLES];
double errors[MAX_TEST_SAMPLES];
double targets[MAX_TEST_SAMPLES];
double actuals[MAX_TEST_SAMPLES];
double results[HIDDEN_NEURONS];

char szCurDir[MAX_PATH];

BOOL ReadSamples(char* filename)
{
	FILE* fin;
	int i;

	fin = fopen(filename, "rt");

	if(!fin)
		return FALSE;
		
	for(i = 0; i < MAX_SAMPLES; i++)
	{
		fscanf(fin, "%lf\t%lf\t%lf", 
			   &(hiddens[0][i]), &(hiddens[1][i]), &(hiddens[2][i]));
	}
	
	fclose(fin);
	
	return TRUE;
}

double MinMax(double *vector, int size)
{
	double min = 1.0, max = -1.0;
	int i;
	
	for(i = 0; i < size; i++)
	{
		if(vector[i] < min)
			min = vector[i];
		if(vector[i] > max)
			max = vector[i];
	}
	
	return fabs(max-min);
}

void MakeCurTimeDir()
{
	SYSTEMTIME SysTime;
	
	GetLocalTime(&SysTime);
	sprintf(szCurDir, "%02d%02d%02d-%02d%02d%02d",
					SysTime.wYear-2000, SysTime.wMonth, SysTime.wDay,
			SysTime.wHour, SysTime.wMinute, SysTime.wSecond);
	_mkdir(szCurDir);
	//_chdir(szCurDir);
}

// not used
void Test(int neuron)
{
	int sample;
	int i, j;
	double err;
	int sum;
	
	/* Test the network */
	for (sample = 0 ; sample < MAX_TRAINING_SAMPLES ; sample++) 
	{
		for (i = 0; i < INPUT_NEURONS; i++)
			inputs[i] = hiddens[neuron][sample+i];
		
		for (j = 0; j < OUTPUT_NEURONS; j++)
			target[j] = hiddens[neuron][sample+i+j];
		
		feedForward();
		
		err = target[0]-actual[0];
		
		//fprintf(out,"%lf,%lf,%lf,%lf,%lf,%lf,%lf\n", 
		//		inputs[0], inputs[1], inputs[2], inputs[3], 
		//		target[0], actual[0], err);
		printf("%lf,%lf,%lf,%lf,%lf,%lf,%lf\n", 
			   inputs[0], inputs[1], inputs[2], inputs[3], 
			   target[0], actual[0], err);
		
		
		if (err > -0.015 && err < 0.015) 
		{
			sum++;
		}
		
	}
	
	printf("Network is %g%% correct\n", 
		   ((float)sum / (float)MAX_TRAINING_SAMPLES) * 100.0);
}

void Training(int neuron)
{
	int sample, iterations;
	int i, j;
	double err;
	
	/* init */
	sample = 0; 
	iterations = 0;
	
	/* Seed the random number generator */
	srand( time(NULL) );
	assignRandomWeights();
	
	while (1) {
		
		if (++sample == MAX_TRAINING_SAMPLES) sample = 0;
		
		for(i = 0; i < INPUT_NEURONS; i++)
			inputs[i] = hiddens[neuron][sample+i];
		
		for(j = 0; j < OUTPUT_NEURONS; j++)
			target[j] = hiddens[neuron][sample+i+j];
		
		feedForward();
		
		/* need to iterate through all ... */
		
		err = 0.0;
		for (i = 0 ; i < OUTPUT_NEURONS ; i++) {
			err += sqr( (target[i] - actual[i]) );
		}
		err = 0.5 * err;
		
		//fprintf(out, "%lf\n", err);
		//printf("mse = %lf\n", err);
		
		if (iterations++ > 100000) break;
		if (err == 0.0) break;
		
		backPropagate();
		
	}	
}

void Run(int neuron, FILE* fout)
{
	int i, j, k;
	int sample;
	
	for (k = 0, sample = MAX_TRAINING_SAMPLES; 
		 sample < (MAX_TRAINING_SAMPLES+MAX_TEST_SAMPLES); sample++, k++) 
	{
		for (i = 0; i < INPUT_NEURONS; i++)
			inputs[i] = hiddens[neuron][sample+i];
		
		for (j = 0; j < OUTPUT_NEURONS; j++)
			target[j] = hiddens[neuron][sample+i+j];
		
		feedForward();
		
		targets[k] = target[0];
		actuals[k] = actual[0];
		errors[k] = target[0]-actual[0];
		
		fprintf(fout,"%lf,%lf,%lf,%lf,%lf,%lf,%lf\n", 
				inputs[0], inputs[1], inputs[2], inputs[3], 
				target[0], actual[0], errors[k]);
		//printf("%lf,%lf,%lf,%lf,%lf,%lf,%lf\n", 
		//	   inputs[0], inputs[1], inputs[2], inputs[3], 
		//	   target[0], actual[0], err);
	}
}

void CalcErrors(int neuron)
{
	double minmax;
	double min_err;
	int sum;
	int k;
	
	/* calculate errors */
	minmax = MinMax(targets, MAX_TEST_SAMPLES);
	min_err = minmax/10.0;
	sum = 0;
	for(k = 0; k < MAX_TEST_SAMPLES; k++)
	{
		if(fabs(errors[k]) < min_err)
			sum++;
	}
	
	results[neuron] = ((float)sum / (float)MAX_TEST_SAMPLES) * 100.0;
	printf("Network is %g%% correct\n", results[neuron]);
}

void SaveResults()
{
	FILE* fp;
	int neuron;
	char szFile[MAX_PATH];
	
	sprintf(szFile, "%s/results.txt", szCurDir);
	
	fp = fopen(szFile, "w");
	for(neuron = 0; neuron < HIDDEN_NEURONS; neuron++)
		fprintf(fp, "%g\n", results[neuron]);
	fclose(fp);
}

int main(int argc, char* argv[])
{
	int neuron;
	FILE *out;
	char szFile[MAX_PATH];
    
	MakeCurTimeDir();
	
	//if(!ReadSamples(argv[1]))
	//if(!ReadSamples("DATA/tg00817f05000_act.txt"))
	if(!ReadSamples("DATA/tg01421f05000_act.txt"))
		return -1;
	
	for(neuron = 0; neuron < HIDDEN_NEURONS; neuron++)
	{
		sprintf(szFile, "%s/h%d.txt", szCurDir, neuron); 
		out = fopen(szFile, "w");
		
		/* training and running */
		Training(neuron);
		Run(neuron, out);
		CalcErrors(neuron);

		fclose(out);
	}
	
	SaveResults();
	
    return 0;
}
