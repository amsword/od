#pragma once
#include <string>
#include "AssertMacro.h"
#include "Vector.h"
using namespace std;

//matrix is not large and the data are stored continiously.
template < class T >
class SMatrix
{
public:
	SMatrix()
		:m_pData(NULL),
		m_isOwnData(false)
	{
	}
	~SMatrix()
	{
		UnLoadData();
	}

	SMatrix(T* p, int rows, int cols)
		:m_pData(p),
		m_isOwnData(false)
	{
		m_nRows = rows;
		m_nCols = cols;
	}
	SMatrix(const SMatrix<T> &from)
		: 
		m_pData(NULL),
		m_isOwnData(false)
	{
		from.CopyTo(*this);
	}

	SMatrix(int rows, int cols)
		: 
		m_pData(NULL),
		m_isOwnData(false)
	{
		AllocateSpace(rows, cols);
	}

public:
	int Rows() const
	{
		return m_nRows;
	}
	int Cols() const
	{
		return m_nCols;
	}
	void AllocateSpace(int m, int n)
	{
		if (m_isOwnData)
		{
			if (m != m_nRows || n != m_nCols)
			{
				release_vector_space(m_pData);
				m_nRows = m;
				m_nCols = n;
				allocate_vector_space(m_pData, (long long)m * (long long)n);
				m_isOwnData = true;
			}
		}
		else
		{
			m_nRows = m;
			m_nCols = n;
			allocate_vector_space(m_pData, (long long)m * (long long)n);
			m_isOwnData = true;
		}
	}

	void Attach(T* p, int rows, int cols)
	{
		UnLoadData();

		m_pData = p;
		m_isOwnData = false;
		m_nRows = rows;
		m_nCols = cols;
	}

	void Detatch(T** &p, int &m, int &n)
	{
		EXIT_ASSERT(0);
		p = m_pData;
		m = m_nRows;
		n = m_nCols;

		m_pData = NULL;
		m_nRows = 0;
		m_nCols = 0;
		m_isOwnData = false;
	}

	void LoadData(const string &file_name)
	{
		FILE* fp = fopen(file_name.c_str(), "rb");
		EXIT_ASSERT(fp);

		BinaryRead(fp);

		fclose(fp);
	}

	void LoadData(const string &file_name, int start, int end)
	{
		FILE* fp = fopen(file_name.c_str(), "rb");
		EXIT_ASSERT(fp);

		BinaryRead(fp, start, end);

		fclose(fp);
	}

	void SaveData(const string &file_name) const
	{
		EXIT_ASSERT(m_nRows > 0 && m_nCols > 0);

		FILE* fp = fopen(file_name.c_str(), "wb");
		EXIT_ASSERT(fp);

		BinaryWrite(fp);

		fclose(fp);
	}

	void BinaryWrite(FILE* fp) const
	{
		fwrite(&m_nRows, sizeof(int), 1, fp);
		fwrite(&m_nCols, sizeof(int), 1, fp);

		fwrite(m_pData, sizeof(T), m_nRows * m_nCols, fp);
	}

	void BinaryRead(FILE* fp)
	{
		int m;
		int n;
		fread(&m, sizeof(int), 1, fp);
		fread(&n, sizeof(int), 1, fp);

		AllocateSpace(m, n);

		fread(m_pData, sizeof(T), n * m, fp);
	}

	void UnLoadData()
	{
		if (m_isOwnData)
		{
			release_vector_space(m_pData);
			m_isOwnData = false;
		}
		m_pData = NULL;
		m_nRows = 0;
		m_nCols = 0;
	}

public:
	void Scale()	
	{
		T s = NormL1();
		s /= m_nRows * m_nCols;
		s = 1.0 / s;

		T* p = m_pData;
		for (int i = 0; i < m_nRows * m_nCols; i++)
		{
			(*p) *= s;
			p++;
		}
	}
public:
	template <class T2>
	void Convert(SMatrix<T2> &mat2)
	{
		mat2.AllocateSpace(m_nRows, m_nCols);
		for (int i = 0; i < m_nRows * m_nCols; i++)
		{
			mat2.m_pData[i] = (T2)m_pData.m_pData[i];
		}
	}

public:
	bool IsEmpty() const
	{
		return m_pData == NULL;
	}
public:
	T Norm(int L) const
	{
		switch(L)
		{
		case 1:
			return NormL1();
		default:
			EXIT_ASSERT(0);
		}
	}

