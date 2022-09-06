#include "wav_pos.h"


void BGMPosExtract(const char* f)
{
	FILE* fp = fopen(f, "rb");

	if (fp == NULL)
	{
		printf("Couldn't open the file.\n");
		exit(-1);
	}

	DWORD pos[2];
	fread(pos, sizeof(int), 2, fp);
	fclose(fp);
	printf("Start: %d,\n", pos[0]);
	printf("End: %d,", pos[1]);
	return;
}

void BGMPosChange(const char* f1, const char* f2)
{
	FILE* in = fopen(f1, "r");
	FILE* out = fopen(f2, "wb");


	if (in == NULL || out == NULL)
	{
		printf("Couldn't open the file.\n");
		exit(-1);
	}

	BYTE tr;
	DWORD pos[2];
	std::string w = readWord(in);
	pos[0] = (DWORD)readNum(in);
#ifdef CLB_DEBUG
	std::cout << w << ": " << pos[0] << "\n";
#endif

	if (w != "Start")
	{
		printf("Start of loop not detected...\n");
		exit(-1);
	}

	w = readWord(in);
	pos[1] = (DWORD)readNum(in);
#ifdef CLB_DEBUG
	std::cout << w << ": " << pos[1] << "\n";
#endif
	if (w != "End")
	{
		printf("End of loop not detected...\n");
		exit(-1);
	}
	fwrite(pos, 4, 2, out);
	fclose(in);
	fclose(out);
	return;
}