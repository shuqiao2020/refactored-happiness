#include <iostream>
#include <time.h>
#include <nmmintrin.h>
void Guass(float **matrix,int N) //加了SSE并行的高斯消去法

{
    __m128 t1, t2, t3, t4;
    for (int k = 0; k < N; k++)
    {
        float tmp[4] = { matrix[k][k], matrix[k][k], matrix[k][k], matrix[k][k] };
        t1 = _mm_loadu_ps(tmp);
        for (int j = N - 4; j >=k; j -= 4) //从后向前每次取四个
        {
            t2 = _mm_loadu_ps(matrix[k] + j);
            if(tmp[0] !=0 )
            {
                t3 = _mm_div_ps(t2, t1);//除法
                _mm_storeu_ps(matrix[k] + j, t3);
            }
        }
        if (k % 4 != (N % 4)) //处理不能被4整除的元素
        {
            for (int j = k; j % 4 != ( N% 4); j++)
            {
                if(tmp[0] !=0 )
                    matrix[k][j] = matrix[k][j] / tmp[0];
            }
        }
        //for (int j = (N % 4) - 1; j>= 0; j--)
        for (int j = k - 1; j>= 0; j--)
        {
            if(tmp[0] !=0 )
                matrix[k][j] = matrix[k][j] / tmp[0];
        }
        for (int i = k + 1; i < N; i++)
        {
            float tmp[4] = { matrix[i][k], matrix[i][k], matrix[i][k], matrix[i][k] };
            t1 = _mm_loadu_ps(tmp);
            for (int j = N - 4; j >k;j -= 4)
            {
                t2 = _mm_loadu_ps(matrix[i] + j);
                t3 = _mm_loadu_ps(matrix[k] + j);
                t4 = _mm_sub_ps(t2,_mm_mul_ps(t1, t3)); //减法
                _mm_storeu_ps(matrix[i] + j, t4);
            }
            for (int j = k + 1; j % 4 !=(N % 4); j++)
            {
                matrix[i][j] = matrix[i][j] - matrix[i][k] * matrix[k][j];
            }
            matrix[i][k] = 0;
        }
    }
}
/*
#include <iostream>
#include <time.h>
#include <nmmintrin.h>
void Guass(float **matrix,int N) //加了SSE并行的高斯消去法//对齐

{
    __m128 t1, t2, t3, t4;
    for (int k = 0; k < N; k++)
    {
        float tmp[4] = { matrix[k][k], matrix[k][k], matrix[k][k], matrix[k][k] };
        t1 = _mm_load_ps(tmp);
        int j = k;
        for (j = k; j%4!=0&&j < N; j++)
        {
            if(tmp[0] !=0 )
                matrix[k][j] = matrix[k][j] / tmp[0];
        }
        for (; j < N; j += 4) //每次取四个
        {
            t2 = _mm_load_ps(matrix[k] + j);
            if(tmp[0] !=0 )
            {
                t3 = _mm_div_ps(t2, t1);//除法
                _mm_store_ps(matrix[k] + j, t3);
            }
        }
        for (; j<N; j++)
        {
            if(tmp[0] !=0 )
                matrix[k][j] = matrix[k][j] / tmp[0];
        }
        /*if (k % 4 != (N % 4)) //处理不能被4整除的元素
        {
            for (int j = N; j % 4 != ( N% 4); j--)
            {
                if(tmp[0] !=0 )
                    matrix[k][j] = matrix[k][j] / tmp[0];
            }
        }*//*
        for (int j = k - 1; j>= 0; j--)
        {
            if(tmp[0] !=0 )
                matrix[k][j] = matrix[k][j] / tmp[0];
        }
        for (int i = k + 1; i < N; i++)
        {
            float tmp[4] = { matrix[i][k], matrix[i][k], matrix[i][k], matrix[i][k] };
            t1 = _mm_load_ps(tmp);
            int j = k+1;
            for (j = k + 1; j % 4 !=0&&j < N; j++)
            {
                matrix[i][j] = matrix[i][j] - matrix[i][k] * matrix[k][j];
            }
            for (; j <N;j += 4)
            {
                t2 = _mm_load_ps(matrix[i] + j);
                t3 = _mm_load_ps(matrix[k] + j);
                t4 = _mm_sub_ps(t2,_mm_mul_ps(t1, t3)); //减法
                _mm_store_ps(matrix[i] + j, t4);
            }
            for (; j <N ; j++)
            {
                matrix[i][j] = matrix[i][j] - matrix[i][k] * matrix[k][j];
            }
            matrix[i][k] = 0;
        }
    }
}*/
/*
#include <iostream>
#include <time.h>
#include <nmmintrin.h>
void Guass(float **matrix,int N) //加了SSE并行的高斯消去法//部分

{
    __m128 t1, t2, t3, t4;
    for (int k = 0; k < N; k++)
    {
        float tmp[4] = { matrix[k][k], matrix[k][k], matrix[k][k], matrix[k][k] };
        t1 = _mm_load_ps(tmp);
        int j = k;
        for (j = k; j%4!=0&&j < N; j++)
        {
            if(tmp[0] !=0 )
                matrix[k][j] = matrix[k][j] / tmp[0];
        }
        for (; j < N; j += 4) //每次取四个
        {
            t2 = _mm_load_ps(matrix[k] + j);
            if(tmp[0] !=0 )
            {
                t3 = _mm_div_ps(t2, t1);//除法
                _mm_store_ps(matrix[k] + j, t3);
            }
        }
        for (; j<N; j++)
        {
            if(tmp[0] !=0 )
                matrix[k][j] = matrix[k][j] / tmp[0];
        }
        /*if (k % 4 != (N % 4)) //处理不能被4整除的元素
        {
            for (int j = N; j % 4 != ( N% 4); j--)
            {
                if(tmp[0] !=0 )
                    matrix[k][j] = matrix[k][j] / tmp[0];
            }
        }*//*
        for (int i = k + 1; i < N; i++)
        {
            float tmp[4] = { matrix[i][k], matrix[i][k], matrix[i][k], matrix[i][k] };
            t1 = _mm_loadu_ps(tmp);
            for (int j = N - 4; j >k;j -= 4)
            {
                t2 = _mm_loadu_ps(matrix[i] + j);
                t3 = _mm_loadu_ps(matrix[k] + j);
                t4 = _mm_sub_ps(t2,_mm_mul_ps(t1, t3)); //减法
                _mm_storeu_ps(matrix[i] + j, t4);
            }
            for (int j = k + 1; j % 4 !=(N % 4); j++)
            {
                matrix[i][j] = matrix[i][j] - matrix[i][k] * matrix[k][j];
            }
            matrix[i][k] = 0;
        }
    }
}
*/


int main()
{
	int step = 10;
	for(int n = 0;n<=5000;n += step)
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
		//std::cout<<n<<' '<<counter<<' '<<seconds<<' '
		std::cout<<seconds / counter <<std::endl;
		/*for(int i = 0;i<n;i++)
		{
			for(int j = 0;j<n;j++)
			{
				std::cout<<matrix[i][j]<<' ';
			}
			std::cout<<std::endl;
		}
*/
		if(n==100)
		{
			step = 300;
		}
		if(n==1000)
			step = 500;
	}
	return 0;
}



/*
int main()
{
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
	Guass(matrix,n);
	for(int i = 0;i<n;i++)
	{
		for(int j = 0;j<n;j++)
		{
			std::cout<<matrix[i][j]<<' ';
		}
		std::cout<<std::endl;
	}
	return 0;
}
*/
