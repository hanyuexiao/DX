#include "common.h"



HANDLE consoleHandle;//全局变量，任何地方都可以使用

//地图的整体偏移量
COORD offset = { 10,5 };

//放置一些公共的数学函数   50~90
int RandInRange(int min, int max)
{
	//0~32767   RAND_MAX  7fff
	int v=rand()%(max-min);

	return v+min;
}

//0~1


  //lib   静态链接库(开发环境，第三方api，文档）    dll  动态链接库（运行环境）  .so
float RandInRange(float min, float max)
{
	float v=static_cast<float>(rand())/RAND_MAX;

	return v*(max-min)+min;
}



//控制台输出文本（形参：操控权HANDLE，内容，坐标，颜色）
void MyDrawText(HANDLE handle, LPCTSTR buf , COORD coord , WORD color)
{
	DWORD ret;
	WriteConsoleOutputCharacter(handle, buf, _tcslen(buf), coord, &ret);

	COORD colorCoord = coord;
	//字符数和字节数（内存空间）
	int len = _tcslen(buf);
	//染色时，必须单个字节的控制绘制
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

	//绘制颜色
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
