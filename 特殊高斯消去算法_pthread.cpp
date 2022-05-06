#include <iostream>
#include <fstream>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#define THREAD_NUM 7

int n1;
int n2;
int m;
int** matrix;
int** matrix2;
int** matrix3;

pthread_mutex_t amutex = PTHREAD_MUTEX_INITIALIZER;
sem_t sem_leader;
sem_t sem_minus[THREAD_NUM-1];

typedef struct{
	int t_id;
}threadParam_t;


void minus(int i,int s,int n1,int n2,int m)//int** matrix,int** matrix2,
{
	int* number = new int[m];
	int k = 0,j = 0,t = 0;
	while(j<m&&matrix[i][j]!=-1&&t<m&&matrix2[s][t]!=-1)
	{
		if(matrix[i][j]>matrix2[s][t])
		{
			number[k] = matrix[i][j];
			k++;
			j++;
		}
		else
		{
			if(matrix[i][j]==matrix2[s][t])
			{
				j++;
				t++;
			}
			else
			{
				if(matrix[i][j]<matrix2[s][t])
				{
					number[k] = matrix2[s][t];
					k++;
					t++;
				}
			}
		}
	}
	while(matrix[i][j]!=-1)
	{
		number[k] = matrix[i][j];
		k++;
		j++;
	}
	while(matrix2[s][t]!=-1)
	{
		number[k] = matrix2[s][t];
		k++;
		t++;
	}
	for(j = 0;j < m;j++)
	{
		if(j < k)
			matrix[i][j] = number[j];
		else
			matrix[i][j] = -1;
	}
}

void *threadFunc(void *param)
{
	threadParam_t *p = (threadParam_t*)param;
	int t_id = p->t_id;


	for(int i = t_id;i<n1&&matrix[i][0]!=-1;i+=THREAD_NUM)
	{
		for(int s = 0;s<n2&&matrix2[s][0]!=-1;s++)
		{
			if(matrix[i][0] == matrix2[s][0])
			{
				minus(i,s,n1,n2,m);//matrix,matrix2,
				s = 0;
			}
		}

	}

	if (t_id == 0)
    {
        for (int i = 0; i < THREAD_NUM-1; ++i)
        {
            sem_wait(&sem_leader); // 等待其它worker完成相减
        }
        for (int i = 0; i < THREAD_NUM-1; ++i)
        {
            sem_post(&sem_minus[i]); // 通知其它worker继续
        }
    }
    else
    {
        sem_post(&sem_leader);// 通知 leader, 已完成消去任务
        sem_wait(&sem_minus[t_id-1]); // 等待通知，进入下一轮
    }

	if(t_id==0)
    {
	for(int i = 0;i<n1;i++)
	{


//		pthread_mutex_lock(&amutex);
/*		for(int s = 0;s<n2&&matrix3[s][0]!=-1;s++)
		{
			if(matrix[i][0] == matrix3[s][0])
			{
				minus(matrix,matrix3,i,s,n1,n2,m);
				s = 0;
			}
		}*/

		for(int s = 0;s<n2&&matrix2[s][0]!=-1;s++)
		{
			if(matrix[i][0] == matrix2[s][0])
			{
				minus(i,s,n1,n2,m);//matrix,matrix2,
				s = 0;
			}
		}

		for(int j = 0;j<m&&matrix[i][j]!=-1;j++)
		{
			if(matrix[i][j]!=-1)
			{
				int k = 0;
				while(matrix2[k][0]!=-1)
					k++;
				for(int t = 0;t < m;t++)
				{
					matrix2[k][t] = matrix[i][t];
				}
				break;
			}
		}
//		pthread_mutex_unlock(&amutex);
	}
    }
	pthread_exit(NULL);
}


