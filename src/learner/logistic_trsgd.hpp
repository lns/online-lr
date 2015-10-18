/**
 * @file logistic_trsgd.hpp
 * @brief Logistic regression with truncated SGD.
 * @author linus
 * @version 1.0
 * @date 2013-10-03
 */
#pragma once

#include <cstdint>
#include <cstdlib>
#include <cmath>
#include <cassert>

#include "headers/bigmap2.hpp"
#include "headers/datatype.hpp"
#include "headers/error.hpp"

/**
 * Parameters for learning.
 */
class Parameter
{
public:
	float stepsize; ///< Basic step size
	uint32_t K; ///< Truncate frequency
	float threshold; ///< Threshold
	float g; ///< Gravity parameter
	float power_eta; ///< Stepsize decay

	/**
	 * Read from a file (with filename)
	 */
	Parameter(const char * filename = NULL):
		stepsize(0.1),K(100),threshold(1e3),g(1e-2),
		power_eta(0.5)
	{
		if(!filename)
			return;
		FILE * f = fopen(filename,"r");
		if(!f)
			error("Cannot open parameter file %s.\n",filename);
#define read_param(x,type) fscanf(f,#x ": " type "\n", &x)
		read_param(K,"%u");
		read_param(stepsize,"%e");
		read_param(threshold,"%e");
		read_param(g,"%e");
		read_param(power_eta,"%e");
		fclose(f);
	}

	/**
	 * Print the content.
	 */
	void print()
	{
#define print_param(x,type) printf(#x ": " type "\n",x)
		print_param(K,"%u");
		print_param(stepsize,"%e");
		print_param(threshold,"%e");
		print_param(g,"%e");
		print_param(power_eta,"%e");
	}

};

/**
 * Logistic Regression Learner
 *
 * Read in Sample and Update model weights. 
 *
 * The update rule is a truncated version of SGD, see
 * [TrSGD](http://jmlr.org/papers/volume10/langford09a/langford09a.pdf)
 */
class LR_Learner
{
	//FILE * fout;
protected:
	const uint32_t max_n_space; ///< max number of feature space
	uint32_t n_space;///< number of feature space
	double intercept;///< intercept
public:
	Parameter * par; ///< parameter for learning
	uint64_t iter; ///< \f$ n \f$
	double eta; ///< \f$ \left( \frac{1}{n} \right)^{\gamma} \f$
	double sum_loss; ///< \f$ \sum_{i=1}^n W_i L(\hat{y}_i,y_i) \f$
	double sum_wt; ///< \f$ \sum_{i=1}^n W_i \f$, \f$W_i\f$ is the weight for this sample

	typedef BigMap<2,float> BIGMAP;
	BIGMAP* m; ///< Stored model weights

	/**
	 * Constructor
	 *
	 * @param _n_space number of feature space
	 */
	LR_Learner(uint32_t _n_space, uint32_t _max_n_space = 256):  
		//fout(NULL),
		max_n_space(_max_n_space), n_space(0), intercept(0),
		par(NULL), iter(0), eta(1), sum_loss(0), sum_wt(0), m(NULL)
	{
		m = (BIGMAP*)malloc(max_n_space*sizeof(BIGMAP));
		if(!m)
			error("malloc(%u*%lu) returned NULL.\n",max_n_space,sizeof(BIGMAP));
		memset(m,0,max_n_space*sizeof(BIGMAP));
		for(uint32_t i=0;i<_n_space;i++)
			incr();
		//fout = fopen("LR_out.txt","a");
	}

	~LR_Learner()
	{
		if(m==NULL)
			return;
		for(uint32_t i=0;i<n_space;i++)
			m[i].~BIGMAP();
		free(m);
		m = NULL;
		//if(fout!=NULL)
		//	fclose(fout);
	}

	/**
	 * Add a feature space, store it with a BigMap of max_length
	 */
	void incr(uint32_t map_max_len = 1<<8)
	{
		if(n_space==max_n_space)
			error("Max size reached.\n");
		m[n_space++].rehash(map_max_len);
	}

