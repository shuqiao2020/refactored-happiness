#include <iostream>
#include <time.h>
#include <arm_neon.h>
void Guass(float** matrix, int n)
{
	float32x4_t t1,t2,t3,t4;
	for(int k = 0;k < n;k++)
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
		for(int i = k + 1;i < n;i++)
		{
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
		std::cout<<n<<' '<<counter<<' '<<seconds<<' '<<seconds / counter <<std::endl;
		/*for(int i = 0;i<n;i++)
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
/*
int main()
{
	int n = 10;
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
	Guass(matrix,n);
	for(int i = 0;i<n;i++)
	{
		for(int j = 0;j<n;j++)
		{
			std::cout<<matrix[i][j]<<' ';
		}
		std::cout<<std::endl;
	}
	return 0;
}
*/