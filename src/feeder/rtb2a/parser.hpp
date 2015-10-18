/**
 * @file parser.hpp
 * @brief Parse raw txt data to Sample
 * @author linus
 * @version 1.0
 * @date 2013-10-03
 */
#pragma once

#include <cstdio>
#include <cstring>
#include "headers/hash.hpp"
using namespace MurmurHash2;



#define SPACE_BID_ID 201
#define SPACE_BIDPRICE 202
#define SPACE_PAYINGPRICE 203
#define SPACE_N_CLK 204 // see feeder
#define SPACE_N_CONV 205 // see feeder
#define SPACE_WEEKDAY 1
#define SPACE_HOUR 2
#define SPACE_OS 3
#define SPACE_IE 4
#define SPACE_REGION 5
#define SPACE_CITY 6
#define SPACE_ADEX 7
#define SPACE_DOMAIN 8
#define SPACE_ADSLOTID 9
#define SPACE_ADWIDTH 10
#define SPACE_ADHEIGHT 11
#define SPACE_ADVISI 12
#define SPACE_ADFORMAT 13
#define SPACE_FLOORPRICE 14
#define SPACE_CREATIVEID 15
#define SPACE_ADERID 16
#define SPACE_USERTAGS 17
// user-ad interaction
#define SPACE_CREATIVEID_USERTAGS 20
#define SPACE_CREATIVEID_IE 21
#define SPACE_CREATIVEID_OS 22
#define SPACE_CREATIVEID_REGION 23
#define SPACE_CREATIVEID_HOUR 24
#define SPACE_ADERID_USERTAGS 25
#define SPACE_ADERID_IE 26
#define SPACE_ADERID_OS 27
#define SPACE_ADERID_REGION 28
#define SPACE_ADERID_HOUR 29
// user-adslot interaction
#define SPACE_ADEX_USERTAGS 30
#define SPACE_ADEX_IE 31
#define SPACE_ADEX_OS 32
#define SPACE_ADEX_REGION 33
#define SPACE_ADEX_HOUR 34
// user interaction
#define SPACE_OS_IE 35
#define SPACE_DOMAIN_IE 36
#define SPACE_DOMAIN_OS 37
#define SPACE_DOMAIN_REGION 38
#define SPACE_DOMAIN_HOUR 39
// adslot-ad interaction
#define SPACE_CREATIVEID_ADEX 40
#define SPACE_ADERID_ADEX 41
#define SPACE_CREATIVEID_ADVISI 42
#define SPACE_ADERID_ADVISI 43
#define SPACE_CREATIVEID_ADFORMAT 44
#define SPACE_ADERID_ADFORMAT 45
// user-adslot2 
#define SPACE_ADVISI_IE 46
#define SPACE_ADVISI_REGION 47
#define SPACE_ADFORMAT_IE 48
#define SPACE_ADFORMAT_REGION 49
// pCTR
#define SPACE_PCTR 50 // deprecated
// Newly added
#define SPACE_OS_USERTAGS 51
#define SPACE_IE_USERTAGS 52
#define SPACE_HOUR_USERTAGS 53
#define SPACE_WEEKDAY_USERTAGS 54
#define SPACE_REGION_USERTAGS 55
#define SPACE_ADVISI_USERTAGS 56
#define SPACE_ADFORMAT_USERTAGS 57
#define SPACE_ADVISI_OS 58
#define SPACE_ADFORMAT_OS 59
//
#define SPACE_CREATIVEID_USERTAGS_HOUR 60
#define SPACE_CREATIVEID_IE_HOUR 61
#define SPACE_CREATIVEID_OS_HOUR 62
#define SPACE_CREATIVEID_CITY_HOUR 63
#define SPACE_ADERID_USERTAGS_HOUR 65
#define SPACE_ADERID_IE_HOUR 66
#define SPACE_ADERID_OS_HOUR 67
#define SPACE_ADERID_CITY_HOUR 68
#define SPACE_ADEX_USERTAGS_HOUR 70
#define SPACE_ADEX_IE_HOUR 71
#define SPACE_ADEX_OS_HOUR 72
#define SPACE_ADEX_CITY_HOUR 73
#define SPACE_ADERID_ADEX_HOUR 74
#define SPACE_OS_IE_HOUR 75
#define SPACE_CREATIVEID_ADEX_HOUR 76
#define SPACE_DOMAIN_ADERID 77
#define SPACE_DOMAIN_USERTAGS 78
#define SPACE_DOMAIN_CREATIVEID 79

/**
 * Convert a number in hex format string to uint64_t
 *
 * Optimized for specific task here.
 */
uint64_t qstrtox(const char * s, int size)
{
	uint64_t res = 0;
	for(const char * p=s;p<s+size;++p)
		res = res<<4 | (*p>'9'?(*p-'a'+10):(*p-'0'));
	return res;
}

