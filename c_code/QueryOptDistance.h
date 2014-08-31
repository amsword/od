#pragma once
#include "SMatrix.h"
#include "BinaryCodePartition.h"
#include "DistanceCalculator.h"


class QueryOptDistance : public DistanceCalculator
{
public:
	void Initialize(
		const SMatrix<double>* pmatAux,
		const Vector<SMatrix<double> >* pvecmatCenters,
		const Vector<Vector<double> >* pvecvecErrors,
		int code_length,
		int num_partitions);

public:
	virtual bool IsPreprocessingQuery() const;

	virtual void PreProcessing(
		const void* p_query, 
		void* &p_pre_out) const;
	
	virtual double DistancePre(
		void* p_query, 
		const void* p_right) const;
	
	virtual void PostProcessing(void* &p_pre_out) const;

private:
	struct QueryPreprocessing
	{
		Vector<double> vec_lookup;
		Vector<uint64> vec_space;
	};

private:
	int m_nNumberPartitions;
	int m_nCodeLength;

	const SMatrix<double>* m_pmatAux;
	const Vector<SMatrix<double> >* m_pvecmatCenters;
	const Vector<Vector<double> >* m_pvecvecErrors;

	int m_nDimension;

	//BinaryCodePartition m_bp;
		BinaryCodePartition2 m_bp2;
	Vector<int> m_vecBucketNumber;
};
