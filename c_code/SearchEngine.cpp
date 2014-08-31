#include "SearchEngine.h"



void SearchEngine::ANNSearching(void* p_query, int* p_idx, int num_candidate)
{
}
int SearchEngine::GetNumberDataPoint() 
{ 
	return m_nNumDataBasePoint; 
}

void* SearchEngine::GetRawData(int idx) 
{
	return NULL;
}

void SearchEngine::CopyRawData(int idx_retrieved, Vector<double> &vec_raw_database_point)
{
}
