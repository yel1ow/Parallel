#include <iostream>
#include <thread>
#include <mutex>
#include <ctime>
#include <cstring>

using namespace std;

struct elem
{
	int key;
	elem* next = NULL;
	mutex m;
	elem(int _key){key = _key;}
};

int num;
mutex mutex_num;
const int max_key = 100;
bool show = false;

void add(elem* list, elem* x)
{
	x->next = list->next;
	list->next = x;
}

void del_list(elem* list)
{
	if(list)
	{
		del_list(list->next);
		delete list;
	}
}

void show_list(elem* list)
{
	elem* current = list->next;
	while(current)
	{
		cout << current->key << " ";
		if(current->m.try_lock())
			{
				cout << "(Good) ";
				current->m.unlock();
			}
		else
			{
				cout << "(Bad) ";
				current->m.unlock();
			}
		cout << "\"" << &current->m << "\" ";
		current = current->next;
	}
	cout << endl;
}

elem* to_del;

void random_sum(elem* list, int id)
{
	mutex_num.lock();
	if (num == 1)
	{
		mutex_num.unlock();
		return;
	}
	int random_elem = rand() % (num - 1);
	mutex_num.unlock();
	//cout << "random value: " << random_elem << endl;
	elem* current = list;
	current->m.lock();
	
	for(int i = 0; i < random_elem; i++)
	{
		if(current->next)
		{
			current->next->m.lock();
			elem* save = current;
			current = current->next;
			save->m.unlock();
		}
		else
		{
			current->m.unlock();
			return;
		}
	}

	if (current->next)
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

	elem* ins = new elem(current->next->key + current->next->next->key);//
	//cout << "add: " << current->next->key << " + " << current->next->next->key << endl;
	ins->next = current->next->next->next;//
	
	//cout << "delete: " << current->next->next->key << endl;
	//delete current->next->next;//

	//cout << "delete: " << current->next->key << endl;
	//delete current->next;//
	elem* save1 = current->next;
	save1->next = NULL;
	elem* save2 = current->next->next;
	save2->next = NULL;
	current->next = ins;//

	save1->m.unlock();//
	save2->m.unlock();//
	//delete save1;//
	//delete save2;//
	// add(to_del, save1);
	// add(to_del, save2);
	

	mutex_num.lock();
	num--;
	mutex_num.unlock();

	current->m.unlock();
}

int main(int argc, char* argv[])
{
	if (argc > 1)
	{
		if(!strcmp(argv[1], "show"))
			show = true;
	}

	int l;
	if(show)
		l = 5;
	else l = 100;

	srand(time(0));

	int* arr = new int[l];
	for(int i = 0; i < l; i++)
	{
		arr[i] = rand() % max_key + 1;
	}
	if(show)
	{
		cout << "generated array <<" << endl;
		for(int i = 0; i < l; i++)
		{
			cout << arr[i] << " ";
		}
		cout << endl;
	}
	int sum = 0;
	for(int i = 0; i < l; i++)
	{
		sum += arr[i];
	}
	if(show)
	{
		cout << "sum = " << sum;
		cout << endl;
	}
	elem* list = new elem(-1);
	for(int i = l - 1; i >= 0; i++)
	{
		add(list, new elem(i));
		// current->next = new elem(arr[i]);
		// current = current->next;
	}
	cout << "list <<" << endl;
	show_list(list);
	int N;
	cout << "N = ";
	cin >> N;
	thread* threads = new thread[N];
	num = l;
	double start = clock();
	while (list->next->next)
	//for(int i = 0; i < 100; i++)
	{
		
		for(int i = 0; i < N; i++)
		{
			threads[i] = thread(random_sum, list, i);
		}
		for(int i = 0; i < N; i++)
		{
			threads[i].join();
		}
		if(show)
		{
			cout << "new list <<" << endl;
			show_list(list);
		}
	}
	if(show)
	{
		cout << "Result: " << list->next->key << endl;
	}
	if(sum == list->next->key)
		cout << "Ok!" << endl;
	else
		cout << "Wrong answer!" << endl;
	cout << (clock() - start) / 1000.0 << " ms" << endl;
	delete[] threads;
	del_list(list);
	del_list(to_del);
}