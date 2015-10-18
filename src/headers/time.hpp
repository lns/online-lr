/**
 * @file time.hpp
 * @brief Timing utilities
 * @author linus
 * @version 1.0
 * @date 2013-10-03
 */
#pragma once

// Link with -lrt

#include <ctime>
#include <cstdio>
#include <cstdint>

// See http://pubs.opengroup.org/onlinepubs/7908799/xsh/time.h.html

/** 
 * Get current time in a string format.
 */
inline const char* qstrtime()
{
	static char b [128];
	struct tm * timeinfo;
	struct timespec spec;
	clock_gettime(CLOCK_REALTIME, &spec);
	timeinfo = localtime((time_t*)&spec.tv_sec);
	strftime(b,128,"%F %X",timeinfo);
	char * t = b;
	while(*t!='\0') ++t;
	sprintf(t,".%03lu",spec.tv_nsec/1000000);
	return b;
}

/**
 * Sleep for lf_sec seconds
 */
inline void qsleep(double lf_sec)
{
	struct timespec t;
	t.tv_sec = lf_sec;
	t.tv_nsec = lf_sec*1000000000;
	t.tv_nsec %= 1000000000;
	nanosleep(&t,NULL);
}

/**
 * Get nano second of current time.
 */
inline uint64_t get_nsec()
{
	struct timespec spec;
	clock_gettime(CLOCK_REALTIME, &spec);
	return spec.tv_nsec;
}
