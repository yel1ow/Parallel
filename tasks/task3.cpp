#include <iostream>
#include <thread>
#include <mutex>
#include <cstring>
#include <ctime>

using namespace std;

bool show;

struct account
{
	int sum = 100;
	mutex m;
};

void transaction(account* accounts, int from, int to, int sum, int id)
{
	if(from == to)
	{
		return;
	}

	if(from < to)
	{
		accounts[from].m.lock();
		accounts[to].m.lock();
	}
	else
	{
		accounts[to].m.lock();
		accounts[from].m.lock();
	}

	if(accounts[from].sum > sum)
	{
		accounts[from].sum -= sum;
		accounts[to].sum += sum;
	}

	accounts[from].m.unlock();
	accounts[to].m.unlock();
}

void showAccounts(account* a, int N)
{
	for (int i = 0; i < N; i++)
	{
		cout << a[i].sum << ' ';
	}
	cout << endl;
}

int main(int argc, char* argv[])
{
	if (argc > 1)
	{
		if(!strcmp(argv[1], "show"))
			show = true;
	}

	int N = 10;

	account* accounts = new account[N];
	thread* threads = new thread[N];
	int m = 10;
	srand(time(0));
	for(int i = 1; i <= m; i++)
	{
		for(int i = 0; i < N; i++)
		{
			int x = rand() % N;
			int y = rand() % N;
			int sum = rand() % 100;
			threads[i] = thread(transaction, accounts, x, y, sum, i);
			if(show)
			{
				//cout << "from " << x << " to " << y << " " << sum << "$" << endl;
			}
		}
		for(int i = 0; i < N; i++)
		{
			threads[i].join();
		}
		if(show)
		{
			cout << "transaction " << i << " results:" << endl;
			showAccounts(accounts, N);
			int sum = 0;
			for(int i = 0; i < N; i++)
			{
				sum += accounts[i].sum;
			}
			cout << sum << endl;
		}
	}

	int sum = 0;
	for(int i = 0; i < N; i++)
	{
		sum += accounts[i].sum;
	}
	cout << sum << endl;
	delete[] accounts;
	delete[] threads;
}