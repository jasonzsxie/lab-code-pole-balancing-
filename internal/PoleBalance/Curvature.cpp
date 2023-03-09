// ==========================================================================
//
// Curvature.cpp
//
// Writer: jrkwon
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

#include <vector>
#include <math.h>
#include "Curvature.h"

CCurvature::CCurvature()
{
#if 0
    // an example of usage
    ///* *******************
    double *pdSrc[3];

    // make src data from m_vdActs
    size_t nRow = 5000;
    
    for(int i = 0; i < nNUM_NEURON_PER_HIDDEN_LAYER; i++)
    {
        pdSrc[i] = new double [nRow];
    }

    FILE *fp;
    fp = fopen("G43_F50_C30.txt", "r+t");
    float d1, d2, d3;

    for(size_t j = 0; j < nRow; j++)
    {
        fscanf(fp, "%f\t%f\t%f", &d1, &d2, &d3);
        pdSrc[0][j] = d1, pdSrc[1][j] = d2, pdSrc[2][j] = d3;
    }
    fclose(fp);


    SCircleData CurvData; 

    GetCircleCountInfo(&CurvData, pdSrc, 50); //5000);
    //printf("\nXY:%f\t YZ:%f\t ZX:%f\nAvg:%f\t Max:%f\t Total: %f\tStd: %f\n", 
    //       CurvData.m_dCurvXY, CurvData.m_dCurvYZ, CurvData.m_dCurvZX,
    //       CurvData.m_dAverage, CurvData.m_dMax, CurvData.m_dTotal, CurvData.m_dStd);

    delete [] pdSrc[0];
    delete [] pdSrc[1];
    delete [] pdSrc[2];
    //******************* */
#endif /* 0 */
}

CCurvature::~CCurvature()
{
}

int CCurvature::CircleThreePoints(SCircle *c, SPoint *K, SPoint *L, SPoint *M)
{	
	double xkl, ykl, xkm, ykm, cr,d, rkl, rkm, xcc, ycc;
	xkl = L ->x - K->x;	
	ykl = L ->y - K->y;	
	xkm = M ->x - K->x;	
	ykm = M ->y - K->y;
	cr=xkl*ykm-xkm*ykl;

    if(fabs(cr) < 0.000001) {		
		c->r = c->x = c->y = 0;
		return(0);	
	}	
	d = 0.5 / cr;	
	rkl = xkl*xkl + ykl*ykl;	
	rkm = xkm*xkm + ykm*ykm;	
	xcc = d*(rkl*ykm - rkm*ykl);	
	ycc = d*(rkm*xkl - rkl*xkm);	
	c->x = xcc + K->x;	
	c->y = ycc + K->y;	
	c->r = sqrt(xcc*xcc + ycc*ycc);		
	return(1);
}


double CCurvature::GetCurvature(double *pdaData[2], int nSize)
{
    double dCurv = 0;
    int nDelta = 3;//1;
    SCircle Circle;
    SPoint pt1, pt2, pt3;

    for(int i = 0; i < nSize; i += nDelta)
    {
        if(i + 2*nDelta < nSize)
        {
            pt1.x = pdaData[0][i];
            pt1.y = pdaData[1][i];
            pt2.x = pdaData[0][i+nDelta];
            pt2.y = pdaData[1][i+nDelta];
            pt3.x = pdaData[0][i+2*nDelta];
            pt3.y = pdaData[1][i+2*nDelta];
            CircleThreePoints(&Circle, &pt1, &pt2, &pt3);

            if(Circle.r > 0 && Circle.r < 4294967295.0)
            {
                dCurv += 1/Circle.r;
            }
        }
    }
    
    return dCurv;
}

#define nMIN_CIRCLE_RADIUS	10

int CCurvature::GetCircleCount(double *pdaData[2], int nSize)
{
    int nCount = 0;
    int nDelta = 3;//1;
    SCircle Circle;
    SPoint pt1, pt2, pt3;

    for(int i = 0; i < nSize; i += nDelta)
    {
        if(i + 2*nDelta < nSize)
        {
            pt1.x = pdaData[0][i];
            pt1.y = pdaData[1][i];
            pt2.x = pdaData[0][i+nDelta];
            pt2.y = pdaData[1][i+nDelta];
            pt3.x = pdaData[0][i+2*nDelta];
            pt3.y = pdaData[1][i+2*nDelta];
            CircleThreePoints(&Circle, &pt1, &pt2, &pt3);

            //if(Circle.r > 0 && Circle.r < 4294967295)
            //{
            //    dCurv += 1/Circle.r;
            //}

			if(Circle.r > 0 && Circle.r < nMIN_CIRCLE_RADIUS)
				nCount++;
        }
    }
    
    return nCount;
}

