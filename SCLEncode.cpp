#include "SCLEncode.h"
#include "SCLIns.h"

constMap globalConst;
constMap localConst;
addMap localLab;
addMap subr;
DWORD ln = 1;
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

			while (blankSpace(l[pos]))
			{
				pos++;
			}
			if(!onSub)
				globalConst.insert({str, getNumber(l, pos, ln)});
			else
				localConst.insert({ str, getNumber(l, pos, ln) });
		}
		break;
	case '@':
	case '.':
	default:
		break;
	}
}