#include <iostream>
#include <fstream>
#include <time.h>
#include <vector>
#include "mpi.h" 
#include<omp.h>
#include<pthread.h>
#include <xmmintrin.h>
#include <nmmintrin.h>
#include <immintrin.h>

using namespace std;

const int n = 500,mpisize = 4;
const int num_mp=4;//openmp的线程数

vector<vector<bool>> matrix;
vector<vector<bool>> matrix2;

MPI_Status status;

vector<bool> xor0(vector<bool> m1,vector<bool> m2,int matrix_i,int matrix2_i,int rank)
{
    if(rank!=0)
    {
        vector<bool> m3;
        __m256 t1,t2,t3;
        #pragma omp parallel num_threads(num_mp)
        #pragma omp for
        for(int i = rank - 1;i<max(m1.size(),m2.size()) - 8;i+=(mpisize - 1)*8)
        {
            float m11[8] = {m1[i],m1[i+mpisize - 1],m1[i+2*(mpisize - 1)],m1[i+3*(mpisize - 1)],m1[i+4*(mpisize - 1)],m1[i+5*(mpisize - 1)],m1[i+6*(mpisize - 1)],m1[i+7*(mpisize - 1)]};
            float m22[8] = {m2[i],m2[i+(mpisize - 1)],m2[i+2*(mpisize - 1)],m2[i+3*(mpisize - 1)],m2[i+4*(mpisize - 1)],m2[i+5*(mpisize - 1)],m2[i+6*(mpisize - 1)],m2[i+7*(mpisize - 1)]};
            float m33[8];
            t1 = _mm256_loadu_ps(m11);
            t2 = _mm256_loadu_ps(m22);
            t3 = _mm256_xor_ps (t1, t2);
            _mm256_storeu_ps(m33, t3);
            m3.push_back(m33[0]);
            m3.push_back(m33[1]);
            m3.push_back(m33[2]);
            m3.push_back(m33[3]);  
            m3.push_back(m33[4]);
            m3.push_back(m33[5]);
            m3.push_back(m33[6]);
            m3.push_back(m33[7]);
            
            MPI_Request request;
            MPI_Isend(&m33[0], 1, MPI_FLOAT, 0, i, MPI_COMM_WORLD, &request);
            MPI_Isend(&m33[1], 1, MPI_FLOAT, 0, i+mpisize - 1, MPI_COMM_WORLD, &request);
            MPI_Isend(&m33[2], 1, MPI_FLOAT, 0, i+2*(mpisize - 1), MPI_COMM_WORLD, &request);
            MPI_Isend(&m33[3], 1, MPI_FLOAT, 0, i+3*(mpisize - 1), MPI_COMM_WORLD, &request);
            MPI_Isend(&m33[4], 1, MPI_FLOAT, 0, i+4*(mpisize - 1), MPI_COMM_WORLD, &request);
            MPI_Isend(&m33[5], 1, MPI_FLOAT, 0, i+5*(mpisize - 1), MPI_COMM_WORLD, &request);
            MPI_Isend(&m33[6], 1, MPI_FLOAT, 0, i+6*(mpisize - 1), MPI_COMM_WORLD, &request);
            MPI_Isend(&m33[7], 1, MPI_FLOAT, 0, i+7*(mpisize - 1), MPI_COMM_WORLD, &request);
            //MPI_Send(&m33, 1, MPI_FLOAT, 0, i, MPI_COMM_WORLD);
            
        }
        for(int i = max(m1.size(),m2.size()) - max(m1.size(),m2.size())%((mpisize - 1)*8) + rank - 1;i<max(m1.size(),m2.size());i+=mpisize - 1)
        {
            float m33 = m1[i]^m2[i];
            m3.push_back(m1[i]^m2[i]);
            MPI_Request request;
            MPI_Isend(&m33, 1, MPI_FLOAT, 0, i, MPI_COMM_WORLD, &request);
            //MPI_Send(&m33, 1, MPI_FLOAT, 0, i, MPI_COMM_WORLD);
            
        }
        return m3;
    }
}
int find1(vector<bool> m)
{
	for(int i = m.size()-1;i> -1;i--)
	{
		if(m[i]==0)
			continue;
		else
			return i;
	}
	return -1;
}
void minus0(vector<bool>& matrix,vector<vector<bool>>& matrix2,int matrix_i, int rank)
{

	for(int i = 0;i < matrix2.size();i++)
	{
		int f1 = find1(matrix);
		int f2 = find1(matrix2[i]);
		if(f1!=-1&&f1>f2)
		{
			matrix2.insert(matrix2.begin()+i,matrix);
/*
			for(int j = matrix2[i].size()-1;j>-1;j--)
			{
				if(matrix2[i][j]==1)
					std::cout<<j<<' ';
			}
			std::cout<<std::endl;
*/
			break;
		}
		if(f1!=-1&&f1==f2)
		{
            if(rank!=0)
            {
                xor0(matrix,matrix2[i],matrix_i,i,rank);
                float* m = new float [matrix.size()];
                MPI_Recv(m, matrix.size(), MPI_FLOAT, 0, rank, MPI_COMM_WORLD, &status);
                for(int i = 0;i<matrix.size();i++)
                    matrix[i] = m[i];
            }
            else
            {
                float* m11 = new float[matrix.size()];
                MPI_Request request;
                for(int i = 0;i < matrix.size();i++)
                {
                    //float m11 = 0;
                    //MPI_Recv(&m11, 1, MPI_FLOAT, MPI_ANY_SOURCE, i, MPI_COMM_WORLD, &status);
                    MPI_Irecv(&m11[i], 1, MPI_FLOAT, MPI_ANY_SOURCE, i, MPI_COMM_WORLD, &request);
                    //matrix[i] = m11;
                }
                MPI_Wait(&request, MPI_STATUSES_IGNORE);
                for(int i = 0;i < matrix.size();i++)
                {
                    matrix[i] = m11[i];
                }
                for(int i = 1;i < mpisize;i++)
                {
                    float* m = new float [matrix.size()];
                    for(int i = 0;i<matrix.size();i++)
                        m[i] = matrix[i];
                    //MPI_Send(m, matrix.size(), MPI_FLOAT, i, i, MPI_COMM_WORLD);
                    MPI_Request request;
                    MPI_Isend(m, matrix.size(), MPI_FLOAT, i, i, MPI_COMM_WORLD, &request);
                }
            }

		}
	}
/*	}
	if(find1(matrix)!=-1)
		matrix2.push_back(matrix);*/
}
vector<vector<bool>> Guass(vector<vector<bool>> matrix,vector<vector<bool>> matrix2)
{
    int rank = 0;
	double st, ed;
	MPI_Status status;
	MPI_Init(0, 0);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);	//获取当前进程号
    
	for(int i = 0;i < matrix.size();i++)
	{
		minus0(matrix[i],matrix2,i,rank);
	}
    MPI_Finalize();
	return matrix2;
}

