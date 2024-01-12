#pragma once
#include <afxwin.h>

struct Line
{
	CPoint start;
	CPoint end;
	Line(const CPoint& s,const CPoint& e):start(s),end(e){}
};