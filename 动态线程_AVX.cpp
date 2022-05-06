#include <iostream>
#include <time.h>
#include <random>
#include<pthread.h>
#include <immintrin.h>

#define THREAD_NUM 4

int n;
float** matrix;
typedef struct{
	int k;
	int t_id;
}threadParam_t;

void *threadFunc(void *param)
{
	threadParam_t *p = (threadParam_t*)param;
	int k = p->k;
	int t_id = p->t_id;
//	int s = (n - k - 1)/THREAD_NUM;
	int i = k + t_id + 1 ;

//	std::cout<<t_id<<' '<<s<<' '<<i<<std::endl;

	while(i < n)
    {
/*        for(int j = k+1;j<n;j++)
        {
            matrix[i][j] = matrix[i][j] - matrix[i][k] * matrix[k][j];
        }
        matrix[i][k] = 0;*/

        __m256 t1, t2, t3, t4;

        float tmp[8] = { matrix[i][k], matrix[i][k], matrix[i][k], matrix[i][k],matrix[i][k], matrix[i][k], matrix[i][k], matrix[i][k] };
        t1 = _mm256_loadu_ps(tmp);
        for (int j = n - 8; j >k;j -= 8)
        {
            t2 = _mm256_loadu_ps(matrix[i] + j);
            t3 = _mm256_loadu_ps(matrix[k] + j);
            t4 = _mm256_sub_ps(t2,_mm256_mul_ps(t1, t3)); //减法
            _mm256_storeu_ps(matrix[i] + j, t4);
        }
        for (int j = k + 1; j % 8 !=(n % 8); j++)
        {
            matrix[i][j] = matrix[i][j] - matrix[i][k] * matrix[k][j];
        }
        matrix[i][k] = 0;

        i+=THREAD_NUM;
    }
	pthread_exit(NULL);
}

void Guass()//(float** matrix, int n)
{
    __m256 t1, t2, t3, t4;
	for(int k = 0;k<n;k++)
	{
		/*float temp = matrix[k][k];
		for(int i = 0;i<n;i++)
		{
			if(temp != 0)
				matrix[k][i] = matrix[k][i]/temp;
		}*/

		float tmp[8] = { matrix[k][k], matrix[k][k], matrix[k][k], matrix[k][k],matrix[k][k], matrix[k][k], matrix[k][k], matrix[k][k] };
        t1 = _mm256_loadu_ps(tmp);
        for (int j = n - 8; j >=k; j -= 8) //从后向前每次取四个
        {
            t2 = _mm256_loadu_ps(matrix[k] + j);
            if(tmp[0] !=0 )
            {
                t3 = _mm256_div_ps(t2, t1);//除法
                _mm256_storeu_ps(matrix[k] + j, t3);
            }
        }
        if (k % 8 != (n % 8)) //处理不能被4整除的元素
        {
            for (int j = k; j % 8 != ( n% 8); j++)
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


		//创建工作线程，进行消去操作
		int worker_count = THREAD_NUM; //工作线程数量
		pthread_t* handles = new pthread_t[THREAD_NUM];// 创建对应的 Handle
		threadParam_t* param = new threadParam_t[THREAD_NUM];// 创建对应的线程数据结构
		//分配任务
		for(int t_id = 0; t_id < worker_count; t_id++)
		{
			param[t_id].k = k;
			param[t_id].t_id = t_id;
		}
		//创建线程
		for(int t_id = 0; t_id < worker_count; t_id++)
		{
			pthread_create(&handles[t_id],NULL,threadFunc,(void*)&param[t_id]);
		}
		//主线程挂起等待所有的工作线程完成此轮消去工作
		for(int t_id = 0; t_id < worker_count; t_id++)
		{
			pthread_join(handles[t_id],NULL);
		}
	}
/*
		for(int i = k+1;i<n;i++)
		{
			for(int j = k+1;j<n;j++)
			{
				matrix[i][j] -= matrix[i][k] * matrix[k][j];
			}
			matrix[i][k] = 0;
		}
	}*/
}

int main()
{
	int step = 10;
	for( n = 0;n<=10000;n += step)
	{
//	n = 10;
		std::random_device rd;
		std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(0.0,100.0);
		matrix = new float*[n];
		for(int i = 0;i< n;i++)
		{
			matrix[i] = new float[n];
		}
		for(int i = 0;i<n;i++)
		{
			for(int j = 0;j<n;j++)
			{
			    if(j>=i)
					matrix[i][j] = i + j + 1;
				else
					matrix[i][j] = dis(gen);
			}
		}
		clock_t start,finish;
		start = clock();
		int counter = 0;
		while(clock() - start < 30)
		{
			counter++;
			Guass();
		}
		finish = clock();
		float seconds = (finish - start)/float (CLOCKS_PER_SEC);
		std::cout<<seconds / counter <<std::endl;
		//std::cout<<n<<" "<<counter<<" "<<seconds<<" "<<seconds / counter <<std::endl;
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

