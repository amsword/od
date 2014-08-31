#pragma once
#include "Vector.h"
#include "SMatrix.h"
#include "BinaryCodePartition.h"
#include "DistanceCalculator.h"




class DenseDistance : public DistanceCalculator
{
public:
	void Initialize(
		const SMatrix<double>* p_mat_lookup,
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
	const SMatrix<double>* m_pmatLookup;
	int m_nNumberPartitions;
	int m_nCodeLength;
	BinaryCodePartition2 m_bp2;
	Vector<int> m_vecBucketNumber;
};