int** Guass(int n1,int n2,int m)//被消元行有n1行m列，消元子共n2行//int** matrix,int** matrix2,
{

    int worker_count = THREAD_NUM; //工作线程数量

    pthread_mutex_init(&amutex,NULL);
    sem_init(&sem_leader,0,0);
    for (int i = 0; i < worker_count-1; ++i)
	{
        sem_init(&sem_minus[i], 0, 0);
	}

    pthread_t* handles = new pthread_t[THREAD_NUM];// 创建对应的 Handle
    threadParam_t* param = new threadParam_t[THREAD_NUM];// 创建对应的线程数据结构
    //分配任务
    for(int t_id = 0; t_id < worker_count; t_id++)
    {
        param[t_id].t_id = t_id;
    }
    for(int t_id = 0; t_id < worker_count; t_id++)
    {
        pthread_create(&handles[t_id],NULL,threadFunc,(void*)&param[t_id]);
    }
    //主线程挂起等待所有的工作线程完成此轮消去工作
    for(int t_id = 0; t_id < worker_count; t_id++)
    {
        pthread_join(handles[t_id],NULL);
    }
    //销毁
    sem_destroy(&sem_leader);
    for (int t_id = 0; t_id < worker_count; ++t_id)
    {
        sem_destroy(&sem_minus[t_id]);
    }

	return matrix2;
}

int main()
{
    float seconds = 0;
    for(int z = 0;z < 100;z++)
    {
	n1 = 1000;
	n2 = 1000;
	m = 1000;
	matrix = new int*[n1];
	matrix2 = new int*[n2];
	for(int i = 0;i < n1;i++)
	{
		matrix[i] = new int[m];
		for(int j = 0;j < m;j++)
		{
			matrix[i][j] = -1;
		}
	}
	for(int i = 0; i < n2;i++)
	{
		matrix2[i] = new int[m];
		for(int j = 0;j < m;j++)
		{
			matrix2[i][j] = -1;
		}
	}

	matrix3 = new int*[n2];
	for(int k = 0;k < n2;k++ )
    {
        matrix3[k] = new int[m];
        for(int a = 0;a < m;a++)
        {
            matrix3[k][a] = -1;
        }
    }

	int i = 0,j = 0;
	char num;
	int number = 0;
	std::ifstream text("2.txt",std::ios::in);
	while(!text.eof())
	{
		text.get(num);
		if(num==' ')
		{
			matrix[i][j] = number;
			number = 0;
			j++;
		}
		if(num=='\n')
		{
			i++;
			j = 0;
		}
		if(num!=' '&&num!='\n')
		{
			number *= 10;
			number += num - '0';
		}
	}
	text.close();
	n1 = i;
	i = 0;
	j = 0;
	number = 0;
	std::ifstream text2("1.txt",std::ios::in);
	while(!text2.eof())
	{
		text2.get(num);
		if(num==' ')
		{
			matrix2[i][j] = number;
			number = 0;
			j++;
		}
		if(num=='\n')
		{
			i++;
			j = 0;
		}
		if(num!=' '&&num!='\n')
		{
			number *= 10;
			number += num - '0';
		}
	}
	text2.close();
	time_t start = clock();
	Guass(n1,n2,m);//matrix,matrix2,
    time_t finish = clock();
    seconds += finish - start;

/*	for(int i = 0;i<n2&&matrix2[i][0]!=-1;i++)
	{
		for(int j = 0;j<m&&matrix2[i][j]!=-1;j++)
		{
			std::cout<<matrix2[i][j]<<' ';
		}
		std::cout<<std::endl;
	}*/

/*	for(int i = 0;i<n2&&matrix3[i][0]!=-1;i++)
	{
		for(int j = 0;j<m&&matrix3[i][j]!=-1;j++)
		{
			std::cout<<matrix3[i][j]<<' ';
		}
		std::cout<<std::endl;
	}*/
    }
    seconds /= float (CLOCKS_PER_SEC);
    seconds /= 100.0;
    std::cout<<seconds<<std::endl;
	return 0;
}
