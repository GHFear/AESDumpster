// AESDumpster 1.2 by GHFear

#include "Tools.h"
#include "KeyScanner.h"

void debugmain(HANDLE hConsole)
{
  // Debug exe path
  std::wstring exe_path = L"Z:\\Exes\\NotProtected\\SessionGame-Win64-Shipping.exe";

  // Print filename (exe full path).
  DumpsterTools::PrintFileName(hConsole, exe_path.c_str());

  //Create filebuffer from filepath.
  auto exe_buffer = DumpsterTools::BufferExeFromPath(exe_path.c_str());
  if (exe_buffer.buffer == nullptr) { return; }

  // Scan buffered file for keys and extract potential matches.
  auto keys = KeyScanner::FindAESKeys(exe_buffer.buffer, exe_buffer.size);
  auto key_entropy_results = KeyScanner::KeyEntropyGenerator(keys);
  auto mex_element_result = DumpsterTools::FindMaxElements(key_entropy_results);

  //Print keys and relevant key information.
  DumpsterTools::PrintKeyInformation(keys, key_entropy_results, mex_element_result.second);

  // Free buffer when done with it.
  free(exe_buffer.buffer);
  return;
}

void releasemain(HANDLE hConsole, int argc, wchar_t* argv[])
{
  if (argc == 1) {
    // Print instructions if launched without command.
    DumpsterTools::PrintInstructions();
    return;
  }

  // Main loop.
  for (int i = 0; i < argc; i++) {
    if (i > 0) {
      // Print filename (exe full path).
      DumpsterTools::PrintFileName(hConsole, argv[i]);

      //Create filebuffer from filepath.
      auto exe_buffer = DumpsterTools::BufferExeFromPath(argv[i]);
      if (exe_buffer.buffer == nullptr) { break; }

      // Scan buffered file for keys and extract potential matches.
      auto keys = KeyScanner::FindAESKeys(exe_buffer.buffer, exe_buffer.size);
      auto key_entropy_results = KeyScanner::KeyEntropyGenerator(keys);
      auto mex_element_result = DumpsterTools::FindMaxElements(key_entropy_results);

      //Print keys and relevant key information.
      DumpsterTools::PrintKeyInformation(keys, key_entropy_results, mex_element_result.second);

      // Free buffer when done with it.
      free(exe_buffer.buffer);
    }
  }
  return;
}

// Wide main.
int wmain(int argc, wchar_t* argv[])
{
  // Console handle.
  HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

  //Print intro.
  DumpsterTools::PrintIntro();

#if defined _DEBUG
  debugmain(hConsole);
#else
  releasemain(hConsole, argc, argv);
#endif
  
  // Print outro before exiting.
  DumpsterTools::PrintOutro(hConsole);
  return 0;
}