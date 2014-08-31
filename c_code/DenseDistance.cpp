#include "DenseDistance.h"
#include "math_basic.h"

void DenseDistance::Initialize(
	const SMatrix<double>* p_mat_lookup,
	int code_length,
	int num_partitions)
{
	m_pmatLookup = p_mat_lookup;

	m_nNumberPartitions = num_partitions;
	m_nCodeLength = code_length;

	m_vecBucketNumber.AllocateSpace(m_nNumberPartitions);

	m_bp2.Initialize(m_nCodeLength, m_nNumberPartitions);
	m_bp2.GetSubCodeLength(m_vecBucketNumber);

	{
		BinaryCodePartition bp;
		Vector<int> vec(m_nNumberPartitions);
		bp.SplitEachPartitionLength(m_nCodeLength, m_nNumberPartitions, vec.Ptr());
		EXIT_ASSERT(squared_distance(vec.Ptr(), m_vecBucketNumber.Ptr(), m_nNumberPartitions) == 0);
	}

	for (int i = 0; i < m_nNumberPartitions; i++)
	{
		m_vecBucketNumber[i] = (1 << m_vecBucketNumber[i]);
	}

	EXIT_ASSERT(m_vecBucketNumber.SumUp() == m_pmatLookup->Rows() && 
		m_vecBucketNumber.SumUp() == m_pmatLookup->Cols());
}

bool DenseDistance::IsPreprocessingQuery() const
{
	return true;
}

void DenseDistance::PreProcessing(const void* p_query, void* &p_pre_out) const
{

	const int total_number = m_pmatLookup->Rows();

	p_pre_out = new QueryPreprocessing();
	Vector<double> &lookup = ((QueryPreprocessing*)p_pre_out)->vec_lookup;
	lookup.AllocateSpace(total_number);
	lookup.SetValueZeros();

	Vector<uint64> &vec_space = ((QueryPreprocessing*)p_pre_out)->vec_space;
	vec_space.AllocateSpace(m_nNumberPartitions);

	uint64* p_partitions_query = vec_space.Ptr();
	m_bp2.SplitPartition(
		(const uchar*)p_query, 
		p_partitions_query);

	int idx_row = 0;
	for (int i = 0; i < m_nNumberPartitions; i++)
	{
		const double* p_from = 
			m_pmatLookup->operator[](idx_row + (int)p_partitions_query[i]);
		double* p_to = lookup.Ptr();

		for (int j = 0; j < total_number; j++)
		{
			(*p_to++) += (*p_from++);
		}
		idx_row += m_vecBucketNumber[i];
	}
}
double DenseDistance::DistancePre(void* p_query, const void* p_right) const
{
	QueryPreprocessing* p_query_processing = (QueryPreprocessing*)p_query;

	Vector<double> &lookup = p_query_processing->vec_lookup;

	uint64* p_partitions = p_query_processing->vec_space.Ptr();
	m_bp2.SplitPartition(
		(const uchar*)p_right,
		p_partitions);
	int idx_row = 0;
	double r = 0;
	for (int i = 0; i < m_nNumberPartitions; i++)
	{
		int idx = *p_partitions++;
		r += lookup[idx_row + idx];
		idx_row += m_vecBucketNumber[i];
	}
	return r;
}
void DenseDistance::PostProcessing(void* &p_pre_out) const
{
	if (p_pre_out)
	{
		delete (QueryPreprocessing*)p_pre_out;
		p_pre_out = NULL;
	}
}

