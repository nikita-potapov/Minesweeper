#pragma once

#include "resource.h"


void drawMinesweeperFrame(HWND hWnd, HDC hdc, int x, int y);
void drawMinesweeperGrid(HDC hdc, int x, int y);
void drawMinesweeperStatistics(HDC hdc, int x, int y);
void drawMinesweeperCell(HDC hdc, int x, int y, int i, int j);
void drawLine(HDC hdc, int x1, int y1, int x2, int y2);
void drawUnexplored(HDC hdc, int x, int y, int i, int j);
void drawOpened(HDC hdc, int x, int y, int i, int j);
void drawMine(HDC hdc, int x, int y, int i, int j);
void drawMineHit(HDC hdc, int x, int y, int i, int j);
void drawFlag(HDC hdc, int x, int y, int i, int j);
void drawLine(HDC hdc, int x1, int y1, int x2, int y2);
void lightCell(HDC hdc, int i, int j);
void fillMines(int i, int j);
void openCell(int i, int j);
void markedCell(int i, int j);
int getMinesAroundCount(int i, int j);
void recursiveOpenCell(int i, int j);
int isCordsGood(int i, int j);
void showAllMines();