/**
 * @file bigmap2.hpp
 * @brief An Optimized HashMap.
 * @author linus
 * @version 1.0
 * @date 2013-10-03
 */
#pragma once

#include <cstdio>
#include <cstdint>
#include <cstring>
#include <cmath>
#include "headers/error.hpp"

/**
 * Power 2 Ceiling.
 *
 * Return 2**N such that 2**(N-1) < x <= 2**N, for 2<=x<2**31.
 * Return 1 for x = 0,1.
 */
inline uint32_t power2ceil(uint32_t x)
{
	qassert(x<=(~0u>>1));
	uint32_t rtn = 1;
	while(rtn!=0 and rtn<x)
		rtn <<= 1;
	return rtn;
}

/**
 * A unit of (key,value) in BigMap
 *
 * Note that T should be basic types, need not T() or ~T(),
 * and can be initialized by bzero or equivalence.
 */
template <unsigned N, typename T>
class Atom
{ 
public:
	uint64_t k; ///< Key
	T v[N]; ///< Value

	/**
	 * Default constructor.
	 *
	 * if True, then memset 0.
	 */
	Atom(bool set_zero): k(0) { if(set_zero) memset(v,0,N*sizeof(T)); }
};

/**
 * A HashMap like structure.
 *
 * This is usually known as Open Addressing. See [CLRS] 11.4
 */
template <unsigned N, typename T>
class BigMap
{
private:
	uint32_t max_len; ///< max number of Atom. Should be in the power of 2
	uint32_t len; ///< number of Atom stored. the count of 0b11
	uint32_t mask; ///< mask.
	uint32_t vacancy; ///< number of Atom corpse. the count of 0b01

	typedef Atom<N,T> ATOM;
	ATOM* array;

public:

	BigMap(): max_len(0),len(0),mask(0),vacancy(0),array(NULL) {}
	
	BigMap(uint32_t _max_len): max_len(0),len(0),mask(0),vacancy(0),array(NULL)
	{
		rehash(_max_len);
	}

	~BigMap() {dtor();}

	void dtor()
	{
		if(array==NULL)
			return;
		free(array);
		array = NULL;
	}

	/**
	 * Print (for debug)
	 */
	inline void print() const
	{
		printf("max_len %u, len %u, vacancy %u, array %p\n",
				max_len,len,vacancy,array);
	}

private:
	bool _find(uint64_t k, uint32_t & index_found, uint32_t & index_new) const
	// Find key k, return whether found
	// when found, save index in index_found.
	// when no-found, save insert index in index_new.
	{
		uint32_t count_miss = 0;
		index_found = ~0u;
		index_new = ~0u;
		uint32_t index = k & mask; // which bucket to look for
		while(true)
		{
			switch((array[index].k>>62)&3)
			{
				case 0: // 0b00 : nothing
					if(index_new==~0u)
						index_new = index;
					return false;
				case 1: // 0b01 : corpse
					if(index_new==~0u)
						index_new = index;
					if(((array[index].k^k)&~(3lu<<62))==0)
						return false;
					break;
				case 3: // 0b11 : living
					if(((array[index].k^k)&~(3lu<<62))==0)
					{
						index_found = index;
						return true;
					}
					break;
				default:
					error("Corrupted Key Head.\n");
			}
			count_miss ++;
			if(count_miss==max_len)
				return false;
			index = (index+count_miss)&mask;
		}
	}

	bool _find(uint64_t k, uint32_t & index_found) const
	// Find key k, return whether found
	// when found, save index in index_found.
	// when no-found, save ~0u in index_found.
	{
		uint32_t count_miss = 0;
		index_found = ~0u;
		uint32_t index = k & mask; // which bucket to look for
		while(true)
		{
			switch((array[index].k>>62)&3)
			{
				case 0: // 0b00
					return false;
				case 1: // 0b01
					if(((array[index].k^k)&~(3lu<<62))==0)
						return false;
					break;
				case 3: // 0b11
					if(((array[index].k^k)&~(3lu<<62))==0)
					{
						index_found = index;
						return true;
					}
					break;
				default:
					error("Corrupted Key Head.\n");
			}
			count_miss ++;
			if(count_miss==max_len)
				return false;
			index = (index+count_miss)&mask;
		}
	}

public:
	/**
	 * Return how many Atom are stored.
	 */
	uint32_t size() const { return len; }
	/**
	 * Return max number of Atom could be stored.
	 */
	uint32_t max_size() const { return max_len; }
	
	/**
	 * Return number of dead Atom 's body.
	 */
	uint32_t vacancies() const { return vacancy; }

	/**
	 * Return next Atom after t.
	 *
	 * Note that you should not traverse the map while deleting Atom from it.
	 */	
	ATOM* next(ATOM* t) const 
	{ 
		do t++; while(t<array+max_len and t->k<(1lu<<63));
		return t;
	}

	/**
	 * Return the pointer to the first Atom.
	 */
	ATOM* begin() const { return next(array-1); }
	/**
	 * Return the pointer to the last Atom.
	 */
	ATOM* end() const { return array+max_len; }

