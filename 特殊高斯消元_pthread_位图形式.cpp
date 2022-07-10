#include <iostream>
#include <fstream>
#include <time.h>
#include <vector>
#include<pthread.h>
using namespace std;

#define THREAD_NUM 7

vector<vector<bool>> matrix;
vector<vector<bool>> matrix2;

typedef struct{
	int t_id;
    int matrix_i;
    int matrix2_i;
}threadParam_t;

typedef struct{
	int t_id;
    vector<bool> m;
    int max;
}threadParam2_t;

void *threadFunc(void *param)//异或运算
{
	//pthread_t id = pthread_self();
	//std::cout<<id<<std::endl;
	threadParam_t *p = (threadParam_t*)param;
    
	int t_id = p->t_id;
    int matrix_i = p->matrix_i;
    int matrix2_i = p->matrix2_i;
//	std::cout<<t_id<<' '<<s<<' '<<i<<std::endl;
    
	for(int i = t_id;i<matrix[matrix_i].size();i+=THREAD_NUM)
    {
        matrix[matrix_i][i] = matrix[matrix_i][i]^matrix2[matrix2_i][i];
    }

	pthread_exit(NULL);
}

void *threadFunc2(void *param)//find1函数
{
	//pthread_t id = pthread_self();
	//std::cout<<id<<std::endl;
	threadParam2_t *p = (threadParam2_t*)param;
    
	int t_id = p->t_id;
    vector<bool> m = p->m;
    int max = p->max;
//	std::cout<<t_id<<' '<<s<<' '<<i<<std::endl;

	for(int i = m.size() - 1 - t_id;i>-1;i-=THREAD_NUM)
    {
        if(m[i]==1)
        {
            max = i;
            p->max = max;
            param = (void*)p;
            break;
        }
    }

	pthread_exit(NULL);
}


vector<bool> xor0(vector<bool> m1,vector<bool> m2,int matrix_i,int matrix2_i)
{
/*	vector<bool> m3;
	for(int i = 0;i<max(m1.size(),m2.size());i++)
	{
		m3.push_back(m1[i]^m2[i]);
	}
    return m3;
    */
    
    int worker_count = THREAD_NUM;
	pthread_t* handles = new pthread_t[THREAD_NUM];
	threadParam_t* param = new threadParam_t[THREAD_NUM];

	for(int t_id = 0; t_id < worker_count; t_id++)
	{
		param[t_id].t_id = t_id;
        param[t_id].matrix_i = matrix_i;
        param[t_id].matrix2_i = matrix2_i;
	}
    for(int t_id = 0; t_id < worker_count; t_id++)
	{
		pthread_create(&handles[t_id],NULL,threadFunc,(void*)&param[t_id]);
    }

	for(int t_id = 0; t_id < worker_count; t_id++)
	{
		pthread_join(handles[t_id],NULL);
	}


/*	for(int j = matrix[matrix_i].size()-1;j>-1;j--)
	{
		if(matrix[matrix_i][j]==1)
			std::cout<<j<<' ';
	}
	std::cout<<std::endl;
*/
    
    return matrix[matrix_i];
}
int find1(vector<bool> m)
{
 //find1的pthread并行部分   
/*    int worker_count = THREAD_NUM;
	pthread_t* handles = new pthread_t[THREAD_NUM];
	threadParam2_t* param = new threadParam2_t[THREAD_NUM];

	for(int t_id = 0; t_id < worker_count; t_id++)
	{
		param[t_id].t_id = t_id;
        param[t_id].m = m;
        param[t_id].max = -1;
	}

	for(int t_id = 0; t_id < worker_count; t_id++)
	{
		pthread_create(&handles[t_id],NULL,threadFunc2,(void*)&param[t_id]);
    }

	for(int t_id = 0; t_id < worker_count; t_id++)
	{
		pthread_join(handles[t_id],NULL);
	}
    
    int max = param[0].max;
    for(int t_id = 1; t_id < worker_count; t_id++)
	{
        if(param[t_id].max>max)
            max = param[t_id].max;
	}
    return max;
    */
    //串行代码
	for(int i = m.size()-1;i> -1;i--)
	{
		if(m[i]==0)
			continue;
		else
			return i;
	}
    return -1;
    

}
void minus0(vector<bool>& matrix,vector<vector<bool>>& /*matrix2*/m2,int matrix_i)
{    
	for(int i = 0;i < matrix2.size();i++)
	{
		int f1 = find1(matrix);
		int f2 = find1(matrix2[i]);
		if(f1!=-1&&f1>f2)
		{
            //输出结果
/*			matrix2.insert(matrix2.begin()+i,matrix);

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
			matrix = xor0(matrix,matrix2[i],matrix_i,i);
		}
	}
}
vector<vector<bool>> Guass(vector<vector<bool>> matrix,vector<vector<bool>> matrix2)
{
	for(int i = 0;i < matrix.size();i++)
	{
		minus0(matrix[i],matrix2,i);
	}
	return matrix2;
}

int main()
{
    float seconds = 0;
/*    for(int z = 0;z < 100;z++)
    {*/
	int n1 = 8,n2 = 22,m = 130;
	vector<bool> list;
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
                matrix2[line][number] = 1;
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
    seconds /= float (CLOCKS_PER_SEC);
//    seconds /= 100.0;
    std::cout<<seconds<<std::endl;
	

	return 0;
}
