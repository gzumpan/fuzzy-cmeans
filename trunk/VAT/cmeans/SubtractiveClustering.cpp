#include "stdafx.h"
#include "SubtractiveClustering.h"
#include "Logger.h"

namespace Clustering
{
	void CPotential::operator=(_In_ const CPotential& srcPoten) throw()
	{
		this->SetIndex(srcPoten.m_nIndex);
		this->SetPotential(srcPoten.m_dPotential);
	}


	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	CSubtractiveClustering::CSubtractiveClustering(Matrix* pdataset, double radius)
	{
		m_pLogger = CLogger::Instance();

		m_pmatDataset  = pdataset;
		m_pmatDistance = NULL;

		m_radii			= radius;
		m_squash		= 1.25;
		m_acceptRatio	= 0.5;
		m_rejectRatio	= 0.15;
		m_maxPotential	= 0.0;
		m_pvMax = NULL;
		m_pvMin = NULL;
	}

	CSubtractiveClustering::~CSubtractiveClustering(void)
	{
		ResetData();
	}

	void CSubtractiveClustering::ResetData()
	{
		if (m_pvMax) 
		{ 
			delete m_pvMax; 
			m_pvMax = NULL; 
		}
		
		if (m_pvMin) 
		{ 
			delete m_pvMin; 
			m_pvMin = NULL; 
		}
		
		if (m_pmatDistance) 
		{ 
			delete m_pmatDistance; 
			m_pmatDistance = NULL; 
		}

		// Destroy potential array.
		CPotential* pt;
		for (int i = 0; i < m_arrPotential.GetSize(); i++)
		{
			pt = m_arrPotential.GetAt(i);
			delete pt;
			pt = NULL;
		}
		m_arrPotential.RemoveAll();
	}

	void CSubtractiveClustering::PrintCenter()
	{
		int nIndex;

		for (int i = 0; i < m_arrCenter.GetSize(); ++i)
		{
			nIndex = m_arrCenter.GetAt(i);
			for (int f = 0; f < m_pmatDataset->size2(); ++f)
			{
				m_pLogger->LogNoPrefix("%f\t", (*m_pmatDataset)(nIndex, f));
			}

			m_pLogger->LogNoPrefix("\n");
		}

		m_pLogger->LogNoPrefix("==========================================\n");
	}

	// Run
	void CSubtractiveClustering::Processing()
	{
		double ra = m_radii;
		double rb = m_squash*ra;
		int nRet;
		int nIdxCenter;
		int nIdxMaxPoten;
		BOOL bStop = FALSE;

		// Allocate memory.
		AllocateMemory(m_pmatDataset->size1(), m_pmatDataset->size2());

		// Normalize data
		NormalizedData(*m_pmatDataset);
		//memcpy(m_pmatNorData, m_pmatDataset, sizeof(Matrix));

		// Step 1: Compute the potential of each data point; 
		// set the number of cluster centers as k = 1.
		CalculatePotential(*m_pmatNorData, ra);

		// Step 2:
		// Select the point with the highest potential denoted 
		while (!bStop)
		{
			SortPotential();

			// loop until SetCenter is ok.
			nIdxMaxPoten = m_arrPotential.GetSize() - 1;
			while (nIdxMaxPoten >= 0 && !bStop)
			{
				nRet = SetCenter(nIdxMaxPoten);
				// nRet :
				// ==  0 --> break.
				// ==  1 --> continue.
				// == -1 --> stop.
				if (nRet == 0)
				{
					ReCalculatePotential(*m_pmatNorData, nIdxMaxPoten, rb);
					break;
				}
				else
				{
					if (nRet == 1) --nIdxMaxPoten;
					if (nRet == -1 || nIdxMaxPoten < 0) bStop = TRUE;
				}
			}
		}

		PrintCenter();
	}

	void CSubtractiveClustering::AllocateMemory(int nDataSize, int nDim)
	{
		ResetData();
		// Each coordinate has two vector max and min.
		m_pvMax			= new Vector(nDim);
		m_pvMin			= new Vector(nDim);
		m_pmatNorData	= new Matrix(nDataSize, nDim);
		m_pmatDistance	= new Matrix(nDataSize, nDataSize);
		m_pmatDistance->clear();
	}

	void  CSubtractiveClustering::SortPotential()
	{
		qsort(m_arrPotential.GetData(), m_arrPotential.GetSize(), sizeof(CPotential *), ComparePotential);
	}

	void CSubtractiveClustering::NormalizedData(Matrix &dataset)
	{
		int nNumberPoints;
		int nSizeOfPoint;
		double dmax, dmin;

		nNumberPoints = dataset.size1();
		nSizeOfPoint  = dataset.size2();

		// Init values for matrix.
		for (int f = 0; f < nSizeOfPoint; f++)
		{
			(*m_pvMax)(f) = dataset(0, 0);
			(*m_pvMin)(f) = dataset(0, 0);
		}
		
		// Find the max and min vector each coordinate.
		for (int f = 0; f < nSizeOfPoint; f++)
		{
			for (int i = 1; i < nNumberPoints; i++)
			{
				// Find the max
				if ( (*m_pvMax)(f) < dataset(i,f) )
				{
					(*m_pvMax)(f) = dataset(i,f);
				}

				// Find the min
				if ( (*m_pvMin)(f) > dataset(i,f) )
				{
					(*m_pvMin)(f) = dataset(i,f);
				}
			}
		}

		// Normalized data.
		for (int i = 0; i < nNumberPoints; i++)
		{
			for (int f = 0; f < nSizeOfPoint; f++)
			{
				(*m_pmatNorData)(i,f) = (dataset(i,f) - (*m_pvMin)(f)) / 
										((*m_pvMax)(f) - (*m_pvMin)(f));
			}
		}
	}

