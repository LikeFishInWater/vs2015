// grammar.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include <string>
using namespace std;

class Item_base  {
public:
	Item_base(const string &book = "", double scale_price = 0.0): isbn(book), price(scale_price) {};
	string book()const { return isbn; };
	virtual double net_price(size_t n)const { return n*price; };
	virtual ~Item_base() {};
private:
	string isbn;
protected:
	double price;
};
// class classname: access-label base-class{};
int main()
{
	//数组
	const unsigned buf_size = 512, max_files = 20;
	int staff_size = 27;
	//const unsigned sz = get_size();
	char input_buffer[buf_size];
	string file_table[max_files + 1];
	//double salaries[staff_size];//error 非常量值，运行时才知道
	//int test_scores[get_size()];
	//int vals[sz];

	int ia1[] = { 0,1,2 };
	string str_arr[] = {"hi","bye","","",""};

	char ca1[] = { 'C','+','+' };
	char ca2[] = { 'C','+','+','\0' };
	char ca3[] = "C++";
	//ca2 ca3维数为4，ca1维数为3

	//数组越界不检查
	//二维数组
	int ia2[2][3] = { {0,1,2},{1,2,3} };
	cout << ia2[1][2] << endl;

	int * ip = new int(5);
	delete[]ip;

	//new delete 二维数组
	//第一种，内存不连续
	int** pNum;//以int为例
	const int M = 10;
	const int N = 20;
	pNum = new int*[M];
	for (int i = 0; i<M; i++)
	{
		pNum[i] = new int[N];
	}
	//删除的时候是
	for (int j = 0; j<M; j++)
	{
		delete[]pNum[j];
	}
	delete[]pNum;
	//第二种，内存连续
	int* a = new int[M*N];
	// a[i*n+j]是a[i][j]
	delete[]a;
	//第三种，第一种特例
	int(*pNum3)[N] = new int[M][N];
	delete[]  pNum3;

	system("pause");
    return 0;
}

