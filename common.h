#pragma once
#include "head.h"


extern HANDLE consoleHandle;//ȫ�ֱ������κεط�������ʹ��
//��ͼ������ƫ����
extern COORD offset;

//����һЩ��������ѧ����

//���һ���з�Χ������
int RandInRange(int min, int max);

//���һ���з�Χ��С��
float RandInRange(float min, float max);


//����̨����ı����βΣ��ٿ�ȨHANDLE�����ݣ����꣬��ɫ��
void MyDrawText(HANDLE handle, LPCTSTR buf , COORD coord = { 1,2 }, WORD color = 7);

void SetStdHandle();

// This function draws content on the screen
void DrawContent(const TCHAR buf[], int x, int y, WORD color = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

void common1(const TCHAR *buf);
