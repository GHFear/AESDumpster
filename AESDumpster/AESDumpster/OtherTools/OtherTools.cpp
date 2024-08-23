#include "OtherTools.h"

OtherTools::OtherTools()
{
}

void OtherTools::PrintIntro() {
  std::wcout << "AESDumpster 1.2.5 - By GHFear @ IllusorySoftware" << std::endl;
  std::wcout << "Supports Unreal Engine 4.19 -> 5.3 | (Will soon support UE 4.0 - 4.18 as well)\n\n" << std::endl;
}

void OtherTools::PrintInstructions() {
  std::wcout << L"Usage:\n";
  std::wcout << L"-Drag and drop Unreal Engine executables onto AESDumpster.exe.\n";
  std::wcout << L"-Wait for the tool to finish.\n";
  std::cin.ignore();
}

void OtherTools::PrintFileName(HANDLE hConsole, const wchar_t* argv) {
  SetConsoleTextAttribute(hConsole, 14);
  std::wcout << argv << std::endl;
  SetConsoleTextAttribute(hConsole, 15);
}

void OtherTools::PrintOutro(HANDLE hConsole) {
  SetConsoleTextAttribute(hConsole, 10);
  std::wcout << "Done!\n";
  SetConsoleTextAttribute(hConsole, 15);
}

int OtherTools::CreateExeBuffer(std::wstring filepath) {
  
  // Open file (exe).
  FILE* exe_file = nullptr;
  _wfopen_s(&exe_file, filepath.c_str(), L"rb");
  if (exe_file == NULL) {
    assert("Error opening exe file");
    return 1;
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
    return 1;
  }

  // Read file into buffer.
  fread(file_buffer, exe_size, 1, exe_file);

  //Fill struct return values.
  retval.buffer = file_buffer;
  retval.size = exe_size;

  return 0;
}

OtherTools::~OtherTools()
{
}
