//
// Created by x1y2z on 14.04.2023.
//

#pragma once


#include <cstdint>
#include <cstdio>

// For backward compatibility

constexpr size_t DIRNAMELENGTH = 128;
typedef FILE* FILESTREAM;

class CFileMgr {
public:
    static inline char ms_path[512];
    static char (&ms_dirName)[128];
    static char (&ms_rootDirName)[128];

    static void Initialise();
    static int32_t ChangeDir(const char* path);
    static void SetDir(const char* path);
    static int32_t SetDirMyDocuments();
    static size_t LoadFile(const char* path, uint8_t* buf, size_t size, const char* mode);
    static FILE* OpenFile(const char* path, const char* mode);
    static FILE* OpenFileForWriting(const char* path);
    static FILE* OpenFileForAppending(const char* path);
    static size_t Read(FILE* file, void* buf, size_t size);
    static size_t Write(FILE* file, const void* buf, size_t size);
    static bool Seek(FILE* file, long offset, int32_t origin);
    static bool ReadLine(FILE* file, char* str, int32_t num);
    static int32_t CloseFile(FILE* file);
    static int32_t GetTotalSize(FILE* file);
    static int32_t Tell(FILE* file);
    static bool GetErrorReadWrite(FILE* file);

    //! Increment the file's seek pointer until after the next new line (`\n`) (make sure file is open in non-binary mode!)
    static void SeekNextLine(FILE* file);
};

