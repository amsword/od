#pragma once

#include <algorithm>


template <class T>
class PAIR
{
public:
	int index;
	T distance;
	friend bool operator < (const PAIR<T> &n1, const PAIR<T> &n2)
	{
		if (n1.distance == n2.distance) return n1.index > n2.index;
		return n1.distance > n2.distance;
	}
	friend ostream& operator << (ostream &os, const PAIR<T> &p)
	{
		os << p.index << "\t" << p.distance << "\t";
		return os;
	}

	PAIR() {}
	PAIR(int idx, T dist) {index = idx; distance = dist;}
};


/**
 * Priority Queue Implementation
 *
 * The priority queue is implemented with a heap.  A heap is a complete
 * (full) binary tree in which each parent is less than both of its
 * children, but the order of the children is unspecified.
 * Note that a heap uses 1-based indexing to allow for power-of-2
 * location of parents and children.  We ignore element 0 of Heap array.
 */
template < typename T >
class Heap {

public:
	/**
	* Storage array for the heap.
	* Type T must be comparable.
	*/
	T* heap;
    int length;

	/**
	 * Number of element in the heap
	 */
	int count;

public:
	void Load(const string &file_name)
	{
		release();

		FILE* fp = fopen(file_name.c_str(), "rb");
		EXIT_ASSERT(fp);
		
		fread(&count, sizeof(int), 1, fp);
		fread(&length, sizeof(int), 1, fp);

		if (length > 1)
		{
			Reserve(length - 1);
			fread(heap, sizeof(T), length, fp);
		}
		fclose(fp);
	}

public:
	Heap()
	{
		count = 0;
		heap = NULL;
		length = 0;
	}

	/**
	 * Constructor.
	 *
	 * Params:
	 *     size = heap size
	 */

	Heap(int size)
	{
        length = size+1;
		heap = new T[length];  // heap uses 1-based indexing
		count = 0;
	}

	void Reserve(int size)
	{
		if (length != size + 1)
		{
			release();
			length = size+1;
			heap = new T[length];  // heap uses 1-based indexing
		}
		count = 0;
	}

	/**
	 * Destructor.
	 *
	 */
	~Heap()
	{
		release();
	}

	void release()
	{
		if (heap != NULL)
		{
			delete[] heap;
			heap = 0;
			count = 0;
			length = 0;
		}
	}

	/**
	 *
	 * Returns: heap size
	 */
	int size()
	{
		return count;
	}

	/**
	 * Tests if the heap is empty
	 *
	 * Returns: true is heap empty, false otherwise
	 */
	bool empty()
	{
		return size()==0;
	}

	/**
	 * Clears the heap.
	 */
	void clear()
	{
		count = 0;
	}


	/**
	 * Insert a new element in the heap.
	 *
	 * We select the next empty leaf node, and then keep moving any larger
	 * parents down until the right location is found to store this element.
	 *
	 * Params:
	 *     value = the new element to be inserted in the heap
	 */
	void insert(T value)
	{
		/* If heap is full, then return without adding this element. */
		if (count == length-1) 
		{
			return;
		}

		int loc = ++(count);   /* Remember 1-based indexing. */

		/* Keep moving parents down until a place is found for this node. */
		int par = loc / 2;                 /* Location of parent. */
		while (par > 0  && value < heap[par]) {
			heap[loc] = heap[par];     /* Move parent down to loc. */
			loc = par;
			par = loc / 2;
		}
		/* Insert the element at the determined location. */
		heap[loc] = value;
	}

	bool popMin()
	{
		if (count == 0) 
		{
			return false;
		}

		/* Switch first node with last. */
		swap(heap[1],heap[count]);

		count -= 1;
		heapify(1);      /* Move new node 1 to right position. */

		return true;  /* Return old last node. */
	}


	/**
 	 * Returns the node of minimum value from the heap (top of the heap).
	 *
	 * Params:
	 *     value = out parameter used to return the min element
	 * Returns: false if heap empty
	 */
	bool popMin(T& value)
	{
		if (count == 0) {
 			return false;
		}

		/* Switch first node with last. */
		swap(heap[1],heap[count]);

		count -= 1;
		heapify(1);      /* Move new node 1 to right position. */

		value = heap[count + 1];
		return true;  /* Return old last node. */
	}

	/**
 	 * ????
	 */
	const T& Top()
	{
		// just for ...
        return heap[1];
	}

	/**
	 * Reorganizes the heap (a parent is smaller than its children)
	 * starting with a node.
	 *
	 * Params:
	 *     parent = node form which to start heap reorganization.
	 */
	void heapify(int parent)
	{
		int minloc = parent;

		/* Check the left child */
		int left = 2 * parent;
		if (left <= count && heap[left] < heap[parent]) {
			minloc = left;
		}

		/* Check the right child */
		int right = left + 1;
		if (right <= count && heap[right] < heap[minloc]) {
			minloc = right;
		}

		/* If a child was smaller, than swap parent with it and Heapify. */
		if (minloc != parent) {
			swap(heap[parent],heap[minloc]);
			heapify(minloc);
		}
	}

};

template <class T>
bool IsExist(int p, Heap<PAIR<T> >& h)
{
	bool r = false;
	for (int i = 1; i < h.count; i++)
	{
		if (p == h.heap[i].index)
		{
			r = true;
			break;
		}
	}
	return r;
}
