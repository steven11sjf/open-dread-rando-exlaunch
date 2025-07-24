#include "globals.hpp"
#include <stdint.h>
#include "lib/util/modules.hpp"
#include <nn.hpp>
#include <cstring>

// =======
// Raw pointers
// =======


/* Function ptr to dread's crc function. */
uint64_t (*crc64)(char const *str, uint64_t size) = NULL;

/* The main executable's pcall, so we get proper error handling. */
int (*exefs_lua_pcall) (lua_State *L, int nargs, int nresults, int errfunc) = NULL;

/* Returns if you can cast from one type to the other safely. */
bool (*can_cast_to) (CType* from, CType* to) = NULL;

/* Casts from  CType  F to CType T, F == current */
bool (*cast_object) (CType* current, TypeObject* src, TypeObject* dst) = NULL;

/* Offset of the hashed types structure */
ReflectionManager **reflection_mgr = NULL;

/* Offset of mainallocator instance */
MainAllocator **main_allocator = NULL;

CType* (*class_from_hash) (ReflectionManager* rm, uint64_t* hash) = NULL;

// =====
// header bindings
// =====
namespace Globals {
    uint64_t CRC64(char const* str,  uint64_t size) {
        return crc64(str, size);
    }

    int ExefsLuaPCall (lua_State *L, int nargs, int nresults, int errfunc)  {
        return exefs_lua_pcall(L,  nargs, nresults, errfunc);
    }

    bool CanCastTo(CType* from, CType* to) {
        return can_cast_to(from, to);
    }

    bool CastObject(CType* current, TypeObject* src, TypeObject* dst)  {
        return cast_object(current, src, dst);

    }

    CType* ClassFromHash(uint64_t hash)  {
        // return (uint64_t)reflection_mgr - exl::util::modules::GetTargetOffset(0);
        return class_from_hash(GetReflectionManager(), &hash);
    }

    ReflectionManager* GetReflectionManager() {
        return *reflection_mgr;
    }

    MainAllocator* GetMainAllocator() {
        return *main_allocator;
    }

    void SetValues(FunctionOffsets* offsets) {
        crc64 = (uint64_t (*)(char const *, uint64_t))exl::util::modules::GetTargetOffset(offsets->crc64);
        exefs_lua_pcall = (int (*) (lua_State *L, int nargs, int nresults, int errfunc)) exl::util::modules::GetTargetOffset(offsets->lua_pcall);
        can_cast_to = (bool (*)(CType*, CType*))exl::util::modules::GetTargetOffset(offsets->CanCastTo);
        cast_object = (bool (*)(CType*, TypeObject*, TypeObject*))
            exl::util::modules::GetTargetOffset(offsets->CastObject);
        class_from_hash = (CType* (*) (ReflectionManager*, uint64_t*))exl::util::modules::GetTargetOffset(offsets->GetClassFromHash);

        reflection_mgr = (ReflectionManager**)exl::util::modules::GetTargetOffset(offsets->reflectionMgr);
        main_allocator = (MainAllocator**)exl::util::modules::GetTargetOffset(offsets->mainAllocator);
    }
}