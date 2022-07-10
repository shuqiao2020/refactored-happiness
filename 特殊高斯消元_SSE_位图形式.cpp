#include <iostream>
#include <fstream>
#include <time.h>
#include <vector>

#include <xmmintrin.h>
#include <nmmintrin.h>
using namespace std;

vector<bool> xor0(vector<bool> m1,vector<bool> m2)
{
/*    for(int j = m1.size()-1;j>-1;j--)
	{
        if(m1[j]==1)
            std::cout<<j<<' ';
	}
	std::cout<<std::endl;
    for(int j = m2.size()-1;j>-1;j--)
	{
        if(m2[j]==1)
            std::cout<<j<<' ';
	}
	std::cout<<std::endl;*/
    
	vector<bool> m3;
    //m3.resize(max(m1.size(),m2.size()));
    __m128 t1,t2,t3;
	//for(int i = 0;i<max(m1.size(),m2.size());i++)
    for(int i = 0;i<max(m1.size(),m2.size()) - 4;i+=4)
	{
        float m11[4] = {m1[i],m1[i+1],m1[i+2],m1[i+3]};
        //float m11[4] = {0,0,0,0};
        //cout<<m11[0]<<m11[1]<<m11[2]<<m11[3]<<endl;
        float m22[4] = {m2[i],m2[i+1],m2[i+2],m2[i+3]};
        //float m22[4] = {1,1,1,1};
        //cout<<m22[0]<<m22[1]<<m22[2]<<m22[3]<<endl;
        float m33[4];
        t1 = _mm_loadu_ps(m11);
        t2 = _mm_loadu_ps(m22);
        t3 = _mm_xor_ps (t1, t2);
        _mm_storeu_ps(m33, t3);
        m3.push_back(m33[0]);
        m3.push_back(m33[1]);
        m3.push_back(m33[2]);
        m3.push_back(m33[3]);
        //cout<<m3[i]<<m3[i+1]<<m3[i+2]<<m3[i+3]<<endl<<endl;
		//m3.push_back(m1[i]^m2[i]);
	}
    for(int i = max(m1.size(),m2.size()) - max(m1.size(),m2.size())%4;i<max(m1.size(),m2.size());i++)
    {
        //m3[i] = m1[i]^m2[i];
        m3.push_back(m1[i]^m2[i]);
    }
    
/*    for(int j = m3.size()-1;j>-1;j--)
	{
        if(m3[j]==1)
            std::cout<<j<<' ';
	}
	std::cout<<std::endl;
    */
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
    __m128 t1,t2,t3;
    float tmp[4] = {0,0,0,0};
    for(int i = m.size()-4;i> -1;i-=4)
    {
        float m11[4] = {m[i],m[i+1],m[i+2],m[i+3]};
//        cout<<m11[0]<<m11[1]<<m11[2]<<m11[3]<<endl;
        t1 = _mm_loadu_ps(m11);
        t2 = _mm_loadu_ps(tmp);
//        cout<<_mm_comieq_ss (t1, t2)<<' ';
//        if(_mm_comieq_ss (t1, t2))
        t3 = _mm_cmpeq_ps (t1, t2);//比较大小
        _mm_storeu_ps(m11, t3);
//        cout<<m11[0]<<m11[1]<<m11[2]<<m11[3]<<endl;
        if(m11[0]!=0&&m11[1]!=0&&m11[2]!=0&&m11[3]!=0)
        {
//            cout<<i<<endl;
            continue;
        }
        else
        {
            for(int j = i + 3;j > i - 1;j--)
            {
                if(m[j]==1)
                {
//                    cout<<j<<'j'<<endl;
                    return j;
                }
            }
        }
    }
    for(int i = m.size() % 4;i > -1;i--)
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
        //cout<<f1<<' '<<f2;
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
