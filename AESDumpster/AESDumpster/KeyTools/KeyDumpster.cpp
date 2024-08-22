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
std::string KeyDumpster::ConcatenateAESType(PBYTE key_addr, int type) {
  std::string hex_string = "";
  for (size_t i = 0; i < key_DWORD_offsets[type].size(); i++) {
    hex_string += DumpsterTools::hexStr(&key_addr[key_DWORD_offsets[type][i]], 4);
  }
  std::transform(hex_string.begin(), hex_string.end(), hex_string.begin(), ::toupper);
  return hex_string;
}

//Scan memory aes key buffer and extract the individual dword key buffers.
int KeyDumpster::FindAESKeys(char* file_buffer, uint64_t size) {
  for (size_t i = 0; i < aes_key_sig_patterns.size(); i++) {
    auto local_aes_key_addresses = Find(aes_key_sig_patterns[i].c_str(), (PBYTE)file_buffer, (PBYTE)file_buffer + size);
    for (size_t j = 0; j < local_aes_key_addresses.size(); j++) {
      // Concat and push string to vector.
      keys.push_back(ConcatenateAESType(local_aes_key_addresses[j], i));
    }
  }
  return 0;
}

//Generate aes key entropy score.
int KeyDumpster::KeyEntropyGenerator(std::vector<std::string> KeyVector)
{
  for (size_t i = 0; i < KeyVector.size(); i++) {
    key_entropy_results.push_back(DumpsterTools::CalcEntropy(KeyVector[i]));
  }
  return 0;
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
