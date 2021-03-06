#include <iostream>
#include <time.h>
#include <random>
#include<pthread.h>
#include <arm_neon.h>

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

	    float32x4_t t1,t2,t3,t4;

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

        i+=THREAD_NUM;
    }
	pthread_exit(NULL);
}

void Guass()//(float** matrix, int n)
{
    float32x4_t t1,t2,t3,t4;
	for(int k = 0;k<n;k++)
	{
		/*float temp = matrix[k][k];
		for(int i = 0;i<n;i++)
		{
			if(temp != 0)
				matrix[k][i] = matrix[k][i]/temp;
		}*/

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

		//??????????????????????????
		int worker_count = THREAD_NUM; //????????????
		pthread_t* handles = new pthread_t[THREAD_NUM];// ?????????? Handle
		threadParam_t* param = new threadParam_t[THREAD_NUM];// ??????????????????????
		//????????
		for(int t_id = 0; t_id < worker_count; t_id++)
		{
			param[t_id].k = k;
			param[t_id].t_id = t_id;
		}
		//????????
		for(int t_id = 0; t_id < worker_count; t_id++)
		{
			pthread_create(&handles[t_id],NULL,threadFunc,(void*)&param[t_id]);
		}
		//????????????????????????????????????????????
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
	for( n = 0;n<=5000;n += step)
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
//			    if(j>=i)
					matrix[i][j] = i + j + 1;
//				else
//					matrix[i][j] = dis(gen);
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

