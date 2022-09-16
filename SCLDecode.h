#pragma once
#include "main_libs.h"
void SCLDecode(const char* f);
void readIns(FILE* in, Address sub, Address lab, DWORD texEntry);
void getAddress(FILE*, Address&, Address&);
void printHeader(SCLHeader, Address);