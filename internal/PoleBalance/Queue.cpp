#ifdef WIN32
#include "stdafx.h"
#endif

#include "Queue.h"

CQueue::CQueue()
{
	Init();
}

void CQueue::Init()
{
	nFirst = 0;
	nLast = nMAX_QUEUE_SIZE-1;
	nCount = 0;
}

void CQueue::Put(double* pdData, int nSize)
{
	if (nCount >= nMAX_QUEUE_SIZE)
		printf("Warning: queue overflow enqueue \n");
	else {
		nLast = (nLast+1) % nMAX_QUEUE_SIZE;
		memcpy(adData[ nLast ], pdData, nSize);    
		nCount++;

		//for(int i = 0; i < 8; i++)
		//	printf("%.6f\t", q[last][i]);
		//printf("\n");
	}
}

double* CQueue::Get(void)
{
	double *pData;

	if (nCount <= 0) 
	{
		printf("Warning: empty queue dequeue.\n");
	}
	else {
		pData = adData[ nFirst ];
		nFirst = (nFirst+1) % nMAX_QUEUE_SIZE;
		nCount = nCount - 1;
	}
	
	//for(int i = 0; i < 8; i++)
	//	printf("%.6f\t", data[i]);
	//printf("\n");
	return(pData);
}

bool CQueue::IsEmpty()
{
	if (nCount <= 0) 
		return true;
	else 
		return false;
}

int CQueue::Size()
{
	return nCount;
}

void CQueue::Print()
{
	int i;

	i=nFirst; 
        
	while (i != nLast) {
	    for(int j = 0; j < nNUM_INPUTS; j++)
			printf("%.6f\t", adData[i][j]);
		i = (i+1) % nMAX_QUEUE_SIZE;
	}

	printf("\n");
}
