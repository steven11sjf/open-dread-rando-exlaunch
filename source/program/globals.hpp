// handles connecting to version-dependent offsets in exefs

#include "bindings/main_allocator.hpp"
#include "bindings/reflection.hpp"
#include "bindings/types.hpp"
#include "lua-5.1.5/src/lua.hpp"
#include <stdint.h>

typedef struct
{
    ptrdiff_t crc64;
    ptrdiff_t CFilePathStrIdCtor;
    ptrdiff_t luaRegisterGlobals;
    ptrdiff_t lua_pcall;
    ptrdiff_t reflectionMgr;
    ptrdiff_t CanCastTo;
    ptrdiff_t CastObject;
    ptrdiff_t GetClassFromHash;
    ptrdiff_t mainAllocator;
} FunctionOffsets;

namespace Globals {
    uint64_t CRC64 (char const *str, uint64_t size);
    int ExefsLuaPCall (lua_State *L, int nargs, int  nresults, int errfunc);
    bool CanCastTo(CType* from, CType* to);
    CType* ClassFromHash(uint64_t  hash);

    bool CastObject(CType* current, TypeObject* src, TypeObject* dst);

    ReflectionManager* GetReflectionManager();
    MainAllocator* GetMainAllocator();

    void SetValues(FunctionOffsets* funcs);
};