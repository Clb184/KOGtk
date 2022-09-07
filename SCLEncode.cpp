#include "SCLEncode.h"
#include "SCLIns.h"

constMap globalConst;
subCont curSubr;
DWORD ln = 1, outPos = sizeof(SCLHeader), subIndex = outPos;
bool onSub = false;

void SCLEncode(const char* in, const char* out)
{
	FILE* r = fopen(in, "r");
	FILE* s = fopen(out, "wb");
	if (r == NULL)
	{
		printf("Couldn't open the file.\n");
		exit(-1);
	}
	readFile(r);
}

void readFile(FILE* f)
{
	char line[0x100];
	while (!feof(f))
	{
		fgets(line, sizeof(line), f);
		readLine(line);
		ln++;
	}

}

void readLine(char* l)
{
	int pos = 0;
	std::string str;
	while (blankSpace(l[pos]))
	{
		pos++;
	}
	switch (l[pos])
	{
	case '#':
		do
		{
			str.push_back(l[pos]);
			pos++;
		} while ((l[pos] >= 'A' && l[pos] <= 'Z') || (l[pos] >= 'a' && l[pos] <= 'z'));
		if (directives.find(str) != directives.end())
		{
			if (directives[str] == "#define")
			{
				while (blankSpace(l[pos]))
				{
					pos++;
				}
				str = "";
				do
				{
					str.push_back(l[pos]);
					pos++;
				} while (validChar(l[pos]));

				if (!onSub)
					globalConst.insert({ str, getNumber(l, pos, ln) });
				else
					curSubr[subIndex].localConst.insert({str, getNumber(l, pos, ln)});
			}
			else if (directives[str] == "#include")
			{

			}
		}
		else
		{
			printf("Error on line %d: Not a compiler directive.\n", ln);
			exit(1);
		}
		break;
	case '@':
	{
		subIndex = outPos;
		pos++;
		while (validChar(l[pos]))
		{
			str.push_back(l[pos]);
			pos++;
		}
		if (l[pos] != ':')
		{
			printf("Error on line %d: Bad Subroutine name.\n", ln);
			exit(2);
		}

		for (subCont::iterator i = curSubr.begin(); i != curSubr.end(); ++i)
		{
			if (i->second.name == str)
			{
				printf("Error on line %d: Duplicated Subroutine name (\"", ln); std::cout << str << "\").\n";
				exit(2);
			}
		}
		Subroutine sub(str);
		curSubr.insert({ subIndex, sub });
		onSub = true;
		/*
		if (subr.find(str) == subr.end())
		{
			subr.insert({str, 0});
		}
		else
		{
		}*/
	}break;
	case '.':
		if (onSub)
		{
			pos++;
			while (validChar(l[pos]))
			{
				str.push_back(l[pos]);
				pos++;
			}
			if (l[pos] != ':')
			{
				printf("Error on line %d: Bad Label name.\n", ln);
				exit(3);
			}
			
			if (curSubr[subIndex].localLab.find(str) == curSubr[subIndex].localLab.end())
			{
				curSubr[subIndex].localLab.insert({ str, outPos });
			}
			else
			{
				printf("Error on line %d: Duplicated label name (\"", ln); std::cout << str << "\").\n";
				exit(3);
			}
		}
		else
		{
			printf("Error on line %d: Can't create labels outside a Subroutine\n.", ln);
			exit(3);
		}break;
	default:
		if (onSub)
		{
			if (validChar(l[pos]))
			{
				bool foundIns = false;
				while (validChar(l[pos]))
				{
					str.push_back(l[pos]);
					pos++;
				}
				for (InsMap::iterator i = insNames.begin(); i != insNames.end(); i++)
				{
					if (foundIns = i->second == str)
					{
#define B outPos++;
#define W outPos+= 2;
#define D outPos+= 4;

#define C outPos++;
#define S outPos+= 2;
#define I outPos+= 4;
#define CO pos++;
#define SBR outPos+= 4;
#define LAB outPos+= 4;
//#define STR outPos++;
						int ar;
						outPos++;
#ifdef CLB_DEBUG
						std::cout << i->second << "\n";;
#endif
						switch (i->first)
						{
						case NOP: W
							break;
						case SET: S CO S CO SBR
							break;
						case LOAD:
							getNumber(l, pos, ln); CO getNumber(l, pos, ln); CO B B
							while (blankSpace(l[pos]))
							{
								pos++;
							}
							if (l[pos] != '"')
							{
								printf("Error on line %d: String missing.\n");
								exit(4);
							}
							pos++;
							while (l[pos] != '"')
							{
								B
									pos++;
							}
							B
								break;
						case RECT: B CO W CO W CO W CO W
							break;
						case ANIME: B  B getNumber(l, pos, ln); CO
							ar = getNumber(l, pos, ln); outPos += ar;
								  break;
						case LOADEX:
							while (blankSpace(l[pos]))
							{
								pos++;
							}
							if (l[pos] != '"')
							{
								printf("Error on line %d: String missing.\n");
								exit(4);
							}
							pos++;
							while (l[pos] != '"')
							{
								B
									pos++;
							}
							B
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
						case PARENT:
							B 
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
						case PUSHR:
						case POPR: B
							break;
						case MOVC: B CO I
							break;
						case PUSHC: I
							break;
						case TJMP: 
						case FJMP: 
						case JMP: 
						case OJMP:
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
						}
						break;
					}
				}
				if (!foundIns)
				{

					printf("Error on line: %d: Instruction \n", ln); std::cout << str << "doesn't exist.\n";
					exit(4);
				}
			}
		}
		else
		{
			printf("Error on line: %d: Can't write instructions outside a Subroutine.\n", ln);
			exit(4);
		}
		break;
	}
}