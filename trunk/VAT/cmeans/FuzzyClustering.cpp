#include "stdafx.h"
#include "FuzzyClustering.h"
#include "Logger.h"
#include "FCMUtil.h"

namespace Clustering
{
	CFuzzy::CFuzzy(Matrix* pdataset, int nc, double dm, double deps)
	{
		m_pLogger = CLogger::Instance();

		m_nc	= nc;
		m_dm	= dm;
		m_deps	= deps;
		m_disFunc = DistanceFunction::EUCLIDEAN_DIS;

		m_pDataset = pdataset;

		// number of point.
		m_nNumberPoints = m_pDataset->size1();

		// size of dimension.
		m_nSizeOfPoint = m_pDataset->size2();

		// we have number clusters centroids
		m_pCentroids = new Matrix(m_nc, m_nSizeOfPoint);
		
		// each row is member of a cluster to some extent
		m_pMembership = new Matrix(m_nc, m_nNumberPoints);

		// each row is member of a cluster to some extent
		m_pNewMembership = new Matrix(m_nc, m_nNumberPoints);

		RamdomMembership();
		ComputeCentroids();
	}

	CFuzzy::~CFuzzy()
	{
		if (m_pDataset)			{ delete m_pDataset; }
		if (m_pCentroids)		{ delete m_pCentroids; }
		if (m_pMembership)		{ delete m_pMembership; }
		if (m_pNewMembership)	{ delete m_pNewMembership;}
	}

	void CFuzzy::RamdomMembership()
	{
		m_pLogger->LogNoPrefix("\nMatrix U[0](Random)\n===============\n");
		for (int j = 0; j < m_nc; j++)
		{
			m_pLogger->LogNoPrefix("#Cluster[%d]\t", j);
		}

		double fNormalizationFactor;

		for (int i = 0; i < m_nNumberPoints; i++)
		{
			m_pLogger->LogNoPrefix("\n");
			fNormalizationFactor = 0.0;

			for (int j = 0; j < m_nc; j++)
			{
				fNormalizationFactor += (*m_pMembership)(j,i) = (double)(rand()/(RAND_MAX + 0.0));
			}

			for (int j = 0; j < m_nc; j++)
			{
				(*m_pMembership)(j,i) /= fNormalizationFactor;
				m_pLogger->LogNoPrefix("%f\t", (*m_pMembership)(j, i));
			}
		}
	}

	void CFuzzy::ComputeCentroids()
	{
		// Compute the centroid on random assignment above
		// the entroid of a cluster is the mean of all the points,
		// weighted by their degree of belonging to the cluster
		
		// Multiplication between 2 matrix.
		CString szAbt, szTmp;

		*m_pCentroids = prod (*m_pMembership, *m_pDataset);

		Vector sum_uk = ZeroVector(m_nc);
		
		for (int j = 0; j < m_nc; j++)
		{
			for (int i = 0 ; i < m_nNumberPoints; i++)
			{
				sum_uk[j] += (*m_pMembership)(j, i);
			}
		}

		for (int j = 0; j < m_nc; j++)
		{
			szAbt.Empty();
			for (int f = 0 ; f < m_nSizeOfPoint; f++)
			{
				(*m_pCentroids)(j, f) /= sum_uk[j];

				// For log.
				szTmp.Format(_T("%f"), (*m_pCentroids)(j, f));
				szAbt += szTmp;
				if (f != (m_nSizeOfPoint - 1)) { szAbt += _T(", "); }
			}

			m_pLogger->LogNoPrefix("\nCentroid C%d = (%s)", j, FCMUtil::ToString(szAbt).c_str());
		}
	}

