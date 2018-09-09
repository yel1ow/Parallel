#include <iostream>
#include <thread>
#include <string>
#include <ctime>
#include "hash_table.h"
#include "str_gen.h"

using namespace std;

int hashing(string s)
{
	int code = 0;
	for(int i = 0; i < s.length(); i++)
		code += s[i];
	code = code % 256;
	return code;
}

const int threads_num = 256;

string check_array[256];

int main()
{
	srand(time(0));
	thread* threads = new thread[threads_num];

	hash_table hash(256, hashing);

	//insert
	for(int k = 0; k < threads_num; k++)
	{
		check_array[k] = str_gen_good_hash(rand() % 10 + 5, k);
		threads[k] = thread(bind(&hash_table::insert, hash, check_array[k]));
	}

	for(int k = 0; k < threads_num; k++)
	{
		threads[k].join();	
	}

	hash.print_table();
	
	//delete
	for(int k = 0; k < threads_num; k++)
	{
		threads[k] = thread(bind(&hash_table::del, hash, check_array[k]));
	}

	for(int k = 0; k < threads_num; k++)
	{
		threads[k].join();
	}

	hash.print_table();

	// for(int k = 0; k < threads_num; k++)
	// {
	// 	int random = rand() % 3;
	// 	if(random == 0)
	// 	threads[k] = thread(bind(&hash_table::insert, hash, str_gen(rand() % 10 + 5))); //check_array[k]));
	// 	if(random == 1)
	// 	threads[k] = thread(bind(&hash_table::search, hash, str_gen(rand() % 10 + 5)));
	// 	if(random == 2)
	// 	threads[k] = thread(bind(&hash_table::del, hash, str_gen(rand() % 10 + 5)));
	// }

	// for(int k = 0; k < threads_num; k++)
	// {
	// 	threads[k].join();
	// }

	delete[] threads;
}