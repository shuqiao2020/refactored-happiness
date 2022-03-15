#include <iostream>
#include <time.h>
#include <random>
int main()
{
	int step = 10;
	for(int n = 0;n<=5000;n += step)
	{
		float** a = new float* [n];
		float* b = new float [n];
		for(int i = 0;i<n;i++)
		{
			a[i] = new float[n];
		}
		std::random_device rd;
        	std::mt19937 gen(rd());
   	        std::uniform_real_distribution<> dis(0.0,100.0);
	        for(int i = 0;i<n;i++)
        	{
			for(int j = 0;j<n;j++)
			{
				a[i][j] = dis(gen);
				b[i] = dis(gen);
			}
		}
		clock_t start,finish;
//		int step = 10;
//		for(int n = 0;n<=1000;n += step)
//		{
		start = clock();
		int counter = 0;
		while(clock() - start < 30)
		{
			counter++;
			int* sum = new int[n];
			for(int i = 0;i<n;i++)
			{
				sum[i] = 0.0;
			}
			for(int j = 0;j<n;j++)
			{
				for(int i = 0;i<n;i++)
				{
					sum[i] += a[j][i]*b[i];
				}
			}
			delete[] sum;
		}
		finish = clock();
		float seconds = (finish - start)/float (CLOCKS_PER_SEC);
		std::cout<<n<<' '<<counter<<' '<<seconds<<' '<<seconds / counter <<std::endl;
		if(n==100)
		{
			step = 300;
		}
		if(n==1000)
			step = 500;
	}
	return 0;
}