int main(int argc, char* argv[])
{
    float seconds = 0;
/*    for(int z = 0;z < 100;z++)
    {*/
	int n1 = 8,n2 = 22,m = 130;
	vector<vector<bool>> matrix;
	vector<vector<bool>> matrix2;
	vector<bool> list;
/*    list.resize(m);
    matrix.resize(n1);
    matrix2.resize(n2);*/
	for(int i = 0;i< m;i++)
	{
		list.push_back(0);
	}
	for(int i = 0;i < n1;i++)
	{
		matrix.push_back(list);
	}
	for(int i = 0; i < n2;i++)
	{
		matrix2.push_back(list);
	}

	int line = 0;
	char num;
	int number = 0;
	std::ifstream text2("1.txt",std::ios::in);
	while(!text2.eof())
	{
		text2.get(num);
		if(num==' ')
		{
            if(line<n2&&number<m)
            {
                //cout<<number<<' ';
                //matrix2[k][nk] = 1;
                //cout<<matrix2[k][nk];
                //cout<<matrix2[line][number]<<endl;
                //cout<<matrix2[21][129];
                matrix2[line][number] = 1;//?
                number = 0;
            }
		}
		if(num=='\n')
		{
			line++;
            number = 0;
            //cout<<number<<' ';
		}
		if(num!=' '&&num!='\n')
		{
			number *= 10;
			number += num - '0';
            //cout<<number<<' ';
		}
	}
	text2.close();

	line = 0;
	number = 0;
	std::ifstream text("2.txt",std::ios::in);
	while(!text.eof())
	{
		text.get(num);
		if(num==' ')
		{
			matrix[line][number] = 1;
			number = 0;
		}
		if(num=='\n')
		{
			line++;
            number = 0;
		}
		if(num!=' '&&num!='\n')
		{
			number *= 10;
			number += num - '0';
		}
	}
	text.close();

	time_t start = clock();
    
	matrix2 = Guass(matrix,matrix2);
    time_t finish = clock();
    seconds += finish - start;
/*    for(int i = 0;i< n1;i++)
    {
         delete matrix[i];
    }
    for(int i = 0;i< n2;i++)
    {
         delete matrix2[i];
    }
    delete matrix;
    delete matrix2;
    }*/
    seconds /= float (CLOCKS_PER_SEC);
//    seconds /= 100.0;
    std::cout<<seconds<<std::endl;
	
/*	for(int i = n2;i<matrix2.size();i++)
	{
		for(int j = matrix2[i].size()-1;j>-1;j--)
		{
			if(matrix2[i][j]==1)
				std::cout<<j<<' ';
		}
		std::cout<<std::endl;
	}*/
	return 0;
}
