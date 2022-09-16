#pragma once
#include "main_libs.h"

void SCLEncode(const char* in, const char* out);
void readFile(FILE* f, FILE*);

void getSubsLabs(char* );
void parseSCL(char*, FILE*);

void setHeader(char* l, SCLHeader&);
void skipHeader(char* l);


struct Subroutine
{
	std::string name;
	//DWORD add = 0xffffffff;
	addMap localLab;
	//constMap localConst;

	DWORD getLabAdd(std::string nm)
	{
		return localLab[nm];
	}
	Subroutine(std::string n)
	{
		name = n;
	}
	Subroutine()
	{

	}
};

typedef std::map<DWORD, Subroutine> subCont;