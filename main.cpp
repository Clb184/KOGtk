#include "main_libs.h"
#include "SCLDecode.h"
#include "SCLEncode.h"
#include "wav_pos.h"

void printUse()
{
	printf("Usage: KOGtk [sd | sc | pg | pc]\n");
	printf("                SCL     BGM_POS");
}


#ifdef CLB_DEBUG
int main()
{
	std::string fName, out;
	char mode;
	printf("File name: "); std::cin >> fName;
	printf("Mode ( p / o / d / c ): "); std::cin >> mode;
	switch (mode)
	{
	case 'p':
		BGMPosExtract(fName.c_str());
		break;
	case 'o':
		printf("File out: "); std::cin >> out;
		BGMPosChange(fName.c_str(), out.c_str());
		break;
	case 'd':
		SCLDecode(fName.c_str());
		break;
	case 'c':
		printf("File out: "); std::cin >> out;
		SCLEncode(fName.c_str(), out.c_str());
		break;
	}
}
#endif // CLB_DEBUG

#ifndef CLB_DEBUG

int main(int argc, char* argv[])
{
	//argv[1][0] = 'p';
	//argv[1][1] = 'g';
	//
	//argv[2][0] = (char)"POS_kog_00";


	std::string par = argv[1];
	if (argc < 3)
	{
		printUse();
		return -1;
	}
	else
	{

		//printf("%s, %s, %s\n", argv[1], argv[2], argv[3]);
		if (par == "sd")//par == "sd")
		{
			//printf("Still building...");
			SCLDecode(argv[2]);
			return 0;
		}
		else if (par == "sc")//par == "sc")
		{
			//printf("Still building...");

			if (argv[3] == NULL)
			{
				printf("Parameters for sc: input output");
			}
			else
			{
				SCLEncode(argv[2], argv[3]);
				return 0;
			}
		}
		else if (par == "pg")//par == "pg")
		{
			//printf("%s, %s, %s\n", argv[1], argv[2], argv[3]);
			BGMPosExtract(argv[2]);
			return 0;
		}
		else if (par == "pc")//par == "pc")
		{
			if (argv[3] == NULL)
			{
				printf("Parameters for pc: input output");
			}
			else
			{
				BGMPosChange(argv[2], argv[3]);
				return 0;
			}
		}
	}
}

#endif // !CLB_DEBUG
