#include <list>
#include "Vector.h"
#include "SMatrix.h"
#include "TypeConvert.h"
#include "BinaryCodePartition.h"
#include "Criterion.h"
#include "HammingDistance.h"
#include "DistanceCalculator.h"
#include "QueryOptDistance.h"
#include "DenseDistance.h"
#include "LinearSearchEngine.h"
#include "PerformanceOrganizer.h"
#include "FixedTopksCriterion.h"
#include "AverageDistanceRatioCriterion.h"
#include "mAPCriterion.h"

using namespace std;
#define MAT_QUERY prhs[0]
#define MAT_DATA_BASE prhs[1]
#define NUM_CANDIDATE prhs[2]
#define MAT_RAW_QUERY prhs[3]
#define MAT_RAW_DATABASE prhs[4]
#define DISTANCE_INFO prhs[5]
#define CRITERIA_INFO prhs[6]

#define RESULT plhs[0]

#define DISTANCE_HAMMING 0
#define DISTANCE_DENSE 2
#define DISTANCE_QUERY 3


struct DataCollection
{
	SMatrix<double> mat_double;
	SMatrix<float> mat_float;
	SMatrix<uint8> mat_uint8;
};

struct QueryDistanceInfo
{
	int numPartitions;
	SMatrix<double> matAux;
	Vector<SMatrix<double> > vecmatCenters;
	Vector<Vector<double> > vecvecErrors;
};

struct DenseDistanceInfo
{
	int numPartitions;
	SMatrix<double> matDense;
};

struct CriteriaMAP
{
	Vector<int> vec_fixedtopk;
	SMatrix<int> matGND;

	Vector<double> vec_output;
};

struct CriteriaFixedTopKInfo
{
	Vector<int> vec_fixedtopk;
	SMatrix<int> matGND;

	SMatrix<double> mat_output;
};


struct CriteriaAverageDistanceRatio
{
	int num_valid;
	SMatrix<int> matGND;
	Vector<double> vec_output;
};

