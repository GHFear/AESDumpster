// AESDumpster 1.2.2 by GHFear

#include "../KeyTools/KeyDumpster.h"

void debugmain(HANDLE hConsole)
{
  // Debug exe path
  std::wstring exe_path = L"Z:\\Exes\\NotProtected\\SessionGame-Win64-Shipping.exe";

  // Print filename (exe full path).
  DumpsterTools::PrintFileName(hConsole, exe_path.c_str());

  //Create filebuffer from filepath.
  auto exe_buffer = DumpsterTools::BufferExeFromPath(exe_path.c_str());
  if (exe_buffer.buffer == nullptr) { return; }

  // Create KeyTool object and scan buffered file for keys and extract potential matches.
  KeyDumpster* key_dumpster = new KeyDumpster();
  key_dumpster->FindAESKeys(exe_buffer.buffer, exe_buffer.size);
  key_dumpster->KeyEntropyGenerator(key_dumpster->keys);
  auto mex_element_result = DumpsterTools::FindMaxElements(key_dumpster->key_entropy_results);

  //Print keys and relevant key information.
  DumpsterTools::PrintKeyInformation(key_dumpster->keys, key_dumpster->key_entropy_results, mex_element_result.second);

  // Free buffer when done with it.
  free(exe_buffer.buffer);

  //Destruct KeyTool object
  key_dumpster->~KeyDumpster();

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

      // Create KeyTool object and scan buffered file for keys and extract potential matches.
      KeyDumpster* key_dumpster = new KeyDumpster();
      key_dumpster->FindAESKeys(exe_buffer.buffer, exe_buffer.size);
      key_dumpster->KeyEntropyGenerator(key_dumpster->keys);
      auto mex_element_result = DumpsterTools::FindMaxElements(key_dumpster->key_entropy_results);

      //Print keys and relevant key information.
      DumpsterTools::PrintKeyInformation(key_dumpster->keys, key_dumpster->key_entropy_results, mex_element_result.second);

      // Free buffer when done with it.
      free(exe_buffer.buffer);

      //Destruct KeyTool object
      key_dumpster->~KeyDumpster();
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