#pragma once

#include "Vector.h"
#include "SMatrix.h"
#include "Criterion.h"


class FixedTopksCriterion : public Criterion
{
public:

	void Initialize(const SMatrix<int>* pGND,
		int nNumberDataBasePoints, 
		const Vector<int>* pvecTrueNeighbors,
		SMatrix<double> *pmat_result);


public:
	virtual void Evaluate(const SMatrix<int> &matRetrievedIndex, int idx_begin, int idx_end);

private:
	void EvaluateFixedTopKEach(const int* p_idx, const int* p_gnd, 
		int i, Vector<SMatrix<bool> > &vec_mat_indicator) const;

private:
	const SMatrix<int> *m_pmatGND;
	const Vector<int>* m_pvecNumberTrueNeighbors;
	SMatrix<double>* m_pmatResult;

	int m_nNumberCandidate;
	int m_nNumberDataBasePoints;


};