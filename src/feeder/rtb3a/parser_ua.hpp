/**
 * @file parser_ua.hpp
 * @brief Parse UserAgent info
 * @author linus
 * @version 1.0
 * @date 2013-10-18
 */
#pragma once

#include <cstdio>
#include <cstring>

using namespace MurmurHash2;

/**
 * Get OS from UserAgent.
 */
uint64_t get_os(const char * ua,size_t size)
{
	char * p, * q; 
	if(NULL!=(p=(char*)memmem(ua,size,"Windows",7)))
	{
		if(0==memcmp(p+8,"NT 5",4))//strstr(ua,"Windows NT 5"))
			return 0x9b497d1bc3ceb01d;
		else if(0==memcmp(p+8,"NT 6.1",6))//strstr(ua,"Windows NT 6"))
		{
			if((char*)memmem(ua,size,"WOW64",5))
				return 0x99e3550167b4f4dd;
			return 0x6968bd1ddbe11b58;
		}
		else if(0==memcmp(p+8,"NT 6.2",6))//strstr(ua,"Windows NT 6"))
		{
			if((char*)memmem(ua,size,"WOW64",5))
				return 0xbbe2ee1e84258f70;
			return 0x45dfc5d88a7328c9;
		}
		else
			return 0x66c4e90a9a50fa7;
	}else if(NULL!=(p=(char*)memmem(ua,size,"Android",7))) {
		switch(p[8])
		{
		case '2':
			return 0xe504465a3a048743;
		case '3':
			return 0x217afed115d19c99;
		case '4':
			return 0x7c6b3040ca09d821;
		default:
			return 0x353bb7b3419d39bd;
		}
	}else if(NULL!=(p=(char*)memmem(ua,size,"Mac OS",6))) {
		if(NULL!=(q=(char*)memmem(ua,size,"iPad",4)))
		{
			if(0==memcmp(q+13,"5_0",3))
				return 0x1f64919c2ddc6a49;
			else if(0==memcmp(q+13,"5_1",3))
				return 0x1899240d285002b;
			else if(0==memcmp(q+13,"6_0",3))
				return 0x5a7572d22635605b;
			else if(0==memcmp(q+13,"6_1",3))
				return 0xd3131591136f16ef;
			return 0xb546210c468f16cb;
		}
		else if(NULL!=(q=(char*)memmem(ua,size,"iPhone OS",9)))
		{
			if(0==memcmp(q+10,"5_0",3))
				return 0xb063b584b58b41eb;
			else if(0==memcmp(q+10,"5_1",3))
				return 0x77250c885813112f;
			else if(0==memcmp(q+10,"6_0",3))
				return 0xe1348317808ba303;
			else if(0==memcmp(q+10,"6_1",3))
				return 0x7ea0442827ef62ee;
			return 0x7606449365f0b21e;
		}
		else if((char*)memmem(ua,size,"Macintosh",9))
		{
			switch(p[12])
			{
				case '6':
					return 0x62741076a12e8b47;
				case '7':
					return 0x16bb87dce6d68de6;
				case '8':
					return 0xbce3d071ae404b5c;
			}
			return 0x1f909fb8007deeeb;
		}
		return 0x50e0d22e3a24cdc3;
	}else if((char*)memmem(ua,size,"Linux",5))
		return 0x47e2cd2a34371ab3;
	else if((char*)memmem(ua,size,"CoolPad",7))
		return 0x5d1d27bf3342ce72;
	else
		return 0xdc8e141f7f9a7cb1;
}

/**
 * Get IE (browser info) from UserAgent.
 */
