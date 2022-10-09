#include "SCLEncode.h"
#include "SCLIns.h"
std::string mainFile, currentfile;
FileMap openFiles;
constMap globalConst;
constMap localConst;
subCont curSubr;
DWORD ln = 1, outPos = sizeof(SCLHeader), subIndex = outPos, headIndex = -1, headPosIn = -1, SCLLv = 0x0, plSb = 0x0;
bool onSub = false, onHeader = false, inPlSubs = false, inSCLLevels = false, inExTex = false;

#define PRNTFILEERR std::cout << currentfile << ": ";

/*/////// SCL Script compiler for Seihou 2 - Kioh Gyoku /////////////

-14/09/2022
	This compiler follows the structure of the SCL script files for Kioh Gyoku.
	-Header with definitions of most attacks, enemy subroutines and texture loading.
	-Subroutines containing Enemy Control Language (ECL) and Tama (aka bullet) Control Language (TCL).

	This compiler also supports directives like "#define" to 
	define constants and "#include" for including files.

	This is an improved version of my old compiler i 
	made when i was starting to learn about reading and writting files
	few months ago.

	I will also use part of the code for a compiler for ECL and SCL scripts for Seihou 1 - Shuusou Gyoku,
	since it follows a kinda similar structure as KOG's SCL.

	All of this was possible thanks to the game's Source Code uploaded by Ponchi (PBG).
*/
void SCLEncode(const char* in, const char* out)
{
	FILE* r = fopen(in, "r");
	FILE* s = fopen(out, "wb");
	if (r == NULL)
	{
		printf("Couldn't open %s.\n", in);
		exit(-1);
	}
	mainFile = in;
	currentfile = in;
	openFiles.insert({mainFile, mainFile});
	readFile(r, s, false);
	fclose(r);
	fclose(s);
}

void readFile(FILE* f, FILE* out, bool include)
{
	char line[0x100];
	while (!feof(f))
	{
		if (onHeader)
		{
			fgets(line, sizeof(line), f);
			skipHeader(line);
			ln++;
		}
		else
		{
			fgets(line, sizeof(line), f);
			getSubsLabs(line);
			ln++;
		}
	}
	DWORD endSeek = ln;
	ln = 1; outPos = sizeof(SCLHeader); subIndex = outPos;
	fseek(f, 0, SEEK_SET);
	if (!include)
	{
		SCLHeader header;
		memset(&header, 0xff, sizeof(SCLHeader));
		header.SCLLvEnmCnt[0] = 0;
		header.SCLLvEnmCnt[1] = 0;
		header.SCLLvEnmCnt[2] = 0;
		header.SCLLvEnmCnt[3] = 0;
		for (int i = 0; i < KOG_CHAR; i++)
		{
			header.LTEntry[i].numTex = 0;
		}
		subCont::iterator it = curSubr.begin();
		for (; it != curSubr.end(); ++it)
		{
			if (it->second.name == "TexEntry")
			{
				header.texInit = it->first;
				break;
			}
		}
		if (it == curSubr.end())
		{
			printf("Error in line %d: @TexEntry Subroutine not found.", ln);
		}
		while (ln < endSeek)
		{
			fgets(line, sizeof(line), f);
			setHeader(line, header);
			ln++;
		}
		fwrite(&header, sizeof(SCLHeader), 1, out);
		fseek(f, 0, SEEK_SET);
		ln = 1; outPos = sizeof(SCLHeader); subIndex = outPos;
	}
	openFiles.clear();
	openFiles.insert({ mainFile, mainFile });
	while (ln < endSeek)
	{
		if (onHeader)
		{
			fgets(line, sizeof(line), f);
			skipHeader(line);
			ln++;
		}
		else
		{
			fgets(line, sizeof(line), f);
			parseSCL(line, out);
			ln++;
		}
	}
}

