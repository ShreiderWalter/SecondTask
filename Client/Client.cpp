#include "Client.h"

#include <Windows.h>

HINSTANCE Client::BCX_hInstance = nullptr;
int Client::BCX_ScaleX = 0;
int Client::BCX_ScaleY = 0;
std::string Client::BCX_ClassName = "";
HWND Client::Form = nullptr;
int Client::picked_color = -1;
int Client::failed_error = -1;

Client::Client()
{
	hPipe = OpenFileMapping(FILE_MAP_ALL_ACCESS,
							TRUE,
							MAPPING_OBJECT_NAME);
}

Client::~Client()
{
	CloseHandle(hPipe);
}

void threadProgress(HANDLE hPipe)
{
	while(true)
	{

		HANDLE eventNotify = OpenEvent(EVENT_ALL_ACCESS, false, "Global\\AcceptLock");
		LPTSTR buffer = (LPTSTR) MapViewOfFile(hPipe, FILE_MAP_ALL_ACCESS, 0, 0, BUFFER_SIZE);
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		SetEvent(eventNotify);
		CloseHandle(eventNotify);

		//std::cout << buffer << "\n";

		std::string tmp((char *) buffer);
		std::regex comp("(\\+|-)?[[:digit:]]+");
		if(std::regex_match(tmp, comp))
		{
			int color = (int)buffer[0];
			switch(color)
			{
			case 49: //1 digital
				Client::picked_color = Red; break;
			case 50: //2 digital
				Client::picked_color = Blue; break;
			case 51: //3 digital
				Client::picked_color = Orange; break;
			case 52: //4 digital
				Client::picked_color = Green; break;
			case 53: //5 digital
				Client::picked_color = Yellow; break;
			default:
				Client::failed_error = CONNECTION_CORRUPTED; break;
			}
		}
	}

}

void Client::run()
{
	if(hPipe == nullptr)
	{
		failed_error = CONNECTION_FAILED;
		return;
	}

	HANDLE eventNotify = OpenEvent(EVENT_ALL_ACCESS, false, "Global\\SendLock");
	SetEvent(eventNotify);
	CloseHandle(eventNotify);
	
	std::thread thread(threadProgress, hPipe);
	thread.detach();
}

HWND Client::BCX_Form(char * Caption, int X, int Y, int W, int H, int Style, int Exstyle)
{
	HWND A;

	if (!Style)
	{
		Style = WS_MINIMIZEBOX |
			WS_CAPTION |
			WS_POPUP |
			WS_SYSMENU |
			WS_OVERLAPPEDWINDOW | 
			WS_VISIBLE;
	}

	A = CreateWindowEx(Exstyle, BCX_ClassName.c_str(), Caption,
		Style,
		X * BCX_ScaleX,
		Y * BCX_ScaleY,
		(4 + W) * BCX_ScaleX,
		(12 + H) * BCX_ScaleY,
		NULL, (HMENU)NULL, BCX_hInstance, NULL);
	SendMessage(A, (UINT)WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT),
		(LPARAM)MAKELPARAM(FALSE, 0));
	return A;
}

int Client::BCX_Circle(HWND Wnd, int X, int Y, int R, int Pen, int Fill, HDC DrawHDC)
{
	int a, b = 0;

	if (!DrawHDC)
	{
		DrawHDC = GetDC(Wnd);
		b = 1;
	}
	HPEN hNPen = CreatePen(PS_SOLID, 1, Pen);
	HPEN hOPen = (HPEN)SelectObject(DrawHDC, hNPen);
	HBRUSH hOldBrush;
	HBRUSH hNewBrush;

	if (Fill)
	{
		hNewBrush = CreateSolidBrush(Pen);
		hOldBrush = (HBRUSH)SelectObject(DrawHDC, hNewBrush);
	}
	else
	{
		hNewBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
		hOldBrush = (HBRUSH)SelectObject(DrawHDC, hNewBrush);
	}

	a = Ellipse(DrawHDC, X - R, Y + R, X + R, Y - R);
	DeleteObject(SelectObject(DrawHDC, hOPen));
	DeleteObject(SelectObject(DrawHDC, hOldBrush));
	if (b)
	{
		ReleaseDC(Wnd, DrawHDC);
	}
	return a;
}

void Client::LoadForm()
{
	Form = BCX_Form("Lighting Lamp", 50, 50, 210, 210);
	Show(Form);
}