	void CFuzzy::ComputeCentroids2()
	{
		// Compute the centroid on random assignment above
		// the centroid of a cluster is the mean of all the points,
		// weighted by their degree of belonging to the cluster
		
		// compute sum_i=1...N  u_ij^m 
		double nNormalization;
		Matrix u_ji_m(m_nc, m_nNumberPoints);		

		// for each cluster
		for (int j = 0; j < m_nc; j++)
		{
			// for each point
			for (int i = 0 ; i < m_nNumberPoints; i++)
			{
				// compute u_ij_m
				u_ji_m(j, i) = pow ( (*m_pMembership)(j, i), m_dm);
			}
		}

		//////////////////////////////////////////////////////////////////////////
		// Compute centroids matrix.

		// Reset data of matrix.
		m_pCentroids->clear();

		for (int j = 0; j < m_nc; j++)
		{
			for (int i = 0 ; i < m_nNumberPoints; i++)	
			{
				for (int f = 0; f < m_nSizeOfPoint; f++)
					(*m_pCentroids)(j, f) += u_ji_m(j, i) * (*m_pDataset)(i, f);
			}
		}

		CString szAbt, szTmp;

		for (int j = 0; j < m_nc; j++)
		{
			szAbt.Empty();
			nNormalization = 0.0;

			for (int i = 0 ; i < m_nNumberPoints; i++) 
			{
				nNormalization += u_ji_m(j,i);
			}

			for (int f = 0; f < m_nSizeOfPoint; f++)
			{
				(*m_pCentroids)(j, f) /= nNormalization;

				szTmp.Format(_T("%f"), (*m_pCentroids)(j, f));
				szAbt += szTmp;
				if (f != (m_nSizeOfPoint - 1)) { szAbt += _T(", "); }
			}

			m_pLogger->LogNoPrefix("\nCentroid C%d = (%s)", j, FCMUtil::ToString(szAbt).c_str());
		}
		//////////////////////////////////////////////////////////////////////////
	}

	bool CFuzzy::UpdateMembership()
	{
		m_pLogger->LogNoPrefix("\n");

		double coeff;
		int i, j, k;

		for (j = 0; j < m_nc; j++)
		{
			m_pLogger->LogNoPrefix("#Cluster[%d]\t", j);
		}

		Matrix matrix_norm_one_xi_minus_cj = ZeroMatrix(m_nNumberPoints, m_nc);

		if (m_disFunc == DistanceFunction::MANHATTAN_DIS)
		{
			ManhattanDistance(matrix_norm_one_xi_minus_cj);
		}
		else
		{
			EuclideanDistance(matrix_norm_one_xi_minus_cj);
		}

		// for each point
		for (i = 0 ; i < m_nNumberPoints; i++)
		{
			m_pLogger->LogNoPrefix("\n");

			// for each cluster
			for (j = 0; j < m_nc; j++)
			{
				// for each cluster
				coeff = 0.0;
				for (k = 0; k < m_nc; k++)
				{

					coeff += pow ( (matrix_norm_one_xi_minus_cj(i, j) /
									matrix_norm_one_xi_minus_cj(i, k)) , 1 / (m_dm - 1) );
				}

				(*m_pNewMembership)(j, i) = 1 / coeff;
				m_pLogger->LogNoPrefix("%f\t", (*m_pNewMembership)(j, i));
			}
		}

		if (!CanStop())
		{
			(*m_pMembership) = (*m_pNewMembership);  // switch matrices

			return false; 
		}
		
		return true;
	}

	void CFuzzy::ManhattanDistance(Matrix &mat)
	{
		int i, j, f;

		// for each point
		for (i = 0; i < m_nNumberPoints; i++)
		{
			// for each cluster
			for (j = 0; j < m_nc; j++)
			{
				// for each feature
				for (f = 0; f < m_nSizeOfPoint; f++)
				{
					// x_i - c_j
					mat(i, j) += abs((*m_pDataset)(i, f) - (*m_pCentroids)(j, f));
				}
			}
		}

	}

	void CFuzzy::EuclideanDistance(Matrix &mat)
	{
		// for each point
		for (int i = 0; i < m_nNumberPoints; i++)
		{
			// for each cluster
			for (int j = 0; j < m_nc; j++)
			{
				// for each feature
				for (int f = 0; f < m_nSizeOfPoint; f++)
				{
					mat(i, j) += pow((*m_pDataset)(i, f) - (*m_pCentroids)(j, f), 2);
				}
			}
		}

	}

	bool CFuzzy::CanStop()
	{
		double t = norm_1( (*m_pMembership) - (*m_pNewMembership) );
		
		return  (t < m_deps); 
	}

	void CFuzzy::FCMClustering(unsigned int nIter)
	{
		unsigned int nloop = 0;
		
		m_pLogger->LogNoPrefix("\n\nMatrix U[%d]\n===============", nloop + 1);

		while (!UpdateMembership() && (nloop++ < nIter))
		{
			ComputeCentroids2();
			m_pLogger->LogNoPrefix("\n\nMatrix U[%d]\n===============", nloop + 1);
		}

		m_pLogger->LogNoPrefix("\n=====End=====\n\n");
	}
};