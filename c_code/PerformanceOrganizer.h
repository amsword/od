#pragma once

#include <string>

#include <list>
using namespace std;

#include "Vector.h"

#include "SearchEngine.h"
#include "Criterion.h"


template <class QueryType> 
class PerformanceOrganizer
{
public:
	void BatchParallel(const SMatrix<QueryType>* pmatQuery, 
		SearchEngine* psearch_engine,
		int num_candidate,
		list<Criterion*>* plst_cri)
	{
		m_pmatQuery = pmatQuery;
		m_psearchEngine = psearch_engine;
		m_nNumCandidate = num_candidate;
		m_plstCriteria = plst_cri;

		m_nNumTraining = psearch_engine->GetNumberDataPoint();
		m_nNumTest = m_pmatQuery->Rows();

		int batch_size = 200;

		int batch_num = m_nNumTest / batch_size + 1;

		BeginEvaluate();

		for (int batch_idx = 0; batch_idx < batch_num; batch_idx++)
		{
			int idx_begin = batch_idx * batch_size;

			if (idx_begin >= m_nNumTest)
			{
				continue;
			}

			int idx_end = idx_begin + batch_size;

			if (idx_end > m_nNumTest)
			{
				idx_end = m_nNumTest;
			}

			Evaluate(idx_begin, idx_end);
		}


		EndEvaluate();
	}
private:
	void Evaluate(int idx_begin, int idx_end)
	{
		int batch_size = idx_end - idx_begin;

		SMatrix<int> matRetrievedIndex;
		matRetrievedIndex.AllocateSpace(batch_size, m_nNumCandidate);
		matRetrievedIndex.SetValueZeros();

		for (int i = 0; i < batch_size; i++)
		{
			for (int j = 0; j < m_nNumCandidate; j++)
			{
				matRetrievedIndex[i][j] = j;
			}
		}

		//PRINT << m_pmatQuery->Rows() << "\n"
		//	<< m_pmatQuery->Cols() << "\n";
#pragma omp parallel for
		for (int i = idx_begin; i < idx_end; i++)
		{
			int idx_heap = i - idx_begin;
			m_psearchEngine->ANNSearching(
				m_pmatQuery->operator[](i), matRetrievedIndex[idx_heap], m_nNumCandidate);
		}

		for (list<Criterion*>::iterator iter = m_plstCriteria->begin();
			iter != m_plstCriteria->end();
			iter++)
		{
			(*iter)->Evaluate(matRetrievedIndex, idx_begin, idx_end);
		}
	}	
	void BeginEvaluate()
	{
		for (list<Criterion*>::iterator iter = m_plstCriteria->begin();
			iter != m_plstCriteria->end();
			iter++)
		{
			(*iter)->BeginEvaluate();
		}
	}
	void EndEvaluate()
	{
		for (list<Criterion*>::iterator iter = m_plstCriteria->begin();
			iter != m_plstCriteria->end();
			iter++)
		{
			(*iter)->EndEvaluate();
		}
	}

private:
	const SMatrix<QueryType>* m_pmatQuery;

	int m_nNumCandidate;
	int m_nNumTest;
	int m_nNumTraining;

private:
	SearchEngine* m_psearchEngine;
	list<Criterion*>*    m_plstCriteria;
};
