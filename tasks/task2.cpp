#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <ctime>
#include <cstring>

using namespace std;

void quickSort(int arr[], int left, int right) {
	int i = left, j = right;
	int tmp;
	int pivot = arr[(left + right) / 2];
	while (i <= j) {
		while (arr[i] < pivot)
			i++;
		while (arr[j] > pivot)
			j--;
		if (i <= j) {
			tmp = arr[i];
			arr[i] = arr[j];
			arr[j] = tmp;
			i++;
			j--;
		}
	};
	if (left < j)
		quickSort(arr, left, j);
	if (i < right)
		quickSort(arr, i, right);
}

mutex m;
bool show = false;

void sort(int* arr, int beg, int k, int id)
{
	quickSort(arr, beg, beg + k - 1);
	if(show)
	{
		m.lock();
		cout << "thread " << id << " sorted elements: " << beg << "..." << beg + k - 1 << endl;
		m.unlock();
	}
}

void merge(int* beg1, int n1, int* beg2, int n2, int id)
{
	if(show)
	{
		m.lock();
		cout << "merge:" << endl;
		for(int i = 0; i < n1; i++)
		{
			cout << beg1[i] << ' ';
		}
		cout << endl;
		for(int i = 0; i < n2; i++)
		{
			cout << beg2[i] << ' ';
		}
		cout << endl;
		m.unlock();
	}
	int* temp = new int[n1 + n2];
	int i1 = 0, i2 = 0, i = 0;
	while (true)
	{
		// m.lock();
		// cout << "i1 = " << i1 << "i2 = " << i2 << endl;
		// m.unlock();
		if (i1 == n1)
		{
			temp[i] = beg2[i2];
			// cout << "in " << i << " put " << i2 <<
			//  "-iy elem beg2 = " << beg2[i2] << endl;
			i2++;
		}
		else
		if (i2 == n2)
		{
			temp[i] = beg1[i1];
			// cout << "in " << i << " put " << i1 <<
			//  "-iy elem beg1 = " << beg1[i1] << endl;
			i1++;
		}
		else
		if (beg1[i1] > beg2[i2])
		{
			temp[i] = beg2[i2];
			// cout << "in " << i << " put " << i2 <<
			//  "-iy elem beg2 = " << beg2[i2] << endl;
			i2++;
		}
		else
		{
			temp[i] = beg1[i1];
			// cout << "in " << i << " put " << i1 <<
			//  "-iy elem beg1 = " << beg1[i1] << endl;
			i1++;
		}
		i++;
		if (i1 == n1 && i2 == n2)
			break;
	}
	for (int i = 0; i < n1 + n2; i++)
		beg1[i] = temp[i];

	delete[] temp;
	// m.lock();
	// cout << "temp: ";
	// for (int i = 0; i < n1 + n2; i++)
	// 	cout << temp[i] << ' ';
	// cout << endl;
	// m.unlock();
}

int main(int argc, char* argv[])
{
	if (argc > 1)
	{
		if(!strcmp(argv[1], "show"))
			show = true;
	}

	cout << "l = ";
	int l;
	cin >> l;
	cout << "threads: ";
	int N;
	cin >> N;
	if(N > l)
	{
		cout << "too many..." << endl;
		exit(1);
	}
	if(N < 1)
	{
		cout << "i need at least 1 thread" << endl;
		exit(1);
	}
	
	int* arr = new int[l];
	for (int i = 0; i < l; i++)
	{
		arr[i] = rand() % 100;
		if(show)
			cout << arr[i] << ' ';
	}
	if(show)
		cout << endl;

	thread* threads = new thread[N];

	int* index = new int[N + 1];
	for(int i = 0; i < N; i++)
	{
		index[i] = (l / N) * i;
	}
	index[N] = l;//ATTENTION

	double start = clock();

	////////////sort

	for (int i = 0; i < N; ++i)
	{
		threads[i] = thread(sort, arr, index[i], index[i + 1] - index[i], i);
	}

	for (int i = 0; i < N; ++i)
		threads[i].join();

	if(show)
	{
		for (int i = 0; i < l; i++)
		{
			cout << arr[i] << ' ';
		}
		cout << endl;
	}

	////////////2nd part...

	int k = 2;

	while (k <= N)
	{
		int i;
		for(i = 0; i < N / k; i++)
		{
			threads[i] = thread(merge, &arr[index[k * i]], index[k * i + k / 2] - index[k * i],
				&arr[index[k * i + k / 2]], index[k * (i + 1)] - index[k * i + k / 2], i);
		}
		for(int i = 0; i < N / k; i++)
			threads[i].join();
		i--;
		if(index[N] != index[k * (i + 1)])
			merge(&arr[index[k * i]], index[k * (i + 1)] - index[k * i], &arr[index[k * (i + 1)]],
				index[N] - index[k * (i + 1)], i);
		k = k << 1;
	}

	//merge(&arr[index[0]], index[k / 2] - index[0], &arr[index[k / 2]], index[N] - index[k / 2], 0);

	if(show)
	{
		cout << "result:" << endl;
		for (int i = 0; i < l; i++)
		{
			cout << arr[i] << ' ';
		}
		cout << endl;
	}

	cout << (clock() - start) / 1000.0 << " ms" << endl;

	delete[] arr;
	delete[] index;
	delete[] threads;
}