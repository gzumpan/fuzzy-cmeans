
#pragma once
#pragma warning(disable: 4996)

#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>

class CLogger;

namespace Clustering
{
	typedef boost::numeric::ublas::vector<double> Vector;
	typedef boost::numeric::ublas::zero_vector<double> ZeroVector;

	typedef boost::numeric::ublas::matrix<double> Matrix;
	typedef boost::numeric::ublas::zero_matrix<double> ZeroMatrix;

	class CFuzzy
	{
		//
		// Notation 
		// 
		//   i  -> is used for point 
		//   j  -> is used for cluster
		//   f  -> is used for dimension (feature)

	public:
		CFuzzy(Matrix* pdataset, int nc, double dm = 1.1, double deps = 0.01);
		~CFuzzy();

	public:
		void	FCMClustering(unsigned int nIter = 100);

	private:
		void	RamdomMembership();

		void	ComputeCentroids();			// Compute centroid with m = 1
		void	ComputeCentroids2();		// Compute centroid with m = k;

		bool	UpdateMembership();
		inline	bool CanStop();

	private:
		int				m_nc;			// number cluster
		double			m_dm;			// fuzziness
		double			m_deps;			// epsilon

		int				m_nNumberPoints;	// number points in dataset
		int				m_nSizeOfPoint;		// size of a single point.

		Matrix*			m_pDataset;			// dataset.
		Matrix*			m_pCentroids;		// centroid.
		Matrix*			m_pMembership;		// membership.
		Matrix*			m_pNewMembership;	// new membership.

		CLogger*		m_pLogger;
	};
};