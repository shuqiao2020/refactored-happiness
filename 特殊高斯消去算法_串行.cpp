#include <iostream>
#include <fstream>
#include <time.h>
/*
int* searchfirst(int** matrix,int n,int m)
{
	int* first = new int[n];
	for(int i = 0;i < m;i++)
	{
		for(int j = 0;j < n;j++)
		{
			if(matrix[i][j]==1)
			{
				first[i] = m - j;
				break;
			}
		}
	}
	return first;
}*/
void minus(int** matrix,int** matrix2,int i,int s,int n1,int n2,int m)
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
int** Guass(int** matrix,int** matrix2,int n1,int n2,int m)//被消元行有n1行m列，消元子共n2行
{
//	int* first = searchfirst(matrix,n1,m);
//	int* first2 = searchfirst(matrix2,n2,m);
/*	for(int i = 0;i < n1;i++)
	{
		for(int j = 0;j < n2;j++)
		{
			if(matrix[i][m - first2[j]]!=0)
			{
				for(int k = 0;k < m;k++)
				{
					matrix[i][k] = matrix[i][k]^matrix2[j][k];
				}
			}
		}
		for(int j = 0;j < m;j++)
		{
			if(matrix[i][j]!=0)
			{
				m +=1;
				int ** matrix3 = new int*[n2];
				for(int k = 0;k < n2;k++)
				{
					matrix3[k] = new int[m];
					for(int a = 0;a < m;a++)
					{
						if(a!=m-1)
							matrix3[k][a] = matrix2[k][a];
						else
							matrix3[k][a] = matrix[i][a];
					}
				}
				matrix2 = matrix3;
			}
		}
	}
	return matrix2;*/
	for(int i = 0;i<n1&&matrix[i][0]!=-1;i++)
	{
		for(int s = 0;s<n2&&matrix2[s][0]!=-1;s++)
		{
//				if(matrix[i][j] < matrix2[s][0])
//					break;
//				if(matrix[i][j] == matrix2[s][0])
			if(matrix[i][0] == matrix2[s][0])
			{
				minus(matrix,matrix2,i,s,n1,n2,m);
				s = 0;
			}
/*			for(int j = 0; j< m&& matrix[i][j]!=-1;j++)
				std::cout<<matrix[i][j]<<' ';
			std::cout<<std::endl;*/
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
/*		for(int i = 0;i<n2&&matrix[i][0]!=-1;i++)
	{
		for(int j = 0;j<m&&matrix[i][j]!=-1;j++)
		{
			std::cout<<matrix[i][j]<<' ';
		}
		std::cout<<std::endl;
	}*/
	}
	return matrix2;
}

int main()
{
    float seconds = 0;
    for(int z = 0;z < 100;z++)
    {
	int n1 = 1000,n2 = 1000,m = 1000;
	int** matrix = new int*[n1];
	int** matrix2 = new int*[n2];
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
	Guass(matrix,matrix2,n1,n2,m);
    time_t finish = clock();
    seconds += finish - start;
    for(int i = 0;i< n1;i++)
    {
         delete matrix[i];
    }
    for(int i = 0;i< n2;i++)
    {
         delete matrix2[i];
    }
    delete matrix;
    delete matrix2;
    }
    seconds /= float (CLOCKS_PER_SEC);
    seconds /= 100.0;
    std::cout<<seconds<<std::endl;

/*	for(int i = 0;i<n2&&matrix2[i][0]!=-1;i++)
	{
		for(int j = 0;j<m&&matrix2[i][j]!=-1;j++)
		{
			std::cout<<matrix2[i][j]<<' ';
		}
		std::cout<<std::endl;
	}*/
	return 0;
}
