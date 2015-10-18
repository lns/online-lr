/**
 * @file error.hpp
 * @brief Thread-safe (and colorful!) functions to generate log.
 * @author linus
 * @version 1.0
 * @date 2013-10-03
 */

#pragma once

#include <cstdio>
#include <cstdlib>
#include <pthread.h>

#define likely(x) __builtin_expect(!!(x), 1) 
#define unlikely(x) __builtin_expect(!!(x), 0)

// Foreground color
// [31m red
// [32m green
// [33m yellow
// [34m blue
// [35m magenta
// [36m cyan

static pthread_mutex_t mutex_stderr = PTHREAD_MUTEX_INITIALIZER;

/**
 * An object to auto remove the mutex on exit.
 *
 * TODO: Do we really need it?
 */
class _class_auto_remover
{public:
	~_class_auto_remover() { pthread_mutex_destroy(&mutex_stderr); }
} _auto_remover ;

/**
 * Print where it goes wrong and exit.
 */
#define error(...) \
	do { pthread_mutex_lock(&mutex_stderr); \
		fprintf(stderr,"In %s(), %s:%d\n",__func__, __FILE__, __LINE__); \
		fprintf(stderr,"\033[31m[ERROR]\033[m "); \
		fprintf(stderr, __VA_ARGS__); \
		pthread_mutex_unlock(&mutex_stderr); \
		exit(-1); \
	}while(0)

/**
 * Print where it goes wrong and continue.
 */
#define warning(...) \
	do { pthread_mutex_lock(&mutex_stderr); \
		fprintf(stderr,"In %s(), %s:%d\n",__func__, __FILE__, __LINE__); \
		fprintf(stderr,"\033[33m[WARNING]\033[m "); \
		fprintf(stderr, __VA_ARGS__); \
		pthread_mutex_unlock(&mutex_stderr); \
	}while(0)

/**
 * Print where it goes wrong and continue.
 */
#define debug(...) \
	do { pthread_mutex_lock(&mutex_stderr); \
		fprintf(stderr,"In %s(), %s:%d\n",__func__, __FILE__, __LINE__); \
		fprintf(stderr,"\033[36m[DEBUG]\033[m "); \
		fprintf(stderr, __VA_ARGS__); \
		pthread_mutex_unlock(&mutex_stderr); \
	}while(0)

/**
 * Print something.
 */
#define info(...) \
	do { pthread_mutex_lock(&mutex_stderr); \
		fprintf(stderr,"\033[32m[INFO]\033[m "); \
		fprintf(stderr, __VA_ARGS__); \
		pthread_mutex_unlock(&mutex_stderr); \
	}while(0)

/**
 * Assert, print where and exit on failure.
 */
#define qassert(x) \
	do { if(unlikely(!(x))) { pthread_mutex_lock(&mutex_stderr); \
		fprintf(stderr,"In %s(), %s:%d\n",__func__, __FILE__, __LINE__); \
		fprintf(stderr,"\033[35m[ASSERT]\033[m "); \
		fprintf(stderr,"'%s' failed.\n",#x); \
		pthread_mutex_unlock(&mutex_stderr); \
		exit(-1); \
	}}while(0)

