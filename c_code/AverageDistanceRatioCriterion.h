#pragma once
#include <math.h>
#include "math_basic.h"
#include "Vector.h"
#include "SMatrix.h"
#include "Criterion.h"


template <class T>
class AverageDistanceRatioCriterion : public Criterion
{
public:
	void Initialize(const SMatrix<int>* pGND,
		const SMatrix<double>* pmat_query, 
		const SMatrix<T>* pmat_database, 
		Vector<double> *pvec_dist_ratio)
	{
		m_pmatGND = pGND;
		m_pmatQuery = pmat_query;
		m_pmatDatabase = pmat_database;
		m_pvecDistanceRatio = pvec_dist_ratio;

		m_nNumberDataBasePoints = m_pmatDatabase->Rows();
		m_nNumberValid = m_pvecDistanceRatio->Size();

		EXIT_ASSERT(m_nNumberValid > 0 &&
			m_nNumberValid <= m_pmatGND->Cols());

		m_pvecDistanceRatio->SetValueZeros();

		EXIT_ASSERT(m_nNumberDataBasePoints > 0);
		EXIT_ASSERT(m_pmatQuery->Cols() == m_pmatDatabase->Cols());
	}


public:
	virtual void Evaluate(const SMatrix<int> &matRetrievedIndex, int idx_begin, int idx_end)
	{
		SMatrix<double> mat_ratio;

		EvaluateAverageRatioAllQuery(matRetrievedIndex, idx_begin, idx_end, mat_ratio);

		EvaluateAverageRatioAllAddup(mat_ratio);
	}

private:
	void EvaluateAverageRatioAllQuery(const SMatrix<int> &matRetrievedIndex, 
		int idx_begin, int idx_end, SMatrix<double> &mat_ratio) const
	{

		int num_query = idx_end - idx_begin;

		//PRINT << num_query << "\n";

		mat_ratio.AllocateSpace(num_query, m_nNumberValid);

		int dim = m_pmatQuery->Cols();

		long long total = num_query * m_nNumberValid;

#pragma omp parallel for
		for (long long k = 0; k < total; k++)
		{
			int i = (k / m_nNumberValid);
			int j = (k % m_nNumberValid);

			const int* p_idx = matRetrievedIndex[i];
			const int* p_gnd = m_pmatGND->operator[](i + idx_begin);
			const double* p_query = m_pmatQuery->operator[](i + idx_begin);

			int idx_true_neighbor = p_gnd[j];
			int idx_app_neighbor = p_idx[j];

			const T* p_true_neighbor = m_pmatDatabase->operator[](idx_true_neighbor);
			const T* p_app_neighbor = m_pmatDatabase->operator[](idx_app_neighbor);

			double true_dist = squared_distance_db(p_query, 
				p_true_neighbor, dim);
			double app_dist = squared_distance_db(p_query,
				p_app_neighbor, dim);

			double ratio;
			if (true_dist < 0.00001)
			{
				ratio = 1;
			}
			else
			{
				ratio = app_dist / true_dist;
			}
			mat_ratio[i][j] = sqrt(ratio);
		}
	}

	void EvaluateAverageRatioAllAddup(const SMatrix<double> &mat_ratio)
	{
		int num_query = mat_ratio.Rows();

		for (int i = 0; i < num_query; i++)
		{
			for (int j = 0; j < m_nNumberValid; j++)
			{
				m_pvecDistanceRatio->operator[](j) += mat_ratio[i][j];
			}
		}
	}

private:
	const SMatrix<int> *m_pmatGND;
	const SMatrix<double>* m_pmatQuery;
	const SMatrix<T>* m_pmatDatabase;

	Vector<double>* m_pvecDistanceRatio;

	int m_nNumberValid;
	int m_nNumberDataBasePoints;
};