void RunPrecision(
	const DataCollection &matQuery, 
	const SMatrix<uchar> &mat_data_base, 
	int num_candidate,
	const SMatrix<double> &matRawQuery,
	const DataCollection &data_base,
	//const SMatrix<double> &matRawDatabase, 
	int distance_type,
	void* p_distance_info,
	CriteriaFixedTopKInfo &criteria_fixedtopk,
	CriteriaMAP &criteria_map, 
	CriteriaAverageDistanceRatio &criteria_adr)
{
	list<Criterion*> lst_criteria;

	DistanceCalculator* p_dist;
	int code_length = mat_data_base.Cols() * 8;

	switch (distance_type)
	{
	case DISTANCE_HAMMING: // hamming distance
		p_dist = new HammingDistance();
		((HammingDistance*)p_dist)->Initialize(mat_data_base.Cols());
		break;

	case DISTANCE_DENSE: // dense
		{
			DenseDistanceInfo* p_dense_info = (DenseDistanceInfo*)p_distance_info;
			p_dist = new DenseDistance();
			((DenseDistance*)p_dist)->Initialize(
				&(p_dense_info->matDense), 
				code_length, 
				p_dense_info->numPartitions);
		}
		break;

	case DISTANCE_QUERY: // query-sensitive
		{
			QueryDistanceInfo* p_query_info = (QueryDistanceInfo*)p_distance_info;
			p_dist = new QueryOptDistance();

			((QueryOptDistance*)p_dist)->Initialize(
				&(p_query_info->matAux),
				&(p_query_info->vecmatCenters),
				&(p_query_info->vecvecErrors),
				code_length, 
				p_query_info->numPartitions);
		}
		break;

	default:
		EXIT_ASSERT(0);
	}

	LinearSearchEngine<uchar> linear_search;
	linear_search.Initialize(&mat_data_base, p_dist);

	if (criteria_fixedtopk.matGND.Ptr())
	{
		FixedTopksCriterion* p;
		allocate_vector_space(p, 1);
		p->Initialize(&criteria_fixedtopk.matGND,
			mat_data_base.Rows(),
			&criteria_fixedtopk.vec_fixedtopk,
			&criteria_fixedtopk.mat_output);
		lst_criteria.push_back(p);
	}

	if (criteria_adr.matGND.Ptr())
	{
		if (data_base.mat_double.Ptr())
		{
			EXIT_ASSERT(data_base.mat_float.Ptr() == NULL &&
				data_base.mat_uint8.Ptr() == NULL);
			AverageDistanceRatioCriterion<double>* p;
			allocate_vector_space(p, 1);
			p->Initialize(&criteria_adr.matGND,
				&matRawQuery,
				&data_base.mat_double,
				&criteria_adr.vec_output);
			lst_criteria.push_back(p);
		}
		else if (data_base.mat_float.Ptr())
		{
			EXIT_ASSERT(data_base.mat_double.Ptr() == NULL &&
				data_base.mat_uint8.Ptr() == NULL);
			AverageDistanceRatioCriterion<float>* p;
			allocate_vector_space(p, 1);
			p->Initialize(&criteria_adr.matGND,
				&matRawQuery,
				&data_base.mat_float,
				&criteria_adr.vec_output);
			lst_criteria.push_back(p);
		}
		else if (data_base.mat_uint8.Ptr())
		{
			EXIT_ASSERT(data_base.mat_double.Ptr() == NULL &&
				data_base.mat_uint8.Ptr() == NULL);
			AverageDistanceRatioCriterion<uint8>* p;
			allocate_vector_space(p, 1);
			p->Initialize(&criteria_adr.matGND,
				&matRawQuery,
				&data_base.mat_uint8,
				&criteria_adr.vec_output);
			lst_criteria.push_back(p);
		}
		else
		{
			EXIT_ASSERT(0);
		}
	}
	if (criteria_map.vec_fixedtopk.Ptr())
	{
		mAPCriterion* p;
		allocate_vector_space(p, 1);
		p->Initialize(&(criteria_map.matGND),
			mat_data_base.Rows(),
			&(criteria_map.vec_fixedtopk),
			&(criteria_map.vec_output));
		lst_criteria.push_back(p);
	}

	if (distance_type == DISTANCE_HAMMING ||
		distance_type == DISTANCE_DENSE)
	{
		PerformanceOrganizer<uchar> perf_master;

		perf_master.BatchParallel(
			&matQuery.mat_uint8, 
			&linear_search, 
			num_candidate, 
			&lst_criteria);
	}
	else if (distance_type == DISTANCE_QUERY)
	{
		PerformanceOrganizer<double> perf_master;
		perf_master.BatchParallel(
			&matRawQuery,
			&linear_search,
			num_candidate,
			&lst_criteria);
	}
	else
	{
		EXIT_ASSERT(0);
	}

	//SMART_ASSERT(0)(criteria_map.vec_output);

	for (list<Criterion*>::iterator iter = lst_criteria.begin(); 
		iter != lst_criteria.end(); 
		iter++)
	{
		delete[] (*iter);
	}
	//PRINT << Vector<double>(criteria_fixedtopk.mat_output[0], 10) << "\n";

	delete p_dist;
	p_dist = NULL;
}

void ReleaseDistanceInfo(void* &p_distance_info)
{
	if (p_distance_info)
	{
		delete p_distance_info;
		p_distance_info = NULL;
	}
}

#ifdef MATLAB_MEX_FILE

void GetCriteriaInfo(
	const mxArray* p,
	CriteriaFixedTopKInfo &criteria_fixed, 
	CriteriaMAP &criteria_map, 
	CriteriaAverageDistanceRatio &criteria_adr)
{
	if (p == NULL)
	{
		return;
	}

	EXIT_ASSERT(mxIsCell(p));
	int m = mxGetM(p);
	int n = mxGetN(p);
	int size = m * n;

	for (int i = 0; i < size; i++)
	{
		const mxArray* pi = mxGetCell(p, i);

		int type;
		mexConvert(mxGetCell(pi, 0), type);

		const mxArray* ptmp;
		switch (type)
		{
		case 0: // topk
			ptmp = mxGetCell(pi, 1);
			EXIT_ASSERT(mxIsInt32(ptmp));
			mexConvert(ptmp, criteria_fixed.vec_fixedtopk);

			ptmp = mxGetCell(pi, 2);
			EXIT_ASSERT(mxIsInt32(ptmp));
			mexConvert(ptmp, criteria_fixed.matGND);
			break;

		case 2 : // average distance ratio
			ptmp = mxGetCell(pi, 1);
			EXIT_ASSERT(mxIsInt32(ptmp));
			mexConvert(ptmp, criteria_adr.matGND);
			break;

		case 4: // map
			ptmp = mxGetCell(pi, 1);
			EXIT_ASSERT(mxIsInt32(ptmp));
			mexConvert(ptmp, criteria_map.vec_fixedtopk);

			ptmp = mxGetCell(pi, 2);
			EXIT_ASSERT(mxIsInt32(ptmp));
			mexConvert(ptmp, criteria_map.matGND);
			break;

		default:
			EXIT_ASSERT(0);
		}

	}
}



