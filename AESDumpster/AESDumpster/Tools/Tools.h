#pragma once

namespace DumpsterTools 
{
  static std::vector<std::string> false_positives =
  {
    "FFD9FFD9FFD9FFD9FFD9FFD9FFD9FFD9FFD9FFD9FFD9FFD9FFD9FFD9FFD9FFD9",
    "67E6096A85AE67BB72F36E3C3AF54FA57F520E518C68059BABD9831F19CDE05B",
    "D89E05C107D57C3617DD703039590EF7310BC0FF11155868A78FF964A44FFABE",
    "9A99593F9A99593F0AD7633F52B8BE3FE17A543FCDCC4C3D4260E53BAE47A13F",
    "6F168073B9B21449D742241700068ADABC306FA9AA3831164DEE8DE34E0EFBB0",
    "0AD7633FCDCC4C3DCDCCCC3D52B8BE3F9A99593F9A99593FC9767E3FE17A543F",
    "168073C7B21449C7430C00064310BC304314AA3843184DEE431C4E0E83C4205B",
    "E6096AC7AE67BBC7430C3AF543107F5243148C684318ABD9431C19CD436C2000",
    "9E05C1C7D57C36C7430C39594310310B431411154318A78F431CA44F436C1C00",
    "9E05C1C7D57C36C7DD7030C7590EF7C70BC0FFC7155868C78FF964C7A44FFABE",
    "168073C7B21449C7422417C7068ADAC7306FA9C7383116C7EE8DE3C74E0EFBB0",
    "0AD7633FCDCC4C3D00C742143DC742183FC7421C3FC742203FC742247E3FC742",
    "0000803F0AD7A33E0AD7633F52B8BE3FE17A543FCDCC4C3D4260E53B54AE47A1",
    "0AD7A33E0AD7633F52B8BE3FE17A543FCDCC4C3D4260E53BAE47A13F58583934",
    "0AD7A33E0AD7633F52B8BE3FE17A543FCDCC4C3D4260E53BAE47A13F38583934",
    "0000803F0AD7A33E0AD7633F52B8BE3FE17A543FCDCC4C3D4260E53B34AE47A1",
    "0000803F0000803F0AD7A33E0AD7633F52B8BE3FE17A543FCDCC4C3D2C4260E5",
    "0AD7633F52B8BE3FE17A543FCDCC4C3D4260E53BAE47A13F5839343C4CC9767E",
    "0AD7633F52B8BE3FE17A543FCDCC4C3D4260E53BAE47A13F5839343C4CC9767E",
    "07D57C3617DD703039590EF7310BC0FF11155868A78FF964A44FFABE6C1C0000",
    "85AE67BB72F36E3C3AF54FA57F520E518C68059BABD9831F19CDE05B6C200000",
    "E6096AC7AE67BBC7F36E3CC7F54FA5C7520E51C768059BC7D9831FC719CDE05B",
    "0AD7A33E0AD7633F52B8BE3FE17A543FCDCC4C3D4260E53BAE47A13F3C583934"
  };


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
    std::wcout << "AESDumpster 1.2.2 - By GHFear @ IllusorySoftware" << std::endl;
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
        if (std::find(false_positives.begin(), false_positives.end(), keys[i]) == false_positives.end()) {
          printf("Key: 0x%s | Key Entropy: %f\n\n", keys[i].c_str(), key_entropies[i]);
        }
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