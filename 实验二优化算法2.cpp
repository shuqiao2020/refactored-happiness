#include <iostream>
#include <random>
#include <time.h>
void func(float a[],int n)
{
	if(n==1)
		return;
	else
	{
		if(n==0)
			return;
		for(int i = 0;i< n / 2;i++)
		{
			a[i] += a[n - i -1];
		}
		n = (1+n)/2;
		func(a,n);
		return;
	}
}
int main()
{
	int step = 10;
	for(int n = 0;n<=5000;n+=step)
	{
		std::random_device rd;
	        std::mt19937 gen(rd());
	        std::uniform_real_distribution<> dis(0.0,100.0);
        	clock_t start,finish;
//        int step = 10;
//        float a[1000];
		float* a = new float[n];
		 for(int i = 0;i<n;i++)
        	 {
                	 a[i] = dis(gen);
	         }
//         for(int n = 0;n<= 1000;n += step)
//        {
                 start = clock();
                 int counter = 0;
                 float sum = 0.0;
                 while(clock()-start < 20)
                 {
                        counter++;
			func(a,n);
			sum = a[0];
		 }
		 if(n==100)
        	 {
		         step = 300;
         	 }
		 if(n==1000)
			 step = 500;
        	 finish = clock();
         	 float seconds = (finish - start)/float (CLOCKS_PER_SEC);
//         	 std::cout<<n<<' '<<counter<<' '<<seconds<<' '<<seconds / counter <<std::endl;
 		std::cout<<seconds / counter <<std::endl; 
 	}
	 return 0;
}
