#include"Pch.h"
#include<iostream>
#include"Connection.h"
#include"CommonConnectionPool.h"
using namespace std;
int  main() {
	Connection conn;
	//char sql[1024] = {0};
	//���Ĳ岻��ȥ��Ҫ���ַ���
	//sprintf(sql, "insert into user(name,age,sex) values('%s','%d','%s')", "aaaaa", 20, "male");
	conn.connect("127.0.0.1", 3306, "root", "082312", "chat");
	//conn.update(sql);

	////////���̲߳���
	/*clock_t begin = clock();
	ConnectionPool* cp = ConnectionPool::getConnectionPool();
	for (int i = 0; i < 10000; i++) {
		//��ʹ�����ӳ�
		Connection conn;
		char sql[1024] = { 0 };
		sprintf(sql, "insert into user(name,age,sex) values('%s','%d','%s')", "aaaaa", 20, "male");
		conn.connect("127.0.0.1", 3306, "root", "082312", "chat");
		conn.update(sql);
		//ʹ�����ӳص�
		shared_ptr<Connection>sp = cp->getConnection();
		char sql[1024] = { 0 };
		sprintf(sql, "insert into user(name,age,sex) values('%s','%d','%s')", "aaaaa", 20, "male");
		sp->update(sql);
	}
	clock_t end =clock();
	cout<<(end-begin)<<"ms"<<endl;*/

	////////���߳�
	clock_t begin = clock();
	
	thread t1([&]() {
		//��ʹ�����ӳ�
		for (int i = 0; i < 2500; ++i) {
			Connection conn1;
		char sql[1024] = {0};
		sprintf(sql, "insert into user(name,age,sex) values('%s','%d','%s')", "aaaaa", 20, "male");
		conn1.connect("127.0.0.1", 3306, "root", "082312", "chat");
		conn1.update(sql);

		}
		/*//ʹ�����ӳص�
		ConnectionPool* cp = ConnectionPool::getConnectionPool();
		for (int i = 0; i < 2500; i++) {
			shared_ptr<Connection>sp = cp->getConnection();
			char sql[1024] = { 0 };
			sprintf(sql, "insert into user(name,age,sex) values('%s','%d','%s')", "aaaaa", 20, "male");
			sp->update(sql);
		}*/
		});
	thread t2([&]() {
		//��ʹ�����ӳ�
		for (int i = 0; i < 2500; ++i) {
			Connection conn1;
			char sql[1024] = { 0 };
			sprintf(sql, "insert into user(name,age,sex) values('%s','%d','%s')", "aaaaa", 20, "male");
			conn1.connect("127.0.0.1", 3306, "root", "082312", "chat");
			conn1.update(sql);
		}
		/*//ʹ�����ӳص�
	ConnectionPool* cp = ConnectionPool::getConnectionPool();
	for (int i = 0; i < 2500; i++) {
		shared_ptr<Connection>sp = cp->getConnection();
		char sql[1024] = { 0 };
		sprintf(sql, "insert into user(name,age,sex) values('%s','%d','%s')", "aaaaa", 20, "male");
		sp->update(sql);
	}*/
		});
	thread t3([&]() {
		//��ʹ�����ӳ�
		//��ʹ�����ӳ�
		for (int i = 0; i < 2500; ++i) {
			Connection conn1;
			char sql[1024] = { 0 };
			sprintf(sql, "insert into user(name,age,sex) values('%s','%d','%s')", "aaaaa", 20, "male");
			conn1.connect("127.0.0.1", 3306, "root", "082312", "chat");
			conn1.update(sql);
		}
		/*//ʹ�����ӳص�
	ConnectionPool* cp = ConnectionPool::getConnectionPool();
	for (int i = 0; i < 2500; i++) {
		shared_ptr<Connection>sp = cp->getConnection();
		char sql[1024] = { 0 };
		sprintf(sql, "insert into user(name,age,sex) values('%s','%d','%s')", "aaaaa", 20, "male");
		sp->update(sql);
	}*/
		});
	thread t4([&]() {
		//��ʹ�����ӳ�
		for (int i = 0; i < 2500; ++i) {
			Connection conn1;
			char sql[1024] = { 0 };
			sprintf(sql, "insert into user(name,age,sex) values('%s','%d','%s')", "aaaaa", 20, "male");
			conn1.connect("127.0.0.1", 3306, "root", "082312", "chat");
			conn1.update(sql);
		}
		/*//ʹ�����ӳص�
	ConnectionPool* cp = ConnectionPool::getConnectionPool();
	for (int i = 0; i < 2500; i++) {
		shared_ptr<Connection>sp = cp->getConnection();
		char sql[1024] = { 0 };
		sprintf(sql, "insert into user(name,age,sex) values('%s','%d','%s')", "aaaaa", 20, "male");
		sp->update(sql);
	}*/
		});
	//���̵߳ȴ����߳����
	t1.join();
	t2.join();
	t3.join();
	t4.join();
	clock_t end = clock();
	cout << (end - begin) << "ms" << endl;
	cout << "***************************" << endl;
	return 0;
}