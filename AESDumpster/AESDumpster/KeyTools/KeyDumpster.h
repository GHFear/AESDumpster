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
#include "../Tools/Tools.h"

class KeyDumpster
{
#define INRANGE(x,a,b)      (x >= a && x <= b) 
#define getBits(x)          (INRANGE(x,'0','9') ? (x - '0') : ((x&(~0x20)) - 'A' + 0xa))
#define getByte(x)          (getBits(x[0]) << 4 | getBits(x[1]))

public:
  // Constructor
  KeyDumpster();

  // Find patter in memory buffer.
  std::vector<PBYTE> Find(const char* pattern, PBYTE rangeStart = NULL, PBYTE rangeEnd = NULL);

  // AES String concatenator type.
  std::string ConcatenateAESType(PBYTE aes_key_addresses, int type);

  // Find AES Keys in exe buffer.
  int FindAESKeys(char* file_buffer, uint64_t size);

  // Generate entropy score for each key.
  int KeyEntropyGenerator(std::vector<std::string> KeyVector);

  // Destructor
  ~KeyDumpster();

private:
  // AES Binary Pattern Sigs
  std::vector<std::string> aes_key_sig_patterns =
  {
    "C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ?",
    "C7 ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ?",
    "C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? 48 ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? 48 ? ?",
    "C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? C3",
  };

  std::vector<std::vector<int>> key_DWORD_offsets = {
    { 3, 10, 17, 24, 35, 42, 49, 56 },
    { 2, 9, 16, 23, 30, 37, 44, 51 },
    { 3, 10, 21, 28, 35, 42, 49, 56 },
    { 51, 45, 38, 31, 24, 17, 10, 3 }
  };

public:
  std::vector<std::string> keys = {};
  std::vector<double> key_entropy_results = {};

  
};

