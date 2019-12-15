#include <Windows.h>
#include <windowsx.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <random>
#include <time.h>

#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define CUSTOMFVF (D3DFVF_XYZ | D3DFVF_DIFFUSE)

struct cubeNode
{
	int value;
	int posInList;
	float posX = 0.0f;
	float displayPosX = 0.0f;
	float posY = 15.0f;
	float rotY = 0.0f;
	D3DXMATRIX matCube;
	D3DXMATRIX matRotY;
	cubeNode* next = nullptr;
	cubeNode* prev = nullptr;

	D3DXMATRIX littleCube;
	D3DXMATRIX littleCubeScale;
};
struct data
{
	cubeNode* pRoot;
	cubeNode* pLast;
	int count = 0;
};
struct arrowStruct
{
	float value = 0;
	float posX = 0.0f;
	float displayPosX = 0.0f;
	D3DXMATRIX matArrow;

	int calculateValue(int cubes)
	{
		float center = float(cubes) / 2.0f;
		float offset = posX / 3.0f;
		float result = center + offset;
		return (int)result;
	}
};

LPDIRECT3D9 d3d; 
LPDIRECT3DDEVICE9 d3ddev;
LPDIRECT3DVERTEXBUFFER9 v_buffer = NULL;
LPDIRECT3DINDEXBUFFER9 i_buffer = NULL;
LPDIRECT3DVERTEXBUFFER9 a_buffer = NULL;
LPDIRECT3DINDEXBUFFER9 ai_buffer = NULL;

void initD3D(HWND hWnd);
void render_frame(void);
void cleanD3D(void);
void init_graphics(void); 
void addCube(int pos);
void removeCube(int pos);


struct CUSTOMVERTEX
{
	FLOAT x, y, z;
	DWORD color;
};


LRESULT CALLBACK WindowProc(HWND hWnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam);

data* d;
arrowStruct* pArr;
cubeNode* createNode(int pos)
{
	cubeNode* newNode = new cubeNode();
	newNode->value = rand() % 100;
	newNode->posInList = pos;
	if (d->count > 0)
		newNode->rotY = d->pRoot->rotY;
	return newNode;
}


