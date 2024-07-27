#pragma once
#include<string>
#include<queue>
#include<mutex>
#include"Connection.h"
#include<atomic>
#include<thread>
#include<memory>
#include<functional>
#include<condition_variable>
using namespace std;
/*���ݿ����ӳص�ʵ��*/
class ConnectionPool {
public:
	//�ṩ���ʽӿڣ���ȡ���ӳض���ʵ��������������ػ�����ȷ��ֻ���ʼ��һ�Σ����ұ�����ͬ���Ŀ���
	static ConnectionPool* getConnectionPool();
	//���ⲿ�ṩ�ӿڣ������ӳ��л�ȡһ�����õĿ�������
	//ʹ������ָ�룬����ָ�����������Զ������������ͷ����ӣ������ض���ɾ������ʹ֮���������Ƿ��ض���
     shared_ptr<Connection> getConnection();

private :
	//���췽��˽�л�,��ֹ�ⲿʵ����
	ConnectionPool();
	//���������ļ�
	bool loadConfigFile();
	//�ڶ������߳��У�ר�Ÿ�������������
	void produceConnectionTask();
	//�ڶ������߳��У�����Ƿ���������Ҫ����
	void sacnnerConnectionTask();
	string _ip;
	unsigned short _port;
	string _username;
	string _password;
	int _initSize;
	int _maxSize;
	int _maxIdleTime;
	int _connectionTimeOut;
	string _dbname;
	queue<Connection*>  _connectionQue;//�洢�����ӡ�����
	mutex _queueMutex;

	atomic_int _connectionCnt;//��¼��������connection���ӵ�������
	condition_variable cv;//���������������������������̺߳����������̵߳�ͨ��
};