	/**
	 * Remove the last added feature space.
	 */
	void decr()
	{
		if(n_space==0)
			return;
		m[n_space].~BIGMAP();
		// memset(m+n_space,0,sizeof(BigMap)); // This is for safe, not necessary.
		n_space--;
	}

	/**
	 * Number of feature space currently have.
	 */
	uint32_t size() const { return n_space; }

	/**
	 * Max number of features spaces could be stored.
	 */
	uint32_t max_size() const { return max_n_space; }

	/**
	 * Digest a Sample. 
	 *
	 * 1. Get prediction for this Sample.
	 * 2. Update sum_loss and sum_wt.
	 * 3. Update the model if we should.
	 *
	 * @param s the Sample
	 * @param _update whether or not we should update the model.
	 */
	double digest(const Sample& s,bool _update = true)
	{
		double f = predict(s);
		sum_loss += s.wt*Loss(s.y,f,s);
		sum_wt += s.wt;
		if(not _update)
			return f;
		iter++;
		eta = pow(1.0/iter,par->power_eta);
		update(s,f,s.wt);
		if(iter % par->K == 0)
			truncate();
		return f;
	}

	/**
	 * Output statistics.
	 */
	void print(bool omit_newline = false)
	{
		static bool print_head = true;
		if(print_head)
		{
			printf("      iter   size   weight     step     loss  \n");
			print_head = false;
		}
		uint32_t sum_size = 0;
		for(uint32_t i=0;i<n_space;i++)
			sum_size += m[i].size();
		printf("%10lu %6u %4.2le %4.2le %8.6lf ",
				iter,sum_size,sum_wt,eta*par->stepsize,sum_loss/sum_wt);
		if(!omit_newline)
			printf("\n");
		//fprintf(fout,"%10lu\t%6u\t%8le\t%8le\t%8.6lf\n",
		//		iter,sum_size,sum_wt,eta*par->stepsize,sum_loss/sum_wt);
		sum_loss = 0;
		sum_wt = 0;
	}

	/**
	 * Save the model to a file.
	 */
	uint32_t save(const char * filename) const
	{
		info("Save to %s\n",filename);
		uint32_t saved = 0;
		FILE * fo = fopen(filename,"w");
		if(!fo)
			error("Failed to open %s for writing.\n",filename);
		fprintf(fo,"n_space: %u\n",n_space);
		fprintf(fo,"intercept: %12le\n",intercept);
		for(uint32_t i=0;i<n_space;i++)
		{
			fprintf(fo,"=== Space %u ===\n",i);
			saved += m[i].save(fo);
		}
		fprintf(fo,"=== END ===\n");
		fclose(fo);
		return saved;
	}

	/**
	 * Load the model from a file
	 */
	uint32_t load(const char * filename)
	{
		uint32_t loaded = 0;
		FILE* fi = fopen(filename,"r");
		if(!fi)
		{
			warning("Failed to load from %s.\n",filename);
			return 0;
		}
		info("Load from %s\n",filename);
		uint32_t new_n_space,old_n_space = n_space;
		qassert(1==fscanf(fi,"n_space: %u\n",&new_n_space));
		while(n_space>0)
			decr();
		if(old_n_space!=new_n_space)
			warning("n_space: old: %u new: %u, choose %u\n",
					old_n_space,new_n_space,
					(old_n_space>new_n_space?old_n_space:new_n_space));
		qassert(1==fscanf(fi,"intercept: %le\n",&intercept));
		for(uint32_t i=0;i<new_n_space;i++)
		{
			uint32_t space;
			qassert(1==fscanf(fi,"=== Space %u ===\n",&space));
			qassert(space==i and i==n_space);
			loaded += m[n_space++].load(fi);
		}
		while(n_space<old_n_space)
			incr();
		info("%u data loaded.\n",loaded);
		return loaded;
	}

protected:
	/**
	 * Make prediction on Sample s
	 *
	 * Return \f$ f = c + w^T x\f$. (\f$c\f$ is intercept).
	 *
	 * In logistic regression,
	 * \f[
	 * 		p = \frac{1}{1+\exp(- f) }
	 * \f]
	 *
	 */
	inline double predict(const Sample& s) const
	{
		double f = intercept;
		for(auto p=s.x;p<s.x+s.len;p++)
			if(p->space < n_space)
				f += m[p->space].get(p->key).v[0]*p->value;
		if(f!=f)
			debug("predict() yields NaN.\n");
		return f;
	}

