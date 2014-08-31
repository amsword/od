#pragma once

#include "Vector.h"
#include "types.h"


class BinaryCodePartition
{
public:
	void SplitPartition(
		const uchar* pcodes, 
		int code_length, 
		int num_partitions, 
		uint64* partitions) const;

	void SplitEachPartitionLength(
		int code_length, 
		int num_partitions,
		int* p_num) const;

	void SplitEqualPartition(
		const uchar* &pcodes,
		int &idx_bit,
		int partition_length,
		int num_partition,
		uint64* &out) const;
};

class BinaryCodePartition2
{
public:
	void Initialize(int code_length, int num_partitions);
public:
	void SplitPartition(
		const uchar* pcodes, 
		uint64* partitions) const;
	void GetSubCodeLength(Vector<int> &vec) const;
private:
	int m_nNumPartitions;
	Vector<int> m_vecByteOffset;
	Vector<int> m_vecBitOffset;
	Vector<int> m_vecMask;
	Vector<int> m_vec_each_length;
};


