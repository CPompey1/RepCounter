#define LFS_MBED_RP2040_VERSION_MIN_TARGET      "LittleFS_Mbed_RP2040 v1.1.0"
#define LFS_MBED_RP2040_VERSION_MIN             1001000

#define _LFS_LOGLEVEL_          1
#define RP2040_FS_SIZE_KB       64

#define FORCE_REFORMAT          false

#include <LittleFS_Mbed_RP2040.h>

LittleFS_MBED *myFS;
void readCharsFromFile(const char * path);
void readFile(const char * path);
void writeFile(const char * path, const char * message, size_t messageSize);
void appendFile(const char * path, const char * message, size_t messageSize);
void deleteFile(const char * path);
void renameFile(const char * path1, const char * path2);
void testFileIO(const char * path);
void printLine();