// grammar.cpp : �������̨Ӧ�ó������ڵ㡣
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
	//����
	const unsigned buf_size = 512, max_files = 20;
	int staff_size = 27;
	//const unsigned sz = get_size();
	char input_buffer[buf_size];
	string file_table[max_files + 1];
	//double salaries[staff_size];//error �ǳ���ֵ������ʱ��֪��
	//int test_scores[get_size()];
	//int vals[sz];

	int ia1[] = { 0,1,2 };
	string str_arr[] = {"hi","bye","","",""};

	char ca1[] = { 'C','+','+' };
	char ca2[] = { 'C','+','+','\0' };
	char ca3[] = "C++";
	//ca2 ca3ά��Ϊ4��ca1ά��Ϊ3

	//����Խ�粻���
	//��ά����
	int ia2[2][3] = { {0,1,2},{1,2,3} };
	cout << ia2[1][2] << endl;

	int * ip = new int(5);
	delete[]ip;

	//new delete ��ά����
	//��һ�֣��ڴ治����
	int** pNum;//��intΪ��
	const int M = 10;
	const int N = 20;
	pNum = new int*[M];
	for (int i = 0; i<M; i++)
	{
		pNum[i] = new int[N];
	}
	//ɾ����ʱ����
	for (int j = 0; j<M; j++)
	{
		delete[]pNum[j];
	}
	delete[]pNum;
	//�ڶ��֣��ڴ�����
	int* a = new int[M*N];
	// a[i*n+j]��a[i][j]
	delete[]a;
	//�����֣���һ������
	int(*pNum3)[N] = new int[M][N];
	delete[]  pNum3;

	system("pause");
    return 0;
}

