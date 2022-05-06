#include <iostream>
#include <time.h>
#include <random>
#include<pthread.h>
#include <semaphore.h>

#define THREAD_NUM 7

float** matrix;
int n;
//线程数据结构定义
typedef struct{
	int t_id;
}threadParam_t;
//信号量定义
sem_t sem_leader;
sem_t sem_Divsion[THREAD_NUM-1];
sem_t sem_Elimination[THREAD_NUM-1];
//线程函数定义
void *threadFunc(void *param) {
	threadParam_t *p = (threadParam_t*)param;
	int t_id = p -> t_id;
	for (int k = 0; k < n; ++k)
	{
        int s = (n - k - 1)/THREAD_NUM;
        int t = (n - k - 1)%THREAD_NUM;
		// t_id 为 0 的线程做除法操作，其它工作线程先等待
		// 这里采用了多个工作线程完成除法操作
		// 比信号量更简洁的同步方式是使用 barrier
/*		if (t_id == 0)
		{
			for (int j = k+1; j < n; j++)
			{
			    if(matrix[k][k]!=0)
                    matrix[k][j ] = matrix[k][j] / matrix[k][k];
			}
			if(matrix[k][k]!=0)
                matrix[k][k] = 1.0;
		}
		else
		{
			sem_wait(&sem_Divsion[t_id-1]); // 阻塞，等待完成除法操作
		}
		// t_id 为 0 的线程唤醒其它工作线程，进行消去操作
		if (t_id == 0)
		{
			for (int i = 0; i < THREAD_NUM-1; ++i)
			{
				sem_post(&sem_Divsion[i]);
			}
		}*/


		//除法(并行优化)
		for (int j = t_id + k+1; j < n; j+=THREAD_NUM)
		{
			if(matrix[k][k]!=0)
				matrix[k][j] = matrix[k][j] / matrix[k][k];
		}
		if (t_id == 0)//同步
		{
			for (int i = 0; i < THREAD_NUM-1; ++i)
			{
				sem_wait(&sem_leader); // 等待其它worker完成除法
			}
			for (int i = 0; i < THREAD_NUM-1; ++i)
			{
				sem_post(&sem_Divsion[i]); // 通知其它worker继续
			}
			if(matrix[k][k]!=0)
                matrix[k][k] = 1.0;
		}
		else
		{
			sem_post(&sem_leader);// 通知 leader, 已完成消去任务
			sem_wait(&sem_Divsion[t_id-1]); // 等待通知，进入下一轮
		}


		//循环划分任务（同学们可以尝试多种任务划分方式）
		int a,b;
		if(t_id < t)
            a = k+1+t_id * (s + 1 *(bool)t);
        else
            a = k+1+t_id * s + t;
        if(t_id + 1 < t)
            b = k+1+(t_id + 1) * (s + 1 *(bool)t);
        else
            b = k+1+(t_id + 1) * s + t;
		for(int i=a; i < b ; i ++)
		{
			//消去
			for (int j = k + 1; j < n; ++j)
			{
				matrix[i ][ j ] = matrix[i][j] - matrix[i][k] * matrix[k][j];
			}
			matrix[i ][ k]=0.0;
		}

/*		for(int i=k+1+t_id; i < n; i += THREAD_NUM)
		{
			//消去
			for (int j = k + 1; j < n; ++j)
			{
				matrix[i ][ j ] = matrix[i][j] - matrix[i][k] * matrix[k][j];
			}
			matrix[i ][ k]=0.0;
		}*/


		if (t_id == 0)
		{
			for (int i = 0; i < THREAD_NUM-1; ++i)
			{
				sem_wait(&sem_leader); // 等待其它 worker 完成消去
			}
			for (int i = 0; i < THREAD_NUM-1; ++i)
			{
				sem_post(&sem_Elimination[i]); // 通知其它 worker 进入下一轮
			}
		}
		else
		{
			sem_post(&sem_leader);// 通知 leader, 已完成消去任务
			sem_wait(&sem_Elimination[t_id-1]); // 等待通知，进入下一轮
		}
	}
	pthread_exit(NULL);
}

void Guass()//(float** matrix, int n)
{
	//初始化信号量
	sem_init(&sem_leader, 0, 0);
	for (int i = 0; i < THREAD_NUM-1; ++i)
	{
        sem_init(&sem_Divsion[i], 0, 0);
		sem_init(&sem_Elimination[i], 0, 0);
	}
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
	// 销毁所有信号量
	sem_destroy(&sem_leader);
	for (int t_id = 0; t_id < THREAD_NUM; ++t_id)
    {
        sem_destroy(&sem_Divsion[t_id]);
        sem_destroy(&sem_Elimination[t_id]);
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
	return;
}

int main()
{
	int step = 10;
	for(n = 0;n<=5000;n += step)
	{
//        n = 10;
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
