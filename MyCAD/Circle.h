#pragma once
#include <afxwin.h>

struct Circle
{
	CPoint center;
	int radius;
	Circle(const CPoint& c,int r):center(c),radius(r){}
};