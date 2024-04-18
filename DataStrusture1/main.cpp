#include <iostream>
#include <string>
#include <mysql.h>
using namespace std;

class Sql
{
	string host;
	string user;
	string passwd;
	string db;
	unsigned int port;
	MYSQL sqlcon;
	MYSQL_RES* result;
	MYSQL_ROW row;
public:
	Sql();
	Sql(string host, string user, string passwd, string db, unsigned int port);
	~Sql();
	MYSQL getSqlCon();			//��ȡ����
	MYSQL_RES* getResult();	//��ȡres
	MYSQL_ROW  getRow();
	boolean isAccount(string user, string passwd, int type);  //�˻���֤
	int getDataCount();				 						//��ȡ���ݣ�����ֵΪ����������
	void FrozenAccount(string username);      		 		//�����˻�
	int getFundCount(Person* person);						//��ȡ�ʽ�����
	int getFundCount(string account);						//��ȡ�ʽ�����
	int FrozenMoney(string name, int fund);				//������ǰ�жϲ�Ҫ����ȫ��
	string getPasswd(string username); 						//��ȡ����
	void RecordInfo(string username, string event);			//��¼��ˮ
	int getMoneyInfo();    									//�˻���ˮ
	void Save_DrawMoney(Person* person, int fund);			//���/ȡ��
	int TransferAccounts(Person* person, string account, int fund);	//ת��
	void AlterPassword(Person* person, string str);					//�޸�����
	int DeleteAccount(Person* person);								//ע���˻� ����н�����Ҫȡ��
};

class Person
{
protected:
	std::string m_person;
	std::string m_passwd;
public:
	virtual std::string getM_Person() = 0;
	virtual std::string getM_Passwd() = 0;
	virtual void ShowBusiness(Sql* sqlcon) = 0;
};

class Administrator :public Person
{
public:
	Administrator();
	~Administrator();
	virtual std::string getM_Person();
	virtual std::string getM_Passwd();
	virtual void ShowBusiness(Sql* sqlcon);
	void getUserinfo(Sql* sqlcon);			//�����û��˻�
	void BlockedAccount(Sql* sqlcon);		//�����˻�
	void FrozenCapital(Sql* sqlcon);		//�����ʽ�
	void getUserPasswd(Sql* sqlcon);		//��ѯ�û�����
	void getUserStatement(Sql* sqlcon);	//��ѯ�û���ˮ
};

class User :public Person
{
public:
	User();
	~User();
	virtual std::string getM_Person();
	virtual std::string getM_Passwd();
	virtual void ShowBusiness(Sql* sqlcon);
	void Deposit(Sql* sqlcon);			//���
	void Withdrawal(Sql* sqlcon);		//ȡ��
	void TransferAccounts(Sql* sqlcon);//ת��
	void getUserStatement(Sql* sqlcon);//��ѯ�û���ˮ
	void ChangePasswd(Sql* sqlcon);	//��������
	void LogoutUser(Sql* sqlcon);		//ע���˻�
};

