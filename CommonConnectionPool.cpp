#include"Pch.h"
#include"CommonConnectionPool.h"
#include"Public.h"

ConnectionPool* ConnectionPool::getConnectionPool() {
	static ConnectionPool pool;//����������ػ�����ȷ��ֻ���ʼ��һ�Σ����ұ�����ͬ���Ŀ���,���ɱ���������lock��unlock
	return &pool;
}
bool ConnectionPool::loadConfigFile() {
	FILE* pf = fopen("mysql.ini", "r");
	if (pf == nullptr) {

		LOG("my.ini file is not exit");
		return false;
	}
	//����Ƿ��ļ�ĩβ
	while (!feof(pf)) {
		char line[1024] = { 0 };
		fgets(line, 1024, pf);
		string str = line;
		int index = str.find('=');
		if (index == -1) {
			continue;
		}
		else {
			int endex = str.find('\n', index);
			string key = str.substr(0, index);
			string value = str.substr(index + 1,endex-index-1);
			if (key =="ip" ) {
				_ip = value;
			}
			else if (key =="port") {
				_port = atoi(value.c_str());
			}
			else if (key =="username") {
				_username = value;
			}
			else if (key =="password") {
				_password = value;
			}
			else if (key == "initSize") {
				_initSize = atoi(value.c_str());
			}
			else if (key == "maxSize") {
				_maxSize = atoi(value.c_str());
			}
			else if (key == "maxIdleTime") {
				_maxSize = atoi(value.c_str());
			}
			else if (key == "connectionTimeOut") {
				_connectionTimeOut = atoi(value.c_str());
			}
			else if (key == "dbname") {
				_dbname = value;
			}
		}
	}
	return true;
}
void ConnectionPool::produceConnectionTask() {
	for (;;) {
		//�����������߼����Ժ������߾��ò�����Դ
		unique_lock<mutex> lock(_queueMutex);
		while (!_connectionQue.empty()) {
			cv.wait(lock);//�����ﻹ�п������ӣ������ȴ�
		}
		//cv.wait(lock, []{ return _connectionQue.empty(); });
	}
	
	//û�п��������ˣ�����������û�дﵽ����,����������
	if (_connectionCnt < _maxSize) {
		Connection* p = new Connection();
		p->connect(_ip, _port, _username, _password, _dbname);
		p->refreshAliveTime();
		_connectionQue.push(p);
		_connectionCnt++;
	}
	//֪ͨ�������̣߳���������������
	cv.notify_all();
}
void ConnectionPool::sacnnerConnectionTask() {
	for (;;) {
		this_thread::sleep_for(chrono::seconds(_maxIdleTime));
		unique_lock<mutex> lock(_queueMutex);
		while (_connectionCnt > _initSize) {
			Connection* ptr = _connectionQue.front();
			if (ptr->getAliveeTime() >=(_maxIdleTime * 1000)) {
				_connectionQue.pop();
				delete ptr;//�ͷ�
				_connectionCnt--;
			}
		}
	}
}
ConnectionPool::ConnectionPool() {
	//����������
	if (!loadConfigFile() ){
		return;
	}
	//������ʼ����������
	for (int i = 0; i < _initSize; ++i) {
		Connection* p = new Connection();
		p->connect(_ip, _port, _username, _password, _dbname);
		_connectionQue.push(p);
		p->refreshAliveTime();
		_connectionCnt++;
	}
	//����һ���µ��߳���Ϊ����������
	//thisָ����øó�Ա�����Ķ���
	thread produce(std::bind(&ConnectionPool::produceConnectionTask,this));
	produce.detach();
	//����һ�����̼߳�⣬�Ƿ��ж��������Ŀ������ӣ�����ʱ�䳬��maxIdleTime���ͷŵ�
	thread scanner(std::bind(&ConnectionPool::sacnnerConnectionTask, this));
	scanner.detach();
}
shared_ptr<Connection> ConnectionPool::getConnection() {
	unique_lock<mutex> lock(_queueMutex);
	while(_connectionQue.empty()) {
		//���Ի�ȡ���������ȴ��������� cv ��֪ͨ��ʱ��_connectionTimeOu���룩
		if (_connectionQue.empty()&&cv_status::timeout== cv.wait_for(lock, chrono::milliseconds(_connectionTimeOut))) {
			LOG("��ȡ���ӳ�ʱ.......��ȡ����ʧ��");
			return nullptr;
		}
	}
	//��������ָ��sp,ָ���ͷԪ��
	//��lambda���ʽ��д����ָ���ɾ������ʹ֮���������connection�����ն���������
	shared_ptr<Connection> sp(_connectionQue.front(), [&](Connection*ptr_conn) {
		//��֤����_connectionQue���̰߳�ȫ���������õ������ܷ��ʷ���ȴ�
		unique_lock<mutex> lock(_queueMutex);
		//lock_guard<mutex> lock(_queueMutex);
		_connectionQue.push(ptr_conn);
		ptr_conn->refreshAliveTime();
		});
	//��ͷ����
	_connectionQue.pop();
	//֪ͨ�����ߣ�����Ƿ���Ҫ��������
	cv.notify_all();

	return sp;
}