	/**
	 * Return the reference of Atom of key k or of a blank Atom() if not found.
	 */
	const ATOM& get(uint64_t k) const
	{
		static const ATOM null_atom(true);
		uint32_t index_found;
		bool found = _find(k,index_found);
		if(found)
			return array[index_found];
		else
			return null_atom;
	}

	/**
	 * Find Atom* of key k or return end() if no-found.
	 */
	ATOM* find(uint64_t k) const
	{
		uint32_t index_found;
		bool found = _find(k,index_found);
		if(found)
			return array+index_found;
		else
			return array+max_len;
	}

	/**
	 * Get the Atom of key k, Create it if not found.
	 */
	ATOM& operator[](uint64_t k)
	{
		uint32_t index_found, index_new;
		bool found = _find(k,index_found,index_new);
		if(found)
			return array[index_found];
		else
		{
			// We tries to remember everything
			// so after a long run with many keys 
			// it may take a lot of memory
			if(2*(len+vacancy) > max_len) // overfull
			{
				if(4*len > max_len)
					rehash(((max_len<1<<24)?4:2)*max_len); // expand
				else
					rehash(max_len);
				return (*this)[k];
			}
			if(array[index_new].k >> 62 == 1)
				vacancy--;
			array[index_new].k = k | (3lu<<62);
			len++;
			return array[index_new];
		}
	}

	/**
	 * Remove an Atom by pointer.
	 */
	ATOM* erase(ATOM* t)
	// erase atom pointed by t
	{
		if(not(array<=t and t<array+max_len and t->k>>62==3))
			error("Invalid ATOM* to erase. (%p<=%p<=%p,0x%02lx)\n",
					array,t,array+max_len,t->k>>62);
		t->k &= ~(1lu<<63);
		memset(t->v,0,N*sizeof(float));
		len--;
		vacancy++;
		return next(t);
	}

	/**
	 * Remove an Atom by key.
	 */
	bool remove(uint64_t k)
	// remove key k, return if successful
	{
		uint32_t index_found;
		bool found = _find(k,index_found);
		if(found)
		{
			array[index_found].k &= ~(1lu<<63);
			memset(array[index_found].v,0,N*sizeof(float));
			len--;
			vacancy++;
		}
		return found;
	}

	/**
	 * Remove all the Atom.
	 *
	 * clear everything (to the initial state)
	 */
	void clear()
	{
		if(len!=0)
			memset(array,0,max_len*sizeof(ATOM));
		len = 0;
		vacancy = 0;
	}

	/**
	 * Rehash the map.
	 *
	 * Usually this is used when the map is overfull.
	 * If array is NULL, this will make a new valid map.
	 * Else, it will transfer everything to a new map.
	 */
	void rehash(uint32_t new_max_len)
	{
		qassert(new_max_len!=0);
		if(array==NULL)
		{
			max_len = new_max_len;
			len = 0; // normally we don't need to reinit len and vacancy
			vacancy = 0;
			mask = max_len - 1;
			array = (ATOM*)malloc(max_len*sizeof(ATOM));
			if(!array)
				error("malloc(%u*%lu) returned NULL.\n",max_len,sizeof(ATOM));
			memset(array,0,max_len*sizeof(ATOM));
			return;
		}
		// make new map
		BigMap newm(new_max_len);
		if(len!=0)
			for(ATOM* t=begin();t!=end();t=next(t))
				memcpy(newm[t->k].v,t->v,N*sizeof(T)); // only copy v[N]
		// swap
		char * buffer = (char*)malloc(sizeof(*this));
		qassert(buffer!=NULL);
		memcpy(buffer,&newm,sizeof(*this));
		memcpy(&newm,this,sizeof(*this));
		memcpy(this,buffer,sizeof(*this));
		free(buffer);
	}

	/**
	 * Save the map to a (opened) file.
	 */
	uint32_t save(FILE * fo) const
	{
		fprintf(fo,"map_size: %u\n",len);
		for(auto t=begin();t!=end();t=next(t))
		{
			fprintf(fo,"0x%0lx",t->k & ~(3lu<<62));
			for(uint32_t i=0;i<N;i++)
				fprintf(fo,"\t%10e",t->v[i]);
			fprintf(fo,"\n");
		}
		return len;
	}

	/**
	 * Load from a (opened) file.
	 *
	 * Discard everything previously stored in the map if any.
	 */
	uint32_t load(FILE * fi)
	{
		uint32_t new_len;
		qassert(1==fscanf(fi,"map_size: %u\n",&new_len));
		dtor();
		rehash(power2ceil(4*new_len));
		for(uint32_t i=0;i<new_len;i++)
		{
			uint64_t k;
			qassert(1==fscanf(fi,"0x%lx",&k));
			ATOM& t = (*this)[k];
			for(uint32_t i=0;i<N;i++)
			{
				qassert(1==fscanf(fi,"\t%e",&t.v[i]));
				if(!std::isfinite(t.v[i]))
					error("(*0x%p)[0x%lx].v[%u] = %f, isfinite Failed.\n",
							this,k&~(3lu<<62),i,t.v[i]);
			}
			qassert('\n'==fgetc(fi));
		}
		return len;
	}

};

