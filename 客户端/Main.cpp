#define HAVE_STRUCT_TIMESPEC
#include "Winsock2.h"  
#include"SHA.h"
#include <windows.h> 
#include <math.h> 
#include <process.h>  
#include<stdio.h>
#include <WS2tcpip.h>
#include "string"  
#include<pthread.h>

#include<rsa.h>
#include"myRSA.h"
#include"mySHA.h"
#include"myAES.h"
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib, "pthreadVC2.lib")
#pragma comment(lib,"Msimg32.lib")
#define MAX 1024
#define HOST_IP "127.0.0.1"  
#define OK_STR "���ӷ������ɹ�"  
#define ERROR "�������"
#define NOT_FIND "���޴���"
#define MSG_HOST_PORT 8080  
#define LOGIN_HOST_PORT 8081  
#define CLIENT_PORT  8088
#define LOGIN 1                 //��Ϣ���ͱ�� ��1�����¼��Ϣ��2��������Ϣ�� 3���������û�����֪ͨ
#define MSG 2
#define NEW_USR 3
char UserName[MAX];
char PassWord[MAX];
char cRecvBuf[MAX];
//string aesKey = "0123456789ABCDEF0123456789ABCDEF";//256bits, also can be 128 bits or 192bits  
struct UserInfo       //ÿһ�������ϵ��û���Ϣ
{
	char Name[MAX];   //�˺�
	SOCKADDR_IN addr_Clt;  //IP��ַ
};
using namespace std;
string aesKey = "0123456789ABCDEF0123456789ABCDEF";//256bits, also can be 128 bits or 192bits  
void * Listen_Server(void *ptr);    //����������͵�¼���󣬲������̼߳�������������������Ϣ
void * Listen_OtherClient(void *ptr);
UserInfo Client[20];
int main()
{
	
	cout << "����Y/Nѡ���Ƿ����²���һ��RSA��Կ��Կ\n";
	char OP;
	cin >> OP;
	if (OP == 'Y')
	{
		char thisSeed[1024], privFilename[128], pubFilename[128];
		unsigned int keyLength;
		cout << "Key length in bits: ";
		cin >> keyLength;

		cout << "\nSave private key to file: ";
		cin >> privFilename;

		cout << "\nSave public key to file: ";
		cin >> pubFilename;

		cout << "\nRandom Seed: ";
		ws(cin);
		cin.getline(thisSeed, 1024);
		GenerateRSAKey(keyLength, privFilename, pubFilename, thisSeed);
	}
	
	char Message[MAX];
	cout << "�����¼�ʺ�" << endl;
	//scanf_s("%s,%s", &UserName, &PassWord);
	cin >> UserName;
	cout << "�����¼����" << endl;
	cin >> PassWord;
	
	pthread_t RECV_Server;                //�����߳̽�����Ϣ
	pthread_create(&RECV_Server, NULL, Listen_Server, 0); //Ϊ��ʹ���治����������ʹ�ö��߳�
	//pthread_t RECV_Other;
	//pthread_create(&RECV_Other, NULL, Listen_OtherClient, 0);
	
	int version_a = 1;//low bit                    �ⲿ������������Ϣ 
	int version_b = 1;//high bit  
					  //makeword  
	WORD versionRequest = MAKEWORD(version_a, version_b);
	WSAData wsaData;
	int error;
	error = WSAStartup(versionRequest, &wsaData);

	if (error != 0) {
		printf("ERROR!");

	}
	//check whether the version is 1.1, if not print the error and cleanup wsa?  
	if (LOBYTE(wsaData.wVersion) != 1 || HIBYTE(wsaData.wVersion) != 1)
	{
		printf("WRONG WINSOCK VERSION!");
		WSACleanup();

	}
	char requestStr[MAX];
	SOCKET MSGsocClient;
	SOCKADDR_IN MSGaddrSrv;
	//build a sockeet   
	MSGsocClient = socket(AF_INET, SOCK_DGRAM, 0);
	MSGaddrSrv;        // a instance of SOCKADDR_IN, which is used in format of SOCKADDR.  
	inet_pton(AF_INET, "127.0.0.1", (void *)&MSGaddrSrv.sin_addr.S_un.S_addr);        //set the host IP  
	MSGaddrSrv.sin_family = AF_INET;     //set the protocol family  
	MSGaddrSrv.sin_port = htons(MSG_HOST_PORT);      //set the port number  

													 // array to store the data that server feedback.  
	char cSendBuf[MAX];
	char EncryptMSG[MAX];
	int fromlen = sizeof(SOCKADDR);
	while (true)
	{
		cout << "�����뷢������\n" << endl;
		cin >> cSendBuf;
		string Encry = ECB_AESEncryptStr(aesKey,cSendBuf);
		strcpy_s(EncryptMSG,MAX, Encry.c_str());
		sendto(MSGsocClient, EncryptMSG, strlen(EncryptMSG) + 1, 0, (SOCKADDR*)&MSGaddrSrv, sizeof(SOCKADDR));
		
	}


}


