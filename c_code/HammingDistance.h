#pragma once

#include "Vector.h"
#include "SMatrix.h"
#include "DistanceCalculator.h"





class HammingDistance : public DistanceCalculator
{
public:
	void Initialize(int code_length_in_byte);

public:
	virtual double Distance(
		const void* p_query, const void* p_right) const;

private:
	int m_nCodeLengthInByte;
};
