# AESDumpster
AES Key dumper written in C++.<br>
Works for Unreal Engine 4.19 -> 5.4 Windows exes.

## Linux Version:
[Link](https://github.com/GHFear/AESDumpster-Linux)

## Change Log:

**1.2.5:** <br>
-Rewrote source code to make the dumpster its own object anyone can use. <br>
-Improved key signatures. (tool should now work for more games.) <br>
-Increased the scanner speed. <br>

**1.2.3:** <br>
-Fixed AES key type 4 support. (Quidditch Champions) <br>

**1.2.2:** <br>
-Added support for the type of key that can be found in QuidditchChampions-Win64-Shipping.exe <br>
-Improved code. <br>
-Fixed a bunch of bugs. <br>
-I no longer show keys with an entropy score below 3.3 because it's not possible for an AES key to have a score that low. <br>
-I added a list of known false positives and exclude them from the results. (Will keep improving this list) <br>

**1.2:**  <br>
-Updated source code. <br>
-Improved speed. <br>
-Improved the results. (I now sort out all results that can't possibly be the key) <br>

**1.1:** (source not yet updated to 1.1) <br>
-Fixed the type 2 AES keys scanner. <br>
-Fixed the print output so it says "0x" in front of the keys. <br>

**1.0:** <br>
-First version. Please give me feedback. <br>
