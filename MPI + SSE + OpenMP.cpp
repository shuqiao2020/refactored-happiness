//mpi+openmp+simd
#include "mpi.h" 
#include<omp.h>
#include <xmmintrin.h>
#include <emmintrin.h>
#include <pmmintrin.h>
#include <tmmintrin.h>
#include <smmintrin.h>
#include <nmmintrin.h>
#include <immintrin.h>
#include <iostream> 
#include<algorithm>
using namespace std;
const int n = 10,mpisize = 4;
const int num_mp=4;//openmp的线程数
float m1[n][n], time1;

int main(int argc, char* argv[])
{
	int rank;
	double st, ed;
	MPI_Status status;
	MPI_Init(0, 0);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);	//获取当前进程号
	if (rank == 0) {	//0号进程实现初始化
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < n; j++)
				m1[i][j] = 0;
			m1[i][i] = 1.0;
			for (int j = i + 1; j < n; j++)
				m1[i][j] = rand() % 1000 + 1;
		}
		for (int k = 0; k < n; k++)
			for (int i = k + 1; i < n; i++)
				for (int j = 0; j < n; j++)
					m1[i][j] = int((m1[i][j] + m1[k][j])) % 1000 + 1.0;
		for (int j = 1; j < mpisize; j++) {
			for(int i= j;i<n;i+=mpisize-1)
			MPI_Send(&m1[i][0], n, MPI_FLOAT, j, i, MPI_COMM_WORLD);
		}
	}
	else 
		for(int i=rank;i<n;i+=mpisize-1)
			MPI_Recv(&m1[i][0], n, MPI_FLOAT,0, i, MPI_COMM_WORLD, &status);	
	MPI_Barrier(MPI_COMM_WORLD);	//各进程同步
	st = MPI_Wtime();	 //计时
    
    //创建openmp线程
    #pragma omp parallel num_threads(num)
	for (int k = 0; k < n; k++) {	
		if (rank == 0) {	//0号进程负责除法部分
            
            //串行部分，该循环只交给一个线程处理
            #pragma omp single
// 			for (int j = k + 1; j < n; j++)
// 				m1[k][j] /= m1[k][k];
// 			m1[k][k] = 1.0;
            
            //sse不对齐进行并行化
            {
            __m128 vt=_mm_set1_ps(m1[k][k]);
            int j;
            for(j=k+1;j+4<n;j+=4)
            {
                __m128 va=_mm_loadu_ps(m1[k]+j);
				va=_mm_div_ps(va,vt);
				_mm_storeu_ps(m1[k]+j,va);
            }
            //处理剩下的部分
            for(;j<n;j++)
                m1[k][j]=m1[k][j]/m1[k][k];
            m1[k][k]=1.0;
            }
            
			for (int j = 1; j < mpisize; j++)
				MPI_Send(&m1[k][0], n, MPI_FLOAT, j,n+k + 1, MPI_COMM_WORLD);
		}
		else
			MPI_Recv(&m1[k][0], n, MPI_FLOAT, 0, n+k + 1, MPI_COMM_WORLD, &status);
		if (rank != 0) {    
			int r2 = rank;
			while (r2 < k + 1)r2 += mpisize - 1;
            
            //并行部分
            #pragma omp for
            for (int i = r2; i < n; i += mpisize - 1) 
            {
// 				for (int j = k + 1; j < n; j++)
// 					m1[i][j] -= m1[i][k] * m1[k][j];
               __m128 vaik=_mm_set1_ps(m1[i][k]);
                int j;
                for(j=k+1;j+4<n;j+=4)
                {
                    __m128 vakj=_mm_loadu_ps(m1[k]+j);
                    __m128 vaij=_mm_loadu_ps(m1[i]+j);
                    __m128 vx=_mm_mul_ps(vaik,vakj);
                    vaij=_mm_sub_ps(vaij,vx);
                    _mm_storeu_ps(&m1[i][j],vaij);
                }
                for(; j < n; ++j)
                {
                    m1[i][j] = m1[i][j] - m1[i][k] * m1[k][j];
                } 
				m1[i][k] = 0;
				if (i == k + 1) 
					MPI_Send(&m1[i][0], n, MPI_FLOAT, 0, 0, MPI_COMM_WORLD);	
			}        
		}
		else if(k<n-1)
			MPI_Recv(&m1[k + 1][0], n, MPI_FLOAT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
	}
	MPI_Barrier(MPI_COMM_WORLD);	//各进程同步
	ed = MPI_Wtime();	 //计时
	MPI_Finalize();
	if (rank == 0) {	//只有0号进程中有最终结果
		printf("cost time:%.4lf\n", ed - st);
	}
	return 0;
}

