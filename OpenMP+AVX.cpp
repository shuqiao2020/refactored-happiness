#include <iostream>
#include <time.h>

#include <immintrin.h>
#ifdef_OPENMP
    #include <omp.h>
#endif

#ifdef_OPENMP
    int my_rank = omp_get_thread_num();
    int thread_count = omp_get_num_threads();
#else
    int my_rank = 0;
    int thread_count = 1;
#endlif

#endif

#define NUM_THREADS 4


void Guass(float** matrix, int n)
{
    __m256 t1, t2, t3, t4;
    #pragma omp parallel num_threads(NUM_THREADS) private(i, j, k, tmp)
	for(int k = 0;k<n;k++)
	{
	    float tmp[8] = { matrix[k][k], matrix[k][k], matrix[k][k], matrix[k][k],matrix[k][k], matrix[k][k], matrix[k][k], matrix[k][k] };
        t1 = _mm256_loadu_ps(tmp);
	    #pragma omp single
		for (int j = n - 8; j >=k; j -= 8) 
        {
            t2 = _mm256_loadu_ps(matrix[k] + j);
            if(tmp[0] !=0 )
            {
                t3 = _mm256_div_ps(t2, t1);
                _mm256_storeu_ps(matrix[k] + j, t3);
            }
        }
        if (k % 8 != (n % 8)) 
        {
            for (int j = k; j % 8 != ( n % 8); j++)
            {
                if(tmp[0] !=0 )
                    matrix[k][j] = matrix[k][j] / tmp[0];
            }
        }
        for (int j = k - 1; j>= 0; j--)
        {
            if(tmp[0] !=0 )
                matrix[k][j] = matrix[k][j] / tmp[0];
        }

		for (int i = k + 1; i < n; i++)
        {
            float tmp[8] = { matrix[i][k], matrix[i][k], matrix[i][k], matrix[i][k],matrix[i][k], matrix[i][k], matrix[i][k], matrix[i][k] };
            t1 = _mm256_loadu_ps(tmp);

            #pragma omp for
            for (int j = n - 8; j >k;j -= 8)
            {
                t2 = _mm256_loadu_ps(matrix[i] + j);
                t3 = _mm256_loadu_ps(matrix[k] + j);
                t4 = _mm256_sub_ps(t2,_mm256_mul_ps(t1, t3)); 
                _mm256_storeu_ps(matrix[i] + j, t4);
            }
            for (int j = k + 1; j % 8 !=(n % 8); j++)
            {
                matrix[i][j] = matrix[i][j] - matrix[i][k] * matrix[k][j];
            }
            matrix[i][k] = 0;
        }
	}
}
int main()
{
//	int step = 10;
//	for(int n = 0;n<=5000;n += step)
//	{
	int n = 10;
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
		for(int i = 0;i<n;i++)
		{
			for(int j = 0;j<n;j++)
			{
				std::cout<<matrix[i][j]<<' ';
			}
			std::cout<<std::endl;
		}
//		if(n==100)
//		{
//			step = 300;
//		}
//		if(n==1000)
//			step = 500;
//	}
	return 0;
}
