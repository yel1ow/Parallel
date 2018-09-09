#pragma once

#include <mutex>
#include <string>

struct elem
{
	std::string key;
	elem* next;
	std::mutex m;
	elem(std::string s):key(s){}
};

void del_list(elem* el)
{
	if(el)
	{
		del_list(el->next);
		delete el;
	}
}

class hash_table
{
	int size;
	elem** hash;
	int (*hash_func)(std::string);
public:
	hash_table(int N, int (*hash_func)(std::string));
	~hash_table();
	void insert(std::string s);
	elem* search(std::string s);
	void del(std::string s);
	void print_table();
};

hash_table::hash_table(int N, int (*_hash_func)(std::string))
{
	size = N;
	hash_func = _hash_func;
	hash = new elem*[size];
	for(int i = 0; i < size; i++)
		hash[i] = new elem("$");
}

hash_table::~hash_table()
{
	// for(int i = 0; i < size; i++)
	// 	del_list(hash[i]);
	// delete[] hash;
}

void hash_table::insert(std::string s)
{
	elem* el = new elem(s);
	elem* current = hash[hash_func(el->key)];
	current->m.lock();
	el->next = current->next;
	current->next = el;
	current->m.unlock();
}

elem* hash_table::search(std::string s)
{
	elem* current = hash[hash_func(s)];
	current->m.lock();
	if(current->next)
		current->next->m.lock();
	else
	{
		current->m.unlock();
		return NULL;
	}
	elem* save = current;
	current = current->next;
	save->m.unlock();

	while(current && current->key != s)
	{
		if(current->next)
			current->next->m.lock();
		else
		{
			current->m.unlock();
			return NULL;
		}
		elem* save = current;
		current = current->next;
		save->m.unlock();
	}
	
	elem* result = current;
	current->m.unlock();
	return result;
}

void hash_table::del(std::string s)
{
	elem* current = hash[hash_func(s)];
	current->m.lock();
	if(current->next)
		current->next->m.lock();
	else
	{
		current->m.unlock();
		return;
	}

	while(current->next->key != s)
	{
		if(current->next->next)
		{
			current->next->next->m.lock();
			elem* save = current;
			current = current->next;
			save->m.unlock();
		}
		else
		{
			current->next->m.unlock();
			current->m.unlock();
			return;
		}
	}
	elem* save = current->next;
	current->next = save->next;
	save->m.unlock();
	delete save;
	current->m.unlock();
}

void hash_table::print_table()
{
	std::cout << "print_table: " << std::endl;
	for(int i = 0; i < size; i++)
	{
		std::cout << i << ' ';
		elem* current = hash[i];
		// if(!current->next)
		// 	continue;
		while(current)
		{
			std::cout << current->key << " ";
			current = current->next;
		}
		std::cout << std::endl;
	}
}