/**
 * Convert a number in dec format string to uint64_t
 *
 * Optimized for specific task here.
 */
uint64_t qstrtol(const char * s, char ** q)
{
	bool positive = true;
	if(*s=='-')
	{
		positive = false;
		s++;
	}
	uint64_t res = 0;
	for(*q=(char*)s;**q>='0' and **q<='9';++(*q))
		res = 10*res + (**q-'0');
	return positive?res:((~res)+1);
}

#include "feeder/rtb2a/parser_ua.hpp"
/**
 * Parse a line of data.
 *
 * For each feature parsed, push them in s.
 *
 * Return the ptr pointing at the next char of '\n'
 */
const char * parseline2(const char * head, Sample & s)
{
	const char * tail = head;
	while(*tail!='\t') tail++;
	// buffer[head:tail] = Bid id
	// NOTICE: Bid id must be hashed with hash64. 
	// There are keys that hash32 exactly collide.
	s.push_back(Feature(SPACE_BID_ID,0,hash64(head,tail-head)));
	s.push_back(Feature(SPACE_BIDPRICE,0,0));
	s.push_back(Feature(SPACE_PAYINGPRICE,0,0));
	s.push_back(Feature(SPACE_N_CLK,0,0));
	s.push_back(Feature(SPACE_N_CONV,0,0));
	head = ++tail;
	while(*tail!='\t') tail++;
	// buffer[head:tail] = Timestamp
	//int month = 10*(head[4]-'0') + (head[5]-'0');
	int day = 10*(head[6]-'0') + (head[7]-'0');
	int hour = 10*(head[8]-'0') + (head[9]-'0');
	s.push_back(Feature(SPACE_WEEKDAY,1,day%7)); // only for June
	s.push_back(Feature(SPACE_HOUR,1,hour));
	//int minute = 10*(head[10]-'0') + (head[11]-'0');
	head = ++tail;
	while(*tail!='\t') tail++;
	// Log Type : 0,1,2
	head = ++tail;
	while(*tail!='\t') tail++;
	// iPinyou id
	head = ++tail;
	while(*tail!='\t') tail++;
	// User Agent
	s.push_back(Feature(SPACE_OS,1,get_os(head,tail-head)));
	s.push_back(Feature(SPACE_OS,1,get_ie(head,tail-head)));
	head = ++tail;
	while(*tail!='\t') tail++;
	// IP
	head = ++tail;
	//while(*tail!='\t') tail++;
	// Region
	s.push_back(Feature(SPACE_REGION,1,qstrtol(head,(char**)&tail)));
	qassert(*tail=='\t');
	head = ++tail;
	//while(*tail!='\t') tail++;
	// City
	s.push_back(Feature(SPACE_CITY,1,qstrtol(head,(char**)&tail)));
	qassert(*tail=='\t');
	head = ++tail;
	//while(*tail!='\t') tail++;
	// Ad Exchange
	s.push_back(Feature(SPACE_ADEX,1,qstrtol(head,(char**)&tail)));
	qassert(*tail=='\t');
	head = ++tail;
	while(*tail!='\t') tail++;
	// Domain
	s.push_back(Feature(SPACE_DOMAIN,1,hash64(head,tail-head)));
	head = ++tail;
	while(*tail!='\t') tail++;
	// URL
	head = ++tail;
	while(*tail!='\t') tail++;
	// Anonymous URL
	head = ++tail;
	while(*tail!='\t') tail++;
	// Ad slot id
	s.push_back(Feature(SPACE_ADSLOTID,1,hash64(head,tail-head)));
	head = ++tail;
	//while(*tail!='\t') tail++;
	// Ad slot width
	s.push_back(Feature(SPACE_ADWIDTH,1,qstrtol(head,(char**)&tail)));
	qassert(*tail=='\t');
	head = ++tail;
	//while(*tail!='\t') tail++;
	// Ad slot height
	s.push_back(Feature(SPACE_ADHEIGHT,1,qstrtol(head,(char**)&tail)));
	qassert(*tail=='\t');
	head = ++tail;
	while(*tail!='\t') tail++;
	// Ad slot visibility
	s.push_back(Feature(SPACE_ADVISI,1,hash64(head,tail-head)));
	head = ++tail;
	while(*tail!='\t') tail++;
	// Ad slot format
	s.push_back(Feature(SPACE_ADFORMAT,1,hash64(head,tail-head)));
	head = ++tail;
	//while(*tail!='\t') tail++;
	// Floor price
	s.push_back(Feature(SPACE_FLOORPRICE,1,qstrtol(head,(char**)&tail)));
	qassert(*tail=='\t');
	head = ++tail;
	while(*tail!='\t') tail++;
	// Creative id
	//qassert(tail-head>=16);
	s.push_back(Feature(SPACE_CREATIVEID,1,hash64(head,tail-head)));
	head = ++tail;
	//while(*tail!='\t') tail++;
	// Bid price
	s.x[1].key = qstrtol(head,(char**)&tail);
	qassert(*tail=='\t');
	head = ++tail;
	//while(*tail!='\t') tail++;
	// Paying price
	s.x[2].key = qstrtol(head,(char**)&tail);
	qassert(*tail=='\t');
	head = ++tail;
	while(*tail!='\t') tail++;
	// Key page
	head = ++tail;
	//while(*tail!='\t') tail++;
	// Advertiser ID (NEW)
	s.push_back(Feature(SPACE_ADERID,1,qstrtol(head,(char**)&tail)));
	qassert(*tail=='\t');
	head = ++tail;
	while(*tail!='\n') tail++;
	// User Tags (NEW)	
	//printf("%s\n",string(head,tail-head).c_str());
	if(head[0]=='n' and head[1]=='u' and head[2]=='l' and head[3]=='l')
		;
	else if(head==tail)// \t\n (user tags missing)
		;
	else
		while(true)// false to disable UserTags
		{
			const char * tp = head;
			//while(*tp != ',' and *tp != '\n')
			//	tp++;
			s.push_back(Feature(SPACE_USERTAGS,1,qstrtol(head,(char**)&tp)));
			if(*tp=='\n')
				break;
			head = ++tp;
		}
#define CROSS_KEY(s,a,b) (s.x[a].key^(s.x[b].key^(s.x[b].key<<8)))
#define CROSS_TRIPLE(s,a,b,c) (s.x[a].key^(s.x[b].key^(s.x[b].key<<8))^(s.x[c].key^(s.x[c].key<<8)))
	// interactions
	const uint32_t start_usertags = 21;
	const uint32_t end_usertags = s.len;
	//double usertags_weight = 1.0/(end_usertags-start_usertags);
	double usertags_weight = 1.0;
	for(uint32_t i=start_usertags;i<end_usertags;i++)
	{
		qassert(s.x[i].space==SPACE_USERTAGS);
		s.push_back(Feature(SPACE_CREATIVEID_USERTAGS,usertags_weight,CROSS_KEY(s,19,i)));
		s.push_back(Feature(SPACE_ADERID_USERTAGS,usertags_weight,CROSS_KEY(s,20,i)));
		s.push_back(Feature(SPACE_ADEX_USERTAGS,usertags_weight,CROSS_KEY(s,11,i)));
	//	s.push_back(Feature(SPACE_OS_USERTAGS,usertags_weight,CROSS_KEY(s,7,i)));
	//	s.push_back(Feature(SPACE_IE_USERTAGS,usertags_weight,CROSS_KEY(s,8,i)));
	//	s.push_back(Feature(SPACE_HOUR_USERTAGS,usertags_weight,CROSS_KEY(s,6,i)));
	//	s.push_back(Feature(SPACE_WEEKDAY_USERTAGS,usertags_weight,CROSS_KEY(s,5,i)));
	//	s.push_back(Feature(SPACE_REGION_USERTAGS,usertags_weight,CROSS_KEY(s,9,i)));
	//	s.push_back(Feature(SPACE_ADVISI_USERTAGS,usertags_weight,CROSS_KEY(s,16,i)));
	//	s.push_back(Feature(SPACE_ADFORMAT_USERTAGS,usertags_weight,CROSS_KEY(s,17,i)));
	//	s.push_back(Feature(SPACE_CREATIVEID_USERTAGS_HOUR,usertags_weight,
	//				CROSS_TRIPLE(s,19,i,6)));
	//	s.push_back(Feature(SPACE_ADERID_USERTAGS_HOUR,usertags_weight,
	//				CROSS_TRIPLE(s,20,i,6)));
	//	s.push_back(Feature(SPACE_ADEX_USERTAGS_HOUR,usertags_weight,
	//				CROSS_TRIPLE(s,11,i,6)));
	//	s.push_back(Feature(SPACE_DOMAIN_USERTAGS,usertags_weight,
	//				CROSS_KEY(s,20,i)));
	}
	s.push_back(Feature(SPACE_CREATIVEID_IE,1,CROSS_KEY(s,19,8)));
	s.push_back(Feature(SPACE_CREATIVEID_OS,1,CROSS_KEY(s,19,7)));
	s.push_back(Feature(SPACE_CREATIVEID_REGION,1,CROSS_KEY(s,19,9)));
	s.push_back(Feature(SPACE_CREATIVEID_HOUR,1,CROSS_KEY(s,19,6)));
	s.push_back(Feature(SPACE_ADERID_IE,1,CROSS_KEY(s,20,8)));
	s.push_back(Feature(SPACE_ADERID_OS,1,CROSS_KEY(s,20,7)));
	s.push_back(Feature(SPACE_ADERID_REGION,1,CROSS_KEY(s,20,9)));
	s.push_back(Feature(SPACE_ADERID_HOUR,1,CROSS_KEY(s,20,6)));
	s.push_back(Feature(SPACE_ADEX_IE,1,CROSS_KEY(s,11,8)));
	s.push_back(Feature(SPACE_ADEX_OS,1,CROSS_KEY(s,11,7)));
	s.push_back(Feature(SPACE_ADEX_REGION,1,CROSS_KEY(s,11,9)));
	s.push_back(Feature(SPACE_ADEX_HOUR,1,CROSS_KEY(s,11,6)));
	s.push_back(Feature(SPACE_OS_IE,1,CROSS_KEY(s,7,8)));
	///*
	s.push_back(Feature(SPACE_DOMAIN_IE,1,CROSS_KEY(s,12,8)));
	s.push_back(Feature(SPACE_DOMAIN_OS,1,CROSS_KEY(s,12,7)));
	s.push_back(Feature(SPACE_DOMAIN_REGION,1,CROSS_KEY(s,12,9)));
	s.push_back(Feature(SPACE_DOMAIN_HOUR,1,CROSS_KEY(s,12,6)));
	//*/
	s.push_back(Feature(SPACE_CREATIVEID_ADEX,1,CROSS_KEY(s,19,11)));
	s.push_back(Feature(SPACE_ADERID_ADEX,1,CROSS_KEY(s,20,11)));
	///* ADVISI & ADFORMAT
	s.push_back(Feature(SPACE_CREATIVEID_ADVISI,1,CROSS_KEY(s,19,16)));
	s.push_back(Feature(SPACE_ADERID_ADVISI,1,CROSS_KEY(s,20,16)));
	//s.push_back(Feature(SPACE_CREATIVEID_ADFORMAT,1,CROSS_KEY(s,19,17)));
	//s.push_back(Feature(SPACE_ADERID_ADFORMAT,1,CROSS_KEY(s,20,17)));
	s.push_back(Feature(SPACE_ADVISI_IE,1,CROSS_KEY(s,16,8)));
	s.push_back(Feature(SPACE_ADVISI_REGION,1,CROSS_KEY(s,16,9)));
	//s.push_back(Feature(SPACE_ADFORMAT_IE,1,CROSS_KEY(s,17,8)));
	//s.push_back(Feature(SPACE_ADFORMAT_REGION,1,CROSS_KEY(s,17,9)));
	s.push_back(Feature(SPACE_ADVISI_OS,1,CROSS_KEY(s,16,7)));
	//s.push_back(Feature(SPACE_ADFORMAT_OS,1,CROSS_KEY(s,17,7)));
	//*/
	/*
	s.push_back(Feature(SPACE_CREATIVEID_IE_HOUR,1,CROSS_TRIPLE(s,19,8,6)));
	s.push_back(Feature(SPACE_CREATIVEID_OS_HOUR,1,CROSS_TRIPLE(s,19,7,6)));
	s.push_back(Feature(SPACE_CREATIVEID_CITY_HOUR,1,CROSS_TRIPLE(s,19,10,6)));
	s.push_back(Feature(SPACE_ADERID_IE_HOUR,1,CROSS_TRIPLE(s,20,8,6)));
	s.push_back(Feature(SPACE_ADERID_OS_HOUR,1,CROSS_TRIPLE(s,20,7,6)));
	s.push_back(Feature(SPACE_ADERID_CITY_HOUR,1,CROSS_TRIPLE(s,20,10,6)));
	s.push_back(Feature(SPACE_ADEX_IE_HOUR,1,CROSS_TRIPLE(s,11,8,6)));
	s.push_back(Feature(SPACE_ADEX_OS_HOUR,1,CROSS_TRIPLE(s,11,7,6)));
	s.push_back(Feature(SPACE_ADEX_CITY_HOUR,1,CROSS_TRIPLE(s,11,10,6)));
	s.push_back(Feature(SPACE_OS_IE_HOUR,1,CROSS_TRIPLE(s,7,8,6)));
	s.push_back(Feature(SPACE_CREATIVEID_ADEX_HOUR,1,CROSS_TRIPLE(s,19,11,6)));
	s.push_back(Feature(SPACE_ADERID_ADEX_HOUR,1,CROSS_TRIPLE(s,20,11,6)));
	*/
	s.push_back(Feature(SPACE_DOMAIN_ADERID,1,CROSS_KEY(s,12,20)));
	s.push_back(Feature(SPACE_DOMAIN_CREATIVEID,1,CROSS_KEY(s,12,19)));
	return(tail+1);
}

