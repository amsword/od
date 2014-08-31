#include <mex.h>
#include "Vector.h"
#include "SMatrix.h"
#include "TypeConvert.h"
#include "BinaryCodePartition.h"

#define BINARY_CODE prhs[0]
#define RAW_DATA prhs[1]
#define MAT_MAT_E prhs[2]

#define RESULT_G plhs[0]
#define RESULT_CENTERS plhs[1]
#define RESULT_ERRORS plhs[2]


template < class T >
T squared_distance(const T* p1, const T* p2, int num)
{
	T r = 0;
	for (int i = 0; i < num; i++)
	{
		T diff = (*p1++) - (*p2++);
		r += diff * diff;
	}
	return r;
}

template < class T1, class T2 >
double squared_distance_db(const T1* p1, const T2* p2, int num)
{
	double r = 0;
	for (int i = 0; i < num; i++)
	{
		double diff = (double)(*p1++) - (double)(*p2++);
		r += diff * diff;
	}
	return r;
}

template <class T>
void ComputeHyperCenter(
	const SMatrix<uchar> &matCodes, 
	const SMatrix<T> &matRawData,
	int num_partitions, 
	Vector<SMatrix<double> > &vecmatCenters)
{
	int dim = matRawData.Cols();

	for (int i = 0; i < vecmatCenters.Size(); i++)
	{
		vecmatCenters[i].SetValueZeros();
	}

	int num_point = matRawData.Rows();
	int code_length = matCodes.Cols() * 8;
	uint64 partitions[100];

	BinaryCodePartition bp;

	for (int i = 0; i < num_point; i++)
	{
		bp.SplitPartition(
			matCodes[i], 
			code_length, 
			num_partitions, 
			partitions);

		for (int u = 0; u < num_partitions; u++)
		{
			int idx_partition = partitions[u];

			double* p_center = vecmatCenters[u][idx_partition];
			const T* p_raw_data = matRawData[i];

			for (int k = 0; k < dim; k++)
			{
				(*p_center++) += (double)(*p_raw_data++);
			}
		}
	}

}

void DivideTotalCenters(const SMatrix<SMatrix<double> > &matmatE, 
	Vector<SMatrix<double> > &vecmatCenters)
{
	int dim = vecmatCenters[0].Cols();
	int num_partition = vecmatCenters.Size();

	for (int u = 0; u < num_partition; u++)
	{
		int num_bucket = vecmatCenters[u].Rows();

		for (int i = 0; i < num_bucket; i++)
		{
			int count = matmatE[u][u][i][i];

			double* p_center = vecmatCenters[u][i];
			if (count > 0)
			{
				for (int d = 0; d < dim; d++)
				{
					(*p_center++) /= (double)count;
				}
			}
		}
	}
}

template <class T>
void ComputeTotalErrors(
	const SMatrix<uchar> &matCodes, 
	const SMatrix<T> &matRawData, 
	const Vector<SMatrix<double> > &vecmatCenters,
	Vector<Vector<double> > &vecvec_errors)
{
	int num_partitions = vecmatCenters.Size();

	for (int i = 0; i < num_partitions; i++)
	{
		vecvec_errors[i].SetValueZeros();
	}

	int num_point = matCodes.Rows();
	int dim = matRawData.Cols();

	int code_length = matCodes.Cols() * 8;
	uint64 partitions[100];

	BinaryCodePartition bp;

	for (int i = 0; i < num_point; i++)
	{
		bp.SplitPartition(
			matCodes[i], 
			code_length, 
			num_partitions, 
			partitions);

		const T* p_raw_data = matRawData[i];

		for (int u = 0; u < num_partitions; u++)
		{
			int idx_bucket = partitions[u];

			const double* p_center = vecmatCenters[u][idx_bucket];

			vecvec_errors[u][idx_bucket] += 
				squared_distance_db(p_center, p_raw_data, dim);
		}
	}
}

void DivideTotalErrors(
	const SMatrix<SMatrix<double> > &matmatE,
	Vector<Vector<double> > &vecvec_errors)
{
	int num_partitions = matmatE.Rows();

	for (int u = 0; u < num_partitions; u++)
	{
		int num_bucket = vecvec_errors[u].Size();

		for (int i = 0; i < num_bucket; i++)
		{
			double e = matmatE[u][u][i][i];

			if (e > 0.5)
			{
				vecvec_errors[u][i] /= e;
			}
		}
	}
}



