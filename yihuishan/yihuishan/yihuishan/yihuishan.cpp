// yihuishan.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

int main()
{
	string rub;
	ifstream profile("initial.txt"); 
	int a[7];
	profile >> a[0]  >> a[1]  >> a[2]  >> a[3] >> a[4] >> a[5]  >> a[6];
	cout << a[0] << a[1] << a[2] << a[3] << a[4] << a[5] << a[6] << endl;
	profile.close();
	while (1) {};
    return 0;
}

