#pragma once

#include "lua-5.1.5/src/lua.hpp"

struct CType;
struct CClass;
struct CVariable;
struct CFunction;

struct TStringPoolEntry {
    char* string;
    uint8_t index;
    uint8_t poolIndex;
    uint8_t used;
    int16_t _padding;
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

const char* GetCStrId(CStrId);

template<class T>
struct CTypedValue {
    T* value;
    CType* reflection;
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
    uint64_t vtab;
    CStrId className;
    uint64_t typeHash;
    uint64_t size;
    int32_t unk0;
    int32_t unk1;
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
    CType* parentClass;
    CRntDictionary<CStrId, CTypedValue<void*>>* metadata;
    uint64_t unk2;
    CRntVector<CType*> childClasses;
    CRntDictionary<uint64_t, void*> dict1;
};

struct CClass {
    CType typeData;
    uint64_t fields_func;
    CRntDictionary<uint64_t, CVariable> vctVariables;
    CRntDictionary<CStrId, CFunction> vctFunctions;
};

struct CCollectionType {
    CType typeData;
    uint unk0;
    uint unk1;
    CType* keyType;
    CType* valType;
    uint64_t unk2;
    uint64_t unk3;
    uint64_t funcGetLength;
    uint64_t funcClearMembers;
    uint64_t funcInsertCtor;
    uint64_t funcInsertDtor;
    uint64_t funcRemoveAt;
    uint64_t funcGetElement;
    uint64_t funcGetWithBoundCheck;
    uint64_t funcGetCreateElCopy;
    uint64_t funcGetCreateElDtor;
    uint64_t unk4;
    uint64_t funcGetNextEmpty;
    uint64_t unk6;
    uint64_t unk7;
};

struct EnumValue {
    CStrId enumName;
    uint enumValue;
    uint _padding;
};

struct CEnumType {
    CType typeData;
    CType* item_type;
    uint64_t valuesCtor;
    CRntVector<EnumValue> values;
};

struct CFlagsetType {
    CType typeData;
    CType* itemType;
    CEnumType* flagset;
};

struct CPointerType {
    CType typeData;
    uint64_t unk0;
    CType* pointingType;
    uint64_t fun1;
    uint64_t fun2;
    uint64_t fun3;
};

struct CVariable {
    CStrId sName;
    uint32_t offset;
    uint32_t unk0;
    CClass* typeReflection;
    CFunction* getter;
    CType* getterType;
    CFunction* setter;
    CType* setterType;
    CRntDictionary<CStrId, CTypedValue<void*>> metadata;
    CClass* field_reflection;
    uint32_t unk01;
    uint32_t unk02;
    uint32_t unk2;
    uint32_t unk03;
    CRntDictionary<CStrId, CTypedValue<void*>> dict2;
    uint64_t unk3;
    uint32_t unk4;
    uint32_t unk04;
};

struct CFunction {
    uint64_t vtable;
    CRntVector<CClass*> params;
    CStrId sName;
    CType* return_type;
    uint64_t caller_value;
    uint64_t func_ptr;
    CRntDictionary<CStrId, CTypedValue<void*>> metadata;
    uint64_t unk1;
    uint64_t unk2;
    CRntVector<CRntDictionary<CStrId, CTypedValue<void*>>> params2_maybe;
    int8_t unk3;
    int8_t unk4;
    int8_t unk5;
    int8_t unk6;
    uint32_t unk7;
};

struct CompleteInfo {
    uint64_t unk0;
    CRntVector<CRntString> varNames;
};

const char* ClassInfoString(lua_State* L, CClass* cls);

void ParseCType(lua_State* L, CType* cls);