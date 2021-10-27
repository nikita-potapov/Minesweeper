// Minesweeper.cpp : Defines the entry point for the application.
//
#include <stdlib.h>
#include "framework.h"
#include "Minesweeper.h"

#define MAX_LOADSTRING 100

#define GAME_GRID_X 50
#define GAME_GRID_Y 50

#define GAME_CELL_MINE 0
#define GAME_CELL_FREE 1

#define VIEW_CELL_UNEXPLORED 0
#define VIEW_CELL_OPENED 1
#define VIEW_CELL_MINE 2
#define VIEW_CELL_MINE_HIT 3
#define VIEW_CELL_FLAG 4

#define GRID_ROWS_COUNT 16
#define GRID_COLUMNS_COUNT 16

#define MENU_HEIGHT 30

#define COLOR_UNEXPLORED RGB(0, 128, 128)
#define COLOR_OPENED RGB(238, 238, 236)
#define COLOR_MINE_HIT RGB(247, 64, 64)


#define MIN_CELL_SIZE 20

int gameField[GRID_ROWS_COUNT][GRID_COLUMNS_COUNT] = {0};
int viewField[GRID_ROWS_COUNT][GRID_COLUMNS_COUNT] = {0};

int lightI, lightJ;

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_MINESWEEPER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MINESWEEPER));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}


void drawLine(HDC hdc, int x1, int y1, int x2, int y2)
{
    MoveToEx(hdc, x1, y1, NULL);
    LineTo(hdc, x2, y2);
}

void drawMinesweeperFrame(HDC hdc, int x, int y)
{
    drawMinesweeperGrid(hdc, x, y);
}

void drawMinesweeperGrid(HDC hdc, int x, int y)
{
    
    for (int i = 0; i < GRID_ROWS_COUNT; i++)
    {
        for (int j = 0; j < GRID_COLUMNS_COUNT; j++)
        {
            drawMinesweeperCell(hdc, x, y, i, j);
        }
    }

}

void drawMinesweeperCell(HDC hdc,int x, int y, int i, int j)
{
    viewField[3][4] = VIEW_CELL_OPENED;
    viewField[7][4] = VIEW_CELL_MINE;
    viewField[9][4] = VIEW_CELL_MINE_HIT;
    viewField[11][4] = VIEW_CELL_FLAG;

    int cellState = viewField[i][j];
    if (cellState == VIEW_CELL_UNEXPLORED) drawUnexplored(hdc, x, y, i, j);
    if (cellState == VIEW_CELL_OPENED) drawOpened(hdc, x, y, i, j);
    if (cellState == VIEW_CELL_MINE) drawMine(hdc, x, y, i, j);
    if (cellState == VIEW_CELL_MINE_HIT) drawMineHit(hdc, x, y, i, j);
    if (cellState == VIEW_CELL_FLAG) drawFlag(hdc, x, y, i, j);

    if (lightI == i && lightJ == j) lightCell(hdc, i, j);
}