void ComputeG8(
	const Vector<SMatrix<double> > &vecmatCenters, 
	const Vector<Vector<double> > &vecvec_errors, 
	const SMatrix<SMatrix<double> > &matmatE, 
	SMatrix<SMatrix<double> > &matmatG)
{
	int num_partitions = matmatE.Rows();
	const int num_bucket_each_partition = 256;

	int dim = vecmatCenters[0].Cols();

	// u, m is the column in matlab style
	for (int u = 0; u < num_partitions; u++)
	{
		const SMatrix<double> &matEu = matmatE[u][u];
		SMatrix<double> *pmatF = matmatG[u];

		int num_bucket_column = vecmatCenters[u].Rows();

		for (int v = 0; v < num_partitions; v++)
		{
			const SMatrix<double> &matEv = matmatE[v][v];
			SMatrix<double> &matF = pmatF[v];
			int num_bucket_row = vecmatCenters[v].Rows();

			for (int m = 0; m < num_bucket_column; m++)
			{
				double eum = matEu[m][m];
				double* pF = matF[m];
				const double* p_center_um = vecmatCenters[u][m];

				double error_um = vecvec_errors[u][m];
				for (int n = 0; n < num_bucket_row; n++)
				{
					double evn = matEv[n][n];
					double &f = pF[n];

					const double* p_center_vn = vecmatCenters[v][n];
					double error_vn = vecvec_errors[v][n];

					f = squared_distance(p_center_um, p_center_vn, dim);
					f = eum * evn * (f + error_um +  error_vn);
				}
			}
		}
	}
}

template <class T>
void ComputeG(const SMatrix<uchar> &matCodes, 
			  const SMatrix<T> &matRawData,
			  const SMatrix<SMatrix<double> > &matmatE,
			  int num_partitions, 
			  Vector<SMatrix<double> > &vecmatCenters,
			  Vector<Vector<double> > &vecvec_errors,
			  SMatrix<SMatrix<double> > &matmatG)
{
	ComputeHyperCenter(matCodes, matRawData, num_partitions, vecmatCenters);
	DivideTotalCenters(matmatE, vecmatCenters);
	ComputeTotalErrors(matCodes, matRawData, vecmatCenters, vecvec_errors);
	DivideTotalErrors(matmatE, vecvec_errors);
	ComputeG8(vecmatCenters, vecvec_errors, matmatE, matmatG);
}

// [G, centers, errors] = mexComputeG(binary_code, raw_data, matmatE, num_partitions)
void mexFunction(int nlhs, mxArray *plhs[], 
				 int nrhs, const mxArray *prhs[])
{
	EXIT_ASSERT(nrhs == 3);
	EXIT_ASSERT(nlhs == 3);

	SMatrix<uchar> matCodes;
	mexConvert(BINARY_CODE, matCodes);

	int num_points = matCodes.Rows();


	SMatrix<double> matRawData_db;
	SMatrix<float> matRawData_fl;
	int dim;
	if (mxIsDouble(RAW_DATA))
	{
		mexConvert(RAW_DATA, matRawData_db);
		dim = matRawData_db.Cols();
		EXIT_ASSERT(matRawData_db.Rows() == num_points);
	}
	else if (mxIsSingle(RAW_DATA))
	{
		mexConvert(RAW_DATA, matRawData_fl);
		dim = matRawData_fl.Cols();
		EXIT_ASSERT(matRawData_fl.Rows() == num_points);
	}
	else
	{
		EXIT_ASSERT(0);
	}
	SMatrix<SMatrix<double> > matmatE;
	mexConvert(MAT_MAT_E, matmatE);

	int num_partitions = matmatE.Rows();

	Vector<int> vecNumberBucket(num_partitions);
	int code_length = matCodes.Cols() * 8;

	BinaryCodePartition bp;

	bp.SplitEachPartitionLength(code_length, num_partitions, vecNumberBucket.Ptr());

	RESULT_G = mxCreateCellMatrix(num_partitions, num_partitions);
	for (int i = 0; i < num_partitions; i++)
	{
		for (int j = 0; j < num_partitions; j++)
		{
			mxSetCell(RESULT_G, j * num_partitions + i, 
				mxCreateDoubleMatrix(1 << vecNumberBucket[i], 1 << vecNumberBucket[j], mxREAL));
		}
	}



	RESULT_CENTERS = mxCreateCellMatrix(num_partitions, 1);

	RESULT_ERRORS = mxCreateCellMatrix(num_partitions, 1);

	for (int i = 0; i < num_partitions; i++)
	{
		mxSetCell(RESULT_CENTERS, i, mxCreateDoubleMatrix(dim, 1 << vecNumberBucket[i], mxREAL));
		mxSetCell(RESULT_ERRORS, i, mxCreateDoubleMatrix(1 << vecNumberBucket[i], 1, mxREAL));
	}

	SMatrix<SMatrix<double> > matmatG;
	mexConvert(RESULT_G, matmatG);

	Vector<SMatrix<double> > vecmatCenters;
	mexConvert(RESULT_CENTERS, vecmatCenters);

	Vector<Vector<double> > vecvecErrors;
	mexConvert(RESULT_ERRORS, vecvecErrors);

	if (matRawData_db.Ptr())
	{
		ComputeG(matCodes, matRawData_db, matmatE, num_partitions, vecmatCenters, vecvecErrors, matmatG);
	}
	else if (matRawData_fl.Ptr())
	{
		ComputeG(matCodes, matRawData_fl, matmatE, num_partitions, vecmatCenters, vecvecErrors, matmatG);
	}
} 
