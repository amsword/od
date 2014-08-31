#pragma once




struct QueryPreprocessing
{
};
class DistanceCalculator
{
public:
	virtual bool IsPreprocessingQuery() const;
	virtual double Distance(
		const void* p_query, const void* p_right) const;

	virtual void PreProcessing(const void* p_query, void* &p_pre_out) const;
	virtual double DistancePre(void* p_query, const void* p_right) const;
	virtual void PostProcessing(void* &p_pre_out) const;
};