int WINAPI WinMain(HINSTANCE hInstance, 
	HINSTANCE hPrevInstance, 
	LPSTR lpCmdLine, 
	int nCmdShow)
{
	srand(time(0));
	HWND hWnd;
	WNDCLASSEX wc;

	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW; 
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpszClassName = "VisualPointers";

	RegisterClassEx(&wc);

	hWnd = CreateWindowEx(NULL,
		"VisualPointers",
		"Visual Pointers",
		WS_OVERLAPPEDWINDOW,
		400,
		400,
		SCREEN_WIDTH,
		SCREEN_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL);

	ShowWindow(hWnd, nCmdShow);

	initD3D(hWnd);
	 
	MSG msg;

	//Code Here
	cubeNode* startNode = new cubeNode();
	startNode->value = rand() % 100;
	startNode->posInList = 0;
	startNode->posY = 0.0f;
	d = new data();
	d->pRoot = startNode;
	d->pLast = startNode;
	d->count = 1;
	pArr = new arrowStruct();

	bool added = false;
	bool removed = false;
	bool action = false;
	bool moveArrow = false;

	while (TRUE)
	{
		cubeNode* pNode = d->pRoot;
		for (int i = 0; i < d->count; i++)
		{
			if (pNode->posY > 0)
				pNode->posY -= 0.5f;

			if (pNode->displayPosX > pNode->posX)
			{
				pNode->displayPosX -= 0.1f;
				if (pNode->displayPosX - pNode->posX < 0.11f)
				{
					pNode->displayPosX = pNode->posX;
				}
			}
			else
			{
				pNode->displayPosX += 0.1f;
				if (pNode->posX - pNode->displayPosX < 0.11f)
				{
					pNode->displayPosX = pNode->posX;
				}
			}

			pNode = pNode->next;
		}
		if (pArr->displayPosX > pArr->posX)
		{
			pArr->displayPosX -= 0.5f;
			if (pArr->displayPosX - pArr->posX < 0.5f)
			{
				pArr->displayPosX = pArr->posX;
			}
		}
		else
		{
			pArr->displayPosX += 0.5f;
			if (pArr->posX - pArr->displayPosX < 0.5f)
			{
				pArr->displayPosX = pArr->posX;
			}
		}

		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if (msg.message == WM_QUIT)
			break;

		if (msg.message == WM_KEYDOWN)
		{
			switch (msg.wParam)
			{
			case VK_DOWN:
				if (!action)
				{
					if (d->count % 2 == 0)
						if(floorf(pArr->posX) == pArr->posX)
							addCube(pArr->value);
						else
						{
							removeCube(pArr->value);
						}
					else
						if (floorf(pArr->posX) == pArr->posX)
							removeCube(pArr->value);
						else
						{
							addCube(pArr->value);
						}
					action = true;
				}
				pArr->value = pArr->calculateValue(d->count);
				break;
			case VK_RIGHT:
				if (!moveArrow && pArr->posX > d->count * -1.5f)
				{
					pArr->posX -= 1.5f;
					pArr->value = pArr->calculateValue(d->count);
					moveArrow = true;
				}
				break;
			case VK_LEFT:
				if (!moveArrow && pArr->posX < d->count * 1.5f)
				{
					pArr->posX += 1.5f;
					pArr->value = pArr->calculateValue(d->count);
					moveArrow = true;
				}
				break;
			}
		}

		if (msg.message == WM_KEYUP)
		{
			switch (msg.wParam)
			{
			case VK_DOWN:
				action = false;
				break;
			case VK_RIGHT:
				moveArrow = false;
				break;
			case VK_LEFT:
				moveArrow = false;
				break;
			}
		}

		render_frame();
	}
	cleanD3D();
	return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hWnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam)
{
	switch(message)
	{
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
			break;
		}
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

void initD3D(HWND hWnd)
{
	d3d = Direct3DCreate9(D3D_SDK_VERSION);
	D3DPRESENT_PARAMETERS d3dpp;

	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = hWnd;
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	d3dpp.BackBufferWidth = SCREEN_WIDTH;
	d3dpp.BackBufferHeight = SCREEN_HEIGHT;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

	d3d->CreateDevice(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp,
		&d3ddev);

	init_graphics();

	d3ddev->SetRenderState(D3DRS_LIGHTING, FALSE); 
	d3ddev->SetRenderState(D3DRS_ZENABLE, TRUE);
	d3ddev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
}

void render_frame(void)
{
	d3ddev->Clear(0,
		NULL,
		D3DCLEAR_TARGET,
		D3DCOLOR_XRGB(0, 0, 0),
		1.0f,
		0);
	d3ddev->Clear(0, 
		NULL, 
		D3DCLEAR_ZBUFFER, 
		D3DCOLOR_XRGB(0, 5, 15), 
		1.0f, 
		0);

	d3ddev->BeginScene();
	d3ddev->SetFVF(CUSTOMFVF);

	D3DXMATRIX matView;
	D3DXMatrixLookAtLH(&matView, 
		&D3DXVECTOR3(0.0f, 8.0f, 30.0f), 
		&D3DXVECTOR3(0.0f, 0.0f, 0.0f),
		&D3DXVECTOR3(0.0f, 1.0f, 0.0f));
	d3ddev->SetTransform(D3DTS_VIEW, &matView);

	D3DXMATRIX matProjection;
	D3DXMatrixPerspectiveFovLH(&matProjection,
		D3DXToRadian(45),
		(FLOAT)SCREEN_WIDTH / (FLOAT)SCREEN_HEIGHT,
		1.0f,
		100.0f);
	d3ddev->SetTransform(D3DTS_PROJECTION, &matProjection);

	d3ddev->SetStreamSource(0, v_buffer, 0, sizeof(CUSTOMVERTEX));
	cubeNode* pNode = d->pRoot;
	while (pNode != nullptr)
	{
		pNode->rotY += 2.5f;
		D3DXMatrixTranslation(&pNode->matCube, pNode->displayPosX, pNode->posY, 0.0f);
		D3DXMatrixRotationY(&pNode->matRotY, D3DXToRadian(pNode->rotY));
		d3ddev->SetTransform(D3DTS_WORLD, &(pNode->matRotY * pNode->matCube));
		d3ddev->SetIndices(i_buffer);
		d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 8, 0, 12);

		D3DXMatrixTranslation(&pNode->littleCube, pNode->posX, -8.0f, 0.0f);
		D3DXMatrixScaling(&pNode->littleCubeScale, ((float)pNode->value / 100.0f), ((float)pNode->value / 100.0f), ((float)pNode->value / 100.0f));
		d3ddev->SetTransform(D3DTS_WORLD, &(pNode->littleCubeScale * pNode->littleCube));
		d3ddev->SetIndices(i_buffer);
		d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 8, 0, 12);
		pNode = pNode->next;
	}

	d3ddev->SetStreamSource(0, a_buffer, 0, sizeof(CUSTOMVERTEX));

	D3DXMatrixTranslation(&pArr->matArrow, pArr->displayPosX, 3.0f, 0.0f);
	d3ddev->SetTransform(D3DTS_WORLD, &pArr->matArrow);
	d3ddev->SetIndices(ai_buffer);
	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 14, 0, 20);

	d3ddev->EndScene();
	d3ddev->Present(NULL, NULL, NULL, NULL);
}

