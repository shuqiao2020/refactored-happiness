#include <iostream>
#include <time.h>
#include <omp.h>
#include <arm_neon.h>

#define NUM_THREADS 4



void Guass(float** matrix, int n)
{
	#pragma omp parallel num_threads(NUM_THREADS)
    for(int k=0;k<n;k++)
    {
        //对于openMP,只将该任务交给一个线程来执行
		#pragma omp single
		{
            float32x4_t vt=vdupq_n_f32(matrix[k][k]);
            int j;
            for(j=k+1;j+4<=n;j+=4)
            {
                //将四个单精度浮点数从内存加载到向量寄存器中
                //va<-load4FloatFrom(&A[k,j]);
                float32x4_t va=vld1q_f32(&matrix[k][j]);
                //向量对位相除
                //va<-va/vt;
                va=vdivq_f32(va,vt);
                //将四个单精度浮点数从向量寄存器存储到内存
                //store4FloatTo(&A[k,j],va);
                vst1q_f32(&matrix[k][j],va);
            }
            for(;j<n;j++)
                matrix[k][j]=matrix[k][j]/matrix[k][k];//该行结尾处有几个元素还未计算
            matrix[k][k]=1.0;
        }
        #pragma omp for

            for(int i=k+1;i<n;i++)
            {
				int j;
                float32x4_t vaik=vdupq_n_f32(matrix[i][k]);
                for(j=k+1;j+4<=n;j+=4)
                {
                    float32x4_t vakj=vld1q_f32(&matrix[k][j]);
                    float32x4_t vaij=vld1q_f32(&matrix[i][j]);
                    //vx ← vakj*vaik;
                    float32x4_t vx=vmulq_f32(vakj,vaik);
                    vaij=vsubq_f32(vaij,vx);
                    //store4FloatTo(&A[i,j],vaij);
                    vst1q_f32(&matrix[i][j],vaij);
                }
                for(;j<n;j++)
                    matrix[i][j]=matrix[i][j]-matrix[k][j]*matrix[i][j];
                matrix[i][k]=0;
            }

    }
}

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
