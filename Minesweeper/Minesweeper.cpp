// Minesweeper.cpp : Defines the entry point for the application.
//
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "framework.h"
#include "Minesweeper.h"


#define MAX_LOADSTRING 100

#define RANDOM_MINES 0

#define MINES_COUNT 24

#define PROGRAM_SECOND 1000

#define GAME_GRID_X 50
#define GAME_GRID_Y 50

#define GAME_CELL_FREE 0
#define GAME_CELL_MINE 1

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

int lightI = -1;
int lightJ = -1;

int mouseX, mouseY;

int isActual = 0;

int isFirstClick = 1;

int flagsCount = MINES_COUNT;

unsigned int timer = 0;

int isKillTimer = 0;

int inGame = 1;

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

int getMinesAroundCount(int i, int j)
{
    int variants[] = {-1, 0, 1};
    int mines = 0;
    for (int m = 0; m < 3; m++)
    {
        for (int n = 0; n < 3; n++)
        {
            if (!(m == 1 && n == 1) &&
                i + variants[m] >= 0 && i + variants[m] < GRID_ROWS_COUNT &&
                j + variants[n] >= 0 && j + variants[n] < GRID_COLUMNS_COUNT)
            {
                mines += gameField[i + variants[m]][j + variants[n]];
            }
        }
    }
    return mines;
}

void drawMinesweeperFrame(HWND hWnd, HDC hdc, int x, int y)
{ 
    drawMinesweeperGrid(hdc, x, y);
}

void recursiveOpenCell(int i, int j)
{
    if (viewField[i][j] == VIEW_CELL_OPENED) return;
    if (gameField[i][j] == GAME_CELL_MINE) return;

    viewField[i][j] = VIEW_CELL_OPENED;
    if (getMinesAroundCount(i, j) == 0)
    {
        int dx, dy;
        int v[3] = { -1, 0, 1 };
        for (int m = 0; m < 3; m++)
        {
            for (int n = 0; n < 3; n++)
            {
                dx = v[m];
                dy = v[n];

                if ((dx != 0) && (dy != 0)) continue;
                if (isCordsGood(i + dy, j + dx))
                    recursiveOpenCell(i + dy, j + dx);
            }
        }
    }   
}


int isCordsGood(int i, int j)
{
    return (((i >= 0) && (i < GRID_ROWS_COUNT)) && ((j >= 0) && (j < GRID_COLUMNS_COUNT)));
}

void drawMinesweeperStatistics(HDC hdc, int x, int y)
{
    TCHAR str1[] = _T("Мин осталось: ");
    TextOut(hdc, x, y, str1, _tcsclen(str1));

    TCHAR str2[] = _T("Времени прошло: ");
    TextOut(hdc, x, y + 50, str2, _tcsclen(str2));

    char text[5];
    TCHAR textOut[5];

    sprintf_s(text, "%d", flagsCount);
    OemToChar(text, textOut);
    TextOut(hdc, x + 150, y, textOut, _tcsclen(textOut));

    char timerText[10];
    TCHAR timerTextOut[10];

    int temp = timer;

    int seconds = temp % 60;
    temp /= 60;
    int minutes = temp % 60;
    temp /= 60;
    int hours = temp;
    if (hours > 0)
    {
        sprintf_s(timerText, "%d:%d:%d", hours, minutes, seconds);
    }
    else
    {
        sprintf_s(timerText, "%d:%d", minutes, seconds);
    }
        
    OemToChar(timerText, timerTextOut);
    TextOut(hdc, x + 170, y + 50, timerTextOut, _tcsclen(timerTextOut));
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
    int cellState = viewField[i][j];
    if (cellState == VIEW_CELL_UNEXPLORED) drawUnexplored(hdc, x, y, i, j);
    if (cellState == VIEW_CELL_OPENED) drawOpened(hdc, x, y, i, j);
    if (cellState == VIEW_CELL_MINE) drawMine(hdc, x, y, i, j);
    if (cellState == VIEW_CELL_MINE_HIT) drawMineHit(hdc, x, y, i, j);
    if (cellState == VIEW_CELL_FLAG) drawFlag(hdc, x, y, i, j);

    if (lightI == i && lightJ == j &&
    (viewField[i][j] == VIEW_CELL_UNEXPLORED ||
        viewField[i][j] == VIEW_CELL_FLAG)) lightCell(hdc, i, j);
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

    int minesAroundCount = getMinesAroundCount(i, j);

    if (minesAroundCount == 0) return;

    char minesAroundText[2];

    sprintf_s(minesAroundText, "%d", minesAroundCount);
    TCHAR minesAround[2];
    OemToChar(minesAroundText, minesAround);
    TextOut(hdc, x + j * MIN_CELL_SIZE + 5, y + i * MIN_CELL_SIZE,
        minesAround, _tcsclen(minesAround));

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
    SelectObject(hdc, hPen);

    SelectObject(hdc, GetStockObject(NULL_BRUSH));

    Rectangle(hdc, GAME_GRID_X + j * MIN_CELL_SIZE,
                GAME_GRID_Y + i * MIN_CELL_SIZE,
                GAME_GRID_X + (j + 1) * MIN_CELL_SIZE,
                GAME_GRID_Y + (i + 1) * MIN_CELL_SIZE);
    

    DeleteObject(hPen);

}