uint64_t get_ie(const char * ua,size_t size)
{
	char * p, * q;
	if(NULL!=(p=(char*)memmem(ua,size,"MSIE",4)))
	{
		if((char*)memmem(ua,size,"QQBrowser",9))
		{
			switch(p[5])
			{
			case '6':
				return 0x9f17f02ebdaefcd5;
			case '7':
				return 0xdb7c22ada0287bab;
			case '8':
				return 0x4967cb19f65f6cb9;
			case '9':
				return 0x31de874d0b9911a7;
			default:;
			}
			return 0x596701edd359020a;
		}
		else if((char*)memmem(ua,size,"TheWorld",8))
		{
			switch(p[5])
			{
			case '6':
				return 0x6dfd68af87495668;
			case '7':
				return 0x825d3246918a80bc;
			case '8':
				return 0xd6c5496fc705e805;
			case '9':
				return 0xeb251306d147ea49;
			default:;
			}
			return 0x524ee6da5df8b82;
		}
		else if((char*)memmem(ua,size,"MetaSr",6))
		{
			switch(p[5])
			{
			case '6':
				return 0xffc137fd5abf3c12;
			case '7':
				return 0xcf60a1f0fd5099ac;
			case '8':
				return 0xf93592e7cae360a4;
			case '9':
				return 0x2141a10983df942f;
			default:;
			}
			return 0xa3cf2e94a00c4629;
		}
		else if((char*)memmem(ua,size,"360SE",5))
		{
			switch(p[5])
			{
			case '6':
				return 0x859a6e8fa74f253f;
			case '7':
				return 0xdae5a41bcf0f7d2d;
			case '8':
				return 0xb81505d5da250f9e;
			case '9':
				return 0xec3c3b24128c2bff;
			default:;
			}
			return 0x34c251f9afa64079;
		}
		else if((char*)memmem(ua,size,"360EE",5))
			return 0xfaeef68cfc648ef4;
		else if((char*)memmem(ua,size,"bsalsa",6))
		{
			switch(p[5])
			{
			case '6':
				return 0x839e1a3406b8e8a1;
			case '7':
				return 0x2a1bc8833495ee1d;
			case '8':
				return 0xbd5f0e69e72deae0;
			case '9':
				return 0x60d04f78fe4d818b;
			default:;
			}
			return 0x3c64cd271ee8a433;
		}
		else if((char*)memmem(ua,size,"BIDU",4))
		{
			switch(p[5])
			{
			case '6':
				return 0xacdb8139612ffe0a;
			case '7':
				return 0x6812d2ac1c4f6626;
			case '8':
				return 0xda11df55d7f9bc68;
			case '9':
				return 0x43ed3d9b485851b7;
			default:;
			}
			return 0xd9b840f81fe7072;
		}
		else if(NULL!=(q=(char*)memmem(ua,size,"Maxthon",7)))
		{
			switch(*(q+8))
			{
				case '2':
					return 0x28624339e05bec37;
				case '3':
					return 0xa1b2fdacc6a3bf00;
				case '4':
					return 0x24a2d8e7bf851cf0;
				default:;
			}
			return 0x4c8b370a72055de4;
		}
		switch(p[5])
		{
		case '6':
			return 0x6e090bf590cb8bb1;
		case '7':
			return 0x9d3cc9a0f2c8962b;
		case '8':
			return 0xeb1f60d15b95ac8c;
		case '9':
			return 0xc6897093c8c9c273;
		default:;
		}
		return 0x653023e05dc69ad0;
	}	
	else if((char*)memmem(ua,size,"Safari",6))
	{
		if(NULL!=(p=(char*)memmem(ua,size,"QQBrowser",9)))
		{
			if(*(p-1)=='M')
				return 0xf21cb18761eae153;
			return 0xf3bc1b3253ae84c;
		}
		else if((char*)memmem(ua,size,"Chrome",6))
		{
			if((char*)memmem(ua,size,"MetaSr",6))
				return 0xc3a7d04f9f6dfa1c;
			else if((char*)memmem(ua,size,"BIDU",4))
				return 0x884f740bb4aa32b3;
			else if(NULL!=(p=(char*)memmem(ua,size,"Maxthon",7)))
			{
				switch(*(p+8))
				{
					case '2':
						return 0x471882f5a45a2d29;
					case '3':
						return 0x194256eec5a3ca0e;
					case '4':
						return 0xb768a34a78349f73;
					default:;
				}
				return 0x1664201727dbef37;
			}
			return 0x5ffaad16a65253a4;
		}
		return 0xdd49e698f20d09f2;
	}
	else if(NULL!=(p=(char*)memmem(ua,size,"Opera",5)))
	{
		if((char*)memmem(ua,size,"Opera Mini",10))
			return 0x99217a357bf19c3a;
		return 0xb38328877f0078ff;
	}
	else if(NULL!=(p=(char*)memmem(ua,size,"Firefox",7)))
	{
		if(0==memcmp(p+7,"/3.6.8",6))
			return 0xbfad8e1f25509754;
		else if(0==memcmp(p+7,"/3.0.11",7))
			return 0x8d29ff5cb966f0fd;
		else if(0==memcmp(p+7,"/21",3))
			return 0x15687d4b9b8549e7;
		else if(0==memcmp(p+7,"/18",3))
			return 0xaef8f3d311a989c6;
		else if(0==memcmp(p+7,"/17",3))
			return 0x722a847f02afdf1d;
		return 0xaec31979e386adbd;
	}
	return 0xdc8e141f7f9a7cb1;
}

