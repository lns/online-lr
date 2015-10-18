/**
 * @file feeder.hpp
 * @brief Definition for Feeder. 
 * @author linus
 * @version 1.0
 * @date 2013-10-03
 */
#pragma once

#include <cstdlib>
#include <cstdint>
#include "headers/hash.hpp"
#include "headers/error.hpp"
#include "headers/datatype.hpp"
#include "headers/bigmap2.hpp"
#include "headers/lcg64.hpp"
#include "feeder/rtb2b/parser.hpp"
#include <vector>
#include <random>

using namespace MurmurHash2;
using namespace std;

/**
 * A piece of memory storing raw text data. 
 *
 * Usually a piece of virtual memory-mapped memory.
 */
typedef struct
{
	const char * head; ///< head address
	uint64_t len; ///< length
	double wt; ///< level of importance of this data.
	double cum_prob; ///< cumulated probability (for internal usage).
} Mem;

/**
 * A basic worker who parses raw data to Sample for Learner.
 */
class Feeder
{
public:
	BigMap<2,uint32_t> m; ///< Stored clk(in v[0]) and conv(in v[1]) info.
	vector<Mem> mem; ///< A vector of Mem (storing raw txt data).
	const char * p; ///< current pointer (to read in a sample line).
	vector<Mem>::iterator cur; ///< which Mem we are currently parsing.

	Feeder()
	{
		m.rehash(1<<16);
		const char * clk_file = "data/clk.txt";
		const char * conv_file ="data/conv.txt";
		const char * clk_file_srv = "/home/player2/data/clk.txt";
		const char * conv_file_srv ="/home/player2/data/conv.txt";
		// Put clk in m[].w, conv in m[].v	
		FILE * f;
		if(NULL!=(f = fopen(clk_file,"r")))
			;
		else if(NULL!=(f = fopen(clk_file_srv,"r")))
			;
		else
			error("Cannot open %s for reading.\n",clk_file);
		size_t buffersize = 1024;
		char * buffer = (char*)malloc(buffersize*sizeof(char));
		while(true)
		{
			ssize_t len = getline(&buffer,&buffersize,f);
			if(feof(f) or len<0)
				break;
			char * tail = buffer;
			while(*tail!='\t') tail++;
			m[hash64(buffer,tail-buffer)].v[0] ++;
		}
		fclose(f);
		if(NULL!=(f = fopen(conv_file,"r")))
			;
		else if(NULL!=(f = fopen(conv_file_srv,"r")))
			;
		else
			error("Cannot open %s for reading.\n",conv_file);
		while(true)
		{
			ssize_t len = getline(&buffer,&buffersize,f);
			if(feof(f) or len<0)
				break;
			char * tail = buffer;
			while(*tail!='\t') tail++;
			m[hash64(buffer,tail-buffer)].v[1] ++;
		}
		fclose(f);
		free(buffer);
	}

	~Feeder()
	{
		m.~BigMap();
	}

	/**
	 * Add a mapped memory of data to Feeder 's collection of data.
	 */
	void link(const char * _memhead, uint64_t _memsize, double weight)
	{
		qassert(_memhead[_memsize-1]=='\n');
		auto it = mem.begin();
		while(it!=mem.end() and it->wt*it->len > weight*_memsize)
			++it;
		mem.insert(it,{_memhead,_memsize,weight,0});
		// Update cum_prob
		double sum = 0;
		for(auto it=mem.begin();it!=mem.end();++it)
			sum += it->len*it->wt;
		double cum = 0;
		for(auto it=mem.begin();it!=mem.end();++it)
		{
			cum += it->wt*it->len/sum;
			it->cum_prob = cum;
		}
		//
		if(mem.size()==1)
		{
			cur = mem.begin();
			p = cur->head;
		}
	}

	/**
	 * Random pick a file and a line ( for future reading ).
	 */	
	void random_seek()
	{
		// draw cur
		double ran = (double)lcg64()/(uint64_t)~0;
		do {
			cur = mem.begin();
			while(cur!=mem.end() and cur->cum_prob<=ran)
				cur++;
		}while(cur==mem.end());
		// random offset
		p = cur->head + lcg64()%cur->len;
		while(*(p++)!='\n');
		if(p==cur->head + cur->len)
			p = cur->head;
	}

	/**
	 * Feed a Sample slot with a line from p
	 */	
	void feed(Sample& slot)
	{
		slot.len = 0;
		p = parseline2(p,slot);
		if(p==cur->head + cur->len)
			p = cur->head;
		const Atom<2,uint32_t>& a = m.get(slot.x[0].key);
		slot.x[3].key = a.v[0]; // n_clk
		slot.x[4].key = a.v[1]; // n_conv
	}
};

