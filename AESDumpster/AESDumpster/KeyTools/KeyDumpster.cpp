#include "KeyDumpster.h"

KeyDumpster::KeyDumpster()
{
#if defined _DEBUG
  std::wcout << "KeyDumpster Constructed." << std::endl;
#else

#endif
}

// Find patter in memory buffer.
std::vector<PBYTE> KeyDumpster::Find(const char* pattern, PBYTE rangeStart, PBYTE rangeEnd)
{
  std::vector<PBYTE> keyLocations;
  const unsigned char* pat = reinterpret_cast<const unsigned char*>(pattern);
  PBYTE firstMatch = 0;
  for (PBYTE pCur = rangeStart; pCur < rangeEnd; ++pCur) {
    if (*(PBYTE)pat == (BYTE)'\?' || *pCur == getByte(pat)) {
      if (!firstMatch) firstMatch = pCur;
      pat += (*(PWORD)pat == (WORD)'\?\?' || *(PBYTE)pat != (BYTE)'\?') ? 2 : 1;
      if (!*pat) {
        keyLocations.push_back(firstMatch);
        continue;
      }
      pat++;
      if (!*pat) {
        keyLocations.push_back(firstMatch);
        continue;
      }
    }
    else if (firstMatch) {
      pCur = firstMatch;
      pat = reinterpret_cast<const unsigned char*>(pattern);
      firstMatch = 0;
    }
  }
  return keyLocations;
}

// AES String concatenator type
std::string KeyDumpster::ConcatenateAESType(std::vector<PBYTE> aes_key_addresses, int i, int type) {
  if (type == 0) { // Binary layout 1
    return
      DumpsterTools::hexStr(&aes_key_addresses[i][3], 4) + DumpsterTools::hexStr(&aes_key_addresses[i][10], 4) +
      DumpsterTools::hexStr(&aes_key_addresses[i][17], 4) + DumpsterTools::hexStr(&aes_key_addresses[i][24], 4) +
      DumpsterTools::hexStr(&aes_key_addresses[i][35], 4) + DumpsterTools::hexStr(&aes_key_addresses[i][42], 4) +
      DumpsterTools::hexStr(&aes_key_addresses[i][49], 4) + DumpsterTools::hexStr(&aes_key_addresses[i][56], 4);
  }
  else if (type == 1) { // Binary layout 2
    return
      DumpsterTools::hexStr(&aes_key_addresses[i][2], 4) + DumpsterTools::hexStr(&aes_key_addresses[i][9], 4) +
      DumpsterTools::hexStr(&aes_key_addresses[i][16], 4) + DumpsterTools::hexStr(&aes_key_addresses[i][23], 4) +
      DumpsterTools::hexStr(&aes_key_addresses[i][30], 4) + DumpsterTools::hexStr(&aes_key_addresses[i][37], 4) +
      DumpsterTools::hexStr(&aes_key_addresses[i][44], 4) + DumpsterTools::hexStr(&aes_key_addresses[i][51], 4);
  }
  else if (type == 2) { // Binary layout 3
    return
      DumpsterTools::hexStr(&aes_key_addresses[i][3], 4) + DumpsterTools::hexStr(&aes_key_addresses[i][10], 4) +
      DumpsterTools::hexStr(&aes_key_addresses[i][21], 4) + DumpsterTools::hexStr(&aes_key_addresses[i][28], 4) +
      DumpsterTools::hexStr(&aes_key_addresses[i][35], 4) + DumpsterTools::hexStr(&aes_key_addresses[i][42], 4) +
      DumpsterTools::hexStr(&aes_key_addresses[i][49], 4) + DumpsterTools::hexStr(&aes_key_addresses[i][56], 4);
  }
  return "";
}

std::vector<std::string> KeyDumpster::FindAESKeys(char* file_buffer, uint64_t size) {
  // Console handle.
  HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

  // AES String vectors.
  std::vector<std::string> KeyVector;
  std::vector<PBYTE> aes_key_addresses = {};

  for (size_t i = 0; i < aes_key_sig_patterns.size(); i++)
  {
    auto local_aes_key_addresses = Find(aes_key_sig_patterns[i].c_str(), (PBYTE)file_buffer, (PBYTE)file_buffer + size);
    aes_key_addresses.insert(aes_key_addresses.end(), local_aes_key_addresses.begin(), local_aes_key_addresses.end());

    for (size_t j = 0; j < aes_key_addresses.size(); j++) {
      // Concat and push string to vector.
      KeyVector.push_back(ConcatenateAESType(aes_key_addresses, j, i));
    }
  }

  keys = KeyVector;
  return KeyVector;
}

std::vector<double> KeyDumpster::KeyEntropyGenerator(std::vector<std::string> KeyVector)
{
  // AES Entropy vector.
  std::vector<double> KeyEntropyVector;
  for (size_t i = 0; i < KeyVector.size(); i++) {
    KeyEntropyVector.push_back(DumpsterTools::CalcEntropy(KeyVector[i]));
  }
  key_entropy_results = KeyEntropyVector;
  return KeyEntropyVector;
}


KeyDumpster::~KeyDumpster()
{
  aes_key_sig_patterns.clear();
  key_entropy_results.clear();
  keys.clear();
#if defined _DEBUG
  std::wcout << "KeyDumpster Destructed." << std::endl;
#else

#endif
}
