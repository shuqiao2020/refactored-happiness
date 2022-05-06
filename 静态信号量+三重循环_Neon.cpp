#include <iostream>
#include <time.h>
#include <random>
#include<pthread.h>
#include <semaphore.h>
#include <arm_neon.h>

#define THREAD_NUM 4

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

	float32x4_t t1,t2,t3,t4;

	for (int k = 0; k < n; ++k)
	{
		// t_id 为 0 的线程做除法操作，其它工作线程先等待
		// 这里只采用了一个工作线程负责除法操作，同学们可以尝试采用多个工作线程完成除法操作
		// 比信号量更简洁的同步方式是使用 barrier
		if (t_id == 0)
		{
		    float temp[4] = {matrix[k][k],matrix[k][k],matrix[k][k],matrix[k][k]};
            t2 = vld1q_f32(temp);
            for(int j = n-4;j >= k;j -= 4)
            {
                t1 = vld1q_f32(matrix[k] + j);
                //temp = vrecpeq_f32(temp);
                if(temp[0]!=0)
                {
                    t3 = vdivq_f32(t1,t2);
                    vst1q_f32(matrix[k] + j,t3);
                }
            }
            if (k % 4 != (n % 4))
            {
                for(int j = k;j%4 != n%4;j++)
                {
                    if(temp[0]!=0)
                    {
                        matrix[k][j] /= temp[0];
                    }
                }
            }
            for (int j = k - 1; j>= 0; j--)
            {
                if(temp[0] !=0 )
                    matrix[k][j] = matrix[k][j] / temp[0];
            }

/*			for (int j = k+1; j < n; j++)
			{
			    if(matrix[k][k]!=0)
                    matrix[k][j ] = matrix[k][j] / matrix[k][k];
			}
			if(matrix[k][k]!=0)
                matrix[k][k] = 1.0;*/
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
		}
		//循环划分任务（同学们可以尝试多种任务划分方式）
		for(int i=k+1+t_id; i < n; i += THREAD_NUM)
		{
			//消去

			float t0[4] = {matrix[i][k],matrix[i][k],matrix[i][k],matrix[i][k]};
            t2 = vld1q_f32(t0);
            for(int j = n - 4;j > k;j -=4)
            {
                t1 = vld1q_f32(matrix[i] + j);
                t3 = vld1q_f32(matrix[k] + j);
                t4 = vmulq_f32(t2,t3);
                t4 = vsubq_f32(t1,t4);
                vst1q_f32(matrix[i] + j,t4);
            }
            for(int j = k + 1;j%4 != n%4;j++)
            {
                matrix[i][j] = matrix[i][j] - matrix[i][k] * matrix[k][j];
            }
            matrix[i][k] = 0;

/*			for (int j = k + 1; j < n; ++j)
			{
				matrix[i ][ j ] = matrix[i][j ] - matrix[i][k] * matrix[k][j ];
			}
			matrix[i ][ k]=0.0;*/
		}
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
