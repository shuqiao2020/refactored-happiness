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
const int mpisize = 4;
const int num_mp=4;


int main(int argc, char* argv[])
{

//		int step = 10;
//	for(int n = 0;n<=5000;n += step)
//	{
	int n = 30;
	float  time1;
	float** m1 = new float*[n];
	for(int i = 0;i<n;i++)
	{
		m1[i] = new float[n];
	}

	int rank;
	double st = 0, ed = 0;   
        
	MPI_Status status;
	MPI_Init(0, 0);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);	
    
	if (rank == 0) {	
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
            if(j!=mpisize - 1)
                for(int i= (j - 1)*(n/(mpisize - 1));i<j*(n/(mpisize - 1));i++)
                    MPI_Send(&m1[i][0], n, MPI_FLOAT, j, i, MPI_COMM_WORLD);
            else
                for(int i= (j-1)*(n/mpisize);i<n;i++)
                    MPI_Send(&m1[i][0], n, MPI_FLOAT, j, i, MPI_COMM_WORLD);
		}
	}
	else 
        if(rank!=mpisize-1)
        {
            for(int i=(rank-1)*(n/(mpisize-1));i<rank*(n/(mpisize-1));i++)
                MPI_Recv(&m1[i][0], n, MPI_FLOAT,0, i, MPI_COMM_WORLD, &status);
        }
        else
        {
            for(int i=(rank-1)*(n/(mpisize-1));i<n;i++)
                MPI_Recv(&m1[i][0], n, MPI_FLOAT,0, i, MPI_COMM_WORLD, &status);
        }
	MPI_Barrier(MPI_COMM_WORLD);
	st = MPI_Wtime();



    
	for (int k = 0; k < n; k++) {	
		if (rank == 0) {	
            
            
 			for (int j = k + 1; j < n; j++)
 				m1[k][j] /= m1[k][k];
 			m1[k][k] = 1.0;
            
            
/*            {
            __m128 vt=_mm_set1_ps(m1[k][k]);
            int j;
            for(j=k+1;j+4<n;j+=4)
            {
                __m128 va=_mm_loadu_ps(m1[k]+j);
				va=_mm_div_ps(va,vt);
				_mm_storeu_ps(m1[k]+j,va);
            }
            //����ʣ�µĲ���
            for(;j<n;j++)
                m1[k][j]=m1[k][j]/m1[k][k];
            m1[k][k]=1.0;
            }*/
            
			for (int j = 1; j < mpisize; j++)
				MPI_Send(&m1[k][0], n, MPI_FLOAT, j,n+k + 1, MPI_COMM_WORLD);
		}
		else
			MPI_Recv(&m1[k][0], n, MPI_FLOAT, 0, n+k + 1, MPI_COMM_WORLD, &status);
		if (rank != 0) { 
            int r2 = rank*(n/(mpisize-1));
            if(rank==mpisize - 1)
                r2 = n;
            if(k>=(rank-1)*(n/(mpisize-1))&&k<r2)
            {

            for (int i = k+1; i < r2; i ++) 
            {
 				for (int j = k + 1; j < n; j++)
 					m1[i][j] -= m1[i][k] * m1[k][j];
				m1[i][k] = 0;
				if (i == k + 1) 
					MPI_Send(&m1[i][0], n, MPI_FLOAT, 0, 0, MPI_COMM_WORLD);	
			}
            }
            else
            {
                if(k<(rank-1)*(n/(mpisize-1)))
                {
                    for (int i = (rank-1)*(n/(mpisize-1)); i < r2; i ++) 
                    {
                        for (int j = k + 1; j < n; j++)
                                m1[i][j] -= m1[i][k] * m1[k][j];
                        m1[i][k] = 0;
                        if (i == k + 1) 
                            MPI_Send(&m1[i][0], n, MPI_FLOAT, 0, 0, MPI_COMM_WORLD);
                    }
                }
            }              
        }
		else if(k<n-1)
			MPI_Recv(&m1[k + 1][0], n, MPI_FLOAT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
	}
	MPI_Barrier(MPI_COMM_WORLD);

	

	ed = MPI_Wtime();	
    

    
	MPI_Finalize();
	if (rank == 0) {	
		printf("cost time:%.4lf\n", (ed - st));
	}


	
/*		if(n==100)
		{
			step = 300;
		}
		if(n==1000)
			step = 500;
	}*/
    
	return 0;
}