void getSubsLabs(char* l)
{
	int pos = 0;
	std::string str;
	while (blankSpace(l[pos]))
	{
		pos++;
	}

	if (l[pos] == '\n' || skipComment(l[pos]))
	{
		return;
	}
	switch (l[pos])
	{
	case '#':
		do
		{
			str.push_back(l[pos]);
			pos++;
		} while ((l[pos] >= 'a' && l[pos] <= 'z'));
		if (directives.find(str) != directives.end())
		{
			if (directives[str] == "#define")
			{
				return;
			}
			else if (directives[str] == "#include")
			{
				while (blankSpace(l[pos]))
				{
					pos++;
				}
				if (l[pos] == '"')
					pos++;
				else
				{
					PRNTFILEERR
					printf("Error on line %d: Start of string not found.\n");
					exit(1);
				}
				str = "";
				while (l[pos] != '"' && pos < 0x100)
				{
					str.push_back(l[pos]);
					pos++;
				}
				includedFileAdd(str.c_str());
				return;
			}
			else if (onHeader = directives[str] == "#header")
			{
				return;
			}
		}
		else
		{
			PRNTFILEERR
			printf("Error on line %d: Not a compiler directive.\n", ln);
			exit(1);
		}
		break;
	case '@':
	{
		if (onSub)
		{
			PRNTFILEERR
			printf("Error on line %d: Can't create a Subroutine inside a Subroutine.\n", ln);
			exit(2);
		}
		subIndex = outPos;
		onSub = true;
		pos++;
		while (validChar(l[pos]))
		{
			str.push_back(l[pos]);
			pos++;
		}
		if (l[pos] != ':')
		{
			PRNTFILEERR
			printf("Error on line %d: Bad Subroutine name.\n", ln);
			exit(2);
		}
		pos++;
		for (subCont::iterator i = curSubr.begin(); i != curSubr.end(); ++i)
		{
			if (i->second.name == str)
			{
				PRNTFILEERR
				printf("Error on line %d: Duplicated Subroutine name (\"", ln); std::cout << str << "\").\n";
				exit(2);
			}
		}
		Subroutine sub(str);
		curSubr.insert({ subIndex, sub });
	}
	break;
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
				PRNTFILEERR
				printf("Error on line %d: Bad Label name.\n", ln);
				exit(3);
			}
			
			if (curSubr[subIndex].localLab.find(str) == curSubr[subIndex].localLab.end())
			{
				curSubr[subIndex].localLab.insert({ str, outPos });
			}
			else
			{
				PRNTFILEERR
				printf("Error on line %d: Duplicated label name (\"", ln); std::cout << str << "\").\n";
				exit(3);
			}
		}
		else
		{
			PRNTFILEERR
			printf("Error on line %d: Can't create labels outside a Subroutine\n.", ln);
			exit(3);
		}break;
	default:
		if (onSub && validChar(l[pos]))
		{
				bool foundIns = false;
				while (validChar(l[pos]))
				{
					str.push_back(l[pos]);
					pos++;
				}
				for (InsMap::iterator i = insNames.begin(); i != insNames.end(); i++)
				{
					if (i->second == str || str == "END")
					{
						foundIns = true;
						if (str == "END")
						{
							onSub = false;
							return;
						}
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
								PRNTFILEERR
								printf("Error on line %d: String missing.\n", ln);
								exit(4);
							}
							pos++;
							while (l[pos] != '"')
							{
								B
									pos++;
							}
							pos++;
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
								PRNTFILEERR
								printf("Error on line %d: String missing.\n", ln);
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
					PRNTFILEERR
					printf("Error on line: %d: Instruction ", ln); std::cout << str << " doesn't exist.\n";
					exit(4);
				}
		}
		else
		{
			PRNTFILEERR
			printf("Error on line: %d: Can't write instructions outside a Subroutine.\n", ln);
			exit(4);
		}
		break;
	}

		return;
}