void cleanD3D(void)
{
	ai_buffer->Release();
	i_buffer->Release();
	v_buffer->Release();
	i_buffer->Release();
	d3ddev->Release();
	d3d->Release();
}

void init_graphics(void)
{
	struct CUSTOMVERTEX vertices[] =
	{
		//Cube
		{1.0f, 1.0f, 1.0f, D3DCOLOR_XRGB(255,0,0)},
		{-1.0f, 1.0f, 1.0f, D3DCOLOR_XRGB(0,255,0)},
		{1.0f, -1.0f, 1.0f, D3DCOLOR_XRGB(0,0,255)},
		{-1.0f, -1.0f, 1.0f, D3DCOLOR_XRGB(255,255,0)},
		{1.0f, 1.0f, -1.0f, D3DCOLOR_XRGB(255,0,0)},
		{-1.0f, 1.0f, -1.0f, D3DCOLOR_XRGB(0,255,0)},
		{1.0f, -1.0f, -1.0f, D3DCOLOR_XRGB(0,0,255)},
		{-1.0f, -1.0f, -1.0f, D3DCOLOR_XRGB(255,255,0)}
	};

	struct CUSTOMVERTEX arrow[] =
	{
	{0.0f, -0.5f, -0.5f, D3DCOLOR_XRGB(255,0,0)},
	{0.5f, 0.0f, -0.5f, D3DCOLOR_XRGB(0,255,0) },
	{-0.5f, 0.0f, -0.5f, D3DCOLOR_XRGB(0,0,255)},
	{0.25f, 0.0f, -0.5f, D3DCOLOR_XRGB(255,255,0)},
	{-0.25f, 0.0f, -0.5f, D3DCOLOR_XRGB(255,0,0)},
	{0.25f, 1.0f, -0.5f, D3DCOLOR_XRGB(0,255,0)},
	{-0.25f, 1.0f, -0.5f, D3DCOLOR_XRGB(0,0,255)},

	{0.0f, -0.5f, 0.5f, D3DCOLOR_XRGB(255,0,0)},
	{0.5f, 0.0f, 0.5f, D3DCOLOR_XRGB(0,255,0) },
	{-0.5f, 0.0f, 0.5f, D3DCOLOR_XRGB(0,0,255)},
	{0.25f, 0.0f, 0.5f, D3DCOLOR_XRGB(255,255,0)},
	{-0.25f, 0.0f, 0.5f, D3DCOLOR_XRGB(255,0,0)},
	{0.25f, 1.0f, 0.5f, D3DCOLOR_XRGB(0,255,0)},
	{-0.25f, 1.0f, 0.5f, D3DCOLOR_XRGB(0,0,255)}
	};

	d3ddev->CreateVertexBuffer(8 * sizeof(CUSTOMVERTEX),
		0,
		CUSTOMFVF,
		D3DPOOL_MANAGED,
		&v_buffer,
		NULL);

	VOID* pVoid;

	v_buffer->Lock(0, 0, (void**)&pVoid, 0);
	memcpy(pVoid, vertices, sizeof(vertices));
	v_buffer->Unlock(); 
	
	d3ddev->CreateVertexBuffer(14 * sizeof(CUSTOMVERTEX),
		0,
		CUSTOMFVF,
		D3DPOOL_MANAGED,
		&a_buffer,
		NULL);

	a_buffer->Lock(0, 0, (void**)&pVoid, 0);
	memcpy(pVoid, arrow, sizeof(arrow));
	a_buffer->Unlock();

	short indices[] =
	{
		0, 1, 2,
		1, 2, 3,
		0, 1, 4,
		1, 4, 5,
		0, 2, 4,
		2, 4, 6,
		1, 3, 5,
		3, 5, 7,
		2, 3, 6,
		3, 6, 7,
		4, 5, 6,
		5, 6, 7
	};

	short arrowI[] =
	{
		0, 1, 2,
		3, 4, 5,
		4, 5, 6,
		0, 1, 7,
		1, 7, 8,
		0, 2, 7,
		2, 7, 9,
		1, 3, 8,
		3, 8, 10,
		2, 4, 9,
		4, 9, 11,
		3, 5, 10,
		5, 10, 12,
		4, 6, 11,
		6, 11, 13,
		5, 6, 12,
		6, 12, 13,
		7, 8, 9,
		10, 11, 12,
		11, 12, 13
	};

	d3ddev->CreateIndexBuffer(36 * sizeof(short),
		0,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&i_buffer,
		NULL);

	i_buffer->Lock(0, 0, (void**)&pVoid, 0);
	memcpy(pVoid, indices, sizeof(indices));
	i_buffer->Unlock(); 
	
	d3ddev->CreateIndexBuffer(60 * sizeof(short),
		0,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&ai_buffer,
		NULL);

	ai_buffer->Lock(0, 0, (void**)&pVoid, 0);
	memcpy(pVoid, arrowI, sizeof(arrowI));
	ai_buffer->Unlock();
}

