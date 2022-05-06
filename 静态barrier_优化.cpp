#include <iostream>
#include <time.h>
#include <random>
#include<pthread.h>

#define THREAD_NUM 10

float** matrix;
int n;
//�߳����ݽṹ����
typedef struct{
	int t_id;
}threadParam_t;
//barrier ����
pthread_barrier_t barrier_Divsion;
pthread_barrier_t barrier_Elimination;
//�̺߳�������
void *threadFunc(void *param) {
	threadParam_t *p = (threadParam_t*)param;
	int t_id = p -> t_id;
	for (int k = 0; k < n; ++k)
	{
		int s = (n - k - 1)/THREAD_NUM;
        int t = (n - k - 1)%THREAD_NUM;
		// t_id Ϊ 0 ���߳����������������������߳��ȵȴ�
		// ����ֻ������һ�������̸߳������������ͬѧ�ǿ��Գ��Բ��ö�������߳���ɳ�������
	/*	if (t_id == 0)
		{
			for (int j = k+1; j < n; j++)
			{
			    if(matrix[k][k]!=0)
                    matrix[k][j] = matrix[k][j] / matrix[k][k];
			}
			if(matrix[k][k]!=0)
                matrix[k][k] = 1.0;
		}*/

		//����(�����Ż�)
		for (int j = t_id + k+1; j < n; j+=THREAD_NUM)
		{
			if(matrix[k][k]!=0)
				matrix[k][j] = matrix[k][j] / matrix[k][k];
		}


		//��һ��ͬ����
		pthread_barrier_wait(&barrier_Divsion);

		//ѭ����������ͬѧ�ǿ��Գ��Զ������񻮷ַ�ʽ��
/*		for(int i=k+1+t_id; i < n; i += THREAD_NUM)
		{
			//��ȥ
			for (int j = k + 1; j < n; ++j)
			{
				matrix[i ][ j ] = matrix[i][j ] - matrix[i][k] * matrix[k][j ];
			}
			matrix[i ][ k]=0.0;
		}*/
		//ѭ����������ͬѧ�ǿ��Գ��Զ������񻮷ַ�ʽ��
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
			//��ȥ
			for (int j = k + 1; j < n; ++j)
			{
				matrix[i ][ j ] = matrix[i][j] - matrix[i][k] * matrix[k][j];
			}
			matrix[i ][ k]=0.0;
		}
		// �ڶ���ͬ����
		pthread_barrier_wait(&barrier_Elimination);
	}
	pthread_exit(NULL);
}

void Guass(float** matrix, int n)
{
	//��ʼ�� barrier
	pthread_barrier_init(&barrier_Divsion, NULL, THREAD_NUM);
	pthread_barrier_init(&barrier_Elimination, NULL, THREAD_NUM);
	//�����߳�
	pthread_t handles[THREAD_NUM];// ������Ӧ�� Handle
	threadParam_t param[THREAD_NUM];// ������Ӧ���߳����ݽṹ
	for(int t_id = 0; t_id < THREAD_NUM; t_id++)
	{
		param[t_id].t_id = t_id;
		pthread_create(&handles[t_id],NULL,threadFunc,(void*)&param[t_id]);
	}
	for(int t_id = 0; t_id < THREAD_NUM; t_id++)
	{
		pthread_join(handles[t_id],NULL);
	}
	//�������е� barrier
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
