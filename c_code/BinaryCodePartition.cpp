#include "BinaryCodePartition.h"



void BinaryCodePartition2::SplitPartition(
	const uchar* pcodes, 
	uint64* partitions) const
{
	for (int i = 0; i < m_nNumPartitions; i++)
	{
		int byte_offset = m_vecByteOffset[i];
		int bit_offset = m_vecBitOffset[i];
		const int* ptarget = (const int*)(&pcodes[byte_offset]);
		partitions[i] = (*ptarget >> bit_offset) & m_vecMask[i];
	}
}


void BinaryCodePartition2::GetSubCodeLength(Vector<int> &vec) const
{
	m_vec_each_length.CopyTo(vec);
}

void BinaryCodePartition2::Initialize(
	int code_length, int num_partitions)
{
	m_nNumPartitions = num_partitions;

	m_vec_each_length.AllocateSpace(num_partitions);
	int sub_length1 = code_length / num_partitions;
	m_vec_each_length.SetValue(sub_length1);

	int left = (code_length % num_partitions);
	for (int i = 0; i < left; i++)
	{
		m_vec_each_length[i]++;
	}

	m_vecBitOffset.AllocateSpace(num_partitions);
	m_vecByteOffset.AllocateSpace(num_partitions);
	m_vecMask.AllocateSpace(num_partitions);

	int total_offset = 0;
	for (int i = 0; i < num_partitions; i++)
	{
		m_vecByteOffset[i] = total_offset / 8;
		m_vecBitOffset[i] = total_offset % 8;
		m_vecMask[i] = (1 << m_vec_each_length[i]) - 1;
		total_offset += m_vec_each_length[i];
	}
}

///


void BinaryCodePartition::SplitEqualPartition(
	const uchar* &pcodes,
	int &idx_bit,
	int partition_length,
	int num_partition,
	uint64* &out) const
{
	const uint64 all_1 = 0xFFFFFFFFFFFFFFFF;

	uint64 mask = ~(all_1 << partition_length);

	int i;
	for (i = 0; i < num_partition; i++)
	{
		int idx_end = idx_bit + partition_length - 1;
		int num_byte = idx_end / 8 + 1;
		uchar* p_part = (uchar*)out;
		const uchar* p_in = pcodes;
		for (int j = 0; j < num_byte; j++)
		{
			*p_part++ = *p_in++;
		}
		*out = (*out) >> idx_bit;
		*out = *out & mask;

		out++;
		idx_bit = ((idx_end + 1) % 8);
		pcodes += (idx_end + 1) / 8;
	}
}

void BinaryCodePartition::SplitEachPartitionLength(
	int code_length, 
	int num_partitions,
	int* p_num) const
{
	int part_length_second = code_length / num_partitions;
	int part_length_first = part_length_second + 1;
	int num_first = (code_length % num_partitions);

	int i;
	for (i = 0; i < num_first; i++)
	{
		p_num[i] = part_length_first;
	}
	for (; i < num_partitions; i++)
	{
		p_num[i] = part_length_second;
	}
}

void BinaryCodePartition::SplitPartition(
	const uchar* pcodes, 
	int code_length, 
	int num_partitions, 
	uint64* partitions) const
{
	int part_length_second = code_length / num_partitions;
	int part_length_first = part_length_second + 1;
	int num_first = (code_length % num_partitions);

	int idx_bit = 0;
	SplitEqualPartition(pcodes, idx_bit, part_length_first, num_first, partitions);

	SplitEqualPartition(pcodes, idx_bit, part_length_second, num_partitions - num_first, partitions);
}
