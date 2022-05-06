#include <iostream>
#include <time.h>
#include <random>
#include<pthread.h>
#include <semaphore.h>

#define THREAD_NUM 10

float** matrix;
int n;
typedef struct{
	int t_id;
}threadParam_t;
//线程数据结构定义

//信号量定义
sem_t sem_main;
sem_t sem_workerstart[THREAD_NUM]; // 每个线程有自己专属的信号量
sem_t sem_workerend[THREAD_NUM];
//线程函数定义
void *threadFunc(void *param) {
	threadParam_t *p = (threadParam_t*)param;
	int t_id = p -> t_id;
	for (int k = 0; k < n; ++k)
	{
		sem_wait(&sem_workerstart[t_id]); // 阻塞，等待主线完成除法操作（操作自己专属的信号量）
		//循环划分任务
		for(int i=k+1+t_id; i < n; i += THREAD_NUM)
		{
			//消去
			for (int j = k + 1; j < n; ++j)
			{
				matrix[i][j] = matrix[i][j]  - matrix[i][k] * matrix[k][j];
			}
			matrix[i][ k]=0.0;
		}
		sem_post(&sem_main); // 唤醒主线程
		sem_wait(&sem_workerend[t_id]); //阻塞，等待主线程唤醒进入下一轮
	}
	pthread_exit(NULL);
}
void Guass()//(float** matrix, int n)
{
	//初始化信号量
	sem_init(&sem_main, 0, 0);
	for (int i = 0; i < THREAD_NUM; ++i)
	{
		sem_init(&sem_workerstart[i], 0, 0);
		sem_init(&sem_workerend[i], 0, 0);
	}
	//创建线程
	pthread_t handles[THREAD_NUM];// 创建对应的 Handle
	threadParam_t param[THREAD_NUM];// 创建对应的线程数据结构
	for(int t_id = 0; t_id < THREAD_NUM; t_id++)
	{
		param[t_id].t_id = t_id;
		pthread_create(&handles[t_id],NULL,threadFunc,(void*)&param[t_id]);
	}
	for(int k = 0; k < n; ++k)
	{
		//主线程做除法操作
		for (int j = k+1; j < n; j++)
		{
		    if(matrix[k][k]!=0)
			matrix[k][j] = matrix[k][j] / matrix[k][k];
		}
		if(matrix[k][k]!=0)
            matrix[k][k] = 1.0;
		//开始唤醒工作线程
		for (int t_id = 0; t_id < THREAD_NUM; ++t_id)
		{
			sem_post(&sem_workerstart[t_id]);
		}
		//主线程睡眠（等待所有的工作线程完成此轮消去任务）
		for (int t_id = 0; t_id < THREAD_NUM; ++t_id)
		{
			sem_wait(&sem_main);
		}
		// 主线程再次唤醒工作线程进入下一轮次的消去任务
		for (int t_id = 0; t_id < THREAD_NUM; ++t_id)
		{
			sem_post(&sem_workerend[t_id]);
		}
	}
	for(int t_id = 0; t_id < THREAD_NUM; t_id++)
	{
		pthread_join(handles[t_id],NULL);
	}
	//销毁所有信号量
    for (int t_id = 0; t_id < THREAD_NUM; ++t_id)
    {
        sem_destroy(&sem_workerstart[t_id]);
        sem_destroy(&sem_workerend[t_id]);
    }
	//sem_destroy();
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
//		float**
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
		while(clock() - start < 30||counter<10)
		{
			counter++;
//			Guass(matrix,n);
			Guass();
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
