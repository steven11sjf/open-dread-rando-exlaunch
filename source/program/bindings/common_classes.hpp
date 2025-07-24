// this contains classes that are commonly used in other classes
// i.e. all the string types and collections

#pragma once

#include "main_allocator.hpp"

struct TStringPoolEntry {
    char* string;
    uint8_t index;
    uint8_t poolIndex;
    uint8_t used;
    int8_t _padding;
};

struct CRntString {
    char* string;
    int32_t length;
    int32_t _padding2;
    uint64_t allocator;
    uint64_t _unk;
    uint64_t crc64Hash;
    uint8_t isEmpty;
    int16_t _padding3;
    int8_t _padding4;
};

struct TStringInstance {
    TStringPoolEntry* entry;
    uint32_t uses;
    int32_t _padding;
    CRntString rnt_str;
};

struct CStrId {
    TStringInstance* inst;
};

template<class T>
struct CRntVector {
    T* values;
    uint32_t count;
    uint32_t max_size;
    uint32_t block_size;
    bool initialized;
    uint32_t heapId;
    uint32_t _unk;
    MainAllocator* allocator;
};

template<class K, class V>
struct CRntDictionary{
    V* values;
    K* keys;
    uint32_t count;
    uint32_t max_size;
    uint32_t block_size;
    uint32_t heapId;
    MainAllocator* allocator;
};

struct SDeathImpactDisplacement {
    float fInitialSpeed;
    float fMidSpeed;
    float fTimeToMidSpeed;
    float fEndSpeed;
};

struct CompleteInfo {
    uint32_t unk0;
    uint32_t unk1;
    CRntVector<CRntString> varNames;
};