void CreateDistanceInfo(const mxArray* p, 
						int &distance_type, 
						void* &p_distance_info)
{
	EXIT_ASSERT(mxIsCell(p));

	mexConvert(mxGetCell(p, 0), distance_type);

	switch (distance_type)
	{
	case DISTANCE_HAMMING: // hamming distance
		p_distance_info = NULL;
		break;

	case DISTANCE_DENSE: // dense
		{
			p_distance_info = new DenseDistanceInfo();
			DenseDistanceInfo* p_info = (DenseDistanceInfo*)p_distance_info;

			mexConvert(mxGetCell(p, 1), p_info->numPartitions);

			const mxArray* p2 = mxGetCell(p, 2);
			EXIT_ASSERT(mxIsDouble(p2));
			mexConvert(p2, p_info->matDense);
		}
		break;

	case DISTANCE_QUERY:
		{
			p_distance_info = new QueryDistanceInfo();
			QueryDistanceInfo* p_info = (QueryDistanceInfo*)p_distance_info;

			mexConvert(mxGetCell(p, 1), p_info->numPartitions);

			const mxArray* p2 = mxGetCell(p, 2);
			EXIT_ASSERT(mxIsDouble(p2));
			mexConvert(p2, p_info->matAux);

			p2 = mxGetCell(p, 3);
			EXIT_ASSERT(mxIsCell(p2));
			mexConvert(p2, p_info->vecmatCenters);

			p2 = mxGetCell(p, 4);
			EXIT_ASSERT(mxIsCell(p2));
			mexConvert(p2, p_info->vecvecErrors);
		}
		break;


	default:
		EXIT_ASSERT(0);
	}
}

void OutputParameter(
	mxArray* &pout, 
	CriteriaFixedTopKInfo &criteria_fixed, 
	CriteriaAverageDistanceRatio &criteria_adr,
	CriteriaMAP &criteria_map, 
	int num_candidate)
{
	const char* field[] = 
	{"fixed_topk", 
	"varied_topk", 
	"average_distance_ratio", 
	"semantic", 
	"map", 
	"average_query_time"};

	pout = mxCreateStructMatrix(1, 1, 6, field);

	if (criteria_fixed.matGND.Ptr())
	{
		for (int i = 0; i < criteria_fixed.vec_fixedtopk.Size();
			i++)
		{
			EXIT_ASSERT(criteria_fixed.vec_fixedtopk[i] > 0);
		}

		mxArray* parray =  mxCreateDoubleMatrix(
			num_candidate, 
			criteria_fixed.vec_fixedtopk.Size(), 
			mxREAL);

		mxSetFieldByNumber(pout, 0, 0, parray);

		mexConvert(parray, criteria_fixed.mat_output);
	}



	if (criteria_adr.matGND.Ptr())
	{
		criteria_adr.num_valid = min(criteria_adr.matGND.Cols(), 
			num_candidate);

		mxArray* parray = mxCreateDoubleMatrix(
			criteria_adr.num_valid, 1, 
			mxREAL);

		mxSetFieldByNumber(pout, 0, 2, parray);

		mexConvert(parray, criteria_adr.vec_output);
	}

	if (criteria_map.vec_fixedtopk.Ptr())
	{
		mxArray* parray =  mxCreateDoubleMatrix(
			criteria_map.vec_fixedtopk.Size(), 
			1, 
			mxREAL);

		mxSetFieldByNumber(pout, 0, 4, parray);

		mexConvert(parray, criteria_map.vec_output);
	}


}

