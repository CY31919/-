#pragma once
#include<iostream>
using namespace std;
//<�ļ���>:<�к�> <ʱ���> : <��־��Ϣ>
#define LOG(str)\
	cout<<__FILE__<<":"<<__LINE__<<" "<<\
	__TIMESTAMP__<<":"<<str<<endl;