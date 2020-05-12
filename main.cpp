/*
// Definations for use when we want the specifiled color value
#define GetRValue(rgb)   ((BYTE) (rgb)) 
#define GetBValue(rgb)   ((BYTE) ((rgb) >> 16)) 
#define GetGValue(rgb)   ((BYTE) (((WORD) (rgb)) >> 8)) 
*/

#include <windows.h>
#include "resource.h"
#define width 310
#define height 219
#define TIMER 1

LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM) ;

int ConvertPos(int x,int y);
COLORREF sRGB(int r, int g, int b);
bool FadeTest(HWND hwnd);
void PaintBmp(HWND hwnd);
float value=0.5;
bool bUp;

HBITMAP bitmap,bitmap2;

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    PSTR szCmdLine, int iCmdShow)
{
     static TCHAR szAppName[] = TEXT ("HelloWin") ;
     HWND         hwnd ;
     MSG          msg ;
     WNDCLASS     wndclass ;

     wndclass.style         = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
     wndclass.lpfnWndProc   = WndProc ;
     wndclass.cbClsExtra    = 0 ;
     wndclass.cbWndExtra    = 0 ;
     wndclass.hInstance     = hInstance ;
     wndclass.hIcon         = LoadIcon (NULL, IDI_APPLICATION) ;
     wndclass.hCursor       = LoadCursor (NULL, MAKEINTRESOURCE(IDC_ARROW)) ;
     wndclass.hbrBackground = (HBRUSH) GetStockObject (BLACK_BRUSH) ;
     wndclass.lpszMenuName  = NULL ;
     wndclass.lpszClassName = szAppName ;


     if (!RegisterClass (&wndclass))
     {
          MessageBox (NULL, TEXT ("This program requires Windows NT!"), 
                      szAppName, MB_ICONERROR) ;
          return 0 ;
     }
     
     hwnd = CreateWindow (szAppName, TEXT ("Fader"),WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,
                          CW_USEDEFAULT, width+8, height+26, NULL, NULL, hInstance, NULL);       
     
     ShowWindow (hwnd, iCmdShow) ;
     UpdateWindow (hwnd) ;
     
     while (GetMessage (&msg, NULL, 0, 0))
     {
          TranslateMessage (&msg) ;
          DispatchMessage (&msg) ;
     }
     return msg.wParam ;
}

LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
     
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{

	case WM_CREATE:

		bitmap = (HBITMAP)LoadImage(LPCREATESTRUCT(lParam)->hInstance, MAKEINTRESOURCE(IDB_BITMAP1), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
		 

		bitmap2 = (HBITMAP)LoadImage(LPCREATESTRUCT(lParam)->hInstance, MAKEINTRESOURCE(IDB_BITMAP2), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);

		SetTimer(hwnd,TIMER,57,0);
		return 0;

	case WM_TIMER:

		if(bUp)
		{			
			value +=0.03;

			FadeTest(hwnd);	
		}
		else
		{			
			value -=0.03;		
						
			FadeTest(hwnd);
		}

		if(value >= 0.97)
		{
			Sleep(150);
			bUp = false;
		}

		if(value <= 0.03)
		{
			Sleep(1000);
			bUp = true;
		}

		return 0;

	case WM_PAINT:

		hdc = BeginPaint(hwnd,&ps);
		
		//PaintBmp(hwnd);
		//FadeTest(hwnd);
	
		EndPaint(hwnd,&ps);

		return 0;

	case WM_DESTROY:

	  PostQuitMessage (0) ;
	  return 0 ;
	}

     return DefWindowProc (hwnd, message, wParam, lParam) ;
}


bool FadeTest(HWND hwnd)
{	
	HDC hdc = GetDC(hwnd);
	HDC hdcMem = CreateCompatibleDC(hdc);
	HDC hdcMem2 = CreateCompatibleDC(hdc);
	HDC hdcMem3 = CreateCompatibleDC(hdc);
	HDC hdcMem4 = CreateCompatibleDC(hdc);

	COLORREF newcolor;

	HBITMAP dib,backdib;
	
	int r,g,b;
	int ar,ag,ab;
	
	bool br,bg,bb;

	DWORD * pBackBits;		
	DWORD * pImgBits;	

	BITMAPINFO bmi;
	ZeroMemory(&bmi.bmiHeader, sizeof(BITMAPINFOHEADER));
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = width;
	bmi.bmiHeader.biHeight = height;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;

	dib = CreateDIBSection(hdc,  &bmi, DIB_RGB_COLORS,(void**)&pImgBits, 0,0);
	if (!dib)
		MessageBox(hwnd,"Dib Error","Error",0);

	backdib = CreateDIBSection(hdc,  &bmi, DIB_RGB_COLORS,(void**)&pBackBits, 0,0);
	if (!dib)
		MessageBox(hwnd,"Dib Error","Error",0);

	SelectObject(hdcMem,backdib);
	SelectObject(hdcMem4,bitmap);
	BitBlt(hdcMem, 0,0, width, height, hdcMem4, 0, 0, SRCCOPY);

	SelectObject(hdcMem3,dib);
	SelectObject(hdcMem2,bitmap2);
	BitBlt(hdcMem3, 0,0, width, height, hdcMem2, 0, 0, SRCCOPY);

	for(int x=0;x<=width;x++)
	for(int y=0;y<=height;y++)
	{
		r = (BYTE)(pBackBits[ConvertPos(x,y)]);
		g = (BYTE)(pBackBits[ConvertPos(x,y)] >> 160);
		b = (BYTE)(pBackBits[ConvertPos(x,y)] >> 320);

		ar = (BYTE)(pImgBits[ConvertPos(x,y)]);
		ag = (BYTE)(pImgBits[ConvertPos(x,y)] >> 160);
		ab = (BYTE)(pImgBits[ConvertPos(x,y)] >> 320);
		
		pBackBits[ConvertPos(x,y)] = ((int) (r*value + ar*(1.0-value) )) | ((int) (g*value + ag*(1.0-value)) << 8) | (int) (b*value + ab*(1.0-value)) << 16;
		
	}

	SelectObject(hdcMem,backdib);
	BitBlt(hdc, 0,0, width, height, hdcMem, 0, 0, SRCCOPY);

	DeleteDC(hdc);
	DeleteDC(hdcMem);
	DeleteObject(dib);
	DeleteObject(backdib);
	DeleteObject(hdcMem2);
	DeleteObject(hdcMem3);
	DeleteObject(hdcMem4);

	return true;
}

int ConvertPos(int x,int y)
{
	int apa = (height-y)*width+x;
	return apa;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////
///// FUNCTION:    sRGB(int r, int g, int b)
///// DESCRIPTION: Converts the traditional r,g,b to the dib-adapted r,g,b format (reversed b,g,r)
///// INPUT:       The color-values
///// OUTPUT:      Nothing...
///// RETURN:      The color in COLORREF format (0x00bbggrr)
/////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
COLORREF sRGB(int r, int g, int b)
{
	return RGB(b,g,r);
}

void PaintBmp(HWND hwnd)
{
	HDC hdc = GetDC(hwnd);
	HDC mem = CreateCompatibleDC(hdc);

	SelectObject(mem,bitmap);

	BitBlt(hdc,0,0,330,339,mem,0,0,SRCCOPY);

	DeleteDC(mem);
	DeleteDC(hdc);
}