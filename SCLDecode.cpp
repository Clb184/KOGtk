#include "SCLDecode.h"
#include "SCLIns.h"
void SCLDecode(const char* f)
{
	SCLHeader header;
	FILE* fp = fopen(f, "rb"), *init;
	if (fp == NULL) 
	{ 
		printf("An error ocurred while opening file.\n"); exit(-1); 
	}

	fread(&header, sizeof(SCLHeader), 1, fp);
	init = fp;
	Address sub;
	Address lab;
	for (int i = 0; i < header.SCLLvEnmCnt[0]; i++)
	{
		sub.insert({ header.SCL_lv1[i],header.SCL_lv1[i] });
	}
	for (int i = 0; i < header.SCLLvEnmCnt[1]; i++)
	{
		sub.insert({ header.SCL_lv2[i],header.SCL_lv2[i] });
	}
	for (int i = 0; i < header.SCLLvEnmCnt[2]; i++)
	{
		sub.insert({ header.SCL_lv3[i],header.SCL_lv3[i] });
	}
	for (int i = 0; i < header.SCLLvEnmCnt[3]; i++)
	{
		sub.insert({ header.SCL_lv4[i],header.SCL_lv4[i] });
	}
	for (int i = 0; i < KOG_CHAR; i++)
	{
		sub.insert({header.Lv1Att[i], header.Lv1Att[i] });
		sub.insert({ header.Lv2Att[i], header.Lv2Att[i] });
		sub.insert({ header.BossAtt[i], header.BossAtt[i] });
		sub.insert({ header.ComboAtt[i], header.ComboAtt[i] });
		sub.insert({ header.Lv1Anm[i], header.Lv1Anm[i] });
		sub.insert({ header.Lv2Anm[i], header.Lv2Anm[i] });
		sub.insert({ header.BossAnm[i], header.BossAnm[i] });
		sub.insert({ header.WinAnm[i], header.WinAnm[i] });
		for (int j = 0; j < header.LTEntry[i].numTex; j++)
		{
			sub.insert({ header.LTEntry[i].LoadExTexAdd[j],header.LTEntry[i].LoadExTexAdd[j] });
		}
	}
	getAddress(fp, sub, lab);
	//fp = init;
	int j = 0;
	for (Address::iterator i = sub.begin(); i != sub.end(); ++i)
	{
		if (i->second >= sizeof(SCLHeader))
		{
			i->second = j;
			j++;
		}
	}
	j = 0;
	for (Address::iterator i = lab.begin(); i != lab.end(); ++i)
	{
		i->second = j;
		j++;
	}
	readIns(fp, sub, lab, header.texInit);
	fclose(fp);
}


void readIns(FILE* in, Address sub, Address lab, DWORD texEntry)
{
	fseek(in, 0, SEEK_END);
	DWORD pos = sizeof(SCLHeader), end = ftell(in);
	BYTE cmd;
	int param = 0, ar = 0;

	fseek(in, sizeof(SCLHeader), SEEK_SET);
#define B fread(&param, 1, 1, in); printf("%d", (BYTE)param);
#define W fread(&param, 2, 1, in); std::cout << (WORD)param;
#define D fread(&param, 4, 1, in); std::cout << (DWORD)param;

#define C fread(&param, 1, 1, in); printf("%d", (char)param); 
#define S fread(&param, 2, 1, in); std::cout << (short)param; 
#define I fread(&param, 4, 1, in); std::cout << param; 

#define STR readStr(in);

#define CO std::cout << ", ";
#define LAB fread(&param, 4, 1, in); std::cout << "lab_"<< lab[param]; 
#define SBR fread(&param, 4, 1, in); std::cout << "Sub"<< sub[param]; 

	
	while (pos < end - 1) 
	{
		pos = ftell(in);
		if (pos == texEntry)
			std::cout << "TexEntry:" << "\n";
		if (sub.find(pos) != sub.end())
			std::cout << "\n@Sub" << sub[pos] <<  ":\n";
		if (lab.find(pos) != lab.end())
			std::cout << ".lab_" << lab[pos] << ":\n";


		fread(&cmd, 1, 1, in);
		if (insNames.find(cmd) != insNames.end())
		{
			std::cout << "	" << insNames[cmd] << " ";
		}
		switch (cmd)
		{
		case NOP: W
			break;
		case SET: S CO S CO SBR
			break;

		case RECT: B CO W CO W CO W CO W
			break;
		case ANIME: B CO B ar = (BYTE)param;
			for (int i = 0; i < ar; i++)
		{
			CO B
		}
			break;
		case LOADEX: STR
			break;
		case STOP:
			break;

		case CALL: SBR
			break;
		case ATK: S CO S CO SBR
			break;
		case ESET: SBR
			break;
		case RET: B
			break;
		case ANM: B CO B
			break;
		case FATK: SBR
			break;
		case ATKNP: SBR
			break;
		case MOV: S CO W
			break;
		case ACC: S CO S CO W
			break;
		case ROL: S CO C CO W
			break;
		case WAITATOBJ:
			break;
		case PSE: B
			break;
		case KILL:
			break;
		case MDMG: W
			break;
		case CHILD: B CO SBR
			break;
		case CHGTASK: B CO SBR
			break;
		case PMOV: S CO W
			break;
		case PACC: S CO S CO W
			break;
		case PROL: S CO C CO W
			break;
		case PNOP: W
			break;
		case ATK2: S CO S CO I CO SBR
			break;
		case EFC: S CO S CO B
			break;

		case TAMA:
		case LASER:
		case DEGE:
		case DEGS:
		case LLCHARGE:
			break;

		case LLOPEN: D
			break;

		case LLCLOSE:
		case HLASER:
		case LSPHERE:
		case RLASER:
		case CROSS:
		case FLOWER:
		case GFIRE:
		case IONRING:
			break;

		case TASK: SBR
			break;
		case TEXMODE: B
			break;
		case MOVC: B CO I
			break;
		case PUSHC: I
			break;
		case TJMP: LAB
			break;
		case FJMP: LAB
			break;
		case JMP: LAB
			break;
		case OJMP: LAB
			break;
		case AJMP: LAB
			break;
		case EXIT:
		case LPOP:
			break;
		case LJMP: LAB
			break;
		case ADD:
		case SUB:
		case MUL:
		case DIV:
		case MOD:
		case NEG:
		case SINL:
		case COSL:
		case RND:
		case ATAN:
		case EQUAL:
		case NOTEQ:
		case GREAT:
		case LESS:
		case GREATEQ:
		case LESSEQ:
		case MAX:
		case MIN:
			break;
		default:
		{
			BYTE reg = cmd;
			fread(&cmd, 1, 1, in);
			if (insNames.find(cmd) != insNames.end())
			{
				std::cout << "	" << insNames[cmd] << " ";
			}
			else
			{
				printf("Bad instruction format.\n"); exit(-1);
			}
			printf("%d", (BYTE)reg);
			switch (cmd)
			{
			case LOAD: CO B CO STR
				break;
			case PARENT:
			case PUSHR:
			case POPR:
				break;
			default:
				printf("Incorrect Instruction syntax on %d.\n", pos); exit(-1);
			}
			break;
		}
		}
		printf("\n");
	}
}

