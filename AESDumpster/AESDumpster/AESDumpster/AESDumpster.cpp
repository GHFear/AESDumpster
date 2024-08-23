// AESDumpster 1.2.5 by GHFear

#include "../OtherTools/OtherTools.h"
#include "../KeyTools/KeyDumpster.h"

// Debug main logic
void debugmain(HANDLE hConsole)
{
  // Debug exe path (set your own path to an exe on your ssd here or this won't work)
  std::wstring exe_path = L"Z:\\Exes\\NotProtected\\SessionGame-Win64-Shipping.exe";

  // Print filename (exe full path).
  OtherTools* other_tools = new OtherTools();
  other_tools->PrintFileName(hConsole, exe_path.c_str());

  //Create filebuffer using the given filepath.
  if (other_tools->CreateExeBuffer(exe_path.c_str()) != 0) { return; }
  if (other_tools->retval.buffer == nullptr) { printf("retval.buffer == nullptr.\n"); return; }

  // Create KeyDumpster object, scan for keys and print them.
  KeyDumpster* key_dumpster = new KeyDumpster();
  if (!key_dumpster->FindAESKeys(other_tools->retval.buffer, other_tools->retval.size))
    printf("There were no keys to be found or a problem occurred.\n");
  else
    key_dumpster->PrintKeyInformation();

  // Destruct KeyDumpster
  key_dumpster->~KeyDumpster();
  // Free buffer when done with it.
  free(other_tools->retval.buffer);
  // Print outro before exiting.
  other_tools->PrintOutro(hConsole);
  // Destruct OtherTools
  other_tools->~OtherTools();

  return;
}

// Release main logic
void releasemain(HANDLE hConsole, int argc, wchar_t* argv[])
{
  OtherTools* other_tools = new OtherTools();
 
  // Print intro.
  other_tools->PrintIntro();

  if (argc == 1) {
    // Print instructions if launched without command.
    other_tools->PrintInstructions();
    return;
  }

  // Main loop.
  for (int i = 0; i < argc; i++) {
    if (i > 0) {
      // Print filename (exe full path).
      other_tools->PrintFileName(hConsole, argv[i]);
      // Create filebuffer using the given filepath.
      if (other_tools->CreateExeBuffer(argv[i]) != 0) { return; }
      if (other_tools->retval.buffer == nullptr) { printf("retval.buffer == nullptr.\n");  break; }

      // Create KeyDumpster object, scan for keys and print them.
      KeyDumpster* key_dumpster = new KeyDumpster();
      if (!key_dumpster->FindAESKeys(other_tools->retval.buffer, other_tools->retval.size))
        printf("There were no keys to be found or a problem occurred.\n");
      else
        key_dumpster->PrintKeyInformation();

      // Destruct KeyTool object
      key_dumpster->~KeyDumpster();
      // Free buffer when done with it.
      free(other_tools->retval.buffer);
    }
  }
  // Print outro before exiting.
  other_tools->PrintOutro(hConsole);
  // Destruct OtherTools object
  other_tools->~OtherTools();

  return;
}

// Entrypoint
int wmain(int argc, wchar_t* argv[])
{
  // Console handle.
  HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

#if defined _DEBUG
  debugmain(hConsole);
#else
  releasemain(hConsole, argc, argv);
#endif

  std::cin.ignore();
  return 0;
}