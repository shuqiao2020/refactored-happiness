#include <iostream>
#include <time.h>
#include <random>
#include<pthread.h>
#include <semaphore.h>
#include <immintrin.h>

#define THREAD_NUM 4

float** matrix;
int n;
typedef struct{
	int t_id;
}threadParam_t;
//�߳����ݽṹ����

//�ź�������
sem_t sem_main;
sem_t sem_workerstart[THREAD_NUM]; // ÿ���߳����Լ�ר�����ź���
sem_t sem_workerend[THREAD_NUM];
//�̺߳�������
void *threadFunc(void *param) {
	threadParam_t *p = (threadParam_t*)param;
	int t_id = p -> t_id;
	for (int k = 0; k < n; ++k)
	{
		sem_wait(&sem_workerstart[t_id]); // �������ȴ�������ɳ��������������Լ�ר�����ź�����
		//ѭ����������
		for(int i=k+1+t_id; i < n; i += THREAD_NUM)
		{
			//��ȥ

			__m256 t1, t2, t3, t4;

            float tmp[8] = { matrix[i][k], matrix[i][k], matrix[i][k], matrix[i][k],matrix[i][k], matrix[i][k], matrix[i][k], matrix[i][k] };
            t1 = _mm256_loadu_ps(tmp);
            for (int j = n - 8; j >k;j -= 8)
            {
                t2 = _mm256_loadu_ps(matrix[i] + j);
                t3 = _mm256_loadu_ps(matrix[k] + j);
                t4 = _mm256_sub_ps(t2,_mm256_mul_ps(t1, t3)); //����
                _mm256_storeu_ps(matrix[i] + j, t4);
            }
            for (int j = k + 1; j % 8 !=(n % 8); j++)
            {
                matrix[i][j] = matrix[i][j] - matrix[i][k] * matrix[k][j];
            }
            matrix[i][k] = 0;

/*			for (int j = k + 1; j < n; ++j)
			{
				matrix[i][j] = matrix[i][j]  - matrix[i][k] * matrix[k][j];
			}
			matrix[i][ k]=0.0;*/
		}
		sem_post(&sem_main); // �������߳�
		sem_wait(&sem_workerend[t_id]); //�������ȴ����̻߳��ѽ�����һ��
	}
	pthread_exit(NULL);
}
void Guass()//(float** matrix, int n)
{
	//��ʼ���ź���
	sem_init(&sem_main, 0, 0);
	for (int i = 0; i < THREAD_NUM; ++i)
	{
		sem_init(&sem_workerstart[i], 0, 0);
		sem_init(&sem_workerend[i], 0, 0);
	}
	//�����߳�
	pthread_t handles[THREAD_NUM];// ������Ӧ�� Handle
	threadParam_t param[THREAD_NUM];// ������Ӧ���߳����ݽṹ
	for(int t_id = 0; t_id < THREAD_NUM; t_id++)
	{
		param[t_id].t_id = t_id;
		pthread_create(&handles[t_id],NULL,threadFunc,(void*)&param[t_id]);
	}

	__m256 t1, t2, t3, t4;

	for(int k = 0; k < n; ++k)
	{
		//���߳�����������

		float tmp[8] = { matrix[k][k], matrix[k][k], matrix[k][k], matrix[k][k],matrix[k][k], matrix[k][k], matrix[k][k], matrix[k][k] };
        t1 = _mm256_loadu_ps(tmp);
        for (int j = n - 8; j >=k; j -= 8) //�Ӻ���ǰÿ��ȡ�ĸ�
        {
            t2 = _mm256_loadu_ps(matrix[k] + j);
            if(tmp[0] !=0 )
            {
                t3 = _mm256_div_ps(t2, t1);//����
                _mm256_storeu_ps(matrix[k] + j, t3);
            }
        }
        if (k % 8 != (n % 8)) //�����ܱ�4������Ԫ��
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


/*		for (int j = k+1; j < n; j++)
		{
		    if(matrix[k][k]!=0)
			matrix[k][j] = matrix[k][j] / matrix[k][k];
		}
		if(matrix[k][k]!=0)
            matrix[k][k] = 1.0;*/
		//��ʼ���ѹ����߳�
		for (int t_id = 0; t_id < THREAD_NUM; ++t_id)
		{
			sem_post(&sem_workerstart[t_id]);
		}
		//���߳�˯�ߣ��ȴ����еĹ����߳���ɴ�����ȥ����
		for (int t_id = 0; t_id < THREAD_NUM; ++t_id)
		{
			sem_wait(&sem_main);
		}
		// ���߳��ٴλ��ѹ����߳̽�����һ�ִε���ȥ����
		for (int t_id = 0; t_id < THREAD_NUM; ++t_id)
		{
			sem_post(&sem_workerend[t_id]);
		}
	}
	for(int t_id = 0; t_id < THREAD_NUM; t_id++)
	{
		pthread_join(handles[t_id],NULL);
	}
	//���������ź���
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
	for(n = 0;n<=10000;n += step)
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
