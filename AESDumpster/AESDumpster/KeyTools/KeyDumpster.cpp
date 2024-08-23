#include "KeyDumpster.h"

KeyDumpster::KeyDumpster()
{
#if defined _DEBUG
  std::wcout << "KeyDumpster Constructed." << std::endl;
#else
#endif
}

// Find patter in memory buffer.
std::vector<PBYTE> KeyDumpster::Find(const char* pattern, PBYTE rangeStart, PBYTE rangeEnd) {

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
std::string KeyDumpster::ConcatenateAESType(PBYTE keyAddr, int type) {

  std::string hex_string = "";
  for (size_t i = 0; i < m_keyDwordOffsets[type].size(); i++) {
    hex_string += hexStr(&keyAddr[m_keyDwordOffsets[type][i]], 4);
  }
  std::transform(hex_string.begin(), hex_string.end(), hex_string.begin(), ::toupper);
  return hex_string;
}

//Scan memory aes key buffer and extract the individual dword key buffers.
bool KeyDumpster::FindAESKeys(char* buffer, uint64_t size) {

  for (size_t i = 0; i < m_keyPatterns.size(); i++) {
    std::vector<PBYTE> local_aes_key_addresses = Find(m_keyPatterns[i].c_str(), (PBYTE)buffer, (PBYTE)buffer + size);
    for (size_t j = 0; j < local_aes_key_addresses.size(); j++) {
      // Concat and push key to vector.
      Key key = Key(ConcatenateAESType(local_aes_key_addresses[j], i));
      m_keys.m_keyvector.push_back(key);
    }
  }
  // Generate entropy score for each key.
  m_keyEntropies = KeyEntropyGenerator(m_keys);
  m_MostLikelyKey = FindMaxElements(m_keyEntropies).second;

  if (m_keyEntropies.empty() || m_MostLikelyKey.empty() || m_keys.getKeys().empty())
  {
    return false;
  }

  return true;
}

//Generate aes key entropy score.
std::vector<double> KeyDumpster::KeyEntropyGenerator(Keys keys) {

  std::vector<double> keyEntropies{};
  for (size_t i = 0; i < keys.getKeys().size(); i++) {
    keyEntropies.push_back(CalcEntropy(keys.getKeys()[i].getKey()));
  }
  return keyEntropies;
}

// print key information.
bool KeyDumpster::PrintKeyInformation() {

  HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
  for (size_t i = 0; i < m_keys.getKeys().size(); i++) {

    if (m_keyEntropies[i] >= 3.3 && m_keyEntropies[i] < 3.4) {
      SetConsoleTextAttribute(hConsole, 12);
    }
    else if (m_keyEntropies[i] >= 3.4 && m_keyEntropies[i] < 3.5) {
      SetConsoleTextAttribute(hConsole, 6);
    }
    else if (m_keyEntropies[i] >= 3.5 && m_keyEntropies[i] < 3.7) {
      SetConsoleTextAttribute(hConsole, 14);
    }
    else if (m_keyEntropies[i] >= 3.7) {
      SetConsoleTextAttribute(hConsole, 10);
    }

    for (size_t j = 0; j < m_MostLikelyKey.size(); j++) {
      if (i == m_MostLikelyKey[j]) {
        SetConsoleTextAttribute(hConsole, 10);
      }
    }
    if (m_keyEntropies[i] >= 3.3) {
      if (std::find(m_falsePositives.begin(), m_falsePositives.end(), m_keys.getKeys()[i].getKey()) == m_falsePositives.end()) {
        printf("Key: 0x%s | Key Entropy: %f\n\n", m_keys.getKeys()[i].getKey().c_str(), m_keyEntropies[i]);
      }
    }
    SetConsoleTextAttribute(hConsole, 15);
  }
  return true;
}

// log2 from: https://tfetimes.com/c-entropy/
double KeyDumpster::log2_intrinsic(double number) {
  return log(number) / log(2);
}

// hexStr from stackoverflow: https://stackoverflow.com/questions/14050452/how-to-convert-byte-array-to-hex-string-in-visual-c
std::string KeyDumpster::hexStr(const uint8_t* data, int len) {

  std::stringstream ss;
  ss << std::hex;

  for (int i(0); i < len; ++i)
    ss << std::setw(2) << std::setfill('0') << (int)data[i];

  return ss.str();
}

// Calculate entropy score on AES keys.
double KeyDumpster::CalcEntropy(std::string keyString) {

  std::map<char, int> frequencies;
  for (char c : keyString)
    frequencies[c]++;

  int numlen = keyString.length();
  double infocontent = 0;
  for (std::pair<char, int> p : frequencies) {
    double freq = static_cast<double>(p.second) / numlen;
    infocontent += freq * log2_intrinsic(freq);
  }
  infocontent *= -1;
  return infocontent;
}

// Find the biggest double value in a vector.
std::pair<double, std::vector<std::size_t>> KeyDumpster::FindMaxElements(std::vector<double> const& v) {

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

//Destructor.
KeyDumpster::~KeyDumpster() 
{
  m_keyEntropies.clear();
  m_MostLikelyKey.clear();

#if defined _DEBUG
  std::wcout << "KeyDumpster Destructed." << std::endl;
#else
#endif
}
