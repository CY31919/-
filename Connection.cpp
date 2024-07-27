#include"Pch.h"
#include"Connection.h"
#include"Public.h"

Connection::Connection() {
	_conn = mysql_init(nullptr);
}
Connection::~Connection() {
	if (_conn != nullptr) {
		mysql_close(_conn);
	}
}
bool Connection::connect(string ip, unsigned short port, string username, string password, string dbname) {
	if (!_conn) {
		LOG("��ʼ��ʧ��");
		return false; // _conn ��ʼ��ʧ��
	}
	MYSQL* p = mysql_real_connect(_conn, ip.c_str(), username.c_str(), password.c_str(), dbname.c_str(), port, nullptr, 0);
	return p != nullptr;
}
bool Connection::update(string sql) {
	if (mysql_query(_conn, sql.c_str())) {
		LOG("����ʧ�ܣ�");
		return false;
	}
	return true;
}
MYSQL_RES* Connection::query(string sql) {
	//��ѯ����
	if (mysql_query(_conn, sql.c_str())) {
		LOG("��ѯʧ�ܣ�", +sql);
		return nullptr;
	}
	return mysql_use_result(_conn);
}