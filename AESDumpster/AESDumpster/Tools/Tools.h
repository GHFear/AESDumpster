#pragma once

namespace DumpsterTools 
{
  // log2 from: https://tfetimes.com/c-entropy/
  static double log2_intrinsic(double number) {
    return log(number) / log(2);
  }

  // hexStr from stackoverflow: https://stackoverflow.com/questions/14050452/how-to-convert-byte-array-to-hex-string-in-visual-c
  static std::string hexStr(const uint8_t* data, int len) {
    std::stringstream ss;
    ss << std::hex;

    for (int i(0); i < len; ++i)
      ss << std::setw(2) << std::setfill('0') << (int)data[i];

    return ss.str();
  }

  // Calculate entropy score on AES keys.
  static double CalcEntropy(std::string key_string) {
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

  static std::pair<double, std::vector<std::size_t>> FindMaxElements(std::vector<double> const& v) {
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

  static void PrintIntro() {
    std::wcout << "AESDumpster 1.2.1 - By GHFear @ IllusorySoftware" << std::endl;
    std::wcout << "Supports Unreal Engine 4.19 -> 5.3 | (Will soon support UE 4.0 - 4.18 as well)\n\n" << std::endl;
  }

  static void PrintInstructions() {
    std::wcout << L"Usage:\n";
    std::wcout << L"-Drag and drop Unreal Engine executables onto AESDumpster.exe.\n";
    std::wcout << L"-Wait for the tool to finish.\n";
    std::cin.ignore();
  }

  static void PrintFileName(HANDLE hConsole, const wchar_t* argv) {
    SetConsoleTextAttribute(hConsole, 14);
    std::wcout << argv << std::endl;
    SetConsoleTextAttribute(hConsole, 15);
  }

  static void PrintOutro(HANDLE hConsole) {
    SetConsoleTextAttribute(hConsole, 10);
    std::wcout << "Done!";
    std::cin.ignore();
  }

  static void PrintKeyInformation(std::vector<std::string> keys, std::vector<double> key_entropies, std::vector<size_t> most_likely) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    for (size_t i = 0; i < keys.size(); i++) {

      if (key_entropies[i] >= 3.3 && key_entropies[i] < 3.4) {
        SetConsoleTextAttribute(hConsole, 12);
      }
      else if (key_entropies[i] >= 3.4 && key_entropies[i] < 3.5) {
        SetConsoleTextAttribute(hConsole, 6);
      }
      else if (key_entropies[i] >= 3.5 && key_entropies[i] < 3.7) {
        SetConsoleTextAttribute(hConsole, 14);
      }
      else if (key_entropies[i] >= 3.7) {
        SetConsoleTextAttribute(hConsole, 10);
      }

      for (size_t j = 0; j < most_likely.size(); j++) {
        if (i == most_likely[j]) {
          SetConsoleTextAttribute(hConsole, 10);
        }
      }
      if (key_entropies[i] >= 3.3) {
        printf("Key: 0x%s | Key Entropy: %f\n\n", keys[i].c_str(), key_entropies[i]);
      }
      SetConsoleTextAttribute(hConsole, 15);
    }
  }

  static auto BufferExeFromPath(std::wstring filepath) {
    struct RETVAL { char* buffer = nullptr; uint64_t size = 0; } retval;

    // Open file (exe).
    FILE* exe_file = nullptr;
    _wfopen_s(&exe_file, filepath.c_str(), L"rb");
    if (exe_file == NULL) {
      assert("Error opening exe file"); 
      return retval;
    }

    // Save start position.
    uint64_t exe_start_pos = _ftelli64(exe_file);
    // Save archive size and go back to start position.
    _fseeki64(exe_file, 0, SEEK_END);
    uint64_t exe_size = _ftelli64(exe_file);
    _fseeki64(exe_file, exe_start_pos, SEEK_SET);

    // Create buffer.
    char* file_buffer = (char*)malloc(exe_size);
    if (file_buffer == NULL) {
      assert("Error allocating memory");
      return retval;
    }

    // Read file into buffer.
    fread(file_buffer, exe_size, 1, exe_file);

    //Fill struct return values.
    retval.buffer = file_buffer;
    retval.size = exe_size;

    return retval;
  }
}