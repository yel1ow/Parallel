#include <iostream>
#include <mutex>
#include <thread>
#include <ctime>
#include <cstring>

std::mutex m;
bool show = false;

void max(int *begin, int k, int id, int* max_arr)
{
	int max = begin[0];
	for (int i = 1; i < k; i++)
		if (begin[i] > max)
			max = begin[i];
	if(show)
	{
		m.lock();
		std::cout << "thread " << id << " max = " << max << std::endl;
		std::cout << "elementov: " << k << std::endl;
		m.unlock();
	}
	max_arr[id] = max;
}

int main(int argc, char* argv[])
{
	if (argc > 1)
	{
		if(!strcmp(argv[1], "show"))
			show = true;
	}

	std::cout << "l = ";
	int l;
	std::cin >> l;
	std::cout << "threads: ";
	int N;
	std::cin >> N;
	if(N > l)
	{
		std::cout << "too many..." << std::endl;
		exit(1);
	}
	if(N < 1)
	{
		std::cout << "i need at least 1 thread" << std::endl;
		exit(1);
	}

	int* arr = new int[l];
	for (int i = 0; i < l; i++)
	{
		arr[i] = rand() % 100;
		if(show)
			std::cout << arr[i] << ' ';
	}
	if(show)
		std::cout << std::endl;

	std::thread* threads = new std::thread[N];

	int* max_arr = new int[N];

	double start = clock();

	for (int i = 0; i < N - 1; ++i)
	{
		threads[i] = std::thread(max, &arr[i * (l / N)], l / N, i, max_arr);
	}
	threads[N - 1] = std::thread(max, &arr[(N - 1) * (l / N)], l - (l / N * (N - 1)), N - 1, max_arr);

	for (int i = 0; i < N; ++i)
		threads[i].join();

	int res = max_arr[0];
	for (int i = 0; i < N; i++)
		if (max_arr[i] > res)
			res = max_arr[i];

	std::cout << "max: " << res << std::endl;
	std::cout << ((clock() - start) * 1.0) / 100.0 << " ms" << std::endl;

	delete[] arr;
	delete[] max_arr;
	delete[] threads;

	return 0;
}