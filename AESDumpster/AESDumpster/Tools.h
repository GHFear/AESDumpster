#pragma once
#include <iomanip>
#include <sstream>
#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <cassert>
#include <sys/stat.h>
#include <Windows.h>
#include <Psapi.h>
#include <vector>

namespace DumpsterTools 
{
  // log2 from: https://tfetimes.com/c-entropy/
  double log2_intrinsic(double number) {
    return log(number) / log(2);
  }

  // hexStr from stackoverflow: https://stackoverflow.com/questions/14050452/how-to-convert-byte-array-to-hex-string-in-visual-c
  std::string hexStr(const uint8_t* data, int len) {
    std::stringstream ss;
    ss << std::hex;

    for (int i(0); i < len; ++i)
      ss << std::setw(2) << std::setfill('0') << (int)data[i];

    return ss.str();
  }

  // Calculate entropy score on AES keys.
  double CalcEntropy(std::string key_string) {
    std::map<char, int> frequencies;
    for (char c : key_string)
      frequencies[c]++;
    int numlen = key_string.length();
    double infocontent = 0;
    for (std::pair<char, int> p : frequencies) {
      double freq = static_cast<double>(p.second) / numlen;
      infocontent += freq * log2_intrinsic(freq);
    }
    infocontent *= -1;
    return infocontent;
  }

  std::pair<double, std::vector<std::size_t>> FindMaxElements(std::vector<double> const& v) {
    std::vector<std::size_t> indices;
    double current_max = -DBL_MAX;

    for (std::size_t i = 0; i < v.size(); ++i) {
      if (v[i] > current_max) {
        current_max = v[i];
        indices.clear();
      }

      if (v[i] == current_max) {
        indices.push_back(i);
      }
    }
    return std::make_pair(current_max, indices);
  }

  // AES String concatenator type 1
  std::string ConcatenateAESType1(std::vector<PBYTE> aes_key_addresses1, int i) {
    std::string key_string =
      hexStr(&aes_key_addresses1[i][3], 4) +
      hexStr(&aes_key_addresses1[i][10], 4) +
      hexStr(&aes_key_addresses1[i][17], 4) +
      hexStr(&aes_key_addresses1[i][24], 4) +
      hexStr(&aes_key_addresses1[i][35], 4) +
      hexStr(&aes_key_addresses1[i][42], 4) +
      hexStr(&aes_key_addresses1[i][49], 4) +
      hexStr(&aes_key_addresses1[i][56], 4);

    return key_string;
  }

  // AES String concatenator type 2
  std::string ConcatenateAESType2(std::vector<PBYTE> aes_key_addresses2, int i) {
    std::string key_string =
      hexStr(&aes_key_addresses2[i][2], 4) +
      hexStr(&aes_key_addresses2[i][9], 4) +
      hexStr(&aes_key_addresses2[i][16], 4) +
      hexStr(&aes_key_addresses2[i][23], 4) +
      hexStr(&aes_key_addresses2[i][30], 4) +
      hexStr(&aes_key_addresses2[i][37], 4) +
      hexStr(&aes_key_addresses2[i][44], 4) +
      hexStr(&aes_key_addresses2[i][51], 4);

    return key_string;
  }

  // AES String concatenator type 3
  std::string ConcatenateAESType3(std::vector<PBYTE> aes_key_addresses3, int i) {
    std::string key_string =
      hexStr(&aes_key_addresses3[i][3], 4) +
      hexStr(&aes_key_addresses3[i][10], 4) +
      hexStr(&aes_key_addresses3[i][21], 4) +
      hexStr(&aes_key_addresses3[i][28], 4) +
      hexStr(&aes_key_addresses3[i][35], 4) +
      hexStr(&aes_key_addresses3[i][42], 4) +
      hexStr(&aes_key_addresses3[i][49], 4) +
      hexStr(&aes_key_addresses3[i][56], 4);

    return key_string;
  }

  void PrintIntro() {
    std::wcout << "AESDumpster 1.0 - By GHFear @ IllusorySoftware" << std::endl;
    std::wcout << "Supports Unreal Engine 4.19 -> 5.3 | (Will soon support UE 4.0 - 4.18 as well)\n\n" << std::endl;
  }

  void PrintInstructions() {
    std::wcout << L"Usage:\n";
    std::wcout << L"-Drag and drop Unreal Engine executables onto AESDumpster.exe.\n";
    std::wcout << L"-Wait for the tool to finish.\n";
    std::cin.ignore();
  }

  void PrintFileName(HANDLE hConsole, wchar_t* argv) {
    SetConsoleTextAttribute(hConsole, 14);
    std::wcout << argv << std::endl;
    SetConsoleTextAttribute(hConsole, 15);
  }

  void PrintOutro(HANDLE hConsole) {
    SetConsoleTextAttribute(hConsole, 10);
    std::wcout << "Done!";
    std::cin.ignore();
  }

  void PrintKeyInformation(std::vector<std::string> keys, std::vector<double> key_entropies, std::vector<size_t> most_likely) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    for (size_t i = 0; i < keys.size(); i++) {
      SetConsoleTextAttribute(hConsole, 15);

      if (key_entropies[i] < 2.0) {
        SetConsoleTextAttribute(hConsole, 12);
      }

      if (key_entropies[i] < 3.0 && key_entropies[i] >= 2.0) {
        SetConsoleTextAttribute(hConsole, 6);
      }

      if (key_entropies[i] >= 3.0) {
        SetConsoleTextAttribute(hConsole, 14);
      }

      for (size_t j = 0; j < most_likely.size(); j++) {
        if (i == most_likely[j]) {
          SetConsoleTextAttribute(hConsole, 10);
        }
      }

      printf("Key: %s | Key Entropy: %f \n\n", keys[i].c_str(), key_entropies[i]);
    }
  }

  auto BufferExeFromPath(std::wstring filepath) {
    struct RETVAL { char* buffer = nullptr; uint64_t size = 0; } retval;

    // Open archive.
    FILE* archive = nullptr;
    _wfopen_s(&archive, filepath.c_str(), L"rb");
    if (archive == NULL) {
      assert("Error opening archive");
      return retval;
    }

    // Save start position.
    uint64_t start = _ftelli64(archive);

    // Save archive size and go back to start position.
    _fseeki64(archive, 0, SEEK_END);
    uint64_t size = _ftelli64(archive);
    _fseeki64(archive, start, SEEK_SET);

    // Create buffer.
    char* file_buffer = (char*)malloc(size);
    if (file_buffer == NULL) {
      assert("Error allocating memory");
      return retval;
    }

    // Read file into buffer.
    fread(file_buffer, size, 1, archive);

    //Fill struct return values.
    retval.buffer = file_buffer;
    retval.size = size;

    return retval;
  }
}