void drawUnexplored(HDC hdc, int x, int y, int i, int j)
{   HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    SelectObject(hdc, hPen);
    HBRUSH hBrush = CreateSolidBrush(COLOR_UNEXPLORED);
    SelectObject(hdc, hBrush);
    Rectangle(hdc, x + j * MIN_CELL_SIZE, y + i * MIN_CELL_SIZE,
        x + (j + 1) * MIN_CELL_SIZE - 1, y + (i + 1) * MIN_CELL_SIZE - 1);
    DeleteObject(hBrush);
    DeleteObject(hPen);
}
void drawOpened(HDC hdc, int x, int y, int i, int j)
{
    SelectObject(hdc, GetStockObject(NULL_PEN));
    HBRUSH hBrush = CreateSolidBrush(COLOR_OPENED);
    SelectObject(hdc, hBrush);
    Rectangle(hdc, x + j * MIN_CELL_SIZE, y + i * MIN_CELL_SIZE,
        x + (j + 1) * MIN_CELL_SIZE - 1, y + (i + 1) * MIN_CELL_SIZE - 1);
    DeleteObject(hBrush);
}
void drawMine(HDC hdc, int x, int y, int i, int j)
{
    drawOpened(hdc, x, y, i, j);

    int sx, sy;
    sx = j * MIN_CELL_SIZE + x;
    sy = i * MIN_CELL_SIZE + y;

    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    SelectObject(hdc, hPen);

    HBRUSH hBrush = CreateSolidBrush(RGB(96, 96, 96));
    SelectObject(hdc, hBrush);

    drawLine(hdc, sx + 1, sy + 1, sx + 18, sy + 18);
    drawLine(hdc, sx + 1, sy + 18, sx + 18, sy + 1);
    drawLine(hdc, sx + 9, sy + 1, sx + 10, sy + 18);
    drawLine(hdc, sx + 1, sy + 10, sx + 18, sy + 9);

    Ellipse(hdc, sx + 4, sy + 4, sx + 15, sy + 15);

    DeleteObject(hPen);
    DeleteObject(hBrush);
}
void drawMineHit(HDC hdc, int x, int y, int i, int j)
{
    SelectObject(hdc, GetStockObject(NULL_PEN));
    HBRUSH hBrush = CreateSolidBrush(COLOR_MINE_HIT);
    SelectObject(hdc, hBrush);
    Rectangle(hdc, x + j * MIN_CELL_SIZE, y + i * MIN_CELL_SIZE,
        x + (j + 1) * MIN_CELL_SIZE, y + (i + 1) * MIN_CELL_SIZE);
    DeleteObject(hBrush);
    
    int sx, sy;
    sx = j * MIN_CELL_SIZE + x;
    sy = i * MIN_CELL_SIZE + y;

    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    SelectObject(hdc, hPen);

    hBrush = CreateSolidBrush(RGB(96, 96, 96));
    SelectObject(hdc, hBrush);

    drawLine(hdc, sx + 1, sy + 1, sx + 18, sy + 18);
    drawLine(hdc, sx + 1, sy + 18, sx + 18, sy + 1);
    drawLine(hdc, sx + 9, sy + 1, sx + 10, sy + 18);
    drawLine(hdc, sx + 1, sy + 10, sx + 18, sy + 9);

    Ellipse(hdc, sx + 4, sy + 4, sx + 15, sy + 15);

    DeleteObject(hPen);
    DeleteObject(hBrush);
}
void drawFlag(HDC hdc, int x, int y, int i, int j)
{
    int sx, sy;
    sx = j * MIN_CELL_SIZE + x;
    sy = i * MIN_CELL_SIZE + y;

    drawUnexplored(hdc, x, y, i, j);
    SelectObject(hdc, GetStockObject(NULL_PEN));
    HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255));
    SelectObject(hdc, hBrush);

    Rectangle(hdc, sx + 2, sy + 1, sx + 5, sy + 18);
    Rectangle(hdc, sx + 2, sy + 2, sx + 8, sy + 10);
    Rectangle(hdc, sx + 2, sy + 3, sx + 12, sy + 9);
    Rectangle(hdc, sx + 2, sy + 4, sx + 15, sy + 8);
    Rectangle(hdc, sx + 2, sy + 5, sx + 16, sy + 7);

    hBrush = CreateSolidBrush(RGB(255, 0, 0));
    SelectObject(hdc, hBrush);

    drawLine(hdc, sx + 3, sy + 2, sx + 3, sy + 17);
    drawLine(hdc, sx + 4, sy + 2, sx + 4, sy + 17);

    Rectangle(hdc, sx + 3, sy + 3, sx + 7, sy + 9);
    Rectangle(hdc, sx + 3, sy + 4, sx + 11, sy + 8);
    Rectangle(hdc, sx + 3, sy + 5, sx + 14, sy + 7);
    Rectangle(hdc, sx + 3, sy + 6, sx + 15, sy + 6);

    DeleteObject(hBrush);
}


void lightCell(HDC hdc, int i, int j)
{
    HPEN hPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
    
}


//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MINESWEEPER));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_MINESWEEPER);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_MOUSEMOVE:
    {
        int xPos, yPos;
        xPos = LOWORD(lParam);
        yPos = HIWORD(lParam);

        if (xPos > GAME_GRID_X && xPos < GAME_GRID_X + GRID_COLUMNS_COUNT * MIN_CELL_SIZE)
        {
            lightJ = (xPos - GAME_GRID_X) / MIN_CELL_SIZE;
            lightI = (yPos - GAME_GRID_Y) / MIN_CELL_SIZE;
        }

    }
    break;
    case WM_CREATE:
    {
        
    }
        break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...

            drawMinesweeperFrame(hdc, GAME_GRID_X, GAME_GRID_Y);
            

            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