void fillMines(int i, int j)
{
    int mines = MINES_COUNT;
    while (mines > 0)
    {
        int ri = rand() % GRID_ROWS_COUNT;
        int rj = rand() % GRID_COLUMNS_COUNT;

        if (ri == i && rj == j) continue;
        if (gameField[ri][rj] == GAME_CELL_MINE) continue;

        gameField[ri][rj] = GAME_CELL_MINE;
        mines--;
    }
}

void openCell(int i, int j)
{
    if (!inGame) return;

    if (viewField[i][j] == VIEW_CELL_FLAG) return;
    if (viewField[i][j] != VIEW_CELL_UNEXPLORED) return;

    if (gameField[i][j] == GAME_CELL_FREE)
        recursiveOpenCell(i, j);
    else
    {
        viewField[i][j] = VIEW_CELL_MINE_HIT;
        isKillTimer = 1;
        showAllMines();
        inGame = 0;
    }
}

void markedCell(int i, int j)
{
    if (viewField[i][j] == VIEW_CELL_UNEXPLORED && flagsCount > 0)
    {
        viewField[i][j] = VIEW_CELL_FLAG;
        flagsCount--;
    }
    else if (viewField[i][j] == VIEW_CELL_FLAG)
    {
        viewField[i][j] = VIEW_CELL_UNEXPLORED;
        flagsCount++;
    }

}

