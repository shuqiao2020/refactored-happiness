#include <iostream>
#include <fstream>
#include <time.h>
#include <vector>
using namespace std;

vector<bool> xor0(vector<bool> m1,vector<bool> m2)
{
	vector<bool> m3;
	for(int i = 0;i<max(m1.size(),m2.size());i++)
	{
		m3.push_back(m1[i]^m2[i]);
	}
	return m3;
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
void minus0(vector<bool>& matrix,vector<vector<bool>>& matrix2)
{
/*	bool j = 1;
	while(j)
	{
		j = 0;*/
	for(int i = 0;i < matrix2.size();i++)
	{
		int f1 = find1(matrix);//找到这一行的第一个1
		int f2 = find1(matrix2[i]);//找到这一行的第一个1
		if(f1!=-1&&f1>f2)
		{
			matrix2.insert(matrix2.begin()+i,matrix);

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
			matrix = xor0(matrix,matrix2[i]);//异或操作
//			j = 1;
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
	int n1 = 4535,n2 = 6375,m = 8399;
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
