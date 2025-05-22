#include "common.h"



HANDLE consoleHandle;//ȫ�ֱ������κεط�������ʹ��

//��ͼ������ƫ����
COORD offset = { 10,5 };

//����һЩ��������ѧ����   50~90
int RandInRange(int min, int max)
{
	//0~32767   RAND_MAX  7fff
	int v=rand()%(max-min);

	return v+min;
}

//0~1


  //lib   ��̬���ӿ�(����������������api���ĵ���    dll  ��̬���ӿ⣨���л�����  .so
float RandInRange(float min, float max)
{
	float v=static_cast<float>(rand())/RAND_MAX;

	return v*(max-min)+min;
}



//����̨����ı����βΣ��ٿ�ȨHANDLE�����ݣ����꣬��ɫ��
void MyDrawText(HANDLE handle, LPCTSTR buf , COORD coord , WORD color)
{
	DWORD ret;
	WriteConsoleOutputCharacter(handle, buf, _tcslen(buf), coord, &ret);

	COORD colorCoord = coord;
	//�ַ������ֽ������ڴ�ռ䣩
	int len = _tcslen(buf);
	//Ⱦɫʱ�����뵥���ֽڵĿ��ƻ���
	for (int i = 0; i < len; i++)
	{
		WriteConsoleOutputAttribute(handle, &color, 1, colorCoord, &ret);

		colorCoord.X += 2;
	}
}

void SetStdHandle()
{
	consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
}

void DrawContent(const TCHAR buf[], int x, int y, WORD color )
{
	DWORD ret;
	COORD c = { static_cast<SHORT>(x), static_cast<SHORT>(y) };
	WriteConsoleOutputCharacter(consoleHandle, buf, _tcslen(buf), c, &ret);

	//������ɫ
	COORD c1;
	c1.X = c.X;
	c1.Y = c.Y;

	for (int i = 0; i < _tcslen(buf); i++)
	{
		WriteConsoleOutputAttribute(consoleHandle, &color, 1, c, &ret);

		c1.X++;
	}
}

void ConolePrintf(const TCHAR* buf)
{
	_tprintf(buf);
}