void showAllMines()
{
    for (int i = 0; i < GRID_ROWS_COUNT; i++)
    {   
        for (int j = 0; j < GRID_COLUMNS_COUNT; j++)
        {
            if (gameField[i][j] == GAME_CELL_MINE && viewField[i][j] != VIEW_CELL_MINE_HIT)
                viewField[i][j] = VIEW_CELL_MINE;
        }
    }
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
    case WM_TIMER:
    {
        if (isKillTimer) KillTimer(hWnd, 1);
        timer++;
        InvalidateRect(hWnd, NULL, 1);

        //RECT rect = { 540, 124, 600, 150 };
        //InvalidateRect(hWnd, &rect, 1);
    }
    break;
    case WM_MOUSEMOVE:
    {
        int xPos, yPos;
        xPos = LOWORD(lParam);
        yPos = HIWORD(lParam);

        mouseX = xPos;
        mouseY = yPos;

        if (xPos > GAME_GRID_X && xPos < GAME_GRID_X + GRID_COLUMNS_COUNT * MIN_CELL_SIZE &&
            yPos > GAME_GRID_Y && yPos < GAME_GRID_Y + GRID_ROWS_COUNT * MIN_CELL_SIZE)
        {
            
    
            int newLightI, newLightJ;
            newLightJ = (xPos - GAME_GRID_X) / MIN_CELL_SIZE;
            newLightI = (yPos - GAME_GRID_Y) / MIN_CELL_SIZE;

            if (newLightI < 0 || newLightI > GRID_ROWS_COUNT - 1) newLightI = -1;
            if (newLightJ < 0 || newLightJ > GRID_COLUMNS_COUNT - 1) newLightJ = -1;

            if (newLightI != lightI || newLightJ != lightJ)
            {
                lightI = newLightI;
                lightJ = newLightJ;
                InvalidateRect(hWnd, NULL, 1);

                isActual = 0;
            }

            
        }
        else
        {
            if (!isActual)
            {
                lightI = -1;
                lightJ = -1;
                InvalidateRect(hWnd, NULL, 1);

                isActual = !isActual;
            }
        }

    }
    break;
    case WM_LBUTTONDOWN:
    {
        int xPos, yPos;
        xPos = LOWORD(lParam);
        yPos = HIWORD(lParam);

        if (xPos > GAME_GRID_X && xPos < GAME_GRID_X + GRID_COLUMNS_COUNT * MIN_CELL_SIZE &&
            yPos > GAME_GRID_Y && yPos < GAME_GRID_Y + GRID_ROWS_COUNT * MIN_CELL_SIZE)
        {
            int clickedCellI, clickedCellJ;
            clickedCellJ = (xPos - GAME_GRID_X) / MIN_CELL_SIZE;
            clickedCellI = (yPos - GAME_GRID_Y) / MIN_CELL_SIZE;

            if (clickedCellI >= 0 && clickedCellI < GRID_ROWS_COUNT &&
                clickedCellJ >= 0 && clickedCellJ < GRID_COLUMNS_COUNT)
            {
                if (isFirstClick)
                {
                    isFirstClick = !isFirstClick;
                    fillMines(clickedCellI, clickedCellJ);

                    SetTimer(hWnd, 1, PROGRAM_SECOND, 0);
                }
                openCell(clickedCellI, clickedCellJ);
                InvalidateRect(hWnd, NULL, 1);
            }
        }
    }
    break;
    case WM_RBUTTONDOWN:
    {
        int xPos, yPos;
        xPos = LOWORD(lParam);
        yPos = HIWORD(lParam);

        if (xPos > GAME_GRID_X && xPos < GAME_GRID_X + GRID_COLUMNS_COUNT * MIN_CELL_SIZE &&
            yPos > GAME_GRID_Y && yPos < GAME_GRID_Y + GRID_ROWS_COUNT * MIN_CELL_SIZE)
        {
            int clickedCellI, clickedCellJ;
            clickedCellJ = (xPos - GAME_GRID_X) / MIN_CELL_SIZE;
            clickedCellI = (yPos - GAME_GRID_Y) / MIN_CELL_SIZE;

            if (clickedCellI >= 0 && clickedCellI < GRID_ROWS_COUNT &&
                clickedCellJ >= 0 && clickedCellJ < GRID_COLUMNS_COUNT)
            {
                markedCell(clickedCellI, clickedCellJ);
                InvalidateRect(hWnd, NULL, 1);
            }
        }
    }
    break;
    case WM_CREATE:
    {
        if (RANDOM_MINES)
            srand(time(NULL));
        else
            srand(1000);
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

            // Настройки шрифта
            HFONT hFont = CreateFont(20,
                0, 0, 0, 0, 0, 0, 0,
                DEFAULT_CHARSET,
                0, 0, 0, 0,
                L"Courier New"
            );
            SelectObject(hdc, hFont);
            SetTextColor(hdc, RGB(0, 0, 128));
            SetBkMode(hdc, TRANSPARENT);


            drawMinesweeperFrame(hWnd, hdc, GAME_GRID_X, GAME_GRID_Y);
            drawMinesweeperStatistics(hdc, GAME_GRID_X + GRID_COLUMNS_COUNT * MIN_CELL_SIZE + 20, GAME_GRID_Y);

            

            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
    {
        PostQuitMessage(0);
    }
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
