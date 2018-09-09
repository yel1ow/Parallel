#include <iostream>
#include <thread>
#include <ctime>
#include "bin_tree_v2.0.h"

using namespace std;

const int threads_count = 100;
const int MAX = 999;

const int MAX2 = 10;

int random_array[threads_count];

template <class T>
void insert(bin_tree<T>* tree, T x)
{
	tree->insert(x);
}

template <class T>
void search(bin_tree<T>* tree, T x)
{
	tree->search(x);
}

template <class T>
void remove(bin_tree<T>* tree, T x)
{
	tree->remove(x);
}

int main()
{
	srand(time(0));
	bin_tree<int> tree;
	
	thread* threads = new thread[threads_count];

	for(int i = 0; i < threads_count; i++)
		random_array[i] = i;

	// //insert
	// for(int i = 0; i < threads_count; i++)
	// {
	// 	//random_array[i] = rand() % MAX + 1;
	// 	threads[i] = thread(insert<int>, &tree, random_array[i]);
	// }

	// for(int i = 0; i < threads_count; i++)
	// {
	// 	threads[i].join();
	// }

	// cout << "PRINT_TREE x1" << endl;
	// tree.print(tree.get_root(), 0);
	// cout << "elem count: " << tree.get_elem_count() << endl;
	// cout << endl << endl;

	// // //search
	// // for(int i = 0; i < threads_count; i++)
	// // {
	// // 	threads[i] = thread(search<int>, &tree, rand() % MAX + 1);
	// // }

	// // for(int i = 0; i < threads_count; i++)
	// // {
	// // 	threads[i].join();
	// // }

	// //delete
	// for(int i = 0; i < threads_count; i++)
	// {
	// 	threads[i] = thread(remove<int>, &tree, random_array[i]);
	// }

	// for(int i = 0; i < threads_count; i++)
	// {
	// 	threads[i].join();
	// }

	// cout << "PRINT_TREE x2" << endl;
	// tree.print(tree.get_root(), 0);
	// cout << "elem count: " << tree.get_elem_count() << endl;
	// cout << endl << endl;

	//test
	for(int i = 0; i < threads_count; i++)
	{
		int q = rand() % 2;
		if(q == 0)
			threads[i] = thread(insert<int>, &tree, rand() % MAX2 + 1);
		if(q == 1)
			threads[i] = thread(remove<int>, &tree, rand() % MAX2 + 1);
	}

	for(int i = 0; i < threads_count; i++)
	{
		threads[i].join();
	}

	cout << "PRINT_TREE x3" << endl;
	tree.print(tree.get_root(), 0);
	cout << "elem count: " << tree.get_elem_count() << endl;
	cout << endl << endl;

	delete[] threads;
}