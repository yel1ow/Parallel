#include <iostream>
#include <thread>
#include <mutex>

using namespace std;

const int elem_count = 100;

struct node
{
	int key;
	mutex m;//mutex* m
	node* next = NULL;
	node(int key):key(key){}
};

void insert(node* list, node* x)
{
	x->next = list->next;
	list->next = x;
}

void show_list(node* list)
{
	if(list)
	{
		show_list(list->next);
		cout << list->key << ' ';
	}
}

void sum(node* list)
{
	int random_elem = rand() % (elem_count - 1);
	node* current = list;
	current->m.lock();
	for(int i = 0; i < random_elem; i++)
	{
		if(current->next)
		{
			current->next->m.lock();
			node* save = current;
			current = current->next;
			save->m.unlock();
		}
		else
		{
			current->m.unlock();
			return;
		}
	}
	if(current->next)
	{
		current->next->m.lock();
		if(current->next->next)
		{
			current->next->next->m.lock();
		}
		else
		{
			current->next->m.unlock();
			current->m.unlock();
			return;
		}
	}
	else
	{
		current->m.unlock();
		return;
	}
	current->next->key = current->next->key + current->next->next->key;
	node* save = current->next->next;
	current->next->next = current->next->next->next;
	save->m.unlock();
	delete save;
	current->next->m.unlock();
	current->m.unlock();
}

void delete_list(node* list)
{
	if(list)
	{
		delete_list(list->next);
		delete list;
	}
}

int main()
{
	node* list = new node(-1);
	cout << list->key << endl;
	
	for(int i = 0; i < elem_count; i++)
	{
		insert(list, new node(rand() % 10));
	}
	show_list(list);
	cout << endl;

	int N = 1;
	cout << "N = ";
	cin >> N;

	thread* threads = new thread[N];
	while(list->next->next)
	{
		for(int i = 0; i < N; i++)
		{
			threads[i] = thread(sum, list);
		}
		for(int i = 0; i < N; i++)
		{
			threads[i].join();
		}
	}
	show_list(list);
	delete_list(list);

	delete[] threads;
}