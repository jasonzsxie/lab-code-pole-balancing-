// ==========================================================================
//
// Curvature.h
//
// Writer: jrkwon
//
//
// 
//
// Hisotry:
//      12/19/05 Start!
//
// ==========================================================================

#ifndef CURVATURE_H
#define CURVATURE_H

#include <vector>
#include "Types.h"
#include "Params.h"

//#define nMAX_SCALE_DATA     50
//#define nMIN_SCALE_FACTOR   0.0001

struct SCurvData
{
    double m_dCurvXY;
    double m_dCurvYZ;
    double m_dCurvZX;
    double m_dMax;
	double m_dMin;
    double m_dAverage;
    double m_dTotal;
    double m_dStd;
    double m_dNorm;  // = m_dAverage/# of data
};

struct SCircleData
{
	int m_nCountXY;
	int m_nCountYZ;
	int m_nCountZX;
	int m_nTotal;
	int m_nAverage;
};

struct SPoint
{
    double x, y;
};

struct SCircle {
    double x, y, r;
};

class CCurvature
{
public:
    CCurvature();
    ~CCurvature();

    void GetCurvatureInfo(SCurvData *pCurvData, double *pdaData[3], int nSize);
    void GetCircleCountInfo(SCircleData *pCurvData, double *pdaData[3], int nSize);

private:
    void ScaleData(double *pdaSrcData[2], double *pdaDstData[2], int nSize);
    double GetMin(double *pdaData, int nSize);
    double GetMax(double *pdaData, int nSize);
    double GetCurvature(double *pdaData[2], int nSize);
	int GetCircleCount(double *pdaData[2], int nSize);

    int CircleThreePoints(SCircle *c, SPoint *K, SPoint *L, SPoint *M);
	int Max(double dXYZ[]);
	int Min(double dXYZ[]);
	int Max(int nXYZ[]);
	int Min(int nXYZ[]);
};

#endif // CURVATURE_H

