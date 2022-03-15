#include <iostream>
#include <time.h>
#include <random>
int main()
{
	int step = 10;
	for(int n = 0;n<=5000;n+=step)
	{
		float* a = new float[n];
		std::random_device rd;
	        std::mt19937 gen(rd());
	     	std::uniform_real_distribution<> dis(0.0,100.0);
		clock_t start,finish;
//	int step = 10;
//	int a[1000];
		for(int i = 0;i<n;i++)
		{
			a[i] = dis(gen);
		}
//		for(int n = 0;n< 1000;n += step)
//	{
		start = clock();
		int counter = 0;
		float sum = 0.0;
		while(clock()-start < 20)
		{
			counter++;
			for(int i = 0;i<n;i++)
			{
				sum += a[i];
			}
		}
		if(n==100)
		{
			step = 300;
		}
		if(n==1000)
			step = 500;
		finish = clock();
	        float seconds = (finish - start)/float (CLOCKS_PER_SEC);
	        std::cout<<n<<' '<<counter<<' '<<seconds<<' '<<seconds / counter <<std::endl;
//		std::cout<<seconds / counter <<std::endl;
	}
	return 0;
}
