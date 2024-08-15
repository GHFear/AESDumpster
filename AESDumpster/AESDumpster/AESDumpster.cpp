// AESDumpster 1.0 by GHFear

#include "Tools.h"
#include "KeyScanner.h"

// Wide main.
int wmain(int argc, wchar_t* argv[])
{
  HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

  //Print intro
  DumpsterTools::PrintIntro();

  if (argc == 1) {
    // Print instructions if launched without command.
    DumpsterTools::PrintInstructions();
    return 0;
  }

  // Main loop
  for (int i = 0; i < argc; i++) {
    if (i > 0) {
      // Print filename (exe full path)
      DumpsterTools::PrintFileName(hConsole, argv[i]);

      //Create filebuffer from filepath.
      auto exe_buffer = DumpsterTools::BufferExeFromPath(argv[i]);
      if (exe_buffer.buffer == nullptr) {  break; }

      // Scan buffered file for keys and extract potential matches.
      auto keys = KeyScanner::FindAESKeys(exe_buffer.buffer, exe_buffer.size);
      auto key_entropies = KeyScanner::KeyEntropyGenerator(keys);
      auto mex_element_result = DumpsterTools::FindMaxElements(key_entropies);

      //Print keys and relevant key information.
      DumpsterTools::PrintKeyInformation(keys, key_entropies, mex_element_result.second);

      // Free buffer when done with it.
      free(exe_buffer.buffer);
    }
  }
    
  // Print outro before exiting.
  DumpsterTools::PrintOutro(hConsole);
  return 0;
}