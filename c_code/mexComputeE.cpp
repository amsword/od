#include "Vector.h"
#include "SMatrix.h"
#include "TypeConvert.h"
#include "BinaryCodePartition.h"

#define BINARY_CODE prhs[0]
#define NUMBER_PARTITION prhs[1]

#define RESULT plhs[0]


void ComputeE(
	const SMatrix<uchar> &matCodes, 
	int num_partitions,
	SMatrix<SMatrix<double> > &matmatE)
{
	int num_point = matCodes.Rows();
	int code_length = matCodes.Cols() * 8;

	int count = 0;
	uint64 partitions[100];

	EXIT_ASSERT(num_partitions <= 100);

	BinaryCodePartition bp;

	for (int i = 0; i < num_point; i++)
	{
		const uchar* p_bits = matCodes[i];

		bp.SplitPartition(p_bits, code_length, num_partitions, partitions);

		for (int v = 0; v < num_partitions; v++)
		{
			int n = partitions[v];
			for (int u = 0; u < num_partitions; u++)			
			{
				int m = partitions[u];

				matmatE[v][u][n][m]++;
			}
		}
	}
}

void SetZeros(SMatrix<SMatrix<double> >& matmat)
{
	int rows = matmat.Rows();
	int cols = matmat.Rows();

	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			matmat[i][j].SetValueZeros();
		}
	}
}

// E = mexComputeE8(binary_code, num_partitions)
void mexFunction(int nlhs, mxArray *plhs[], 
				 int nrhs, const mxArray *prhs[])
{
	SMatrix<uchar> matCodes;
	mexConvert(BINARY_CODE, matCodes);

	int num_partitions;
	mexConvert(NUMBER_PARTITION, num_partitions);

	int code_length = matCodes.Cols() * 8;

	BinaryCodePartition bp;

	Vector<int> vecNumberBucket(num_partitions);
	bp.SplitEachPartitionLength(code_length, num_partitions, vecNumberBucket.Ptr());

	RESULT = mxCreateCellMatrix(num_partitions, num_partitions);

	for (int i = 0; i < num_partitions; i++)
	{
		for (int j = 0; j < num_partitions; j++)
		{
			mxSetCell(RESULT, j * num_partitions + i, 
				mxCreateDoubleMatrix(1 << vecNumberBucket[i], 
				1 << vecNumberBucket[j], mxREAL));
		}
	}

	SMatrix<SMatrix<double> > matmatE;
	mexConvert(RESULT, matmatE);

	SetZeros(matmatE);


	ComputeE(matCodes, num_partitions, matmatE);
}