void * Listen_Server(void *ptr)
{
	int ch;
	char Cname[MAX];
	char Cip[MAX];
	int Client_Num = 0;

	
	int version_a = 1;//low bit  
	int version_b = 1;//high bit  
					  //makeword  
	WORD versionRequest = MAKEWORD(version_a, version_b);
	WSAData wsaData;
	int error;
	error = WSAStartup(versionRequest, &wsaData);

	if (error != 0) {
		printf("ERROR!");

	}
	//check whether the version is 1.1, if not print the error and cleanup wsa?  
	if (LOBYTE(wsaData.wVersion) != 1 || HIBYTE(wsaData.wVersion) != 1)
	{
		printf("WRONG WINSOCK VERSION!");
		WSACleanup();

	}
	char requestStr[MAX];
	SOCKET socClient;
	SOCKADDR_IN addrSrv;
	//build a sockeet   
	socClient = socket(AF_INET, SOCK_DGRAM, 0);
       // a instance of SOCKADDR_IN, which is used in format of SOCKADDR.  
	inet_pton(AF_INET, "127.0.0.1", (void *)&addrSrv.sin_addr.S_un.S_addr);        //set the host IP  
	addrSrv.sin_family = AF_INET;     //set the protocol family  
	addrSrv.sin_port = htons(LOGIN_HOST_PORT);      //set the port number  

													// array to store the data that server feedback.  
	
	int fromlen = sizeof(SOCKADDR);
	sendto(socClient, ECB_AESEncryptStr(aesKey, UserName).c_str(), strlen(ECB_AESEncryptStr(aesKey, UserName).c_str()) + 1, 0, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));
	Sleep(500);
	sendto(socClient, ECB_AESEncryptStr(aesKey, PassWord).c_str(), strlen(ECB_AESEncryptStr(aesKey, PassWord).c_str()) + 1, 0, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));
	//addrSrv.sin_port = htons(MSG_HOST_PORT);      //set the port number  
	char DecryptMSG[MAX];
	FILE *log;   //д��־
	FILE *READ;
	string Digest;
	
	string Sign;
	char Rstr[MAX * 5];

	cout << "������������Ϣ�߳������ɹ�\n";
	while (true)
	{
		for (ch = 0; ch < MAX; ch++)
		{
			Cname[ch] = '\0';
			Cip[ch] = '\0';
		}
		recv(socClient, cRecvBuf, MAX, 0);	 
		string  De = ECB_AESDecryptStr(aesKey, cRecvBuf);
		fopen_s(&log,"log.txt", "a+");
		 fputs(cRecvBuf, log);
		 fputc('\n', log);
		 fclose(log);   
		 
		 fopen_s(&READ, "log.txt","r");
		 fread(Rstr, 5*MAX, 1,READ);
		                                   //����HASH
		 fclose(READ);
		 CalculateDigest(Digest, (string)Rstr);
		 Sign= ECB_AESEncryptStr(aesKey,Digest.c_str());  //j����HASHֵ
		 fopen_s(&READ, "sign.txt", "w");
		 fputs(Sign.c_str(),READ);
		 fclose(READ);                 //дǩ�����ļ�
		 
	//	 RSASignFile("pri", "log.txt","RSASign");
		//cout <<"mes"<< ECB_AESDecryptStr(aesKey,cRecvBuf) << endl;   //����
		
		 strcpy_s(DecryptMSG, MAX, De.c_str());
		//strcpy_s(DecryptMSG, sizeof(De.c_str()), De.c_str());
		cout <<"message:"<< De.c_str() << endl;
		
		if(strcmp(De.c_str(),OK_STR)==0)
		    cout <<"���ӷ������ɹ�  \n" << endl;
		else if(strcmp(De.c_str(), ERROR) == 0)
			cout << "�������  \n"  << endl;
		else if (strcmp(De.c_str(), NOT_FIND) == 0)
			cout << "���޴���  \n" << endl;
			
		else if(DecryptMSG[0]=='!')
		{

			for (ch = 1; DecryptMSG[ch] != '#'; ch++)
			{
				Cname[ch] = DecryptMSG[ch];
			}
			int hhh = 0;
			for (ch=ch+1; ch < strlen(DecryptMSG); ch++)
			{
				Cip[hhh] = DecryptMSG[ch];
				hhh++;
			}
			//strcpy_s(Client[Client_Num].Name, strlen(Cname), Cname);
			for (int fff = 0; fff < strlen(Cname); fff++)
			{
				Client[Client_Num].Name[fff] = Cname[fff];
			} 
			//Client[Client_Num].addr_Clt.sin_addr = inet_pton((int)Cip);
			inet_pton(AF_INET, Cip, &Client[Client_Num].addr_Clt.sin_addr);//���ַ���ת����IP
			Client[Client_Num].addr_Clt.sin_port = MSG_HOST_PORT;

		}
		else
		{
			cout << "����Ϣ: " << DecryptMSG << endl;
		}
		
	}
}

