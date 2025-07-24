// implements bindings to access reflection data for arbitrary classes via Lua

#include "bindings/reflection.hpp"
#include "bindings/types.hpp"
#include "lua-5.1.5/src/lua.hpp"

// Type hashes for the general types (CClass, CCollectionType, CEnumType, CFlagsetType, CPointerType)
const uint64_t BASE_REFLECTION_CCLASS = UINT64_C(6091256465011624050);
const uint64_t BASE_REFLECTION_CCOLLECTIONTYPE = UINT64_C(16882394163986259376);
const uint64_t BASE_REFLECTION_CENUMTYPE = UINT64_C(18199701582522205282);
const uint64_t BASE_REFLECTION_CFLAGSETTYPE = UINT64_C(14341023533376106569);
const uint64_t BASE_REFLECTION_CPOINTERTYPE = UINT64_C(7506927479173953530);

// type hash for char const*
const uint64_t CHAR_CONSTPTR = UINT64_C(6194730651124384674);
const uint64_t CRNTSTR = UINT64_C(8761614498263454944);
const uint64_t CSTRID = UINT64_C(13777909408246655734);

// config vals
const uint32_t MAX_CVARIABLES = 0x100;
const uint32_t MAX_CFUNCS = 0x100;

namespace TypeExporter {
    void ParseCType(lua_State* L, CType* cls);
    void ParseCType(lua_State *L);

    void ParseHashedClasses(lua_State* L, ReflectionManager* refmgr);
};