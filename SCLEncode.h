#pragma once
#include "main_libs.h"

void SCLEncode(const char* in, const char* out);
void readLine(char* );
void parseSCLIns(char*);
int putConst();
void readFile(FILE* f);


struct Subroutine
{
	std::string name;
	//DWORD add = 0xffffffff;
	addMap localLab;
	constMap localConst;
	Subroutine(std::string n)
	{
		name = n;
	}
	Subroutine()
	{

	}
};

typedef std::map<DWORD, Subroutine> subCont;