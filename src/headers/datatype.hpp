/**
 * @file datatype.hpp
 * @brief Definitions for common datatypes.
 * @author linus
 * @version 1.0
 * @date 2013-10-03
 */

#pragma once

#include <cstdint>

/**
 * A unit of data (feature space, feature key, value) .
 */
class Feature
{
public:
	uint32_t space; ///< feature space (eg. CreativeId, AdslotWidth).
	float value; ///< usually 1.0 for binary feature.
	uint64_t key; ///< (eg. 1 in AdexchangePlatform, hash(str) in Domain)

	/**
	 * Default constructor
	 */
	Feature(uint32_t s, float v, uint64_t k):
		space(s),value(v),key(k) {};
};

/**
 * A collection of Feature extracted from a line in raw txt data.
 */
class Sample
{
public:
	uint32_t max_len; ///< max number of Feature.
	uint32_t len; ///< number of Feature.
	float y; ///< response, 1 or -1 for logistic regression.
	double wt; ///< weight of this sample.
	Feature* x; ///< an array of Feature

	/**
	 * Default constructor.
	 *
	 * Alloc memory for max_len Feature.
	 */
	Sample(uint32_t _max_len):
		max_len(_max_len), len(0), y(0), wt(0), x(NULL) 
	{
		//debug("Sample(%u)\n",max_len);
		qassert(max_len>0);
		qassert((x = (Feature*)malloc(max_len*sizeof(Feature))));
	}

	/**
	 * Push back a Feature.
	 *
	 * just like vector<T>.push_back() in STL.
	 */
	inline void push_back(const Feature & f)
	{
		if(len==max_len)
		{
			warning("max_len = %u reached. Realloc to %u.\n",max_len, 2*max_len);
			max_len *= 2;
			qassert((x = (Feature*)realloc(x,max_len*sizeof(Feature))));
		}
		x[len++] = f;
	}

	/**
	 * Remove all the Feature.
	 */
	inline void clear()
	{
		len = 0;
	}

	Sample& operator=(const Sample& rhs)
	{
		warning("This may cause a double free, do nothing.\n");
		return (*this);
	}

	~Sample()
	{
		//debug("Sample()::~Sample()\n");
		if(x)
			free(x);
		x = NULL;
	}

	/**
	 * Print all the Feature (for debug).
	 */
	void print()
	{
		info("Sample %p: max_len: %u len: %u y:%f wt: %lf\n",
				this,max_len,len,y,wt);
		for(uint32_t i=0;i<len;i++)
			info("x[%3u]: %8u %8f 0x%16lx %lu\n",
					i,x[i].space,x[i].value,x[i].key,x[i].key);
	}
};

