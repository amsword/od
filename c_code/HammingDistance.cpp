#include "HammingDistance.h"
#include "types.h"
// #include <intrin.h>


const int lookup [] = {0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8};


void HammingDistance::Initialize(int code_length_in_byte)
{
	m_nCodeLengthInByte = code_length_in_byte;
}

double HammingDistance::Distance(
	const void* p_query, const void* p_right) const
{
	int output = 0;
	int i;
	const uchar* p_uchRight = (const uchar*)p_right;
	const uchar* p_uchLeft = (const uchar*)p_query;

	switch(m_nCodeLengthInByte) 
	{
	//case 4:
	//	return __popcnt(*(unsigned int*)p_uchLeft ^ *(unsigned int*)p_uchRight);
	//	break;
	//case 8:
	//	return __popcnt64(*(unsigned long long*)p_uchLeft ^ *(unsigned long long*)p_uchRight);
	//	break;
	//case 16:
	//	return    __popcnt64(((unsigned long long*)p_uchLeft)[0] ^ ((unsigned long long*)p_uchRight)[0]) \
	//		+ __popcnt64(((unsigned long long*)p_uchLeft)[1] ^ ((unsigned long long*)p_uchRight)[1]);
	//	break;
	//case 32:
	//	return    __popcnt64(((unsigned long long*)p_uchLeft)[0] ^ ((unsigned long long*)p_uchRight)[0]) \
	//		+ __popcnt64(((unsigned long long*)p_uchLeft)[1] ^ ((unsigned long long*)p_uchRight)[1]) \
	//		+ __popcnt64(((unsigned long long*)p_uchLeft)[2] ^ ((unsigned long long*)p_uchRight)[2]) \
	//		+ __popcnt64(((unsigned long long*)p_uchLeft)[3] ^ ((unsigned long long*)p_uchRight)[3]);
	//	break;
	default:
		for (i=0; i<m_nCodeLengthInByte; i++)
			output+= lookup[p_uchLeft[i] ^ p_uchRight[i]];
		return output;
		break;
	}
}
