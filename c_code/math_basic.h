#pragma once


template < class T >
T squared_distance_th(const T* p1, const T* p2, int num, T th)
{
	T r = 0;
	for (int i = 0; i < num; i++)
	{
		T diff = (*p1++) - (*p2++);
		r += diff * diff;

		if (r > th)
		{
			return r;
		}
	}
	return r;
}

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

template < class T >
T dot(const T* p1, const T* p2, int num)
{
	T s = 0; 
	for (int i = 0; i < num; i++)
	{
		s += (*p1++) * (*p2++);
	}
	return s;
}

template < class T >
T L1_distance(T* p1, T* p2, int num)
{
	T r = 0;
	for (int i = 0; i < num; i++)
	{
		T diff = (*p1++) - (*p2++);
		r += diff >= 0? diff : -diff;
	}
	return r;
}

template < class T >
T Hamming_distance(T* p1, T* p2, int num)
{
	T r = 0;
	for (int i = 0; i < num; i++)
	{
		r += (*p1++) == (*p2++) ? 0 : 1;
	}
	return r;
}

template <class T>
T squared_L2_norm(const T* p, int num)
{
	T r = 0;

	for (int i = 0; i < num; i++)
	{
		r += (*p) * (*p);
		p++;
	}
	return r;
}

template < class T >
T L1_norm(const T* p, int num)
{
	T s = 0;
	for (int i = 0; i < num; i++)
	{
		s += p[i] > 0 ? p[i] : -p[i];
	}

	return s;
}

template < class T >
void scale_multi_vector(T scale, const T* p, T* p_result, int num)
{
	for (int i = 0; i < num; i++)
	{
		(*p_result++) = (*p++) * scale;
	}
}

template < class T >
void linear_combine(T s1, T* p1, T s2, T* p2, T* p_result, int num)
{
	for (int i = 0; i < num; i++)
	{
		(*p_result++) = s1 * (*p1++) + s2 * (*p2++);
	}
}

template < class T >
void linear_combineMP(T s1, T* p1, T s2, T* p2, T* p_result, int num)
{
#pragma omp parallel for
	for (int i = 0; i < num; i++)
	{
		p_result[i] = s1 * p1[i] + s2 * p2[i];
	}
}

template < class T >
void y_plus_ax_to_y(T a, T* x, T* y, int num)
{
	for (int i = 0; i < num; i++)
	{
		y[i] += a * x[i];
	}
}

template < class T >
void VectorAdd(T* y, const T* x, int num)
{
	for (int i = 0; i < num; i++)
	{
		y[i] += x[i];
	}
}

template < class T >
void VectorMinus(T* y, const T* x, int num)
{
	for (int i = 0; i < num; i++)
	{
		y[i] -= x[i];
	}
}

