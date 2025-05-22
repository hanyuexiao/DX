#include "Graphic.h"

#pragma comment(lib,"../DX_function/DX/Lib/d3d9.lib")
#pragma comment(lib,"../DX_function/DX/Lib/d3dx9.lib")

HWND CGraphic::InitWindow(pMainWndProc proc,WORD width,WORD height,TString titelName,TString className)
{
	//1、定义窗口类
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = proc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = 0;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = className.c_str();//绑定一个类名

	//2、注册窗口类
	if (!RegisterClass(&wc))
	{
		MessageBox(0, _T("RegisterClass Failed."), 0, 0);
		return nullptr;
	}

	// Compute window rectangle dimensions based on requested client area dimensions.
	RECT R = { 0, 0, width, height };
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	int newwidth = R.right - R.left;
	int newheight = R.bottom - R.top;

	//3、创建窗口
	m_hMainWnd = CreateWindow(className.c_str(), titelName.c_str(), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, newwidth, newheight, 0, 0, 0, 0);
	if (!m_hMainWnd)
	{
		MessageBox(0, _T("CreateWindow Failed."), 0, 0);
		return nullptr;
	}

	//4、显示窗口
	ShowWindow(m_hMainWnd, SW_SHOW);

	//5、更新窗口
	UpdateWindow(m_hMainWnd);

	return m_hMainWnd;
}

void CGraphic::InitDX(HWND hwnd)
{
	m_hMainWnd = hwnd;

	m_pD3D = Direct3DCreate9(D3D_SDK_VERSION);


	//默认显卡
	//使用显卡的加速功能
	//窗口句柄
	//使用硬件加速处理顶点
	D3DPRESENT_PARAMETERS d3dpp;//定义后台缓存的相关设置
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = true;//可以支持全屏
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;//32位颜色
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;//前后台的交换方式为丢弃
	//设置深度的格式和激活深度计算
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24X8;//设置深度缓存的精度（8，16，24，32）
	d3dpp.EnableAutoDepthStencil = true;
	m_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_hMainWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &m_pDevice);

	if (!m_pDevice)
		return;
	////比较深度，进行测试（相同的坐标，深度值小的通过测试，则绘制出来）
	//m_pDevice->SetRenderState(D3DRS_ZENABLE, true);//激活深度测试
	//m_pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESS);//小于通过测试

	D3DXCreateSprite(m_pDevice, &m_pSprite);

	D3DXCreateLine(m_pDevice, &m_pLine);

	D3DXCreateFont(m_pDevice, 50,50, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, _T("宋体"), &m_pFont);

}

HRESULT CGraphic::LoadTex(LPCTSTR fileName, MyImageInfo& info,DWORD color)
{
	if (FAILED(D3DXCreateTextureFromFileEx(m_pDevice, fileName, D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2,
		0, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_DEFAULT, 0, color, 0, 0, &info.pTex)))
	{
		info.pTex = nullptr;
		return E_FAIL;
	}
	D3DSURFACE_DESC desc;
	info.pTex->GetLevelDesc(0, &desc);
	info.width = desc.Width;
	info.height = desc.Height;

	return S_OK;
}

void CGraphic::BeginDraw()
{
	if (m_pDevice)
	{
		//清空（颜色缓存target，深度缓存）
		m_pDevice->Clear(0, 0, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(128, 128, 128), 1, 0);

		m_pDevice->BeginScene();
	}
}

void CGraphic::EndDraw()
{
	m_pDevice->EndScene();

	//展现到屏幕上
	m_pDevice->Present(0, 0, 0, 0);
}

void CGraphic::CloseDX()
{

}
void CGraphic::DrawTex(LPDIRECT3DTEXTURE9 tex, D3DXVECTOR2 srcPos, D3DXVECTOR2 size, D3DXVECTOR3 pos, D3DXVECTOR3 center /*= D3DXVECTOR3(0.0f, 0, 0)*/, D3DXVECTOR3 rot /*= D3DXVECTOR3(0.0f, 0, 0)*/, D3DXVECTOR3 scale /*= D3DXVECTOR3(0.0f, 0, 0)*/)
{
	m_pSprite->Begin(D3DXSPRITE_ALPHABLEND);

	RECT rc = { static_cast<LONG>(srcPos.x),static_cast<LONG>(srcPos.y),static_cast<LONG>(srcPos.x+size.x),static_cast<LONG>(srcPos.y+size.y)};
	D3DXMATRIX matWorld, matT, matR, matS;
	D3DXMatrixTranslation(&matT, pos.x, pos.y, pos.z);
	D3DXMatrixRotationZ(&matR, rot.z);
	D3DXMatrixScaling(&matS, scale.x, scale.y, scale.z);

	matWorld = matS * matR * matT;
	m_pSprite->SetTransform(&matWorld);
	m_pSprite->Draw(tex, &rc, &center, nullptr, 0xffffffff);

	m_pSprite->End();
}
void CGraphic::DrawTex(MyImageInfo& info, D3DXVECTOR3 pos, D3DXVECTOR3 center /*= D3DXVECTOR3(0.0f, 0, 0)*/, D3DXVECTOR3 rot /*= D3DXVECTOR3(0.0f, 0, 0)*/, D3DXVECTOR3 scale /*= D3DXVECTOR3(0.0f, 0, 0)*/)
{
	m_pSprite->Begin(D3DXSPRITE_ALPHABLEND);

	RECT rc = {0,0,info.width,info.height};
	D3DXMATRIX matWorld, matT, matR, matS;
	D3DXMatrixTranslation(& matT, pos.x, pos.y, pos.z);
	D3DXMatrixRotationZ(&matR, rot.z);
	D3DXMatrixScaling(&matS, scale.x, scale.y, scale.z);

	matWorld = matS * matR * matT;
	m_pSprite->SetTransform(&matWorld);

	if(info.pTex)
		m_pSprite->Draw(info.pTex, &rc, &center, nullptr, 0xffffffff);

	m_pSprite->End();
}


void CGraphic::DrawText(TString text, RECT rc,DWORD style,DWORD color)
{
	m_pFont->DrawText(nullptr, text.c_str(), _tcslen(text.c_str()), &rc, style,color);
}

void CGraphic::DrawTex(MyImageInfo &info, D3DXVECTOR3 center,D3DXMATRIX matWorld) {
    m_pSprite->Begin(D3DXSPRITE_ALPHABLEND);
    RECT rc = { 0,0,info.width,info.height };
    m_pSprite->SetTransform(&matWorld);
    if (info.pTex)
        m_pSprite->Draw(info.pTex, &rc, &center, nullptr, 0xffffffff);

    m_pSprite->End();
}