	BOOL CSubtractiveClustering::IsNewCenter(int nCandiIndex)
	{
		for (int i = 0; i < m_arrCenter.GetSize(); i++)
		{
			if (nCandiIndex == m_arrCenter.GetAt(i))
				return FALSE;
		}

		return TRUE;
	}

	double CSubtractiveClustering::GetMinDistanceToClusterCenters(int nCandiIndex)
	{
		double minDistance = 0.0;
		double tmp;

		if (!m_arrCenter.IsEmpty())
		{
			minDistance = (*m_pmatDistance)(nCandiIndex, m_arrCenter.GetAt(0));

			for (int i = 1; i < m_arrCenter.GetSize(); ++i)
			{
				tmp = (*m_pmatDistance)(nCandiIndex, m_arrCenter[i]);
				if (minDistance > tmp)
				{
					minDistance = tmp;
				}
			}
		}

		return minDistance;
	}

	void CSubtractiveClustering::CalculatePotential(Matrix &dataset, double ra)
	{
		double poten, distance, tmp;
		double alpha = 4/(ra*ra);
		int nDataSize = dataset.size1();
		int nDim = dataset.size2();

		// Compute distance matrix.
		for (int i = 0; i < nDataSize; ++i)
		{
			for (int j = i + 1; j < nDataSize; ++j)
			{
				distance = 0.0;

				// For dimension.
				for (int f = 0; f < nDim; ++f)
				{
					tmp		  = dataset(i, f) - dataset(j, f);
					distance += tmp*tmp;
				}

				// Hold the distance.
				(*m_pmatDistance)(i, j) = (*m_pmatDistance)(j, i) = distance;
			}
		}

		// Compute the potential of each data point
		for (int i = 0; i < dataset.size1(); ++i)
		{
			poten = 0.0;
			for (int j = 0; j < dataset.size1(); j++)
			{
				if (i == j) { continue; }

				poten += exp( -alpha * (*m_pmatDistance)(i, j)); 
			}

			CPotential* p = new CPotential(i, poten);
			m_arrPotential.Add(p);
		}
	}

	void CSubtractiveClustering::ReCalculatePotential(Matrix &dataset, int indexPotenCenter, double rb)
	{
		double distance;
		double poten;
		double beta = 4/(rb*rb);
		int indexCenter;
		double potenCenter;
		int indexIns;

		indexCenter = m_arrPotential.GetAt(indexPotenCenter)->GetIndex();
		potenCenter = m_arrPotential.GetAt(indexPotenCenter)->GetPotential();

		for (int i = 0; i < m_arrPotential.GetSize(); ++i)
		{
			// || x_i - c_k || --> euclindean.
			indexIns = m_arrPotential.GetAt(i)->GetIndex();
			distance = (*m_pmatDistance)(indexIns, indexCenter);

			poten = exp(-beta * distance);
			poten = m_arrPotential.GetAt(i)->GetPotential() - potenCenter * poten;

			// update new value.
			m_arrPotential.GetAt(i)->SetPotential(poten);
		}
	}

	// Return value : 
	//  0 : found center.
	//  1 : not found yet. need to continue.
	// -1 : it doesn't have anymore.
	int CSubtractiveClustering::SetCenter(int nIdxMaxPoten)
	{
		double minDistance;
		double potentialCenter;
		int nIndexVal;
		int nRet = -1;

		potentialCenter = m_arrPotential.GetAt(nIdxMaxPoten)->GetPotential();
		nIndexVal		= m_arrPotential.GetAt(nIdxMaxPoten)->GetIndex();

		// This is the first center.
		if (m_arrCenter.IsEmpty())
		{
			m_arrCenter.Add(nIndexVal);
			m_maxPotential = potentialCenter;
			nRet = 0;
		}
		else
		{
			if (IsNewCenter(nIndexVal))
			{
				// This point more than the threshold
				if (potentialCenter > m_acceptRatio*m_maxPotential)
				{
					m_arrCenter.Add(nIndexVal);
					nRet = 0; // Ok, go to next center.
				}
				else if (potentialCenter < m_rejectRatio*m_maxPotential) // This stop condition.
				{
					nRet = -1; // stop.
				}
				else // this between accept ratio and reject ratio.
				{
					// Compute min distance to all cluster centers.
					minDistance = GetMinDistanceToClusterCenters(nIndexVal);
					if (SUBCLUS_PROPORTION(minDistance, m_radii, potentialCenter, m_maxPotential))
					{
						m_arrPotential.GetAt(nIdxMaxPoten)->SetPotential(0.0);
						nRet = 1; // this point has not been regular yet! Continue with new another potential.
					}
					else
					{
						m_arrCenter.Add(nIndexVal);
						nRet = 0; // Ok, go to next center.
					}
				}
			}
			else
			{
				nRet = 1; // this point has not been regular yet! Continue with new another potential.
			}
		}

		return nRet;
	}
};