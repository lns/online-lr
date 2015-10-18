/**
 * @file util.hpp
 * @brief Miscellaneous utilities
 * @author linus
 * @version 1.0
 * @date 2013-10-03
 */
#pragma once

#include <cstdint>
#include "headers/datatype.hpp"

/**
 * Read a data file into memory.
 * You should call free(*ptr) when finished.
 * @return size of memory
 * This is not used for large data.
 * Try memory map.
 */
uint64_t read_datafile(char * filename, char** ptr)
{
	if(*ptr!=NULL)
		warning("*ptr != NULL (forgot to init?), may cause memory leakage.\n");
	FILE * f = fopen(filename,"r");
	if(!f)
		error("Cannot open %s for reading.\n",filename);
	fseek(f,0,SEEK_END);
	uint64_t len = ftell(f);
	qassert(NULL!=(*ptr = (char*)malloc(len)));
	fseek(f,0,SEEK_SET);
	info("Loading datafile %s .. \n",filename);
	qassert(fread(*ptr,1,len,f)==len);
	fclose(f);
	info("Done. %lu bytes loaded.\n",len);
	return len;
}

#include <sys/mman.h>

/**
 * Map a data file into memory.
 * Note that it shouldn't be used on a link file.
 * A file on NFS may fail too.
 * You are responsible to munmap it after usage.
 *
 * @return size of memory
 */
uint64_t mmap_datafile(char * filename, char** ptr)
{
	if(*ptr!=NULL)
		warning("*ptr != NULL (forgot to init?), may cause memory leakage.\n");
	int fd = open(filename,O_RDONLY);
	if(!fd)
		error("Cannot open %s for reading.\n",filename);
	uint64_t len = lseek(fd,0,SEEK_END);
	*ptr = (char*)mmap(NULL,len,PROT_READ,MAP_SHARED,fd,0);
	info("%s: %lu bytes mapped.\n",filename,len);
	return len;
}

