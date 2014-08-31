#pragma once
#include <string>
#include <iostream>
#include "AssertMacro.h"


template < class T >
void allocate_vector_space(T* &p_data, long long num)
{
	p_data = new T[num];
	EXIT_ASSERT(p_data);
}

template < class T >
void release_vector_space(T* &p_data)
{
	if (p_data)
	{
		delete[] p_data;
		p_data = NULL;
	}
}

template <class T>
void allocate_mat_space(T** &pp_data, int rows, int cols)
{
	pp_data = new T*[rows];
	EXIT_ASSERT(pp_data);

	for (int i = 0; i < rows; i++)
	{
		pp_data[i] = new T[cols];
		EXIT_ASSERT(pp_data[i]);
	}
}

template < class T >
void release_mat_space(T** &pp_data, int rows)
{
	if (pp_data)
	{
		for (int i = 0; i < rows; i++)
		{
			if (pp_data[i])
			{
				delete[] pp_data[i];
				pp_data[i] = 0;
			}
		}
		delete[] pp_data;
		pp_data = 0;
	}
}

using namespace std;


template <class T>
class Vector
{
public:
	Vector():
		m_pData(NULL),
		m_nCount(0),
		m_isOwnData(false)
	{

	}

	Vector(int length):
		m_pData(NULL),
		m_nCount(0),
		m_isOwnData(false)
	{

		AllocateSpace(length);
	}

	Vector(const Vector<T> &vec) :
		m_pData(NULL),
		m_nCount(0),
		m_isOwnData(false)
	{
		vec.CopyTo(*this);
	}

	Vector(T* p, int length) : m_pData (NULL), m_nCount(0), m_isOwnData(false)
	{
		Attach(p, length);
	}

	~Vector()
	{
		UnloadData();
	}

public:

	void UnloadData()
	{
		if (m_isOwnData)
		{
			release_vector_space(m_pData);
		}
		m_isOwnData = false;
		m_pData = NULL;
		m_nCount = 0;
	}

	void Attach(T* p, int length)
	{
		this->UnloadData();
		m_pData = p;
		m_nCount = length;
		m_isOwnData = false;
	}

	void Detach(T* &p, int &count)
	{
		p = m_pData;
		count = m_nCount;

		m_pData = NULL;
		m_nCount = 0;

		m_isOwnData = false;
	}
	template <class T1>
	friend ostream& operator << (ostream &os, const Vector<T1>& vec);

	template <class T1>
	friend istream& operator >> (istream &is, Vector<T1>& vec);

	void BinaryWrite(FILE* fp) const
	{
		int tmp = 1;
		fwrite(&tmp, sizeof(int), 1, fp);
		fwrite(&m_nCount, sizeof(int), 1, fp);
		fwrite(m_pData, sizeof(T), m_nCount, fp);
	}

	void BinaryRead(FILE* fp)
	{
		int m, n;
		fread(&m, sizeof(int), 1, fp);
		fread(&n, sizeof(int), 1, fp);
		EXIT_ASSERT(m == 1 || n == 1);
		m = m == 1 ? n : m;
		AllocateSpace(m);
		fread(m_pData, sizeof(T), m_nCount, fp);
	}

	void BinaryRead(istream& is)
	{
		int m, n;
		is.read((char*)(&m), sizeof(int));
		is.read((char*)(&n), sizeof(int));
		EXIT_ASSERT(m == 1 || n == 1);
		m = m == 1 ? n : m;
		AllocateSpace(m);
		is.read((char*)m_pData, sizeof(T) * m_nCount);
	}
	void BinaryWrite(ostream& os) const
	{
		int tmp = 1;
		os.write((char*)&tmp, sizeof(int));
		os.write((char*)&m_nCount, sizeof(int));
		os.write((char*)m_pData, sizeof(T) * m_nCount);
	}

	void SaveData(const string &file_name) const
	{
		FILE* fp = fopen(file_name.c_str(), "wb");
		EXIT_ASSERT(fp);

		BinaryWrite(fp);
		fclose(fp);
	}

	void LoadData(const string &file_name)
	{
		FILE* fp = fopen(file_name.c_str(), "rb");
		EXIT_ASSERT(fp);
		BinaryRead(fp);
		fclose(fp);
	}
public:
	//value must be initialized to be the smallest value
	void MaxValue(T& value, int &idx) const
	{
		for (int i = 0; i < m_nCount; i++)
		{
			if (m_pData[i] > value)
			{
				value = m_pData[i];
				idx = i;
			}
		}
	}


	void Cumulate()
	{
		for (int i = 1; i < m_nCount; i++)
		{
			m_pData[i] += m_pData[i - 1];
		}
	}
	T Distance(const Vector<T>& vec, int normtype) const
	{
		switch(normtype)
		{
		case 1:
			return DistanceL1(vec);
		case 2:
			return DistanceL2(vec);
		default:
			EXIT_ASSERT(0);
		}
	}
	T SumUp() const
	{
		T s = 0;
		for (int i = 0; i < m_nCount; i++)
		{
			s += m_pData[i];
		}
		return s;
	}

