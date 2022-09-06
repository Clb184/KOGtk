#pragma once
#include <iostream>
#include<stdio.h>
#include<string>
#include<map>


#define KOG_CHAR 10
#define MAX_TEXEX 12

#define SCL_MAXENM 50


typedef uint8_t BYTE;
typedef uint16_t WORD;
typedef uint32_t DWORD;
typedef uint64_t QWORD;

typedef std::map<std::string, std::string> Keywords;
typedef std::map<uint8_t, std::string> InsMap;
typedef std::map<uint32_t,uint32_t> Address;
typedef std::map<std::string, DWORD> constMap;
typedef std::map<std::string, DWORD> addMap;

std::string readWord(FILE*);
std::string readWord(char*);

void readStr(FILE*&);

long int readNum(FILE*);

static Keywords directives
{
	/*
	Funcionamiento como directivas de preprocesador.
	#define aaa 20
	Reemplaza a cada string aaa con 20, sin embargo,
	si tiene un carácter más, entonces no existe.

	#include "file"
	incluye un archivo...
	Ahora tendré que ver como hacer para
	*/


	{"define", "define"},
	{"DEFINE", "DEFINE"},

	{"include", "include"},
	{"INCLUDE", "INCLUDE"}
};


struct lTexEntry
{
	int LoadExTexAdd[MAX_TEXEX];
	int numTex;
};


struct SCLHeader
{
	//Cantidad de enemigos e inicializador de texturas.
	int SCLLvEnmCnt[4];
	int texInit;

	//Direcciones de los ataques.
	int Lv1Att[KOG_CHAR];
	int Lv2Att[KOG_CHAR];
	int BossAtt[KOG_CHAR];
	int ComboAtt[KOG_CHAR];

	//Direcciones de animaciones.
	int Lv1Anm[KOG_CHAR];
	int Lv2Anm[KOG_CHAR];
	int BossAnm[KOG_CHAR];
	int WinAnm[KOG_CHAR];

	//Llama a estas direcciones que crean enemigos.
	int SCL_lv1[SCL_MAXENM];
	int SCL_lv2[SCL_MAXENM];
	int SCL_lv3[SCL_MAXENM];
	int SCL_lv4[SCL_MAXENM];

	//ExTextures (Imágenes con formato .tga)
	lTexEntry LTEntry[KOG_CHAR];
};

static InsMap insNames =
{
	{0x30,"NOP"},
	{0x31, "SET"},

	{0x40, "LOAD"},
	{0x41, "RECT"},
	{0x42, "ANIME"},
	{0x43, "LOAD2"},
	{0x44, "LOADEX"},
	{0x45, "STOP"},

	{0x50, "CALL"},
	{0x51, "ATK"},
	{0x52, "ESET"},
	{0x53, "RET"},
	{0x54, "ANM"},
	{0x55, "FATK"},
	{0x56, "ATKNP"},

	{0x57, "MOV"},
	{0x58, "ACC"},
	{0x59, "ROL"},
	{0x5a, "WAITATOBJ"},

	{0x5b, "PSE"},
	{0x5c, "KILL"},

	{0x5d, "MDMG"},

	{0x5e, "CHILD"},
	{0x5f, "CHGTASK"},
	{0x60, "PARENT"},

	{0x61, "PMOV"},
	{0x62, "PACC"},
	{0x63, "PROL"},
	{0x64, "PNOP"},

	{0x65, "ATK2"},

	{0x66, "EFC"},

	{0x70, "TAMA"},
	{0x71, "LASER"},
	{0x72, "DEGE"},
	{0x73, "DEGS"},
	{0x74, "LLCHARGE"},
	{0x75, "LLOPEN"},
	{0x76, "LLCLOSE"},
	{0x77, "HLASER"},
	{0x78, "LSPHERE"},
	{0x79, "RLASER"},
	{0x7a, "CROSS"},
	{0x7b, "FLOWER"},
	{0x7c, "GFIRE"},
	{0x7d, "IONRING"},

	{0x90, "TALKMSG"},
	{0x91, "TALKKEY"},
	{0x92, "TALKNEWL"},
	{0x93, "TALKWAIT"},

	{0xa0, "TASK"},
	{0xa1, "TEXMODE"},

	{0xc0, "PUSHR"},
	{0xc1, "POPR"},
	{0xc2, "MOVC"},

	{0xc4, "PUSHC"},
	{0xc5, "TJMP"},
	{0xc6, "FJMP"},
	{0xc7, "JMP"},
	{0xca, "OJMP"},
	{0xcb, "AJMP"},
	{0xcc, "EXIT"},
	{0xcd, "LPOP"},
	{0xce, "LJMP"},

	{0xd0, "ADD"},
	{0xd1, "SUB"},
	{0xd2, "MUL"},
	{0xd3, "DIV"},
	{0xd4, "MOD"},
	{0xd5, "NEG"},

	{0xd6, "SINL"},
	{0xd7, "COSL"},
	{0xd8, "RND"},
	{0xd9, "ATAN"},

	{0xda, "EQUAL"},
	{0xdb, "NOTEQ"},
	{0xdc, "GREAT"},
	{0xdd, "LESS"},
	{0xde, "GREATEQ"},
	{0xdf, "LESSEQ"},

	{0xe0, "MAX"},
	{0xe1, "MIN"}
};