void parseSCL(char* l, FILE* out)
{
	int pos = 0;
	std::string str;
	while (blankSpace(l[pos]))
	{
		pos++;
	}
		if (l[pos] == '#')
		{
			do
			{
				str.push_back(l[pos]);
				pos++;
			} while ((l[pos] >= 'a' && l[pos] <= 'z'));
			if (directives.find(str) != directives.end())
			{
				if (directives[str] == "#define")
				{
					while (blankSpace(l[pos]))
					{
						pos++;
					}
					str = "";
					while (validChar(l[pos]))
					{
						str.push_back(l[pos]);
						pos++;
					}

					if (!onSub)
						globalConst.insert({ str, getNumber(l, pos, ln) });
					else
						localConst.insert({ str, getNumber(l, pos, ln) });
				}
				else if (directives[str] == "#include")
				{
					while (blankSpace(l[pos]))
					{
						pos++;
					}
					if (l[pos] == '"')
						pos++;
					else
					{
						PRNTFILEERR
							printf("Error on line %d: Start of string not found.\n");
						exit(1);
					}
					str = "";
					while (l[pos] != '"' && pos < 0x100)
					{
						str.push_back(l[pos]);
						pos++;
					}
					includedFileParse(str.c_str(), out);
					return;
				}
				else if (onHeader = directives[str] == "#header")
				{
					return;
				}
			}
			else
			{
				PRNTFILEERR
					printf("Error on line %d: Not a compiler directive.\n", ln);
				exit(1);
			}
	}
		else if (l[pos] == '@')
		{
			if (onSub)
			{
				PRNTFILEERR
					printf("Error on line %d: Can't create a Subroutine inside a Subroutine.\n", ln);
				exit(2);
			}
			subIndex = outPos;
			pos++;
			while (validChar(l[pos]))
			{
				str.push_back(l[pos]);
				pos++;
			}
			if (l[pos] != ':')
			{
				PRNTFILEERR
					printf("Error on line %d: Bad Subroutine name.\n", ln);
				exit(2);
			}
			pos++;
			onSub = true;
		}
		else if (l[pos] == '.')
		{
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
					PRNTFILEERR
						printf("Error on line %d: Bad Label name.\n", ln);
					exit(3);
				}
				pos++;
			}
			else
			{
				PRNTFILEERR
					printf("Error on line %d: Can't create labels outside a Subroutine\n.", ln);
				exit(3);
			}
		}
		else if (validChar(l[pos]))
		{
			if (onSub || blankSpace(l[pos]))
			{
				int param;
				DWORD add = 0;
				if (validChar(l[pos]))
				{
					bool foundIns = false;
					while (validChar(l[pos]))
					{
						str.push_back(l[pos]);
						pos++;
					}
#ifdef CLB_DEBUG
					std::cout << l[pos] << "\n";
#endif
					for (InsMap::iterator i = insNames.begin(); i != insNames.end(); i++)
					{
						if (i->second == str || str == "END")
						{
							foundIns = true;
							if (onSub && str == "END")
							{
								localConst.clear();
								onSub = false;
								return;
							}
							bool foundAdd = false;
							int ar;
							outPos++;
#define B param = getNumber(l, pos, ln); fwrite(&param, 1, 1, out); outPos++;
#define W param = getNumber(l, pos, ln); fwrite(&param, 2, 1, out); outPos+= 2;
#define D param = getNumber(l, pos, ln); fwrite(&param, 4, 1, out); outPos+= 4;

#define C param = getNumber(l, pos, ln); fwrite(&param, 1, 1, out); outPos++;
#define S param = getNumber(l, pos, ln); fwrite(&param, 2, 1, out); outPos+= 2;
#define I param = getNumber(l, pos, ln); fwrite(&param, 4, 1, out); outPos+= 4;
#define CO pos++;
#define SBR str = "";while (blankSpace(l[pos])){pos++;}while (validChar(l[pos])){str.push_back(l[pos]);pos++;}for (subCont::iterator i = curSubr.begin(); i != curSubr.end(); ++i){if (i->second.name == str){add = i->first;fwrite(&add, 4, 1, out);foundAdd = true;outPos += 4; break;}}if (!foundAdd){printf("Error on line %d: Sub not found.\n"); exit(5);}
#define LAB str = "";while (blankSpace(l[pos])){pos++;}while (validChar(l[pos])) {str.push_back(l[pos]);pos++;}add = curSubr[subIndex].getLabAdd(str);if (curSubr[subIndex].localLab.find(str) != curSubr[subIndex].localLab.end()){fwrite(&add, 4, 1, out);outPos += 4;}else{PRNTFILEERR printf("Error on line %d: Local label not found.\n", ln);exit(5);}if (add == 0x00000000){PRNTFILEERR printf("Error in line %d: Local label not found.\n", ln); exit(5);}



							//#define STR outPos++;
#ifdef CLB_DEBUG
							std::cout << i->second << "\n";;
#endif
							if (!(str == insNames[POPR] || str == insNames[PUSHR] || str == insNames[PARENT] || str == insNames[LOAD]))
							{
								putc(i->first, out);
							}

							switch (i->first)
							{
							case NOP: W
								break;
							case SET: S CO S CO SBR
								break;
							case LOAD:
								str = "";
								B putc(i->first, out); CO B CO
									while (blankSpace(l[pos]))
									{
										pos++;
									}
								if (l[pos] != '"')
								{
									PRNTFILEERR
										printf("Error on line %d: String missing.\n", ln);
									exit(4);
								}
								pos++;
								while (l[pos] != '"')
								{
									putc(l[pos], out);
									outPos++;
									pos++;
								}
								putc(0x00, out);
								pos++;
								outPos++;
								break;
							case RECT: B CO W CO W CO W CO W
								break;
							case ANIME: {
								B CO B
									int i = param;
								for (int j = 0; j < i; j++)
								{
									CO B
								}
							}break;
							case LOADEX:
								str = "";
								while (blankSpace(l[pos]))
								{
									pos++;
								}
								if (l[pos] != '"')
								{
									PRNTFILEERR
										printf("Error on line %d: String missing.\n", ln);
									exit(4);
								}
								pos++;
								while (l[pos] != '"')
								{
									putc(l[pos], out);
									outPos++;
									pos++;
								}
								putc(0x00, out);
								pos++;
								outPos++;
								break;
							case STOP:
								break;

							case CALL:
								SBR
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
							case ACC:
								S CO
									S CO
									W
									break;
							case ROL:
								S CO
									C CO
									W
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
								B putc(i->first, out);
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
							case POPR: B putc(i->first, out);
								break;
							case MOVC: B CO I
								break;
							case PUSHC: I
								break;
							case TJMP:
							case FJMP:
							case JMP:
							case OJMP:
							case AJMP:
								LAB
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
						PRNTFILEERR
							printf("Error on line: %d: Instruction ", ln); std::cout << str << " doesn't exist.\n";
						exit(4);
					}
				}
			}
			else
			{
				PRNTFILEERR
					printf("Error on line: %d: Can't write instructions outside a Subroutine.\n", ln);
				exit(4);
			}
		}
		while (blankSpace(l[pos]))
		{
			pos++;
		}
		if (l[pos] == '\n' || skipComment(l[pos]) || l[pos] == 0x00)
		{
			return;
		}
		else
		{
			PRNTFILEERR
				printf("Error on line %d: Instruction can't receive more parameters.\n", ln);
			exit(14);
		}
}


void setHeader(char* l, SCLHeader& h)
{
	int pos = 0;
	std::string str;
	while (blankSpace(l[pos]))
	{
		pos++;
	}
	if (l[pos] == '#')
	{

		do
		{
			str.push_back(l[pos]);
			pos++;
		} while ((l[pos] >= 'a' && l[pos] <= 'z'));
		if (directives.find(str) != directives.end())
		{
			if (directives[str] == "#define" || directives[str] == "#include")
			{
				return;
			}
			else if (onHeader = directives[str] == "#header")
			{
				//headerRead = true;
				return;
			}
		}
		else
		{
			PRNTFILEERR
			printf("Error on line %d: Not a compiler directive.\n", ln);
			exit(7);
		}
	}
	else if (l[pos] == '@')
	{
		pos++;
		if (onHeader)
		{
			while (validChar(l[pos]))
			{
				str.push_back(l[pos]);
				pos++;
			}
			if (l[pos] != ':')
			{
				PRNTFILEERR
				printf("Error on line %d: Bad Header type name.\n", ln);
				exit(2);
			}
			pos++;
			/*////////////

			Layout:

			@SCLLevels:
				@SCL1:
					Sub1,
					Sub2,
					Sub3
				@SCL2:
					Sub1,
					Sub2,
					Sub3
				@SCL3:
					Sub1,
					Sub2,
					Sub3
				@SCL4:
					Sub1,
					Sub2,
					Sub3
			@VIVIT:
				ATK2,
				ATK3,
				ATKBOSS,
				COMBO,
				ANMLv2,
				ANMLv3,
				ANMBoss,
				ANMWin
				@ExTex: //Max of 12
					SubTx1,
					SubTx1,
					SubTx1,
					SubTx1,
					SubTx1,
					SubTx1

			////////////*/

			if (str == "SCLLevels" || SCLLv < 0b1111)
				inSCLLevels = true;
			else if (validKOGChar(str) && plSb < 0b111111111)
			{
				if (str == "VIVIT")
				{
					headIndex = VIVIT;
					plSb |= 0b1;
				}
				else if (str == "Milia")
				{
					headIndex = MILIA;
					plSb |= 0b10;
				}
				else if (str == "Mei_Mai")
				{
					headIndex = MEI_MAI;
					plSb |= 0b100;
				}
				else if (str == "Gates")
				{
					headIndex = GATES;
					plSb |= 0b1000;
				}
				else if (str == "Marie")
				{
					headIndex = MARIE;
					plSb |= 0b10000;
				}
				else if (str == "Erich")
				{
					headIndex = ERICH;
					plSb |= 0b100000;
				}
				else if (str == "Morgan")
				{
					headIndex = MORGAN;
					plSb |= 0b1000000;
				}
				else if (str == "Muse")
				{
					headIndex = MUSE;
					plSb |= 0b10000000;
				}
				else if (str == "Yuuka")
				{
					headIndex = YUUKA;
					plSb |= 0b100000000;
				}

				headPosIn = 0;
				inPlSubs = true;
			}

			if (inSCLLevels && (str == "SCL1" || str == "SCL2" || str == "SCL3" || str == "SCL4"))
			{
				if (!(SCLLv == 0b1111))
				{
					if (str == "SCL1")
					{
						headIndex = 0; SCLLv |= 0b1;
					}
					else if (str == "SCL2")
					{
						headIndex = 1; SCLLv |= 0b10;
					}
					else if (str == "SCL3")
					{
						headIndex = 2; SCLLv |= 0b100;
					}
					else if (str == "SCL4")
					{
						headIndex = 3; SCLLv |= 0b1000;
					}
				}
				else
				{
					PRNTFILEERR
					printf("Error on line %d: Re-declaring SCLLevel Enemy timelines.\n", ln);
					exit(8);
				}
				headPosIn = 0;
			}
			else if (inExTex = str == "ExTex")
			{
				h.LTEntry[headIndex].numTex = 0;
				headPosIn = 0;
			}
		}
		else
		{
			return;
		}
	}
	else if (! (l[pos] == '\n' || skipComment(l[pos])))
	{
		DWORD address;
		bool foundAdd = false;
		if (inSCLLevels)
		{

			while (validChar(l[pos]))
			{
				str.push_back(l[pos]);
				pos++;
			}
			if (blankSpace(l[pos]) || l[pos] == '\n')
			{
				inSCLLevels = false;
			}
			for (subCont::iterator i = curSubr.begin(); i != curSubr.end(); ++i)
			{
				if (i->second.name == str)
				{
					address = i->first;
					foundAdd = true;
					break;
				}
			}
			if (!foundAdd)
			{
				PRNTFILEERR
				printf("Error on line %d: Sub not found.\n", ln);
				exit(8);
			}
			switch (headIndex)
			{
			case -1:
				PRNTFILEERR
				printf("Error on line %d: Can't write Subroutine names outside SCL Level Subs.\n", ln);
				exit(8);
			case 0:
				h.SCL_lv1[headPosIn] = address; h.SCLLvEnmCnt[0]++; headPosIn++; break;
			case 1:
				h.SCL_lv2[headPosIn] = address; h.SCLLvEnmCnt[1]++; headPosIn++; break;
			case 2:
				h.SCL_lv3[headPosIn] = address; h.SCLLvEnmCnt[2]++; headPosIn++; break;
			case 3:
				h.SCL_lv4[headPosIn] = address; h.SCLLvEnmCnt[3]++; headPosIn++; break;
			}
		}
		else if (inPlSubs)
		{
			while (validChar(l[pos]))
			{
				str.push_back(l[pos]);
				pos++;
			}
			if (blankSpace(l[pos]) || l[pos] == '\n')
			{
				inPlSubs = false;
			}
			for (subCont::iterator i = curSubr.begin(); i != curSubr.end(); ++i)
			{
				if (i->second.name == str)
				{
					address = i->first;
					foundAdd = true;
					break;
				}
			}
			if (!foundAdd)
			{
				PRNTFILEERR
				printf("Error on line %d: Sub not found.\n", ln);
				exit(8);
			}
			switch (headPosIn)
			{
			case -1:
				PRNTFILEERR
				printf("Error on line %d.\n", ln);
				exit(8);
			case 0:
				h.Lv1Att[headIndex] = address; headPosIn++; break;
			case 1:
				h.Lv2Att[headIndex] = address; headPosIn++; break;
			case 2:
				h.BossAtt[headIndex] = address; headPosIn++; break;
			case 3:
				h.ComboAtt[headIndex] = address; headPosIn++; break;
			case 4:
				h.Lv1Anm[headIndex] = address; headPosIn++; break;
			case 5:
				h.Lv2Anm[headIndex] = address; headPosIn++; break;
			case 6:
				h.BossAnm[headIndex] = address; headPosIn++; break;
			case 7:
				h.WinAnm[headIndex] = address; headPosIn = 0; inExTex = true; inPlSubs = false; break;
			}
		}
		else if (inExTex)
		{

			while (validChar(l[pos]))
			{
				str.push_back(l[pos]);
				pos++;
			}
			if (blankSpace(l[pos]) || l[pos] == '\n')
			{
				inExTex = false;
			}
			for (subCont::iterator i = curSubr.begin(); i != curSubr.end(); ++i)
			{
				if (i->second.name == str)
				{
					address = i->first;
					foundAdd = true;
					break;
				}
			}
			if (!foundAdd)
			{
				PRNTFILEERR
				printf("Error on line %d: Sub not found.\n", ln);
				exit(8);
			}
			if (headIndex < 12)
			{
				h.LTEntry[headIndex].LoadExTexAdd[headPosIn] = address; 
				h.LTEntry[headIndex].numTex++;
				headPosIn++;
			}
		}
		return;
	}


	while (blankSpace(l[pos]))
	{
		pos++;
	}
	if (l[pos] == '\n' || skipComment(l[pos]))
	{
		return;
	}
	else
	{
		PRNTFILEERR
		printf("Error on line %d: Instruction can't receive more parameters.\n", ln);
		exit(14);
	}
}

void skipHeader(char* l)
{
	int pos = 0;
	std::string str;
	while (blankSpace(l[pos]))
	{
		pos++;
	}
	if (l[pos] == '\n' || skipComment(l[pos]))
	{
		return;
	}
	if (l[pos] == '#')
	{

		do
		{
			str.push_back(l[pos]);
			pos++;
		} while ((l[pos] >= 'a' && l[pos] <= 'z'));
		if (directives.find(str) != directives.end())
		{
			if (directives[str] == "#define" || directives[str] == "#include")
			{
				PRNTFILEERR
				printf("Error on line %d: Can't include or define constants inside the header.\n", ln);
				exit(8);
			}
			else if (directives[str] == "#header")
			{
				PRNTFILEERR
				printf("Error on line %d: Header has been defined.\n", ln);
				exit(8); 
				return;
			}
			else if (directives[str] == "#endheader")
			{
				onHeader = false;
				return;
			}

		}
		else
		{
			PRNTFILEERR
			printf("Error on line %d: Not a compiler directive.\n", ln);
			exit(8);
		}
	}
	else
	{
		return;
	}
}

void includedFileAdd(const char* f)
{
	std::string prevfName = currentfile; currentfile = f;
	if(openFiles.find(f) == openFiles.end())
		openFiles.insert({ f, f });
	else
	{
		PRNTFILEERR
		printf("Error on line %d: File %s has been included before.", ln, f);
		exit(10);
	}
	FILE* in = fopen(f, "r");
	DWORD prevline = ln; ln = 1;
	if (in == NULL)
	{
		printf("Couldn't open %s.\n", f);
		exit(-1);
	}
	char l[0x100];
	while(!feof(in))
	{

		fgets(l, sizeof(l), in);
		getSubsLabs(l);
		ln++;
	}
	ln = prevline;
	currentfile = prevfName;
	fclose(in);
}

void includedFileParse(const char* f, FILE* out)
{
	std::string prevfName = currentfile; currentfile = f;
	if (openFiles.find(f) == openFiles.end())
		openFiles.insert({ f, f });
	else
	{
		PRNTFILEERR
		printf("Error on line %d: File %s has been included before.", ln, f);
		exit(11);
	}
	FILE* in = fopen(f, "r");
	DWORD prevline = ln; ln = 1;
	if (in == NULL)
	{
		printf("Couldn't open %s.\n", f);
		exit(-1);
	}
	char l[0x100];
	while (!feof(in))
	{

		fgets(l, sizeof(l), in);
		parseSCL(l, out);
		ln++;
	}
	ln = prevline;
	currentfile = prevfName;
	fclose(in);
}