#include <iostream>
#include <time.h>
#include <random>
#include<pthread.h>
#include <nmmintrin.h>

#define THREAD_NUM 7

float** matrix;
int n;
//线程数据结构定义
typedef struct{
	int t_id;
}threadParam_t;
//barrier 定义
pthread_barrier_t barrier_Divsion;
pthread_barrier_t barrier_Elimination;
//线程函数定义
void *threadFunc(void *param) {
	threadParam_t *p = (threadParam_t*)param;
	int t_id = p -> t_id;

	__m128 t1, t2, t3, t4;

	for (int k = 0; k < n; ++k)
	{
		// t_id 为 0 的线程做除法操作，其它工作线程先等待
		// 这里只采用了一个工作线程负责除法操作，同学们可以尝试采用多个工作线程完成除法操作
/*			for (int j = k+1; j < n; j++)
			{
			    if(matrix[k][k]!=0)
                    matrix[k][j] = matrix[k][j] / matrix[k][k];
			}
			if(matrix[k][k]!=0)
                matrix[k][k] = 1.0;
		}*/

			float tmp[4] = { matrix[k][k], matrix[k][k], matrix[k][k], matrix[k][k] };
            t1 = _mm_loadu_ps(tmp);
            for (int j = n - 4; j >=k; j -= 4) //从后向前每次取四个
            {
                t2 = _mm_loadu_ps(matrix[k] + j);
                if(tmp[0] !=0 )
                {
                    t3 = _mm_div_ps(t2, t1);//除法
                    _mm_storeu_ps(matrix[k] + j, t3);
                }
            }
            if (k % 4 != (n % 4)) //处理不能被4整除的元素
            {
                for (int j = k; j % 4 != ( n% 4); j++)
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

		//第一个同步点
		pthread_barrier_wait(&barrier_Divsion);

		//循环划分任务（同学们可以尝试多种任务划分方式）

/*		for(int i=k+1+t_id; i < n; i += THREAD_NUM)
		{
			//消去
			for (int j = k + 1; j < n; ++j)
			{
				matrix[i ][ j ] = matrix[i][j ] - matrix[i][k] * matrix[k][j ];
			}
			matrix[i ][ k]=0.0;
		}*/

		for(int i=k+1+t_id; i < n; i += THREAD_NUM)
		{
			//消去

			float tmp[4] = { matrix[i][k], matrix[i][k], matrix[i][k], matrix[i][k] };
            t1 = _mm_loadu_ps(tmp);
            for (int j = n - 4; j >k;j -= 4)
            {
                t2 = _mm_loadu_ps(matrix[i] + j);
                t3 = _mm_loadu_ps(matrix[k] + j);
                t4 = _mm_sub_ps(t2,_mm_mul_ps(t1, t3)); //减法
                _mm_storeu_ps(matrix[i] + j, t4);
            }
            for (int j = k + 1; j % 4 !=(n % 4); j++)
            {
                matrix[i][j] = matrix[i][j] - matrix[i][k] * matrix[k][j];
            }
            matrix[i][k] = 0;
		}

		// 第二个同步点
		pthread_barrier_wait(&barrier_Elimination);
	}
	pthread_exit(NULL);
}

void Guass(float** matrix, int n)
{
	//初始化 barrier
	pthread_barrier_init(&barrier_Divsion, NULL, THREAD_NUM);
	pthread_barrier_init(&barrier_Elimination, NULL, THREAD_NUM);
	//创建线程
	pthread_t handles[THREAD_NUM];// 创建对应的 Handle
	threadParam_t param[THREAD_NUM];// 创建对应的线程数据结构
	for(int t_id = 0; t_id < THREAD_NUM; t_id++)
	{
		param[t_id].t_id = t_id;
		pthread_create(&handles[t_id],NULL,threadFunc,(void*)&param[t_id]);
	}
	for(int t_id = 0; t_id < THREAD_NUM; t_id++)
	{
		pthread_join(handles[t_id],NULL);
	}
	//销毁所有的 barrier
//	for (int t_id = 0; t_id < THREAD_NUM; ++t_id)
//    {
        pthread_barrier_destroy(&barrier_Divsion);
        pthread_barrier_destroy(&barrier_Elimination);
//    }
	//pthread_barrier_destroy();
	return;
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
	for(n = 0;n<=5000;n += step)
	{
//	    n = 10;
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
