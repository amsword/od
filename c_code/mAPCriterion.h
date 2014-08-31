#pragma once


#include "Criterion.h"
#include "Vector.h"
#include "SMatrix.h"


class mAPCriterion : public Criterion
{
public:

	void Initialize(const SMatrix<int>* pGND,
		int nNumberDataBasePoints, 
		const Vector<int>* pvecTrueNeighbors,
		Vector<double> *pvec_result);


public:
	virtual void BeginEvaluate();
	virtual void Evaluate(const SMatrix<int> &matRetrievedIndex, int idx_begin, int idx_end);
	virtual void EndEvaluate();


private:
	void EvaluateFixedTopKEach(const int* p_idx, int num_candidate,
		const int* p_gnd, 
		int i, Vector<SMatrix<bool> > &vec_mat_indicator) const;

	void ComputeFromIndicator(const Vector<SMatrix<bool> > &vec_mat_indicator);

private:
	const SMatrix<int> *m_pmatGND;
	const Vector<int>* m_pvecNumberTrueNeighbors;
	Vector<double>* m_pvecResult;

	//int m_nNumberCandidate;
	int m_nNumberDataBasePoints;
	int m_nTotalEvaluated;

};
