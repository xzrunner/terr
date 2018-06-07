#pragma once

#include "terr/BinTri.h"

#include <assert.h>

namespace terr
{

struct BinTriPool
{
public:
	BinTriPool(int cap)
		: m_size(cap)
		, m_next(0)
	{
		m_tris = new BinTri[cap];
	}
	~BinTriPool()
	{
		delete[] m_tris;
	}

	BinTri* Alloc() {
		assert(m_next < m_size);
		return &m_tris[m_next++];
	}

	int Size() const { return m_size; }
	int Next() const { return m_next; }

	void Reset() { m_next = 0; }

private:
	BinTri* m_tris;
	int m_size, m_next;

}; // BinTriPool

}