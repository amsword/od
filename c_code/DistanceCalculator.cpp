#include <stdio.h>
#include <stdlib.h>

#include "DistanceCalculator.h"
#include "AssertMacro.h"


bool DistanceCalculator::IsPreprocessingQuery() const
{
	return false;
}

double DistanceCalculator::Distance(
	const void* p_query, const void* p_right) const
{
	EXIT_ASSERT(0);
	return -1;
}

void DistanceCalculator::PreProcessing(const void* p_query, void* &p_pre_out) const
{
	EXIT_ASSERT(0);
}
double DistanceCalculator::DistancePre(void* p_query, const void* p_right) const
{
	EXIT_ASSERT(0);
	return -1;
}
void DistanceCalculator::PostProcessing(void* &p_pre_out) const
{
	EXIT_ASSERT(0);
}

