// contains bindings for the Mercury Engine type system (CType, CClass, CCollectionType, etc)
// 

#pragma once

#include "common_classes.hpp"

template<class T>
struct CTypedValue;
struct CVariable;
struct CFunction;

struct Conversion{
    uint64_t from;
    uint64_t to;
    uint64_t func;
};

struct CType {
    uint64_t vtab;
    CStrId className;
    uint64_t typeHash;
    size_t size;
    int32_t unk0;
    int32_t unk1;

    // this is allocated to type.size
    void (*funcCtor)(void*);
    // this is allocated to type.size, other is an initialized T
    void (*copyCtor)(void*, void*);
    // same as above, but other is destroyed
    void (*moveCtor)(void*, void*);
    // destroys initialized this
    void (*funcDtor)(void*);
    // destroys initialized this, copies other into this
    void (*copyDtor)(void*, void*);
    // same as above but other is destroyed
    void (*moveDtor)(void*, void*);

    // compares two instances
    bool (*funcCompare)(void*, void*);
    // unknown, usually returns empty crc32 or -1?
    int (*funcGetHashCode)(void*);
    // returns the CType
    CType* (*funcGetRefInfo)();

    uint64_t membersFunction;
    CType* parentClass;
    CRntDictionary<CStrId, CTypedValue<void*>>* metadata;
    uint64_t unk2;
    CRntVector<CType*> childClasses;
    CRntDictionary<uint64_t, Conversion*> conversions;
};

struct CClass {
    CType typeData;
    uint64_t fields_func;
    CRntDictionary<uint64_t, CVariable> vctVariables;
    CRntDictionary<CStrId, CFunction> vctFunctions;
};

struct CCollectionType {
    CType typeData;
    uint32_t unk0;
    uint32_t unk1;
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
    uint32_t enumValue;
    uint32_t _padding;
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
    int32_t offset;
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

struct ObjectType {
    void* object;
    CType* type;
};

struct TypeObject {
    CType* type;
    void* object;
};

namespace TypeFuncs {
    void* FuncCtor(CType* type);
    void* CopyCtor(CType* type, void* other);
    void* MoveCtor(CType* type, void* other);
    void FuncDtor(CType* type, void* obj);
    void CopyDtor(CType* type, void* obj, void* other);
    void MoveDtor(CType* type, void* obj, void* other);
};

template<class T>
struct CTypedValue {
    T* value;
    CType* ctype;
    uint32_t heapId;
    uint32_t unk0;
};