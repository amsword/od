#pragma once 

#include "SMatrix.h"

class Criterion
{
public:
	virtual void BeginEvaluate();
	virtual void Evaluate(const SMatrix<int> &matRetrievedIndex, int idx_begin, int idx_end);
	virtual void EndEvaluate();
};
