#ifndef _QUEUE_H_
#define	_QUEUE_H_

#include "Params.h"

#define nMAX_QUEUE_SIZE       100

class CQueue 
{
public:
	CQueue();
	
	void	Put(double *data, int size);
	double	*Get();
	bool	IsEmpty();
	int		Size();
	void	Print();
	void    Init();
public:
	double adData[nMAX_QUEUE_SIZE+1][nNUM_INPUTS];		/* body of queue */
	int nFirst;                      /* position of first element */
	int nLast;                       /* position of last element */
	int nCount;                      /* number of queue elements */
};

#endif /* _QUEUE_H_ */
