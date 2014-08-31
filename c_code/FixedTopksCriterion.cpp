#include "FixedTopksCriterion.h"

void FixedTopksCriterion::Initialize(const SMatrix<int>* pGND,
		int nNumberDataBasePoints, 
		const Vector<int>* pvecTrueNeighbors,
		SMatrix<double> *pmat_result)
{
	m_pmatGND = pGND;

	m_pmatResult = pmat_result;
	m_pvecNumberTrueNeighbors = pvecTrueNeighbors;
	m_nNumberDataBasePoints = nNumberDataBasePoints;

	if (m_pvecNumberTrueNeighbors->Ptr())
	{
		m_pmatResult->SetValueZeros();
		m_nNumberCandidate = m_pmatResult->Cols();
	}

	EXIT_ASSERT(m_nNumberCandidate > 0);

	int largest = m_pmatGND->Cols();
	for (int i = 0; i < m_pvecNumberTrueNeighbors->Size(); i++)
	{
		int required = m_pvecNumberTrueNeighbors->operator[](i);
		EXIT_ASSERT(required <= largest);
	}

}

void FixedTopksCriterion::EvaluateFixedTopKEach(
	const int* p_idx, const int* p_gnd, 
	int idx_point, Vector<SMatrix<bool> > &vec_mat_indicator) const
{
	int num_topks = m_pvecNumberTrueNeighbors->Size();

	Vector<bool> vec_indicator(m_nNumberDataBasePoints);

	for (int i = 0; i < num_topks; i++)
	{
		int topk = m_pvecNumberTrueNeighbors->operator[](i);

		vec_indicator.SetValueZeros();

		for (int j = 0; j < topk; j++)
		{
			int idx = p_gnd[j];
			EXIT_ASSERT(idx >= 0 && idx < m_nNumberDataBasePoints);
			vec_indicator[idx] = 1;
		}

		for (int j = 0; j < m_nNumberCandidate; j++)
		{
			int idx = p_idx[j];
			if (vec_indicator[idx])
			{
				vec_mat_indicator[i][idx_point][j]++;
			}
		}
	}
}

void FixedTopksCriterion::Evaluate(
	const SMatrix<int> &matRetrievedIndex, int idx_begin, int idx_end)
{
	int num_query = idx_end - idx_begin;

	EXIT_ASSERT(m_nNumberCandidate == matRetrievedIndex.Cols());
	EXIT_ASSERT(num_query <= matRetrievedIndex.Rows());

	Vector<bool> vec_indicator(m_nNumberDataBasePoints);

	int num_topks = m_pvecNumberTrueNeighbors->Size();

	//return;
	Vector<SMatrix<bool> > vec_mat_indicator;
	vec_mat_indicator.AllocateSpace(num_topks);

	for (int i = 0; i < num_topks; i++)
	{
		vec_mat_indicator[i].AllocateSpace(num_query, m_nNumberCandidate);
		vec_mat_indicator[i].SetValueZeros();
	}
	//return;
#pragma omp parallel for
	for (int i = 0; i < num_query; i++)
	{
		const int* p_idx = matRetrievedIndex[i];
		EXIT_ASSERT(i + idx_begin < m_pmatGND->Rows());

		const int* p_gnd = m_pmatGND->operator[](i + idx_begin);
		
		EvaluateFixedTopKEach(p_idx, p_gnd, i, vec_mat_indicator);
	}
	//return;
	for (int i = 0; i < num_topks; i++)
	{
		for (int j = 0; j < num_query; j++)
		{
			for (int k = 0; k < m_nNumberCandidate; k++)
			{
				if (vec_mat_indicator[i][j][k])
				{
					m_pmatResult->operator[](i)[k]++;
				}
			}
		}
	}
	//return;
	//SMART_ASSERT(0).Exit();
}

