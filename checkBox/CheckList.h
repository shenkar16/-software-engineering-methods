#include <iostream>
using namespace std;

class CheckList {
	string cont;
	COORD c;
	DWORD wAttr;
	DWORD wAttr2;

public:
	CheckList(string text, int, int);
	void set_values(int wid, int he);
	COORD retC(){ return c; }
	string GetCont(){ return cont; };
	DWORD GetFColor(){ return wAttr; };
	DWORD GetBColor(){ return wAttr2; };
	void SetBColor(DWORD a){ wAttr2 = a; };
};