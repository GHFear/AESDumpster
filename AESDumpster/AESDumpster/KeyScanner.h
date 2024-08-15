#pragma once

namespace KeyScanner
{
#define INRANGE(x,a,b)      (x >= a && x <= b) 
#define getBits(x)          (INRANGE(x,'0','9') ? (x - '0') : ((x&(~0x20)) - 'A' + 0xa))
#define getByte(x)          (getBits(x[0]) << 4 | getBits(x[1]))


	static std::vector<PBYTE> Find(const char* pattern, PBYTE rangeStart = NULL, PBYTE rangeEnd = NULL)
	{
		std::vector<PBYTE> keyLocations;

		const unsigned char* pat = reinterpret_cast<const unsigned char*>(pattern);
		PBYTE firstMatch = 0;
		for (PBYTE pCur = rangeStart; pCur < rangeEnd; ++pCur)
		{
			if (*(PBYTE)pat == (BYTE)'\?' || *pCur == getByte(pat))
			{
				if (!firstMatch) firstMatch = pCur;
				pat += (*(PWORD)pat == (WORD)'\?\?' || *(PBYTE)pat != (BYTE)'\?') ? 2 : 1;
				if (!*pat)
				{
					keyLocations.push_back(firstMatch);
					continue;
				}
				pat++;
				if (!*pat)
				{
					keyLocations.push_back(firstMatch);
					continue;
				}
			}
			else if (firstMatch)
			{
				pCur = firstMatch;
				pat = reinterpret_cast<const unsigned char*>(pattern);
				firstMatch = 0;
			}
		}
		return keyLocations;
	}

  std::vector<std::string> FindAESKeys(char* file_buffer, uint64_t size)
  {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    // Scan for AES key locations using the most simple and universal sigs possible.
    auto aes_key_addresses1 = KeyScanner::Find("C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ?", (PBYTE)file_buffer, (PBYTE)file_buffer + size);
    auto aes_key_addresses2 = KeyScanner::Find("C7 ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ?", (PBYTE)file_buffer, (PBYTE)file_buffer + size);
    auto aes_key_addresses3 = KeyScanner::Find("C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ?", (PBYTE)file_buffer, (PBYTE)file_buffer + size);

    // AES String vectors.
    std::vector<std::string> KeyVector;

    // Start type 1
    for (size_t i = 0; i < aes_key_addresses1.size(); i++) // AES Key Signature Type 1
    {
      // Concat and push string to vector.
      KeyVector.push_back(DumpsterTools::ConcatenateAESType1(aes_key_addresses1, i));
    }

    // Start type 2
    for (size_t i = 0; i < aes_key_addresses2.size(); i++) // AES Key Signature Type 2
    {
      // Concat and push string to vector.
      KeyVector.push_back(DumpsterTools::ConcatenateAESType2(aes_key_addresses2, i));
    }

    // Start type 3
    for (size_t i = 0; i < aes_key_addresses3.size(); i++) // AES Key Signature Type 3
    {
      // Concat and push string to vector.
      KeyVector.push_back(DumpsterTools::ConcatenateAESType3(aes_key_addresses3, i));
    }

    return KeyVector;
  }

  std::vector<double> KeyEntropyGenerator(std::vector<std::string> KeyVector)
  {
    // AES Entropy vector.
    std::vector<double> KeyEntropyVector;

    for (size_t i = 0; i < KeyVector.size(); i++)
    {
      KeyEntropyVector.push_back(DumpsterTools::CalcEntropy(KeyVector[i]));
    }

    return KeyEntropyVector;
  }



}