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
/*数据库连接池的实现*/
class ConnectionPool {
public:
	//提供访问接口，获取连接池对象实例，利用了类加载机制来确保只会初始化一次，并且避免了同步的开销
	static ConnectionPool* getConnectionPool();
	//给外部提供接口，从连接池中获取一个可用的空闲连接
	//使用智能指针，智能指针出作用域会自动析构，进而释放连接，可以重定义删除器，使之不析构而是返回队列
     shared_ptr<Connection> getConnection();

private :
	//构造方法私有化,防止外部实例化
	ConnectionPool();
	//加载配置文件
	bool loadConfigFile();
	//在独立的线程中，专门负责生产新连接
	void produceConnectionTask();
	//在独立的线程中，检测是否有连接需要回收
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
	queue<Connection*>  _connectionQue;//存储“连接”队列
	mutex _queueMutex;

	atomic_int _connectionCnt;//记录所创建的connection连接的总数量
	condition_variable cv;//设置条件变量，用于连接生产线程和连接消费线程的通信
};