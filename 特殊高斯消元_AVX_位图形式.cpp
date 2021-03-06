#include <iostream>
#include <fstream>
#include <time.h>
#include <vector>

#include <xmmintrin.h>
#include <nmmintrin.h>
#include <immintrin.h>
using namespace std;

vector<bool> xor0(vector<bool> m1,vector<bool> m2)
{
	vector<bool> m3;
    //m3.resize(max(m1.size(),m2.size()));
    __m256 t1,t2,t3;
	//for(int i = 0;i<max(m1.size(),m2.size());i++)
    for(int i = 0;i<max(m1.size(),m2.size()) - 8;i+=8)
	{
        float m11[8] = {m1[i],m1[i+1],m1[i+2],m1[i+3],m1[i+4],m1[i+5],m1[i+6],m1[i+7]};
        float m22[8] = {m2[i],m2[i+1],m2[i+2],m2[i+3],m2[i+4],m2[i+5],m2[i+6],m2[i+7]};
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
		//m3.push_back(m1[i]^m2[i]);
	}
    for(int i = max(m1.size(),m2.size()) - max(m1.size(),m2.size())%8;i<max(m1.size(),m2.size());i++)
    {
        m3[i] = m1[i]^m2[i];
    }
	return m3;
}
int find1(vector<bool> m)
{
/*	for(int i = m.size()-1;i> -1;i--)
	{
		if(m[i]==0)
			continue;
		else
			return i;
	}*/
    __m256 t1,t2,t3;
    float tmp[8] = {0,0,0,0,0,0,0,0};
    for(int i = m.size()-8;i> -1;i-=8)
    {
        //这个部分的优化可能SSE比AVX效果更好
        float m11[8] = {m[i],m[i+1],m[i+2],m[i+3],m[i+4],m[i+5],m[i+6],m[i+7]};
        t1 = _mm256_loadu_ps(m11);
//        t1 = _mm256_loadu_ps(&m[i]);
        t2 = _mm256_loadu_ps(tmp);
        t3 = _mm256_cmp_ps (t1, t2, 0);//比较大小
        _mm256_storeu_ps(m11, t3);
        if(m11[0]!=0&&m11[1]!=0&&m11[2]!=0&&m11[3]!=0&&m11[4]!=0&&m11[5]!=0&&m11[6]!=0&&m11[7]!=0)
        {
            continue;
        }
        else
        {
            for(int j = i + 7;j > i - 1;j--)
            {
                if(m[j]==1)
                    return j;
            }
        }
    }
    for(int i = m.size() % 8;i > -1;i--)
    {
        if(m[i]==0)
			continue;
		else
			return i;
    }
    
	return -1;
}
void minus0(vector<bool>& matrix,vector<vector<bool>>& matrix2)
{
/*	bool j = 1;
	while(j)
	{
		j = 0;*/
	for(int i = 0;i < matrix2.size();i++)
	{
		int f1 = find1(matrix);
		int f2 = find1(matrix2[i]);
		if(f1!=-1&&f1>f2)
		{
			matrix2.insert(matrix2.begin()+i,matrix);
            //输出结果
/*			for(int j = matrix2[i].size()-1;j>-1;j--)
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
			matrix = xor0(matrix,matrix2[i]);
			//j = 1;
		}
	}
/*	}
	if(find1(matrix)!=-1)
		matrix2.push_back(matrix);*/
}
vector<vector<bool>> Guass(vector<vector<bool>> matrix,vector<vector<bool>> matrix2)
{
	for(int i = 0;i < matrix.size();i++)
	{
		minus0(matrix[i],matrix2);
	}    
	return matrix2;
}

int main()
{
    float seconds = 0;
/*    for(int z = 0;z < 100;z++)
    {*/
	int n1 = 14325,n2 = 18748,m = 23045;
	vector<vector<bool>> matrix;
	vector<vector<bool>> matrix2;
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
			matrix2[line][number] = 1;
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
