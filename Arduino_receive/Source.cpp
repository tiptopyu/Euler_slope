#define _USE_MATH_DEFINES
#include <math.h>
#include <string>
#include <fstream>
#include <Windows.h>
#include <stdio.h>
#include <iostream>
using namespace std;

HANDLE init_arduino(int arduinoCOM, HANDLE hComm)
{
	string com = "\\\\.\\COM" + to_string(arduinoCOM);
	hComm = CreateFile(com.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hComm == INVALID_HANDLE_VALUE){
		printf("�V���A���|�[�g���J�����Ƃ��ł��܂���ł����B");
		char z;
		z = getchar();
		return 0;
	}
	//�|�[�g���J���Ă���ΒʐM�ݒ���s��
	else
	{
		DCB lpTest;
		GetCommState(hComm, &lpTest);
		lpTest.BaudRate = 115200;
		lpTest.ByteSize = 8;
		lpTest.Parity = NOPARITY;
		lpTest.StopBits = ONESTOPBIT;
		SetCommState(hComm, &lpTest);
	}
	printf("opened!!\n");

	unsigned char	sendbuf[1];
	int				ret;
	unsigned long	len;
	unsigned char	receive_data[30] = {};

	//Sleep(3000);
	
	// �o�b�t�@�N���A
	memset(sendbuf, 0x00, sizeof(sendbuf));
	// �p�P�b�g�쐬
	sendbuf[0] = (unsigned char)1;
	// �ʐM�o�b�t�@�N���A
	PurgeComm(hComm, PURGE_RXCLEAR);
	// ���M
	ret = WriteFile(hComm, &sendbuf, 5, &len, NULL);

	memset(receive_data, 0x00, sizeof(receive_data));
	// �ʐM�o�b�t�@�N���A
	PurgeComm(hComm, PURGE_RXCLEAR);
	// Arduino����f�[�^����M
	ret = ReadFile(hComm, &receive_data, 1, &len, NULL);

	Sleep(3000);
	return hComm;
}
void tile_cal(float Euler[3], double d_t[2])
{
	for (int i = 0; i < 3; i++)
	{
		Euler[i] = Euler[i] * M_PI / 180;
	}


	double x, y, z;
	x = abs(tan((double)Euler[2])) / sqrt(pow(sin((double)Euler[1]), 2) + pow(tan((double)Euler[2]), 2));
	y = abs(x*sin((double)Euler[2]));
	z = y*abs(cos((double)Euler[1])) + abs(sin(acos(x)))*abs(sin((double)Euler[1]));


	if (Euler[1] <= 0 && Euler[2] <= 0)
	{
		d_t[0] = acos(x);
		d_t[1] = asin(z);
	}
	else if (Euler[1] <= 0 && Euler[2] > 0)
	{
		d_t[0] = M_PI - acos(x);
		d_t[1] = asin(z);
	}
	else if (Euler[1] > 0 && Euler[2] > 0)
	{
		d_t[0] = acos(x) - M_PI;
		d_t[1] = asin(z);
	}
	else if (Euler[1] > 0 && Euler[2] <= 0)
	{
		d_t[0] = -acos(x);
		d_t[1] = asin(z);
	}
	printf("%lf", 2 * M_PI - Euler[0]);
	printf("(��,��) = (%lf,%lf)\n", d_t[0], d_t[1]);
}

