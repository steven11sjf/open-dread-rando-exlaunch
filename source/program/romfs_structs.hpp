#pragma once

struct CClass;
struct CVariable;
struct CFunction;

struct TStringPoolEntry {
    char* string;
    uint index;
    int8_t poolIndex;
    int8_t used;
    int16_t _padding;
};

struct TStringInstance {
    TStringPoolEntry* entry;
    unsigned int uses;
    int _padding;
    char* string;
    int length;
    int _padding2;
    uint64_t allocator;
    long _unk;
    uint64_t crc64Hash;
    bool isEmpty;
    short _padding3;
    int8_t _padding4;
};

char* GetCStrId(TStringInstance*);

template<class T>
struct CTypedValue {
    T* value;
    CClass* reflection;
    uint heapId;
    uint unk0;
};

template<class T>
struct CRntVector {
    T* values;
    uint count;
    uint max_size;
    uint block_size;
    bool initialized;
    uint heapId;
    uint _unk;
    uint64_t allocator;
};

template<class K, class V>
struct CRntDictionary{
    V* values;
    K* keys;
    uint count;
    uint max_size;
    uint block_size;
    uint heapId;
    uint64_t allocator;
};

struct CType {
    long vtab;
    TStringInstance* className;
    uint64_t typeHash;
    uint64_t size;
    int unk0;
    int unk1;
    uint64_t funcCtor;
    uint64_t copyCtor;
    uint64_t moveCtor;
    uint64_t funcDtor;
    uint64_t copyDtor;
    uint64_t moveDtor;
    uint64_t funcCompare;
    uint64_t funcGetHashCode;
    uint64_t funcGetRefInfo;
    uint64_t membersFunction;
    CClass* parentClass;
    CRntDictionary<TStringInstance*, CTypedValue<uint64_t>> editor_fields;
    CClass* field_reflection;
    uint unk3;
    uint unk4;
    uint64_t unk2;
    CRntVector<CClass> childClasses;
    CRntDictionary<uint64_t, void*> dict1;
};

struct CClass {
    CType typeData;
    uint64_t fields_func;
    CRntDictionary<uint64_t, CVariable> vctVariables;
    CRntDictionary<TStringInstance*, CFunction> vctFunctions;
};

struct CVariable {
    TStringInstance* sName;
    uint offset;
    uint unk0;
    CClass* typeReflection;
    CFunction* getter;
    CType* getterType;
    CFunction* setter;
    CFunction* setterType;
    CRntDictionary<TStringInstance*, CTypedValue<uint64_t>> metadata;
    CClass* field_reflection;
    uint unk01;
    uint unk02;
    uint unk2;
    uint unk03;
    CRntDictionary<TStringInstance*, CTypedValue<void*>> dict2;
    ulong unk3;
    uint unk4;
    uint unk04;
};

struct CFunction {
    uint64_t vtable;
    CRntVector<CClass> params;
    TStringInstance* sName;
    CType* return_type;
    uint64_t caller_value;
    uint64_t func_ptr;
    CRntDictionary<TStringInstance*, CTypedValue<uint64_t>> dict1;
    uint64_t unk1;
    uint64_t unk2;
    CRntVector<CRntDictionary<TStringInstance*, CTypedValue<void*>>> params2_maybe;
    int8_t unk3;
    int8_t unk4;
    int8_t unk5;
    int8_t unk6;
    uint unk7;
};