void CCurvature::GetCurvatureInfo(SCurvData *pCurvData, double *pdaData[3], int nDataSize)
{
    double *pdaDst[2];
    double *pdaXY[2]; 
    double *pdaYZ[2];
    double *pdaZX[2];
    double dCurv;
    double dCurvXY, dCurvYZ, dCurvZX;
    int nProcessedData;

    pCurvData->m_dAverage = 0;
    pCurvData->m_dMax = 0;
    pCurvData->m_dStd = 0;
    pCurvData->m_dTotal = 0;
    pCurvData->m_dNorm = 0;
    dCurvXY = dCurvYZ = dCurvZX = 0;
    int nStep = nMAX_SCALE_DATA;

    int i;
    for(i = 0; i < nDataSize; i+=nStep)
    {
        if(i+nStep > nDataSize)
            break;

        // alloc mem
        pdaXY[0] = new double [nStep];
        pdaXY[1] = new double [nStep];
        pdaYZ[0] = new double [nStep];
        pdaYZ[1] = new double [nStep];
        pdaZX[0] = new double [nStep];
        pdaZX[1] = new double [nStep];

        // copy data
        for(int j = 0; j < nStep; j++)
        {
            pdaXY[0][j] = pdaData[0][j+i];//[0];
            pdaXY[1][j] = pdaData[1][j+i];//[1];
        }
        for(int j = 0; j < nStep; j++)
        {
            pdaYZ[0][j] = pdaData[1][j+i];//[1];
            pdaYZ[1][j] = pdaData[2][j+i];//[2];
        }
        for(int j = 0; j < nStep; j++)
        {
            pdaZX[0][j] = pdaData[2][j+i];//[2];
            pdaZX[1][j] = pdaData[0][j+i];//[0];
        }

        pdaDst[0] = new double[nStep];
        pdaDst[1] = new double[nStep];

        // scale it and get curvature from it
        ScaleData(pdaXY, pdaDst, nStep);
        dCurv = GetCurvature(pdaDst, nStep);
        dCurvXY += dCurv;

        ScaleData(pdaYZ, pdaDst, nStep);
        dCurv = GetCurvature(pdaDst, nStep);
        dCurvYZ += dCurv;

        ScaleData(pdaZX, pdaDst, nStep);
        dCurv = GetCurvature(pdaDst, nStep);
        dCurvZX += dCurv;

        // free mem
        delete [] pdaDst[1];
        delete [] pdaDst[0];

        delete [] pdaZX[1];    
        delete [] pdaZX[0];
        delete [] pdaYZ[1]; 
        delete [] pdaYZ[0]; 
        delete [] pdaXY[1];
        delete [] pdaXY[0];
    }
    nProcessedData = i;

	double daCurv[3];

    pCurvData->m_dCurvXY = daCurv[0] = dCurvXY;
    pCurvData->m_dCurvYZ = daCurv[1] = dCurvYZ;
    pCurvData->m_dCurvZX = daCurv[2] = dCurvZX;

	int iMax = Max(daCurv);
	int iMin = Min(daCurv);

	pCurvData->m_dMax = daCurv[iMax];
	pCurvData->m_dMin = daCurv[iMin];

	int iaTest[3] = { 0, 0, 0 };
	iaTest[iMax] = 1, iaTest[iMin] = 1;
	int iMid;
	for(int i = 0; i < 3; i++)
	{
		if(iaTest[i] == 0)
			iMid = i;
	}

	if(daCurv[iMax]-daCurv[iMid] > daCurv[iMid]-daCurv[iMin])
		daCurv[iMax] = 0;
	else
		daCurv[iMin] = 0;
	
	pCurvData->m_dTotal = daCurv[0] + daCurv[1] + daCurv[2];
	//dCurvXY + dCurvYZ + dCurvZX;

	//pCurvData->m_dAverage = pCurvData->m_dTotal/3;
	// one of daCurv is 0, so I use 2 instead of 3 as a denominator
	pCurvData->m_dAverage = (daCurv[0] + daCurv[1] + daCurv[2])/2;
    pCurvData->m_dNorm = pCurvData->m_dAverage/nProcessedData;
}

