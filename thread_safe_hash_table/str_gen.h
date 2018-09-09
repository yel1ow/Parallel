#pragma once

#include <string>

static const char alphanum[] =
"0123456789"
"!@#$%^&*"
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz";

int stringLength = sizeof(alphanum) - 1;

char genRandom()
{
    return alphanum[rand() % stringLength];
}

std::string str_gen(int length)
{
	std::string s;
	for(int i = 0; i < length; i++)
			s += genRandom();
	return s;
}

std::string str_gen_good_hash(int length, int hash)
{
	std::string s;
	for(int i = 0; i < length - 1; i++)
			s += genRandom();
	int code = 0;
	for(int i = 0; i < s.length(); i++)
		code += s[i];
	code = code % 256;
	int symb = hash - code;
	if (symb < 0)
		symb = symb + 256;
	unsigned char c = symb;
	s += symb;
	//std::cout << "gen_str hash: " << (code + c) % 256 << "&" << hash << std::endl;
	return s;
}