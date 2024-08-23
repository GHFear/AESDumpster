#pragma once
#include <iomanip>
#include <sstream>
#include <string>
#include <algorithm>
#include <map>
#include <iostream>
#include <fstream>
#include <cassert>
#include <sys/stat.h>
#include <Windows.h>
#include <Psapi.h>
#include <vector>

class OtherTools
{
public:
  OtherTools();
  void PrintIntro();
  void PrintInstructions();
  void PrintFileName(HANDLE hConsole, const wchar_t* argv);
  void PrintOutro(HANDLE hConsole);
  int CreateExeBuffer(std::wstring filepath);
	~OtherTools();

public:
  struct RETVAL { char* buffer = nullptr; uint64_t size = 0; } retval;

};

