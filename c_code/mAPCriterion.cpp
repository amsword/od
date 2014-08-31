#include "mAPCriterion.h"

void mAPCriterion::Initialize(const SMatrix<int>* pGND,
		int nNumberDataBasePoints, 
		const Vector<int>* pvecTrueNeighbors,
		Vector<double> *pvec_result)
{
	m_pmatGND = pGND;

	m_pvecResult = pvec_result;
	m_pvecNumberTrueNeighbors = pvecTrueNeighbors;
	m_nNumberDataBasePoints = nNumberDataBasePoints;

	if (m_pvecNumberTrueNeighbors->Ptr())
	{
		m_pvecResult->SetValueZeros();
		//m_nNumberCandidate = m_pmatResult->Cols();
	}
	EXIT_ASSERT(m_pvecResult->Size() == m_pvecNumberTrueNeighbors->Size());
	//SMART_ASSERT(0)(*m_pvecResult).Exit();
	//SMART_ASSERT(m_nNumberCandidate > 0)(m_nNumberCandidate).Exit();

	int largest = m_pmatGND->Cols();
	for (int i = 0; i < m_pvecNumberTrueNeighbors->Size(); i++)
	{
		int required = m_pvecNumberTrueNeighbors->operator[](i);
		EXIT_ASSERT(required <= largest);
	}

}

void mAPCriterion::EvaluateFixedTopKEach(
	const int* p_idx, int num_candidate,
	const int* p_gnd, 
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

		for (int j = 0; j < num_candidate; j++)
		{
			int idx = p_idx[j];
			if (vec_indicator[idx])
			{
				vec_mat_indicator[i][idx_point][j]++;
			}
		}
	}
}

void mAPCriterion::ComputeFromIndicator(
	const Vector<SMatrix<bool> > &vec_mat_indicator)
{
	int num_topks = vec_mat_indicator.Size();

	for (int k = 0; k < num_topks; k++)
	{
		const SMatrix<bool>& mat_indicator = vec_mat_indicator[k];
		double &result = m_pvecResult->operator[](k);

		int num_query = mat_indicator.Rows();
		int num_candidate = mat_indicator.Cols();

		for (int i = 0; i < num_query; i++)
		{
			const bool* p_indicate = mat_indicator[i];

			double num_good = 0;
			for (int j = 0; j < num_candidate; j++)
			{
				if (*p_indicate++)
				{
					result += (++num_good) / ((double)(j + 1));
				}
			}
		}

	}
}

void mAPCriterion::BeginEvaluate()
{
	m_nTotalEvaluated = 0;
}

void mAPCriterion::EndEvaluate()
{
	int num_topks = m_pvecNumberTrueNeighbors->Size();

	//SMART_ASSERT(0)(*m_pvecResult);

	for (int k = 0; k < num_topks; k++)
	{
		int num_nn = m_pvecNumberTrueNeighbors->operator[](k);
		
		double& r = m_pvecResult->operator[](k);
		r /= (double)num_nn;
	}

	*m_pvecResult /= (double)m_nTotalEvaluated;
	//SMART_ASSERT(0)(*m_pvecResult);
}

void mAPCriterion::Evaluate(
	const SMatrix<int> &matRetrievedIndex, int idx_begin, int idx_end)
{
	int num_query = idx_end - idx_begin;

	int num_candidate = matRetrievedIndex.Cols();
	//SMART_ASSERT(m_nNumberCandidate == matRetrievedIndex.Cols())(m_nNumberCandidate)(matRetrievedIndex.Cols()).Exit();
	EXIT_ASSERT(num_query <= matRetrievedIndex.Rows());

	Vector<bool> vec_indicator(m_nNumberDataBasePoints);

	int num_topks = m_pvecNumberTrueNeighbors->Size();

	//return;
	Vector<SMatrix<bool> > vec_mat_indicator;
	vec_mat_indicator.AllocateSpace(num_topks);

	for (int i = 0; i < num_topks; i++)
	{
		vec_mat_indicator[i].AllocateSpace(num_query, num_candidate);
		vec_mat_indicator[i].SetValueZeros();
	}
	//return;
#pragma omp parallel for
	for (int i = 0; i < num_query; i++)
	{
		const int* p_idx = matRetrievedIndex[i];
		EXIT_ASSERT(i + idx_begin < m_pmatGND->Rows());

		const int* p_gnd = m_pmatGND->operator[](i + idx_begin);
		
		EvaluateFixedTopKEach(p_idx, num_candidate, p_gnd, i, vec_mat_indicator);
	}
	//return;

	ComputeFromIndicator(vec_mat_indicator);

	m_nTotalEvaluated += num_query;
}