void CCurvature::GetCircleCountInfo(SCircleData *pCircleData, double *pdaData[3], int nDataSize)
{
//    double *pdaDst[2];
    double *pdaXY[2]; 
    double *pdaYZ[2];
    double *pdaZX[2];
    int nCount;
    int nCountXY, nCountYZ, nCountZX;
    int nProcessedData;

    nCountXY = nCountYZ = nCountZX = 0;
    int nStep = nMAX_SCALE_DATA;

    int i;
    for(i = 0; i < nDataSize; i+=nStep)
    {
        if(i+nStep > nDataSize)
            break;

        // alloc mem
        pdaXY[0] = new double [nStep];
        pdaXY[1] = new double [nStep];
        pdaYZ[0] = new double [nStep];
        pdaYZ[1] = new double [nStep];
        pdaZX[0] = new double [nStep];
        pdaZX[1] = new double [nStep];

        // copy data
        for(int j = 0; j < nStep; j++)
        {
            pdaXY[0][j] = pdaData[0][j+i];//[0];
            pdaXY[1][j] = pdaData[1][j+i];//[1];
        }
        for(int j = 0; j < nStep; j++)
        {
            pdaYZ[0][j] = pdaData[1][j+i];//[1];
            pdaYZ[1][j] = pdaData[2][j+i];//[2];
        }
        for(int j = 0; j < nStep; j++)
        {
            pdaZX[0][j] = pdaData[2][j+i];//[2];
            pdaZX[1][j] = pdaData[0][j+i];//[0];
        }

        //pdaDst[0] = new double[nStep];
        //pdaDst[1] = new double[nStep];

        // scale it and get curvature from it
        nCount = GetCircleCount(pdaXY, nStep);
        nCountXY += nCount;

        nCount = GetCircleCount(pdaYZ, nStep);
        nCountYZ += nCount;

        nCount = GetCircleCount(pdaZX, nStep);
        nCountZX += nCount;

        // free mem
        //delete [] pdaDst[1];
        //delete [] pdaDst[0];

        delete [] pdaZX[1];    
        delete [] pdaZX[0];
        delete [] pdaYZ[1]; 
        delete [] pdaYZ[0]; 
        delete [] pdaXY[1];
        delete [] pdaXY[0];
    }
    nProcessedData = i;

	int naCount[3];

    pCircleData->m_nCountXY = naCount[0] = nCountXY;
    pCircleData->m_nCountYZ = naCount[1] = nCountYZ;
    pCircleData->m_nCountZX = naCount[2] = nCountZX;

	int iMax = Max(naCount);
	int iMin = Min(naCount);

	int iaTest[3] = { 0, 0, 0 };
	iaTest[iMax] = 1, iaTest[iMin] = 1;
	int iMid;
	for(int i = 0; i < 3; i++)
	{
		if(iaTest[i] == 0)
			iMid = i;
	}

	if(naCount[iMax]-naCount[iMid] > naCount[iMid]-naCount[iMin])
		naCount[iMax] = 0;
	else
		naCount[iMin] = 0;
	
	pCircleData->m_nTotal = naCount[0] + naCount[1] + naCount[2];
	pCircleData->m_nAverage = pCircleData->m_nTotal/2; // not 3 because one of naCounts is 0
	//dCurvXY + dCurvYZ + dCurvZX;
}

int CCurvature::Max(double dXYZ[])
{
	double dMax = dXYZ[0];
	int i = 0;

	if(dXYZ[1] > dMax)
		dMax = dXYZ[1], i = 1;
	if(dXYZ[2] > dMax)
		dMax = dXYZ[2], i = 2;
	return i;
}

int CCurvature::Max(int nXYZ[])
{
	int nMax = nXYZ[0];
	int i = 0;

	if(nXYZ[1] > nMax)
		nMax = nXYZ[1], i = 1;
	if(nXYZ[2] > nMax)
		nMax = nXYZ[2], i = 2;
	return i;
}

int CCurvature::Min(double dXYZ[])
{
	double dMin = dXYZ[0];
	int i = 0;

	if(dXYZ[1] < dMin)
		dMin = dXYZ[1], i = 1;
	if(dXYZ[2] < dMin)
		dMin = dXYZ[2], i = 2;
	return i;
}

int CCurvature::Min(int nXYZ[])
{
	int nMin = nXYZ[0];
	int i = 0;

	if(nXYZ[1] < nMin)
		nMin = nXYZ[1], i = 1;
	if(nXYZ[2] < nMin)
		nMin = nXYZ[2], i = 2;
	return i;
}

void CCurvature::ScaleData(double *pdaSrcData[], double *pdaDstData[2], int nSize)
{
    double daX[nMAX_SCALE_DATA], daY[nMAX_SCALE_DATA];

    for(int i = 0; i < nSize; i++)
    {
        daX[i] = pdaSrcData[0][i];
        daY[i] = pdaSrcData[1][i];
    }

    double dMinX = GetMin(daX, nSize);
    double dMinY = GetMin(daY, nSize);

    for(int i = 0; i < nSize; i++)
    {
        daX[i] -= dMinX;
        daY[i] -= dMinY;
    }

    double dMaxX = GetMax(daX, nSize);
    double dMaxY = GetMax(daY, nSize);
    double dScaleFactor;

    if(dMaxX > dMaxY)
        dScaleFactor = dMaxX;
    else
        dScaleFactor = dMaxY;

    double dScaleSize = 100.0;
    if(dScaleFactor > nMIN_SCALE_FACTOR)
    {
        for(int i = 0; i < nSize; i++)
        {
            daX[i] *= dScaleSize/dScaleFactor;
            daY[i] *= dScaleSize/dScaleFactor;

            pdaDstData[0][i] = daX[i];
            pdaDstData[1][i] = daY[i];
        }
    }
    else
    {
        for(int i = 0; i < nSize; i++)
        {
            pdaDstData[0][i] = daX[i];
            pdaDstData[1][i] = daY[i];
        }
    }
}

double CCurvature::GetMin(double *pdaData, int nSize)
{
    double dMin;

    dMin = pdaData[0];
    for(int i = 0; i < nSize; i++)
    {
        if(pdaData[i] < dMin)
            dMin = pdaData[i];
    }
    return dMin;
}

double CCurvature::GetMax(double *pdaData, int nSize)
{
    double dMax;

    dMax = pdaData[0];
    for(int i = 0; i < nSize; i++)
    {
        if(pdaData[i] > dMax)
            dMax = pdaData[i];
    }
    return dMax;
}