	void NormalizeEveryRowL1()
	{
		for (int i = 0; i < m_nRows; i++)
		{
			T* p = operator [](i);
			T v = L1_norm(p, m_nCols);
			v = v / m_nCols;
			scale_multi_vector(1.0 / v, p, p, m_nCols);
		}
	}

public:
	void CopyTo(SMatrix<T> &to) const
	{
		to.AllocateSpace(m_nRows, m_nCols);
		memcpy(to.m_pData, m_pData, sizeof(T) * m_nCols * m_nRows);
		to.m_isOwnData = true;
	}

	void SetValueZeros()
	{
		memset(m_pData, 0, sizeof(T) * m_nRows * m_nCols); 
	}

	void SetValue(T a)
	{
		for (int i = 0; i < m_nRows * m_nCols; i++)
		{
			m_pData[i] = a;
		}
	}

public: // operator
	SMatrix<T>& operator = (const SMatrix<T>& a)
	{
		a.CopyTo(*this);
		return (*this);
	}
	SMatrix<T>& operator *= (T v)
	{
		for (int i = 0; i < m_nRows * m_nCols; i++)
		{
			m_pData[i] *= v;
		}
		return *this;
	}

	T* operator[](int i)
	{
		return m_pData + (long long)i * (long long)m_nCols;
	}

	T* operator[](int i) const
	{
		return m_pData + (long long)i * (long long)m_nCols;
	}

	T* operator[](long long i) const
	{
		return m_pData + i * m_nCols;
	}

	SMatrix<T> operator *(T s)
	{
		SMatrix<T> mat(m_nRows, m_nCols);
		T* p_result = mat.Ptr();
		T* p_source = m_pData;

		for (int i = 0; i < m_nRows * m_nCols; i++)
		{
			(*p_result++) = (*p_source++) * s;
		}
		return mat;
	}

	SMatrix<T>& operator += (const SMatrix<T>& mat)
	{
		for (int i = 0; i < m_nRows * m_nCols; i++)
		{
			m_pData[i] += mat.m_pData[i];
		}
		return *this;
	}

	SMatrix<T>& operator -= (const SMatrix<T>& mat)
	{
		T* p_mat = mat.m_pData;
		T* p = m_pData;
		for (int i = 0; i < m_nRows * m_nCols; i++)
		{
			(*p) -= (*p_mat++);
			p++;
		}
		return *this;
	}

public:

	T MaxMax() const
	{
		T max_max = -10000;
		for (long long i = 0; i < m_nRows * m_nCols; i++)
		{
			if (m_pData[i] > max_max)
			{
				max_max = m_pData[i];
			}
		}
		return max_max;
	}

	void Divide(double d)
	{
		T* p = m_pData;
		for (int i = 0; i < m_nRows * m_nCols; i++)
		{
			(*p++) /= d;
		}
	}

	void Multiple(const T* p, T* out) const
	{
		for (int i = 0; i < m_nRows; i++)
		{
			(*out++) = dot(operator [](i), p, m_nCols);
		}
	}

	void MultipleN(const T* p, T* out) const
	{
		for (int i = 0; i < m_nRows; i++)
		{
			(*out++) = -dot(operator [](i), p, m_nCols);
		}
	}

	T* Ptr() const
	{
		return m_pData;
	}
private:
	T NormL1() const
	{
		T s = 0;
		s += L1_norm(m_pData, m_nCols * m_nRows);
		return s;
	}

private:
	T* m_pData;
	int m_nRows;
	int m_nCols;
	bool m_isOwnData;
};

template <class T>
ostream& operator << (ostream& out, const SMatrix<T> &v)
{
	out << "Matrix: \n";
	out << "Rows = " << v.Rows() << "\n"
		<< "Cols = " << v.Cols() << "\n";

	for (int i = 0; i < v.Rows(); i++)
	{
		for (int j = 0; j < v.Cols(); j++)
		{
			out << v[i][j] << "\t";
		}
		out << "\n";
	}
	out << "\n";
	return out;
}