#define AC(x) fseek(f, x, SEEK_CUR);
void getAddress( FILE* f, Address& sub, Address& lab)
{
	DWORD add;
#define GSUB fread(&add, 4, 1, f); if(sub.find(add) == sub.end()){ sub.insert( {add, add}); }
#define GLAB fread(&add, 4, 1, f); if(lab.find(add) == lab.end()){ lab.insert( {add, add}); }
	BYTE cmd;
	fseek(f, sizeof(SCLHeader), SEEK_SET);
	while(!feof(f))
	{
		fread(&cmd, 1, 1, f);
		switch (cmd)
		{
		case NOP: AC(2)
			break;
		case SET:
			AC(4) GSUB
			break;

		case RECT:
			AC(9)
			break;
		case ANIME: AC(1) fread(&cmd, 1, 1, f); AC(cmd)
			break;
		case LOADEX:
			do
			{
				fread(&cmd, 1, 1, f);
			} while (cmd);
			break;
		case STOP:
			break;

		case CALL: GSUB
			break;
		case ATK: AC(4) GSUB
			break;
		case ESET: GSUB
			break;
		case RET: AC(1) add = ftell(f); if (sub.find(add) == sub.end()) { sub.insert({ add, add }); }
			break;
		case ANM: AC(2)
			break;
		case FATK: GSUB
			break;
		case ATKNP: GSUB
			break;
		case MOV: AC(4)
			break;
		case ACC: AC(6)
			break;
		case ROL: AC(5)
			break;
		case WAITATOBJ:
			break;
		case PSE: AC(1)
			break;
		case KILL:
			break;
		case MDMG: AC(2)
			break;
		case CHILD: AC(1) GSUB
			break;
		case CHGTASK: AC(1) GSUB
			break;
		case PMOV: AC(4)
			break;
		case PACC: AC(6)
			break;
		case PROL: AC(5)
			break;
		case PNOP: AC(2)
			break;
		case ATK2: AC(8) GSUB
			break;
		case EFC: AC(5)
			break;

		case TAMA:
		case LASER:
		case DEGE:
		case DEGS:
		case LLCHARGE:
			break;

		case LLOPEN: AC(4)
			break;

		case LLCLOSE:
		case HLASER:
		case LSPHERE:
		case RLASER:
		case CROSS:
		case FLOWER:
		case GFIRE:
		case IONRING:
			break;

		case TASK: GSUB
			break;
		case TEXMODE: AC(1)
			break;
		case MOVC: AC(5)
			break;
		case PUSHC: AC(4)
			break;
		case TJMP: GLAB
			break;
		case FJMP: GLAB
			break;
		case JMP: GLAB
			break;
		case OJMP: GLAB
			break;
		case AJMP: GLAB
			break;
		case EXIT:
		case LPOP:
			break;
		case LJMP: GLAB
			break;
		case ADD:
		case SUB:
		case MUL:
		case DIV:
		case MOD:
		case NEG:
		case SINL:
		case COSL:
		case RND:
		case ATAN:
		case EQUAL:
		case NOTEQ:
		case GREAT:
		case LESS:
		case GREATEQ:
		case LESSEQ:
		case MAX:
		case MIN:
			break;
		default:
			fread(&cmd, 1, 1, f);
			if (cmd == LOAD)
			{
				AC(1)
				do
				{
					fread(&cmd, 1, 1, f);
				} while (cmd);
			}
		}
	}
}