#pragma once

#include "SearchEngine.h"

#include "Vector.h"
#include "SMatrix.h"
#include "PriorityQueue.h"
#include "DistanceCalculator.h"

template <class T>
class LinearSearchEngine : public SearchEngine
{
public:
	void Initialize(const SMatrix<T> * pDataBase, DistanceCalculator* p_dist)
	{
		m_pDataBase = pDataBase;
		m_pDistanceCalculator = p_dist;

		m_nNumDataBasePoint = m_pDataBase->Rows();
		//PRINT << m_nNumDataBasePoint << "\n";
	}

	virtual void ANNSearching(void* p_query, int* p_idx, int num_candidate) 
	{
		m_nNumCandidate = num_candidate;
		//SMART_ASSERT(0).Exit();
		//PRINT << "1\n";

		Heap<PAIR<double> > heap_knn;
		heap_knn.Reserve(m_nNumCandidate);

		if (m_pDistanceCalculator->IsPreprocessingQuery())
		{
			//PRINT << "1\n";
			void* query;
			m_pDistanceCalculator->PreProcessing(p_query, query);
			//PRINT << "1\n";
			FindBestCandidatePre(query, heap_knn);
			//PRINT << "1\n";
			m_pDistanceCalculator->PostProcessing(query);
			//PRINT << "1\n";
		}
		else
		{
			//PRINT << "1\n";
			FindBestCandidate(p_query, heap_knn);
		}
		PopFromHeap(heap_knn, p_idx);
	}

private:
	void PopFromHeap(Heap<PAIR<double> >& heap, int* p_idx)
	{
		int k = heap.size() - 1;

		while (!heap.empty())
		{
			PAIR<double> node;
			heap.popMin(node);
			*(p_idx + k) = node.index;
			//PRINT << node.distance << "\n";
			k--;
		}
		//SMART_ASSERT(0).Exit();
	}

	void FindBestCandidate(void* p_query, Heap<PAIR<double> >& heap_knn) const
	{
		for (int j = 0; j < m_nNumDataBasePoint; j++)
		{
			PAIR<double> node;

			double s = m_pDistanceCalculator->Distance(
				p_query, m_pDataBase->operator[](j));

			node.distance = s;

			if (heap_knn.size() < m_nNumCandidate)
			{
				node.index = j;
				heap_knn.insert(node);
			}
			else
			{
				const PAIR<double>& top = heap_knn.Top();
				if (top < node)
				{
					node.index = j;
					heap_knn.popMin();
					heap_knn.insert(node);
				}
			}
		}
	}

	void FindBestCandidatePre(void* p_query, Heap<PAIR<double> >& heap_knn) const
	{
		for (int j = 0; j < m_nNumDataBasePoint; j++)
		{
			PAIR<double> node;

			//PRINT << j << "\n";

			double s = m_pDistanceCalculator->DistancePre(
				p_query, m_pDataBase->operator[](j));
			//PRINT << "1\n";

			node.distance = s;

			if (heap_knn.size() < m_nNumCandidate)
			{
				node.index = j;
				heap_knn.insert(node);
			}
			else
			{
				const PAIR<double>& top = heap_knn.Top();
				if (top < node)
				{
					node.index = j;
					heap_knn.popMin();
					heap_knn.insert(node);
				}
			}
		}
	}


	void FindBestCandidate(const QueryPreprocessing& query, Heap<PAIR<double> >& heap_knn) const
	{
		for (int j = 0; j < m_nNumDataBasePoint; j++)
		{
			PAIR<double> node;

			double s = m_pDistanceCalculator->Distance(query, m_pDataBase->operator[](j));

			node.distance = s;

			if (heap_knn.size() < m_nNumCandidate)
			{
				node.index = j;
				heap_knn.insert(node);
			}
			else
			{
				const PAIR<double>& top = heap_knn.Top();
				if (top < node)
				{
					node.index = j;
					heap_knn.popMin();
					heap_knn.insert(node);
				}
			}
		}
	}


protected:
	const SMatrix<T>* m_pDataBase;
	const DistanceCalculator* m_pDistanceCalculator;
	int m_nNumCandidate;
};
