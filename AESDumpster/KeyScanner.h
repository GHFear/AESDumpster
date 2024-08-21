#pragma once

namespace KeyScanner
{
#define INRANGE(x,a,b)      (x >= a && x <= b) 
#define getBits(x)          (INRANGE(x,'0','9') ? (x - '0') : ((x&(~0x20)) - 'A' + 0xa))
#define getByte(x)          (getBits(x[0]) << 4 | getBits(x[1]))

  // Find patter in memory buffer.
	static std::vector<PBYTE> Find(const char* pattern, PBYTE rangeStart = NULL, PBYTE rangeEnd = NULL)
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

  std::vector<const char*> aes_key_sig_patterns = 
  { 
    "C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ?", 
    "C7 ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ?", 
    "C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ?"
  };

  std::vector<std::string> FindAESKeys(char* file_buffer, uint64_t size) {
    // Console handle.
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    // AES String vectors.
    std::vector<std::string> KeyVector;
    std::vector<PBYTE> aes_key_addresses = {};

    for (size_t i = 0; i < aes_key_sig_patterns.size(); i++)
    {
      auto local_aes_key_addresses = KeyScanner::Find(aes_key_sig_patterns[i], (PBYTE)file_buffer, (PBYTE)file_buffer + size);
      aes_key_addresses.insert(aes_key_addresses.end(), local_aes_key_addresses.begin(), local_aes_key_addresses.end());

      for (size_t j = 0; j < aes_key_addresses.size(); j++) {
        // Concat and push string to vector.
        KeyVector.push_back(DumpsterTools::ConcatenateAESType(aes_key_addresses, j, i));
      }
    }

    return KeyVector;
  }

  std::vector<double> KeyEntropyGenerator(std::vector<std::string> KeyVector)
  {
    // AES Entropy vector.
    std::vector<double> KeyEntropyVector;
    for (size_t i = 0; i < KeyVector.size(); i++) {
      KeyEntropyVector.push_back(DumpsterTools::CalcEntropy(KeyVector[i]));
    }
    return KeyEntropyVector;
  }
}