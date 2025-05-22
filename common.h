#pragma once
#include "head.h"


extern HANDLE consoleHandle;//全局变量，任何地方都可以使用
//地图的整体偏移量
extern COORD offset;

//放置一些公共的数学函数

//随机一个有范围的整数
int RandInRange(int min, int max);

//随机一个有范围的小数
float RandInRange(float min, float max);


//控制台输出文本（形参：操控权HANDLE，内容，坐标，颜色）
void MyDrawText(HANDLE handle, LPCTSTR buf , COORD coord = { 1,2 }, WORD color = 7);

void SetStdHandle();

// This function draws content on the screen
void DrawContent(const TCHAR buf[], int x, int y, WORD color = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

void common1(const TCHAR *buf);