void receive_euler(HANDLE hComm, float Euler[3])
{
	
	DWORD start, end;
	start = GetTickCount();
	bool isInitialized = false;
	unsigned char	sendbuf[1];
	unsigned char	receive_data[30] = {};
	int				ret;
	float			DL, DR, DIS, ANG;
	unsigned long	len;
	char *euler_1, *euler_2, *euler_3;
	int i = 0;
	float			droidOrientation[3];


	end = GetTickCount();
	if ((end - start) < 15)
	{
		int sleeptime = 15 - (int)(end - start);
		Sleep(sleeptime);
		end = GetTickCount();
	}
	
	if (!hComm)	return;
	// �o�b�t�@�N���A
	memset(sendbuf, 0x00, sizeof(sendbuf));
	// �p�P�b�g�쐬
	sendbuf[0] = (unsigned char)1;
	// �ʐM�o�b�t�@�N���A
	PurgeComm(hComm, PURGE_RXCLEAR);
	// ���M
	ret = WriteFile(hComm, &sendbuf, 1, &len, NULL);

	
	// �o�b�t�@�N���A
	memset(receive_data, 0x00, sizeof(receive_data));
	// �ʐM�o�b�t�@�N���A
	PurgeComm(hComm, PURGE_RXCLEAR);
	// Arduino����f�[�^����M
	ret = ReadFile(hComm, &receive_data, 20, &len, NULL);
	//cout << static_cast<bitset<8>>(receive_data[0]) << "," << static_cast<bitset<8>>(receive_data[1] )<< endl;

	
	
	

	//����������Ă��Ȃ���Ώ�����(���߂̃f�[�^���̂Ă�)
	if (!isInitialized)
	{
		isInitialized = true;
		//return ;
	}
	
	printf("%s...\n", receive_data);
	euler_1 = strtok((char*)receive_data, ",");
	euler_2 = strtok(NULL, ",");
	euler_3 = strtok(NULL, ",");


	Euler[0] = strtod(euler_1, NULL);
	Euler[1] = strtod(euler_2, NULL);
	Euler[2] = strtod(euler_3, NULL);
	printf("%f\n", Euler[0]);
	printf("%f\n", Euler[1]);
	printf("%f\n", Euler[2]);
}
void main()
{
	int arduinoCOM=71;
	HANDLE hComm=NULL;
	//�V���A���|�[�g���J���ăn���h�����擾
	hComm = init_arduino(arduinoCOM, hComm);
	/*
	string com = "\\\\.\\COM" + to_string(arduinoCOM);
	hComm = CreateFile(com.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hComm == INVALID_HANDLE_VALUE){
		printf("�V���A���|�[�g���J�����Ƃ��ł��܂���ł����B");
		char z;
		z = getchar();
		return;
	}
	//�|�[�g���J���Ă���ΒʐM�ݒ���s��
	else
	{
		DCB lpTest;
		GetCommState(hComm, &lpTest);
		lpTest.BaudRate = 115200;
		lpTest.ByteSize = 8;
		lpTest.Parity = NOPARITY;
		lpTest.StopBits = ONESTOPBIT;
		SetCommState(hComm, &lpTest);
	}
	printf("opened!!\n");
	*/

	bool isInitialized = false;
	unsigned char	sendbuf[1];
	unsigned char	receive_data[30] = {};
	int				ret;
	float			DL, DR, DIS, ANG;
	unsigned long	len;
	char *euler_1, *euler_2, *euler_3;
	float Euler[3];
	double d_t[2];
	int i = 0;
	float			droidOrientation[3];
	printf("waiting\n");
	

	while (1)
	{
		
		/*
		system("cls");
		//Sleep(20);
		printf("%d\n", hComm);
		// �n���h���`�F�b�N
		if (!hComm)	return;
		// �o�b�t�@�N���A
		memset(sendbuf, 0x00, sizeof(sendbuf));
		// �p�P�b�g�쐬
		sendbuf[0] = (unsigned char)1;
		// �ʐM�o�b�t�@�N���A
		PurgeComm(hComm, PURGE_RXCLEAR);
		// ���M
		ret = WriteFile(hComm, &sendbuf, 1, &len, NULL);


		// �o�b�t�@�N���A
		memset(receive_data, 0x00, sizeof(receive_data));
		// �ʐM�o�b�t�@�N���A
		PurgeComm(hComm, PURGE_RXCLEAR);
		// Arduino����f�[�^����M
		ret = ReadFile(hComm, &receive_data, 20, &len, NULL);
		//cout << static_cast<bitset<8>>(receive_data[0]) << "," << static_cast<bitset<8>>(receive_data[1] )<< endl;


		//����������Ă��Ȃ���Ώ�����(���߂̃f�[�^���̂Ă�)
		if (!isInitialized)
		{
			isInitialized = true;
			//return ;
		}
		printf("%s...\n", receive_data);
		euler_1 = strtok((char*)receive_data, ",");
		euler_2 = strtok(NULL, ",");
		euler_3 = strtok(NULL, ",");
		

		Euler[0] = strtod(euler_1, NULL);
		Euler[1] = strtod(euler_2, NULL);
		Euler[2] = strtod(euler_3, NULL);
		printf("%f\n", Euler[0]);
		printf("%f\n", Euler[1]);
		printf("%f\n", Euler[2]);
		*/
		receive_euler(hComm, Euler);
		

		
		tile_cal(Euler, d_t);
		

	}
}



/*

#include <string>
using namespace std;

int main()
{
	string str;
}*/