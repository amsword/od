#pragma once

#include <mex.h>

template <class T>
void mexConvert(const mxArray* input, SMatrix<T> & out)
{
	int m = mxGetM(input);
	int n = mxGetN(input);
	T* p = (T*)mxGetPr(input);
	out.Attach(p, n, m);	
}

template <class T>
void mexConvert(const mxArray* input, SMatrix<SMatrix<T> > & out)
{
	int m = mxGetM(input);
	int n = mxGetN(input);
	
	out.AllocateSpace(n, m);
	
	for (int j = 0; j < n; j++)
	{
		for (int i = 0; i < m; i++)
		{
			const mxArray* arr = mxGetCell(input, j * m + i);
			mexConvert(arr, out[j][i]);
		}
	}
}


// input is a cell
template <class T>
void mexConvert(const mxArray* input, Vector<SMatrix<T> > & out)
{
	int size = 0;
	int m = mxGetM(input);
	int n = mxGetN(input);
	
	size = m * n;

	out.AllocateSpace(size);
	for (int i = 0; i < size; i++)
	{
		const mxArray* parr = mxGetCell(input, i);
		mexConvert(parr, out[i]);
	}
}

template <class T>
void mexConvert(const mxArray* input, T & out)
{
	out = mxGetScalar(input);
}

template <class T>
void mexConvert(const mxArray* input, Vector<Vector<T> > &vecvec)
{
	int size = mxGetM(input) * mxGetN(input);
	
	vecvec.AllocateSpace(size);
	for (int i = 0; i < size; i++)
	{
		const mxArray* parr = mxGetCell(input, i);
		mexConvert(parr, vecvec[i]);
	}
}

template <class T>
void mexConvert(const mxArray* input, Vector<T> &vec)
{
	int num = mxGetM(input) * mxGetN(input);
	
	T* p = (T*)mxGetPr(input);
	vec.Attach(p, num);
}

//void mexConvert(const mxArray* input, string& str);

