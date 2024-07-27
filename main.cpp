#include"Pch.h"
#include<iostream>
#include"Connection.h"
#include"CommonConnectionPool.h"
using namespace std;
int  main() {
	Connection conn;
	//char sql[1024] = {0};
	//中文插不进去，要改字符集
	//sprintf(sql, "insert into user(name,age,sex) values('%s','%d','%s')", "aaaaa", 20, "male");
	conn.connect("127.0.0.1", 3306, "root", "082312", "chat");
	//conn.update(sql);

	////////单线程测试
	/*clock_t begin = clock();
	ConnectionPool* cp = ConnectionPool::getConnectionPool();
	for (int i = 0; i < 10000; i++) {
		//不使用连接池
		Connection conn;
		char sql[1024] = { 0 };
		sprintf(sql, "insert into user(name,age,sex) values('%s','%d','%s')", "aaaaa", 20, "male");
		conn.connect("127.0.0.1", 3306, "root", "082312", "chat");
		conn.update(sql);
		//使用连接池的
		shared_ptr<Connection>sp = cp->getConnection();
		char sql[1024] = { 0 };
		sprintf(sql, "insert into user(name,age,sex) values('%s','%d','%s')", "aaaaa", 20, "male");
		sp->update(sql);
	}
	clock_t end =clock();
	cout<<(end-begin)<<"ms"<<endl;*/

	////////多线程
	clock_t begin = clock();
	
	thread t1([&]() {
		//不使用连接池
		for (int i = 0; i < 2500; ++i) {
			Connection conn1;
		char sql[1024] = {0};
		sprintf(sql, "insert into user(name,age,sex) values('%s','%d','%s')", "aaaaa", 20, "male");
		conn1.connect("127.0.0.1", 3306, "root", "082312", "chat");
		conn1.update(sql);

		}
		/*//使用连接池的
		ConnectionPool* cp = ConnectionPool::getConnectionPool();
		for (int i = 0; i < 2500; i++) {
			shared_ptr<Connection>sp = cp->getConnection();
			char sql[1024] = { 0 };
			sprintf(sql, "insert into user(name,age,sex) values('%s','%d','%s')", "aaaaa", 20, "male");
			sp->update(sql);
		}*/
		});
	thread t2([&]() {
		//不使用连接池
		for (int i = 0; i < 2500; ++i) {
			Connection conn1;
			char sql[1024] = { 0 };
			sprintf(sql, "insert into user(name,age,sex) values('%s','%d','%s')", "aaaaa", 20, "male");
			conn1.connect("127.0.0.1", 3306, "root", "082312", "chat");
			conn1.update(sql);
		}
		/*//使用连接池的
	ConnectionPool* cp = ConnectionPool::getConnectionPool();
	for (int i = 0; i < 2500; i++) {
		shared_ptr<Connection>sp = cp->getConnection();
		char sql[1024] = { 0 };
		sprintf(sql, "insert into user(name,age,sex) values('%s','%d','%s')", "aaaaa", 20, "male");
		sp->update(sql);
	}*/
		});
	thread t3([&]() {
		//不使用连接池
		//不使用连接池
		for (int i = 0; i < 2500; ++i) {
			Connection conn1;
			char sql[1024] = { 0 };
			sprintf(sql, "insert into user(name,age,sex) values('%s','%d','%s')", "aaaaa", 20, "male");
			conn1.connect("127.0.0.1", 3306, "root", "082312", "chat");
			conn1.update(sql);
		}
		/*//使用连接池的
	ConnectionPool* cp = ConnectionPool::getConnectionPool();
	for (int i = 0; i < 2500; i++) {
		shared_ptr<Connection>sp = cp->getConnection();
		char sql[1024] = { 0 };
		sprintf(sql, "insert into user(name,age,sex) values('%s','%d','%s')", "aaaaa", 20, "male");
		sp->update(sql);
	}*/
		});
	thread t4([&]() {
		//不使用连接池
		for (int i = 0; i < 2500; ++i) {
			Connection conn1;
			char sql[1024] = { 0 };
			sprintf(sql, "insert into user(name,age,sex) values('%s','%d','%s')", "aaaaa", 20, "male");
			conn1.connect("127.0.0.1", 3306, "root", "082312", "chat");
			conn1.update(sql);
		}
		/*//使用连接池的
	ConnectionPool* cp = ConnectionPool::getConnectionPool();
	for (int i = 0; i < 2500; i++) {
		shared_ptr<Connection>sp = cp->getConnection();
		char sql[1024] = { 0 };
		sprintf(sql, "insert into user(name,age,sex) values('%s','%d','%s')", "aaaaa", 20, "male");
		sp->update(sql);
	}*/
		});
	//主线程等待子线程完成
	t1.join();
	t2.join();
	t3.join();
	t4.join();
	clock_t end = clock();
	cout << (end - begin) << "ms" << endl;
	cout << "***************************" << endl;
	return 0;
}