void DataConvert(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[],
				 DataCollection &matQuery, SMatrix<uchar> &mat_data_base, 
				 int &num_candidate, 
				 SMatrix<double> &raw_query, DataCollection &raw_data_base,
				 int &distance_type, void* &p_distance_info,
				 CriteriaFixedTopKInfo &criteria_fixed,
				 CriteriaMAP &criteria_map, 
				 CriteriaAverageDistanceRatio &criteria_adr)
{
	//string str_dir = "\\\\dragon\\Data\\Jianfeng\\Data_HashCode\\Labelme\\working_opt_distance\\debug\\";

	int num_query;

	{ // query, used for querying, not evalualtion.
		if (mxIsUint8(MAT_QUERY))
		{
			mexConvert(MAT_QUERY, matQuery.mat_uint8);
			num_query = matQuery.mat_uint8.Rows();
		}
		else if (mxIsDouble(MAT_QUERY))
		{
			mexConvert(MAT_QUERY, matQuery.mat_double);
			num_query = matQuery.mat_double.Rows();
		}
		else
		{
			EXIT_ASSERT(0);
		}
	}

	{	// data base code
		EXIT_ASSERT(mxIsUint8(MAT_DATA_BASE));
		mexConvert(MAT_DATA_BASE, mat_data_base);
	}

	int num_database_point = mat_data_base.Rows();

	{
		// candatea number
		mexConvert(NUM_CANDIDATE, num_candidate);
		EXIT_ASSERT(num_candidate > 0 
			&& num_candidate <= num_database_point);
	}
	// raw query feature
	{
		EXIT_ASSERT(mxIsDouble(MAT_RAW_QUERY));
		mexConvert(MAT_RAW_QUERY, raw_query);

		EXIT_ASSERT(raw_query.Rows() == num_query);
	}

	int dim = raw_query.Cols();

	int num_raw_feature;
	int dim_raw_feature;
	{// raw data base feature
		if (mxIsDouble(MAT_RAW_DATABASE))
		{
			mexConvert(MAT_RAW_DATABASE, raw_data_base.mat_double);
			num_raw_feature = raw_data_base.mat_double.Rows();
			dim_raw_feature = raw_data_base.mat_double.Cols();
		}
		else if (mxIsSingle(MAT_RAW_DATABASE))
		{
			mexConvert(MAT_RAW_DATABASE, raw_data_base.mat_float);
			num_raw_feature = raw_data_base.mat_float.Rows();
			dim_raw_feature = raw_data_base.mat_float.Cols();
		}
		else if (mxIsUint8(MAT_RAW_DATABASE))
		{
			mexConvert(MAT_RAW_DATABASE, raw_data_base.mat_uint8);
			num_raw_feature = raw_data_base.mat_uint8.Rows();
			dim_raw_feature = raw_data_base.mat_uint8.Cols();
		}
		else
		{
			EXIT_ASSERT(0);
		}
	}
	//matRawDataBase.LoadData(str_dir + "Xbase");
	EXIT_ASSERT(num_raw_feature == num_database_point);
	EXIT_ASSERT(dim == dim_raw_feature);

	CreateDistanceInfo(DISTANCE_INFO, distance_type, p_distance_info);

	GetCriteriaInfo(CRITERIA_INFO, 
		criteria_fixed, 
		criteria_map,
		criteria_adr);

	EXIT_ASSERT(criteria_fixed.matGND.Rows() == num_query ||
		criteria_fixed.matGND.Rows() == 0 ||
		criteria_adr.matGND.Rows() == num_query ||
		criteria_adr.matGND.Rows() == 0);

	//mxArray* presult;
	OutputParameter(
		RESULT, 
		criteria_fixed, 
		criteria_adr,
		criteria_map,
		num_candidate);

}

// result = mexTrueNeighbors(matquery, mat_data_base_code,
// matRawQuery, matRawDatabase, 
// num_candidate, dist_conf, criteria)
void mexFunction(int nlhs, mxArray *plhs[], 
				 int nrhs, const mxArray *prhs[])
{
	try
	{
		EXIT_ASSERT(nrhs == 7);
		EXIT_ASSERT(nlhs == 1);

		DataCollection matQuery;
		SMatrix<uchar> mat_data_base;
		int num_candidate; 
		SMatrix<double> matRawQuery; 
		DataCollection data_base;
		int distance_type; 
		void* p_distance_info;
		CriteriaFixedTopKInfo criteria_fixed;
		CriteriaMAP criteria_map;
		CriteriaAverageDistanceRatio criteria_adr;

		DataConvert(nlhs, plhs, nrhs, prhs, 
			matQuery, mat_data_base, 
			num_candidate, 
			matRawQuery, data_base,
			distance_type, p_distance_info,
			criteria_fixed,
			criteria_map, 
			criteria_adr);

		RunPrecision(matQuery, mat_data_base, 
			num_candidate, 
			matRawQuery, data_base, 
			distance_type, p_distance_info,
			criteria_fixed, 
			criteria_map, 
			criteria_adr);

		ReleaseDistanceInfo(p_distance_info);
	}
	catch (...)
	{
	}
} 

#endif