Sql::Sql()
	:host("127.0.0.1"), user("root"), passwd("3226960*"), db("db_banking_system"), port(3306)
{
	mysql_init(&(this->sqlcon));
	mysql_real_connect(&(this->sqlcon), this->host.c_str(), this->user.c_str(),
		this->passwd.c_str(), this->db.c_str(), this->port, NULL, 0);
}
Sql::Sql(string host, string user, string passwd, string db, unsigned int port)
{
	this->host = host;
	this->user = user;
	this->passwd = passwd;
	this->db = db;
	this->port = port;
	mysql_init(&(this->sqlcon));
	mysql_real_connect(&(this->sqlcon), this->host.c_str(), this->user.c_str(),
		this->passwd.c_str(), this->db.c_str(), this->port, NULL, 0);
}
Sql::~Sql()
{
	mysql_close(&(this->sqlcon));
}
MYSQL Sql::getSqlCon()
{
	return this->sqlcon;
}
MYSQL_RES* Sql::getResult()
{
	return this->result;
}
MYSQL_ROW  Sql::getRow()
{
	return this->row;
}
boolean Sql::isAccount(string user, string passwd, int type)
{
	string str_a = "SELECT * FROM dt_administrator where user = '" + user + "' and passwd = '" + passwd + "'";
	string str_u = "SELECT * FROM dt_user where user = '" + user + "' and passwd = '" + passwd + "'";
	if (type == 0)
	{
		mysql_query(&(this->sqlcon), str_a.c_str());
		result = mysql_store_result(&(this->sqlcon));
		while ((row = mysql_fetch_row(result)))
		{
			return true;
		}
	}
	if (type == 1)
	{
		mysql_query(&(this->sqlcon), str_u.c_str());
		result = mysql_store_result(&(this->sqlcon));
		while ((row = mysql_fetch_row(result)))
		{
			return true;
		}
	}
	return false;
}
int Sql::getDataCount()
{
	mysql_query(&(this->sqlcon), "select * from dt_user");
	this->result = mysql_store_result(&(this->sqlcon));
	return mysql_num_rows(this->result);//������������
}
void Sql::FrozenAccount(string username)
{
	string sqlstr = "update dt_user set isfrozen = 1 where user = '" + username + "'";
	mysql_query(&(this->sqlcon), sqlstr.c_str());
}
int Sql::getFundCount(Person* person) //��ȡ�ʽ���
{
	string sqlstr = "select * from dt_user where user = '" + person->getM_Person() + "'";
	mysql_query(&(this->sqlcon), sqlstr.c_str());
	this->result = mysql_store_result(&(this->sqlcon));
	this->row = mysql_fetch_row(result);
	stringstream ss;
	int fund_i = 0;
	string fun_s;
	fun_s = this->row[2];
	ss << fun_s;
	ss >> fund_i;
	return fund_i;
}
int Sql::getFundCount(string account) //��ȡ�ʽ���
{
	string strsql = "select * from dt_user where user = '" + account + "'";
	mysql_query(&(this->sqlcon), strsql.c_str());
	this->result = mysql_store_result(&(this->sqlcon));
	this->row = mysql_fetch_row(result);
	stringstream ss;
	int fund_i = 0;
	string fun_s;
	fun_s = this->row[2];
	ss << fun_s;
	ss >> fund_i;
	return fund_i;
}
int Sql::FrozenMoney(string account, int fund)
{
	//���������
	int fund_now = getFundCount(account);
	if (fund_now >= fund)
	{
		int fund_i = fund_now - fund;
		string fund_s1;
		stringstream ss;
		ss << fund_i;
		ss >> fund_s1;
		stringstream s;
		string fund_s2;
		s << fund;
		s >> fund_s2;
		string str = "update dt_user set fund = " + fund_s1 + ",frozenfund = " + fund_s2 + " where user = '" + account + "'";
		cout << "sql:" << str << endl;
		mysql_query(&(this->sqlcon), str.c_str());
		system("pause");
		return 1;
	}
	else
	{
		return 0;
	}
}
string Sql::getPasswd(string username)
{
	string str = "select * from dt_user where user = '" + username + "'";
	mysql_query(&(this->sqlcon), str.c_str());
	this->result = mysql_store_result(&(this->sqlcon));
	this->row = mysql_fetch_row(result);
	return this->row[1];
}
void Sql::RecordInfo(string username, string event)
{
	//��¼��ˮ
	string strsql = "insert into dt_event(user,event) values('" + username + "','" + event + "')";
	mysql_query(&(this->sqlcon), "SET CHARACTER SET GBK");
	if (mysql_query(&(this->sqlcon), strsql.c_str()))
	{
		cout << "���ɹ�";
	}
}
int Sql::getMoneyInfo()
{
	//��ˮ
	string strsql = "select * from dt_event";
	mysql_query(&(this->sqlcon), strsql.c_str());
	this->result = mysql_store_result(&(this->sqlcon));
	return mysql_num_rows(this->result);
}
void Sql::Save_DrawMoney(Person* person, int fund)
{
	string fund_s;
	stringstream ss;
	ss << fund;
	ss >> fund_s;
	string str = "update dt_user set fund = " + fund_s + " where user = '" + person->getM_Person() + "'";
	mysql_query(&(this->sqlcon), str.c_str());
}
int Sql::TransferAccounts(Person* person, string account, int fund)
{
	//���˻�
	string strsql = "select * from dt_user where user = '" + account + "'";
	mysql_query(&(this->sqlcon), strsql.c_str());
	result = mysql_store_result(&(this->sqlcon));
	while ((row = mysql_fetch_row(result)))
	{
		//�����
		int fund_now = getFundCount(person);
		if (fund_now >= fund)
		{
			cout << "���н�" << fund_now << "  ת����" << fund << endl;
			Save_DrawMoney(person, fund_now - fund);
			stringstream ss;
			int fund_i = 0;
			string fun_s;
			fun_s = this->row[2];
			ss << fun_s;
			ss >> fund_i;
			stringstream ss_2;
			ss_2 << fund + fund_i;
			string fund_s2;
			ss_2 >> fund_s2;
			cout << "��ȷ��ת��";
			system("pause");
			strsql = "update dt_user set fund = " + fund_s2 + " where user = '" + account + "'";
			mysql_query(&(this->sqlcon), strsql.c_str());
			return 1;
		}
		else
		{
			return 0;
		}
	}
	return 0;
}
void Sql::AlterPassword(Person* person, string str)
{
	string passwd = "update dt_user set passwd = " + str + " where user = '" + person->getM_Person() + "'";
	mysql_query(&(this->sqlcon), passwd.c_str());
}
int Sql::DeleteAccount(Person* person)
{
	int fund = getFundCount(person);
	if (fund == 0)
	{
		string str = "delete from dt_user where user = '" + person->getM_Person() + "'";
		mysql_query(&(this->sqlcon), str.c_str());
		return 0;
	}
	else
	{
		return fund;
	}
	return fund;
}