	void VarMean(T& mean, double& var) const
	{
		T total = 0;
		T total2 = 0;
		for (int i = 0; i < m_nCount; i++)
		{
			total += m_pData[i];
			total2 += m_pData[i] * m_pData[i];
		}
		total /= m_nCount;
		total2 /= m_nCount;
		mean = total;

		var = total2 - total * total;
	}

public:
	void AllocateSpace(int length)
	{
		if (m_nCount != length)
		{
			release_vector_space(m_pData);
			allocate_vector_space(m_pData, length);
			m_nCount = length;
			m_isOwnData = true;
		}
	}

	template <class T2>
	void CopyTo(Vector<T2> &target) const
	{
		target.AllocateSpace(m_nCount);

		for (int i = 0; i < m_nCount; i++)
		{
			target.m_pData[i] = (T2)m_pData[i];
		}
	}


	T* begin() const
	{
		return m_pData;
	}
	T* end() const
	{
		return m_pData + m_nCount;
	}

	T Norm1()
	{
		T s = 0;
		for (int i = 0; i < m_nCount; i++)
		{
			T v = m_pData[i];
			s += v > 0 ? v : -v;
		}
		return s;
	}
	T Norm2Squared()
	{
		return squared_L2_norm(m_pData, m_nCount);
	}


	void SetValue(const T& t)
	{
		for (int i = 0; i < m_nCount; i++)
		{
			m_pData[i] = t;
		}
	}
	void SetValueZeros()
	{
		EXIT_ASSERT(m_pData);
		memset(m_pData, 0, sizeof(T) * m_nCount);
	}

	void resize(int length)
	{
		AllocateSpace(length);
	}
public:
	const T& operator [](int i) const
	{
		return m_pData[i];
	}
	T& operator [](int i)
	{
		return m_pData[i];
	}
	Vector<T>& operator = (const Vector<T> &vec)
	{
		//m_pData = vec.m_pData;
		//m_nCount = vec.m_nCount;
		//m_isOwnData = false;
		vec.CopyTo(*this);
		return *this;
	}
	Vector<T> & operator /= (T t)
	{
		for (int i = 0; i < m_nCount; i++)
		{
			m_pData[i] /= t;
		}

		return *this;
	}
	Vector<T> & operator += (const Vector<T>& vec)
	{
		for (int i = 0; i < m_nCount; i++)
		{
			m_pData[i] += vec[i];
		}
		return *this;
	}

	Vector<T> & operator += (T* vec)
	{
		for (int i = 0; i < m_nCount; i++)
		{
			m_pData[i] += vec[i];
		}
		return *this;
	}

	Vector<T> & operator -= (const T* p2)
	{
		for (int i = 0; i < m_nCount; i++)
		{
			m_pData[i] -= p2[i];
		}
		return *this;
	}


	Vector<T> & operator *= (T v)
	{
		for (int i = 0; i < m_nCount; i++)
		{
			m_pData[i] *= v;
		}
		return *this;
	}

	T* Ptr() const
	{
		return m_pData;
	}

	void DivideByCount()
	{
		for (int i = 0; i < m_nCount; i++)
		{
			m_pData[i] /= (i + 1);
		}
	}

	double Mean()
	{
		double s = 0;
		for (int i = 0; i < m_nCount; i++)
		{
			s += m_pData[i];
		}
		return s / m_nCount;
	}

	int Size() const
	{
		return m_nCount;
	}
private:
	T DistanceL1(const Vector<T>& vec) const
	{
		T s = 0;
		for (int i = 0; i < m_nCount; i++)
		{
			T diff = m_pData[i] - vec.m_pData[i];
			if (diff < 0) diff = -diff;
			s += diff;
		}
		return s;
	}

	T DistanceL2(const Vector<T> &vec) const
	{
		T s = 0;
		for (int i = 0; i < m_nCount; i++)
		{
			T diff = m_pData[i] - vec.m_pData[i];
			s += diff * diff;
		}
		return s;
	}

private:
	T* m_pData;
	int m_nCount;
	bool m_isOwnData;
};

template <class T>
ostream& operator << (ostream& out, const Vector<T> &v)
{
	out << "Vector: \n";
	out << "length = " << v.m_nCount << "\n";
	for (int i = 0; i < v.Size(); i++)
	{
		out << v[i] << "\t";
	}
	out << "\n";
	return out;
}

template <class T>
ostream& operator << (ostream& out, const Vector<pair<int, T> > &v)
{
	out << "Vector: \n";
	out << "length = " << v.m_nCount << "\n";
	for (int i = 0; i < v.Size(); i++)
	{
		out << "(" << v[i].first << ", " << v[i].second << ")\t";
	}
	out << "\n";
	return out;
}

