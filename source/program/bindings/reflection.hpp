// defines Reflection-specific structures and functions,
// mainly access to ReflectionManager

#pragma once

#include "types.hpp"

struct CType;
struct CClass;
struct CVariable;
struct CFunction;

struct HashedType {
    CStrId typeName;
    CType* type;
    HashedType* next;
};

struct ReflectionManager {
    uintptr_t vtab;
    CRntVector<HashedType*> types;
};

struct ReflectionManagerPtr {
    ReflectionManager* inner;
};

namespace Types {
    CType* GetType(uint64_t hash);
    CType* GetType(char const* name);
    TypeObject* CastTo(TypeObject* object, CType* type);   
};