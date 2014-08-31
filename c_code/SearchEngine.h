#pragma once


#include "Vector.h"

class SearchEngine
{
public:
	virtual void ANNSearching(void* p_query, int* p_idx, int num_candidate);

	int GetNumberDataPoint();

	virtual void* GetRawData(int idx); 

	virtual void CopyRawData(int idx_retrieved, Vector<double> &vec_raw_database_point);

protected:
	int m_nNumDataBasePoint;
};
