#include "main_libs.h"


std::string readWord(FILE* fp)
{
	BYTE c;
	std::string ret; 
	do
	{
		fread(&c, 1, 1, fp);
		if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') && !(feof(fp)))
			ret.push_back(c);
	} while (c == ' ' || c == '\n' || c == '	');
	do
	{
		fread(&c, 1, 1, fp);
		if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') && !(feof(fp)))
			ret.push_back(c);
	} while ((
		(c >= '0' && c <= '9') || 
		(c >= 'A' && c <= 'Z') || 
		(c >= 'a' && c <= 'z') ) && !feof(fp));
	return ret;
}

std::string readWord(char* buffer)
{
	return "none";
}

long int readNum(FILE* fp)
{
	bool hex = false, bin = false, dec = false;
	BYTE c;
	std::string ret;
	do
	{
		fread(&c, 1, 1, fp);
		if ((c >= '0' && c <= '9'))
			ret.push_back(c);
	} while (c == ' ' || c == '\n' || c == '	');
	do
	{
		if(!feof(fp)) 
		{
		fread(&c, 1, 1, fp);
		if (hex)
		{
			if (((c >= '0' && c <= '9')) || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f') && !(feof(fp)))
				ret.push_back(c);
			else
			{
				hex = false;
			}
		}
		else if (bin)
		{
			if (c == '0' || c == '1')
				ret.push_back(0);
			else
			{
				bin = false;
			}
		}
		else if (dec)
		{
			if (((c >= '0' && c <= '9')) && !(feof(fp)))
				ret.push_back(c);
			else
			{
				dec = false;
			}
		}

		if (!hex && !bin && !dec)
		{
			hex = c == 'x';
			if (hex && !feof(fp))
				ret.push_back(c);
		}
		if (!bin && !hex && !dec)
		{
			bin = c == 'b';
			if (bin && !feof(fp))
				ret.push_back(c);
		}
		if (!dec && !hex && !bin)
		{
			dec = (c >= '0' && c <= '9');
			if (dec && !feof(fp))
				ret.push_back(c);
		}
	}
	} while ((hex || bin || dec) && !(feof(fp)));
	return std::stoi(ret, nullptr, 0);
}

void readStr(FILE*& f)
{
	BYTE c;
	std::string str = "\"";
	fread(&c, 1, 1, f);
	while (c)
	{
		str.push_back(c);
		fread(&c, 1, 1, f);
	}
	str.push_back('\"');
	std::cout << str;
}