#include"Pch.h"
#include"CommonConnectionPool.h"
#include"Public.h"

ConnectionPool* ConnectionPool::getConnectionPool() {
	static ConnectionPool pool;//利用了类加载机制来确保只会初始化一次，并且避免了同步的开销,会由编译器进行lock和unlock
	return &pool;
}
bool ConnectionPool::loadConfigFile() {
	FILE* pf = fopen("mysql.ini", "r");
	if (pf == nullptr) {

		LOG("my.ini file is not exit");
		return false;
	}
	//检查是否到文件末尾
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
		//加锁，生产者加锁以后，消费者就拿不到资源
		unique_lock<mutex> lock(_queueMutex);
		while (!_connectionQue.empty()) {
			cv.wait(lock);//队列里还有空闲连接，阻塞等待
		}
		//cv.wait(lock, []{ return _connectionQue.empty(); });
	}
	
	//没有空闲连接了，且连接数量没有达到上限,创建新链接
	if (_connectionCnt < _maxSize) {
		Connection* p = new Connection();
		p->connect(_ip, _port, _username, _password, _dbname);
		p->refreshAliveTime();
		_connectionQue.push(p);
		_connectionCnt++;
	}
	//通知消费者线程，可以消费连接了
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
				delete ptr;//释放
				_connectionCnt--;
			}
		}
	}
}
ConnectionPool::ConnectionPool() {
	//加载配置项
	if (!loadConfigFile() ){
		return;
	}
	//创建初始数量的链接
	for (int i = 0; i < _initSize; ++i) {
		Connection* p = new Connection();
		p->connect(_ip, _port, _username, _password, _dbname);
		_connectionQue.push(p);
		p->refreshAliveTime();
		_connectionCnt++;
	}
	//启动一个新的线程作为连接生产者
	//this指向调用该成员函数的对象。
	thread produce(std::bind(&ConnectionPool::produceConnectionTask,this));
	produce.detach();
	//启动一个新线程检测，是否有额外生产的空闲连接，生存时间超过maxIdleTime，释放掉
	thread scanner(std::bind(&ConnectionPool::sacnnerConnectionTask, this));
	scanner.detach();
}
shared_ptr<Connection> ConnectionPool::getConnection() {
	unique_lock<mutex> lock(_queueMutex);
	while(_connectionQue.empty()) {
		//尝试获取互斥锁并等待条件变量 cv 被通知或超时（_connectionTimeOu毫秒）
		if (_connectionQue.empty()&&cv_status::timeout== cv.wait_for(lock, chrono::milliseconds(_connectionTimeOut))) {
			LOG("获取连接超时.......获取连接失败");
			return nullptr;
		}
	}
	//创建智能指针sp,指向队头元素
	//用lambda表达式重写智能指针的删除器，使之出作用域后connection被回收而不是析构
	shared_ptr<Connection> sp(_connectionQue.front(), [&](Connection*ptr_conn) {
		//保证访问_connectionQue的线程安全，必须先拿到锁才能访问否则等待
		unique_lock<mutex> lock(_queueMutex);
		//lock_guard<mutex> lock(_queueMutex);
		_connectionQue.push(ptr_conn);
		ptr_conn->refreshAliveTime();
		});
	//队头出队
	_connectionQue.pop();
	//通知生产者，检查是否需要生产连接
	cv.notify_all();

	return sp;
}
