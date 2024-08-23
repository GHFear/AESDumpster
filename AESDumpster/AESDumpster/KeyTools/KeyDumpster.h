#pragma once
#include <iomanip>
#include <sstream>
#include <string>
#include <algorithm>
#include <map>
#include <iostream>
#include <fstream>
#include <cassert>
#include <sys/stat.h>
#include <Windows.h>
#include <Psapi.h>
#include <vector>

class Key
{
public:
  std::string m_key{};

  Key(std::string key = "") : m_key{ key } {}

  const std::string& getKey() const { return m_key; }
};

class Keys
{
public:
  std::vector<Key> m_keyvector{};

  Keys(){}
  ~Keys() { m_keyvector.clear(); }

  const std::vector<Key>& getKeys() const { return m_keyvector; }
};

class KeyDumpster
{
#define INRANGE(x,a,b)      (x >= a && x <= b) 
#define getBits(x)          (INRANGE(x,'0','9') ? (x - '0') : ((x&(~0x20)) - 'A' + 0xa))
#define getByte(x)          (getBits(x[0]) << 4 | getBits(x[1]))

public:
  // Constructor
  KeyDumpster();
  // AES String concatenator type.
  std::string ConcatenateAESType(PBYTE keyAddr, int type);
  // Find AES Keys in exe buffer.
  bool FindAESKeys(char* buffer, uint64_t size);
  // Generate entropy score for each key.
  std::vector<double> KeyEntropyGenerator(Keys keys);
  // Print Key information.
  bool PrintKeyInformation();

  //Other tools
  std::string hexStr(const uint8_t* data, int len);
  double CalcEntropy(std::string keyString);
  double log2_intrinsic(double number);
  std::pair<double, std::vector<std::size_t>> FindMaxElements(std::vector<double> const& v);

  // Destructor
  ~KeyDumpster();

private:
  // Find patter in memory buffer.
  std::vector<PBYTE> Find(const char* pattern, PBYTE rangeStart = NULL, PBYTE rangeEnd = NULL);
  // AES Binary Pattern Sigs
  const std::vector<std::string> m_keyPatterns = {
    "C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ?",
    "C7 ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ?",
    "C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? 48 ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ?",
    "C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? C3",
  };
  const std::vector<std::string> m_falsePositives = {
    "FFD9FFD9FFD9FFD9FFD9FFD9FFD9FFD9FFD9FFD9FFD9FFD9FFD9FFD9FFD9FFD9",
    "67E6096A85AE67BB72F36E3C3AF54FA57F520E518C68059BABD9831F19CDE05B",
    "D89E05C107D57C3617DD703039590EF7310BC0FF11155868A78FF964A44FFABE",
    "9A99593F9A99593F0AD7633F52B8BE3FE17A543FCDCC4C3D4260E53BAE47A13F",
    "6F168073B9B21449D742241700068ADABC306FA9AA3831164DEE8DE34E0EFBB0",
    "0AD7633FCDCC4C3DCDCCCC3D52B8BE3F9A99593F9A99593FC9767E3FE17A543F",
    "168073C7B21449C7430C00064310BC304314AA3843184DEE431C4E0E83C4205B",
    "E6096AC7AE67BBC7430C3AF543107F5243148C684318ABD9431C19CD436C2000",
    "9E05C1C7D57C36C7430C39594310310B431411154318A78F431CA44F436C1C00",
    "9E05C1C7D57C36C7DD7030C7590EF7C70BC0FFC7155868C78FF964C7A44FFABE",
    "168073C7B21449C7422417C7068ADAC7306FA9C7383116C7EE8DE3C74E0EFBB0",
    "0AD7633FCDCC4C3D00C742143DC742183FC7421C3FC742203FC742247E3FC742",
    "0000803F0AD7A33E0AD7633F52B8BE3FE17A543FCDCC4C3D4260E53B54AE47A1",
    "0AD7A33E0AD7633F52B8BE3FE17A543FCDCC4C3D4260E53BAE47A13F58583934",
    "0AD7A33E0AD7633F52B8BE3FE17A543FCDCC4C3D4260E53BAE47A13F38583934",
    "0000803F0AD7A33E0AD7633F52B8BE3FE17A543FCDCC4C3D4260E53B34AE47A1",
    "0000803F0000803F0AD7A33E0AD7633F52B8BE3FE17A543FCDCC4C3D2C4260E5",
    "0AD7633F52B8BE3FE17A543FCDCC4C3D4260E53BAE47A13F5839343C4CC9767E",
    "0AD7633F52B8BE3FE17A543FCDCC4C3D4260E53BAE47A13F5839343C4CC9767E",
    "07D57C3617DD703039590EF7310BC0FF11155868A78FF964A44FFABE6C1C0000",
    "85AE67BB72F36E3C3AF54FA57F520E518C68059BABD9831F19CDE05B6C200000",
    "E6096AC7AE67BBC7F36E3CC7F54FA5C7520E51C768059BC7D9831FC719CDE05B",
    "0AD7A33E0AD7633F52B8BE3FE17A543FCDCC4C3D4260E53BAE47A13F3C583934",
    "E4D6E74FE4D667500044AC47926595380080DC43000A9B46000080BF000080BF",
    "D04C8F7D71ECC047D8A60970FBA31C9E9EC1250BBBF6459AC480947212E1DB8C"
  };
  const std::vector<std::vector<int>> m_keyDwordOffsets = {
    { 3, 10, 17, 24, 35, 42, 49, 56 },
    { 2, 9, 16, 23, 30, 37, 44, 51 },
    { 3, 10, 21, 28, 35, 42, 49, 56 },
    { 51, 45, 38, 31, 24, 17, 10, 3 }
  };

public:
  Keys m_keys{};
  std::vector<double> m_keyEntropies{};
  std::vector<size_t> m_MostLikelyKey{};
  
};