void addCube(int pos)
{
	if (pos == 0)
	{
		cubeNode* newNode = createNode(pos);
		newNode->next = d->pRoot;
		if (d->count > 0)
		{
			d->pRoot->prev = newNode;
		}
		d->pRoot = newNode;
		if (d->count <= 0)
			d->pLast = newNode;
		cubeNode* pNode = d->pRoot; 
		d->count += 1;
		while (pNode != nullptr)
		{
			if (pNode != newNode)
				pNode->posInList += 1;
			pNode->posX = ((d->count - 1.0f) / 2.0f) * -3.0f + 3.0f * pNode->posInList;
			pNode = pNode->next;
		}
		newNode->displayPosX = newNode->posX;
	}
	else if (pos == d->count)
	{
		cubeNode* newNode = createNode(pos);
		newNode->prev = d->pLast;
		d->pLast->next = newNode;
		d->pLast = newNode;
		if (d->count <= 0)
			d->pRoot = newNode;
		d->count += 1;
		cubeNode* pNode = d->pRoot;
		while (pNode != nullptr)
		{
			pNode->posX = ((d->count - 1.0f) / 2.0f) * -3.0f + 3.0f * pNode->posInList;
			pNode = pNode->next;
		}
		newNode->displayPosX = newNode->posX;
	}
	else
	{
		cubeNode* newNode = createNode(pos);
		cubeNode* pNode = d->pRoot;
		while (pNode != nullptr)
		{
			if (pNode->posInList == pos)
			{
				pNode->prev->next = newNode;
				newNode->prev = pNode->prev;
				pNode->prev = newNode;
				newNode->next = pNode;
				d->count += 1;
				break;
			}
			pNode = pNode->next;
		}
		pNode = d->pRoot;
		while (pNode != nullptr)
		{
			if (pNode->posInList >= pos && pNode != newNode)
				pNode->posInList += 1;
			pNode->posX = ((d->count - 1.0f) / 2.0f) * -3.0f + 3.0f * pNode->posInList;
			pNode = pNode->next;
		}
		newNode->displayPosX = newNode->posX;
	}
}

void removeCube(int pos)
{
	if (d->count <= 0 || pos > d->count - 1)
		return;
	if (pos == 0)
	{
		d->pRoot = d->pRoot->next;
		if (d->count > 1)
		{
			delete d->pRoot->prev;
			d->pRoot->prev = nullptr;
		}
		d->count -= 1;
		cubeNode* pNode = d->pRoot;
		while (pNode != nullptr)
		{
			pNode->posInList -= 1;
			pNode->posX = ((d->count - 1.0f) / 2.0f) * -3.0f + 3.0f * pNode->posInList;
			pNode = pNode->next;
		}
	}
	else if (pos == d->count - 1)
	{
		if (d->count > 1)
		d->pLast = d->pLast->prev;
		delete  d->pLast->next;
		d->pLast->next = nullptr;
		d->count -= 1;
		cubeNode* pNode = d->pRoot;
		while (pNode != nullptr)
		{
			pNode->posX = ((d->count - 1.0f) / 2.0f) * -3.0f + 3.0f * pNode->posInList;
			pNode = pNode->next;
		}
	}
	else
	{
		cubeNode* pNode = d->pRoot;
		while (pNode != nullptr)
		{
			if (pNode->posInList == pos)
			{
				pNode->prev->next = pNode->next;
				pNode->next->prev = pNode->prev;
				delete pNode;
				d->count -= 1;
				break;
			}
			pNode = pNode->next;
		}
		pNode = d->pRoot;
		while (pNode != nullptr)
		{
			if (pNode->posInList >= pos)
				pNode->posInList -= 1;
			pNode->posX = ((d->count - 1.0f) / 2.0f) * -3.0f + 3.0f * pNode->posInList;
			pNode = pNode->next;
		}
	}
}