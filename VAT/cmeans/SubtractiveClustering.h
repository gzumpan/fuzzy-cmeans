#pragma once

#include "define.h"

class CLogger;

namespace Clustering
{
	class CPotential
	{
	public:
		CPotential(int nIndex, double val)	
		{ 
			m_nIndex = nIndex; 
			m_dPotential	 = val; 
		}
		~CPotential(){}

	public:
		void	operator=(_In_ const CPotential& srcPoten) throw();

	public:
		void	SetIndex(int val)	{ m_nIndex = val; }
		int		GetIndex()			{ return m_nIndex; }

		void	SetPotential(double val)	{ m_dPotential = val; }
		double	GetPotential()				{ return m_dPotential; }

		int		CompareVal(CPotential &val)
		{
			if (this->GetPotential() > val.GetPotential()) { return 1; }
			if (this->GetPotential() < val.GetPotential()) { return -1; }
			return 0;
		}
	private:
		int					m_nIndex;
		double				m_dPotential;
	};

	// This class find the number of clusters

	class CSubtractiveClustering
	{
	public:
		CSubtractiveClustering(Matrix* pdataset, double radius = 0.0);
		~CSubtractiveClustering(void);

	public:
		void	PrintCenter();

	public:
		void	Processing();

	protected:
		void	NormalizedData(Matrix &dataset);
		BOOL	IsNewCenter(int nCandiIndex);
		double	GetMinDistanceToClusterCenters(int nCandiIndex);
		void	CalculatePotential(Matrix &dataset, double ra);
		void	ReCalculatePotential(Matrix &dataset, int indexPotenCente, double rb);
		void	SortPotential();
		int		SetCenter(int maxIndex);
		void	ResetData();
		void	AllocateMemory(int nDataSize, int nDim);

	private:
		CLogger*		m_pLogger;

	private:
		double			m_radii;			// radius
		double			m_squash;
		double			m_acceptRatio;
		double			m_rejectRatio;
		Matrix*			m_pmatDataset;		// dataset.
		Matrix*			m_pmatDistance;

		CArray<CPotential*, CPotential*>	m_arrPotential;
		CUIntArray		m_arrCenter;
		double			m_maxPotential;

		// Normalize data
		Matrix*			m_pmatNorData;		// The dataset is normalized
		Vector*			m_pvMax;			// max boundary of data set.
		Vector*			m_pvMin;			// Min boundary of data set
	};


	inline int ComparePotential(const void* v1, const void* v2)
	{
		LPARAM lparam1, lparam2;
		CPotential *p1, *p2;

		lparam1 = *(LPARAM*)v1;
		lparam2 = *(LPARAM*)v2;

		p1 = (CPotential *) lparam1;
		p2 = (CPotential *) lparam2;

		return p1->CompareVal(*p2);
	}
};

