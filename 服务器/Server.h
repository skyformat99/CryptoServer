#include "Winsock2.h"  
#include<string>
#define MAX 1024
#ifndef Server_H
#define Server_H
struct UserInfo       //ÿһ�������ϵ��û���Ϣ
{
	char Name[MAX];   //�˺�
	SOCKADDR_IN addr_Clt;  //IP��ַ
};
int   Login(UserInfo Client[20]);

#endif
