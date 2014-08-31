#include "QueryOptDistance.h"
#include "math_basic.h"

void QueryOptDistance::Initialize(
	const SMatrix<double>* pmatAux,
	const Vector<SMatrix<double> >* pvecmatCenters,
	const Vector<Vector<double> >* pvecvecErrors,
	int code_length,
	int num_partitions)
{
	m_pmatAux = pmatAux;
	m_pvecmatCenters = pvecmatCenters;
	m_pvecvecErrors = pvecvecErrors;

	m_nNumberPartitions = num_partitions;
	m_nCodeLength = code_length;

	m_nDimension = m_pvecmatCenters->operator[](0).Cols();

	m_vecBucketNumber.AllocateSpace(m_nNumberPartitions);

	m_bp2.Initialize(m_nCodeLength, m_nNumberPartitions);
	m_bp2.GetSubCodeLength(m_vecBucketNumber);

	//{
	//	Vector<int> vec(m_nNumberPartitions);
	//	BinaryCodePartition m_bp;
	//	m_bp.SplitEachPartitionLength(
	//		m_nCodeLength, 
	//		m_nNumberPartitions, 
	//		vec.Ptr());
	//	SMART_ASSERT(squared_distance(vec.Ptr(), m_vecBucketNumber.Ptr(), m_nNumberPartitions) == 0);
	//}
	for (int i = 0; i < m_nNumberPartitions; i++)
	{
		m_vecBucketNumber[i] = (1 << m_vecBucketNumber[i]);
	}



	EXIT_ASSERT(m_vecBucketNumber.SumUp() == m_pmatAux->Rows() && 
		m_vecBucketNumber.SumUp() == m_pmatAux->Cols())
}

bool QueryOptDistance::IsPreprocessingQuery() const
{
	return true;
}

void QueryOptDistance::PreProcessing(const void* p_query, void* &p_pre_out) const
{
	const int total_number = m_pmatAux->Rows();

	Vector<double> vec_inter(total_number);

	const double* pf_query = (const double*)p_query;
	int k = 0;
	int idx_row = 0;
	for (int i = 0; i < m_nNumberPartitions; i++)
	{
		int num_bucket = m_vecBucketNumber[i];
		const SMatrix<double>& mat_centers = m_pvecmatCenters->operator[](i);
		const Vector<double>& vec_errors = m_pvecvecErrors->operator[](i);

		for (int j = 0; j < num_bucket; j++)
		{
			vec_inter[k++] = squared_distance(pf_query, mat_centers[j], m_nDimension) + 
				vec_errors[j];
		}
	}

	p_pre_out = new QueryPreprocessing();

	Vector<double> &lookup = ((QueryPreprocessing*)p_pre_out)->vec_lookup;
	lookup.AllocateSpace(total_number);

	m_pmatAux->Multiple(vec_inter.Ptr(), lookup.Ptr());

	Vector<uint64> &vec_space = ((QueryPreprocessing*)p_pre_out)->vec_space;
	vec_space.AllocateSpace(m_nNumberPartitions);

}

double QueryOptDistance::DistancePre(void* p_query, const void* p_right) const
{
	QueryPreprocessing* p_query_processing = (QueryPreprocessing*)p_query;

	Vector<double> &lookup = p_query_processing->vec_lookup;

	uint64* p_partitions = p_query_processing->vec_space.Ptr();


	m_bp2.SplitPartition((const uchar*)p_right, p_partitions);

	//{
	//	BinaryCodePartition m_bp;
	//	Vector<uint64> vec_aux2(m_nNumberPartitions);
	//	m_bp.SplitPartition(
	//		(const uchar*)p_right,
	//		m_nCodeLength,
	//		m_nNumberPartitions,
	//		vec_aux2.Ptr());
	//	SMART_ASSERT(
	//		squared_distance(vec_aux2.Ptr(), p_partitions, m_nNumberPartitions) == 0)
	//		(vec_aux2)
	//		(Vector<uint64>(p_partitions, m_nNumberPartitions)).Exit();
	//}
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

void QueryOptDistance::PostProcessing(void* &p_pre_out) const
{
	if (p_pre_out)
	{
		delete (QueryPreprocessing*)p_pre_out;
		p_pre_out = NULL;
	}
}
