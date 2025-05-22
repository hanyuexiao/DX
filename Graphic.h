#pragma once
#include "common.h"
#include "Singleton.h"
#include "d3d9.h"
#include "d3dx9core.h"
#include "head.h"

struct MyImageInfo
{
	LPDIRECT3DTEXTURE9 pTex;//具体的图片
	WORD width;//图片的宽度
	WORD height;//图片的高度
};

//函数指针-----专门用来处理消息的函数（窗口句柄，消息类型，消息附加值1，消息的附加值2）
typedef LRESULT(*pMainWndProc)(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

class CGraphic :
    public CSingleton<CGraphic>
{
    friend class CSingleton<CGraphic>;

public:
	HWND InitWindow(pMainWndProc proc, WORD width=800, WORD height=600, string titelName=_T("Game"), string className=_T("MyClass"));
	void InitDX(HWND hwnd);
	HRESULT LoadTex(LPCTSTR fileName, MyImageInfo& info,DWORD color=0xff000000);
	void BeginDraw();
	void EndDraw();
	void CloseDX();
	void DrawTex(LPDIRECT3DTEXTURE9 tex, D3DXVECTOR2 srcPos, D3DXVECTOR2 size, D3DXVECTOR3 pos, D3DXVECTOR3 center = D3DXVECTOR3(0.0f, 0, 0), D3DXVECTOR3 rot = D3DXVECTOR3(0.0f, 0, 0), D3DXVECTOR3 scale = D3DXVECTOR3(1.0f, 1, 1));
	void DrawTex(MyImageInfo& info, D3DXVECTOR3 pos, D3DXVECTOR3 center = D3DXVECTOR3(0.0f, 0, 0), D3DXVECTOR3 rot = D3DXVECTOR3(0.0f, 0, 0), D3DXVECTOR3 scale = D3DXVECTOR3(1.0f, 1, 1));
    void DrawTex(MyImageInfo& info,D3DXVECTOR3 center,D3DXMATRIX matWorld);//默认左上对齐的红色文字
	void DrawText(string text, RECT rc, DWORD style=DT_LEFT|DT_TOP, DWORD color=0xffff0000);//默认左上对齐的红色文字

	LPDIRECT3DDEVICE9 m_pDevice;//DX设备接口---代表显卡设备

private:
	LPDIRECT3DTEXTURE9 m_pTex;//图片接口
	IDirect3D9* m_pD3D;
	//LPDIRECT3D9 m_pD3D;//DX入口接口

	LPD3DXLINE m_pLine;//画线的接口
	LPD3DXSPRITE m_pSprite;//画2D图形的接口
	LPD3DXFONT m_pFont;//绘制文字的接口

	HWND m_hMainWnd;//窗口句柄
};

