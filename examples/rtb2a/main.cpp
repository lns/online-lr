/**
 * @file main.cpp
 * @brief Main learning program for RTB competition.
 * @author linus
 * @version 1.0
 * @date 2013-10-03
 */
#include <cstdlib>
#include <cstdint>

extern "C"
{
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <pthread.h>
}

#include "headers/error.hpp"
#include "headers/time.hpp"
#include "headers/datatype.hpp"
#include "headers/lcg64.hpp"
#include "headers/util.hpp"

#include "learner/logistic_trsgd.hpp"
#include "feeder/rtb2a/feeder.hpp"

/**
 * Get payingprice of a Sample
 */
inline uint32_t get_payingprice(const Sample& sample)
{
	return (uint32_t)sample.x[2].key;
}

/**
 * Get the score for Sample
 * when no conv count, this is the number of clks
 */
inline uint32_t get_score(const Sample& sample)
{
	uint64_t score = sample.x[3].key;
	if(sample.x[20].key==3358)
		score += 2*sample.x[4].key; //score += 2*(sample.x[4].key>0?1:0);
	else if(sample.x[20].key==3476)
		score +=10*sample.x[4].key; //score += 10*(sample.x[4].key>0?1:0);
	return (uint32_t)score;
}

/**
 * Print and Reset statistics.
 */
inline void print_reset(LR_Learner& l, ///< the Learner
		uint64_t& s_score, ///< sum of the score we've won
		uint64_t& s_exp, ///< sum of the expense we've spent
		uint64_t& m_score,  ///< sum of all the score (won or not)
		uint64_t& m_exp) ///< sum of all the expense (won or not)
{
	l.print(true);
	printf("%8lu/%8lu=%8.6lf %4lu/%4lu=%8.6lf\n",
			s_exp,m_exp,(double)s_exp/m_exp,
			s_score,m_score,(double)s_score/m_score);
	s_score = m_score = s_exp = m_exp = 0;
}

/**
 * Memory Mapped by mmap.
 *
 * They are stored for munmap on exit.
 */
typedef struct
{
	char * head; ///< head address of the memory.
	uint64_t size; ///< size (in byte) of the memry
} Memdata;


/**
 * @brief Main Program Entrance.
 *
 * 1. Read training settings and init parameters
 * 2. Train and Trace
 * 3. Save the parameters
 *
 * @param argc Number of args
 * @param argv[] Vector of args
 *
 * @return 
 */
int main(int argc, char* argv[])
{
	lcg64(get_nsec());
	if(argc<3)
	{
		printf(" Usage: %s DATALIST N_ITER [START_ITER]\n",argv[0]);
		exit(0);
	}
	// Loading Data
	Feeder feeder;
	vector<Memdata> memdata;	
	{
		FILE * f = fopen(argv[1],"r");
		if(!f)
			error("Cannot open %s for reading.\n",argv[1]);
		size_t buffersize = 1024;
		char * buffer = (char*)malloc(buffersize*sizeof(char));
		while(true)
		{
			ssize_t len = getline(&buffer,&buffersize,f);
			if(feof(f) or len<=0)
				break;
			if(buffer[0]=='#' or buffer[0]=='\n')
				continue;
			char * p = buffer;
			while(*p!=' ') p++;
			double weight = strtod(p+1,NULL);
			*p = '\0';
			char * memhead = NULL;
			uint64_t memsize = mmap_datafile(buffer,&memhead);
			feeder.link(memhead,memsize,weight);
			memdata.push_back({memhead,memsize});
		}
		fclose(f);
	}		
	feeder.random_seek();
	Parameter param_learner("param_learner.txt");
	param_learner.print();
	LR_Learner learner(80);
	learner.load("model.txt");
	learner.par = &param_learner;
	uint64_t start_iter = 0;
	if(argc>3)
		start_iter = strtol(argv[3],NULL,10);
	learner.iter = start_iter;
	Sample sample(512);
	const uint64_t n_iter = strtol(argv[2],NULL,10);
	info("n_iter: %lu  start_iter: %lu\n",n_iter,start_iter);
	uint64_t sum_score = 0;
	uint64_t sum_expense = 0;
	uint64_t max_score = 0;
	uint64_t max_expense = 0;
	info("[%s] Start Parsing.\n",qstrtime());
	uint64_t stat_score[16];
	memset(stat_score,0,16*sizeof(uint64_t));
	uint64_t stat_exp[16];
	memset(stat_exp,0,16*sizeof(uint64_t));
	uint64_t all_score[16];
	memset(all_score,0,16*sizeof(uint64_t));
	uint64_t all_exp[16];
	memset(all_exp,0,16*sizeof(uint64_t));
	// Start timer
	struct timespec spec_a,spec_b;
	clock_gettime(CLOCK_REALTIME, &spec_a);
	print_reset(learner,sum_score,sum_expense,max_score,max_expense);
	for(uint64_t iter=1;iter<=n_iter;iter++)
	{
		// Learn and Predict
		//feeder.random_seek();
		feeder.feed(sample);
		uint32_t score = get_score(sample);
		sample.y  = (score>0?1:-1); // y \in {-1,1}
		sample.wt = (score>0?500*score:1); // Very Imbalance!
		double f = learner.digest(sample);
		double p = 1/(1+exp(-f));
		// Second model
		uint32_t bidprice = floor(exp(6.5+2.5*log(p))); // XXX
		// Evaluate
		bool win = bidprice >= get_payingprice(sample);
		sum_score += win?score:0;
		sum_expense += win?get_payingprice(sample):0;
		max_score += score;
		max_expense += get_payingprice(sample);
		//
		all_score[get_payingprice(sample)/20] += score;
		all_exp[get_payingprice(sample)/20] += get_payingprice(sample);
		if(win)
		{
			stat_score[get_payingprice(sample)/20] += score;
			stat_exp[get_payingprice(sample)/20] += get_payingprice(sample);
		}
		if(iter % 300000 == 0) // approx. 1 sec
		{
			print_reset(learner,sum_score,sum_expense,max_score,max_expense);
			feeder.random_seek();
		}
	}
	// Stop timer
	clock_gettime(CLOCK_REALTIME, &spec_b);
	info("[%s] End Parsing.\n",qstrtime());
	double dsec = spec_b.tv_sec-spec_a.tv_sec;
	dsec += 1e-9*(spec_b.tv_nsec-spec_a.tv_nsec);
	info("Used time: %12.8lf\n",dsec);
	for(int i=0;i<16;i++)
		printf("%lu,%lu\n",stat_exp[i],stat_score[i]);
	printf("\n");
	for(int i=0;i<16;i++)
		printf("%lu,%lu\n",all_exp[i],all_score[i]);
	// Save
	learner.save("model.txt");
	//free(mem_train);
	for(auto it=memdata.begin();it!=memdata.end();++it)
		munmap(it->head,it->size);
	return 0;
}