	/**
	 * Update model
	 *
	 * Stochastic Gradient Descent update.
	 * \f[
	 * 		w = w - W_i \eta \frac{\partial}{\partial w}L(\hat{y}_i,y_i)
	 * \f]
	 * in which \f$ W_i \f$ is the weight for sample, and
	 * \f[
	 * 		\eta = \left( \frac{1}{n} \right)^\gamma \cdot s
	 * \f]
	 * \f$s\f$ is par->stepsize. (We omit the \f$x_i\f$ since for this data
	 * all features are considered binary.)
	 *
	 * @param f prediction
	 * @param wt \f$W_i\f$, weight for this sample
	 */
	inline void update(const Sample& s, double f, double wt)
	{
		const double y = s.y;
		const double p = 1/(1+exp(-y*f));
		const double d = wt*par->stepsize*eta*(p-1)*y;
		intercept -= d;
		for(auto t=s.x;t<s.x+s.len;t++)
		{
			if(t->space >= n_space)
				continue;
			m[t->space][t->key].v[0] -= d*t->value;
		}
	}

	/**
	 * Truncate the model weights.
	 *
	 * \f[
	 * 		w = T(w)
	 * \f]
	 * in which
	 * \f[
	 * 		T(x) = \left\{\begin{array}{cc} (x-\alpha) \lor 0 & 0 \le x < \theta \\
	 * 		(x+\alpha) \land 0 & -\theta < x < 0 \\
	 * 		x & other
	 * 		\end{array}\right.
	 * \f]
	 * \f$\theta\f$ is par->threshold, \f$\alpha\f$ is \f$\eta\f$*par->g*par->K
	 */
	inline void truncate()
	{
		const double trunc = par->K * par->stepsize * eta * par->g;
		for(uint32_t i=0;i<n_space;i++)
			for(auto t=m[i].begin();t!=m[i].end();)
			{// Be cautious when deleting while traversing
				if(0 <= t->v[0] and t->v[0] < par->threshold)
				{
					t->v[0] -= trunc;
					if(t->v[0] <= 0)
					{
						t = m[i].erase(t); // erase() return next()
						continue;
					}
				}
				else if(0 >= t->v[0] and t->v[0] > -par->threshold)
				{
					t->v[0] += trunc;
					if(t->v[0] >= 0)
					{
						t = m[i].erase(t);
						continue;
					}
				}
				t = m[i].next(t);
			}
	}

	/**
	 * gradient of Loss.
	 *
	 * \f[ \frac{\partial L}{\partial f} \f]
	 *
	 * For logistic regression
	 * \f[
	 * 		\left( \log(1+\exp(-y f)) \right)' = y \left(\frac{1}{1+\exp(-y f)}-1\right) = (p-1) y
	 * \f]
	 *
	 * This method is not used.
	 * See line:3 in LR_Learner::update.
	 */
	// y in {-1,1}
	inline double dLoss(double y, double f, const Sample& s) const
	{
		// This func is not used.
		return y*(1/(1+exp(-y*f))-1);
	}

	/**
	 * Loss function.
	 *
	 * For logistic regression
	 * \f[
	 * 		L(y,f) = \log(1+\exp(-y f))
	 * \f]
	 *
	 * This method is not used.
	 */
	inline double Loss(double y, double f, const Sample& s) const
	{
		return log(1+exp(-y*f));
	}
};