void * Listen_OtherClient(void *ptr)
{
	
	int version_a = 1;//low bit  
	int version_b = 1;//high bit  
					  //makeword  
	WORD versionRequest = MAKEWORD(version_a, version_b);
	WSAData wsaData;
	int error;
	error = WSAStartup(versionRequest, &wsaData);

	if (error != 0) {
		printf("ERROR!");

	}
	//check whether the version is 1.1, if not print the error and cleanup wsa?  
	if (LOBYTE(wsaData.wVersion) != 1 || HIBYTE(wsaData.wVersion) != 1)
	{
		printf("WRONG WINSOCK VERSION!");
		WSACleanup();

	}
	char requestStr[MAX];
	/*
	SOCKET socClient;
	SOCKADDR_IN addrSrv;
	//build a sockeet   
	socClient = socket(AF_INET, SOCK_DGRAM, 0);
	addrSrv;        // a instance of SOCKADDR_IN, which is used in format of SOCKADDR.  
	inet_pton(AF_INET, "127.0.0.1", (void *)&addrSrv.sin_addr.S_un.S_addr);        //set the host IP  
	addrSrv.sin_family = AF_INET;     //set the protocol family  
	addrSrv.sin_port = htons(CLIENT_PORT);      //set the port number  

													// array to store the data that server feedback.  
*/
	SOCKET socClient = socket(AF_INET, SOCK_DGRAM, 0);
	SOCKADDR_IN addrSrv;        // a instance of SOCKADDR_IN, which is used in format of SOCKADDR.  
	inet_pton(AF_INET, "127.0.0.1", (void *)&addrSrv.sin_addr.S_un.S_addr);        //set the host IP  
	addrSrv.sin_family = AF_INET;     //set the protocol family  
	addrSrv.sin_port = htons(MSG_HOST_PORT);      //set the port number  

	int fromlen = sizeof(SOCKADDR);
	SOCKADDR_IN in_addr;        //�����ַ
	
	//addrSrv.sin_port = htons(MSG_HOST_PORT);      //set the port number  
	char DecryptMSG[MAX];
	FILE *log;   //д��־
	char Buf[MAX];
	cout << "������Ϣ�߳������ɹ�\n";
	while (true)
	{
		
		
		recv(socClient, Buf, strlen(Buf) + 1, 0);
		fopen_s(&log, "msg.txt", "at+");
		fputs(Buf, log);
		fputc('\n', log);
		fclose(log);                                        //д��־
		RSASignFile("pri", "log.txt", "RSASign.txt");
		//string De = ECB_AESDecryptStr(aesKey, cRecvBuf);   //����
		cout << "�յ�һ����Ϣ: \n" << ECB_AESDecryptStr(aesKey, Buf) << endl;
		
	}
}