int main()
{
	User* user = new User();
	int system_i = 0;
	Sql* sql = new Sql();
	while (true)
	{
		system("cls");
		cout << "\t\t================================================================" << endl;
		cout << "\t\t*                      ��ѡ������ϵͳ                        *" << endl;
		cout << "\t\t*                                                              *" << endl;
		cout << "\t\t*               ����Ա                   �ͻ�                  *" << endl;
		cout << "\t\t*                                                              *" << endl;
		cout << "\t\t*                0                        1                    *" << endl;
		cout << "\t\t================================================================" << endl;
		cin >> system_i;
		cin.clear();
		cin.ignore(); //��ֹ�������뵼����ѭ��
		switch (system_i)
		{
		case 0:
			LoginIn(sql, system_i);
			break;
		case 1:
			LoginIn(sql, system_i);
			break;
		default:
			cout << "������������������" << endl;
			system("pause");
			break;
		}
	}
	return 0;
}
void LoginIn(Sql* sqlcon, int type)
{
	string name;
	string passwd;
	Person* person = NULL;
	if (type == 0)
	{
		cout << "���������Ա�˻���" << endl;
		cin >> name;
		cout << "���������Ա���룺" << endl;
		cin >> passwd;
		if (sqlcon->isAccount(name, passwd, type))
		{
			person = new Administrator();
			person->ShowBusiness(sqlcon);
		}
		return;
	}
	else
	{
		cout << "�������˻���" << endl;
		cin >> name;
		cout << "���������룺" << endl;
		cin >> passwd;
		if (sqlcon->isAccount(name, passwd, type))
		{
			person = new User();
			person->ShowBusiness(sqlcon);
		}
		return;
	}
}


