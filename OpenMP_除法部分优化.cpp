#include <iostream>
#include <time.h>
#include <omp.h>

#define NUM_THREADS 4



void Guass(float** matrix, int n)
{
    /*
    #pragma omp parallel num_threads(NUM_THREADS)
    for(int k = 0;k<n;k++)
    {
         #pragma omp single
        float temp = matrix[k][k];
        for(int i = 0;i<n;i++)
        {
            if(temp != 0)
                matrix[k][i] = matrix[k][i]/temp;
        }
        #pragma omp for
        for(int i = k+1;i<n;i++)
        {
            for(int j = k+1;j<n;j++)
            {
                matrix[i][j] -= matrix[i][k] * matrix[k][j];
            }
            matrix[i][k] = 0;
        }
    }
    */
    
    #pragma omp parallel num_threads(NUM_THREADS)
	for(int k=0;k<n;++k)
	{
		// 串行部分
		
			float tmp = matrix[k][k];
        #pragma omp for
			for(int j=k+1;j<n;j++)
                if(tmp!=0)
                	matrix[k][j]=matrix[k][j]/tmp;
        if(tmp!=0)
        	matrix[k][k]=1.0;
		//并行部分，使用行划分
		#pragma omp for//之后的for循环将被并行化由多个线程划分执行
	    for(int i = k + 1; i < n; ++i)
        {
			float tmp = matrix[i][k];
			for(int j = k + 1; j < n; ++j)
            {
				matrix[i][j] = matrix[i][j] - tmp * matrix[k][j];
			}
			matrix[i][k] = 0.0;
        }
		// 离开for循环时，各个线程默认同步，进入下一行的处理
	}
    
}
int main()
{
	int step = 10;
	for(int n = 0;n<=10000;n += step)
	{
//	int n = 10;
		float** matrix = new float*[n];
		for(int i = 0;i< n;i++)
		{
			matrix[i] = new float[n];
		}
		for(int i = 0;i<n;i++)
		{
			for(int j = 0;j<n;j++)
			{
				matrix[i][j] = i+j+1;
			}
		}
		clock_t start,finish;
		start = clock();
		int counter = 0;
		while(clock() - start < 30)
		{
			counter++;
			Guass(matrix,n);
		}
		finish = clock();
		float seconds = (finish - start)/float (CLOCKS_PER_SEC);
		std::cout<<seconds / counter <<std::endl;
		//std::cout<<n<<' '<<counter<<' '<<seconds<<' '<<seconds / counter <<std::endl;
/*		for(int i = 0;i<n;i++)
		{
			for(int j = 0;j<n;j++)
			{
				std::cout<<matrix[i][j]<<' ';
			}
			std::cout<<std::endl;
		}*/
		if(n==100)
		{
			step = 300;
        }
		if(n==1000)
			step = 500;
	}
	return 0;
}
