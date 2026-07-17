#include "rbx.hpp"

#include "lib.hpp"
#include "dread_types.hpp"
#include "lib/util/modules.hpp"




// hardcoded 2.1.0 romfs bc lazy
CClass* (*BoolInit) () = (CClass*(*)())exl::util::modules::GetTargetOffset(0xd228);
CClass* (*FloatInit) () = (CClass*(*)())exl::util::modules::GetTargetOffset(0xabbc);
void* (*RegisterVariable) (CClass* cls, CStrId *fieldName, CClass *type, uint32_t offset, int64_t getter, int64_t setter) = (void*(*)(CClass*, CStrId*, CClass*, uint32_t, int64_t, int64_t))exl::util::modules::GetTargetOffset(0x96524);
void (*InitializeCStrId) (CStrId *self, const char* str, bool storeInPool) = (void(*)(CStrId*, const char*, bool))exl::util::modules::GetTargetOffset(0x3d4);
void (*ReleaseCStrId) (SStringPool *self, CStringInstance *str) = (void(*)(SStringPool*, CStringInstance*))exl::util::modules::GetTargetOffset(0x80384);

SStringPool* stringPool = *(SStringPool**)exl::util::modules::GetTargetOffset(0x1d47108);


/* Hook RBX fields. Run this after  */
/* Once romfs is mounted, we can read files from it in order to populate our string replacement list. */
HOOK_DEFINE_TRAMPOLINE(RBXFields) {
    static void Callback(CClass *cls) {
        CStrId varName;
        
        Orig(cls);

        InitializeCStrId(&varName, "fTimeDamaged", true);
        RegisterVariable(cls, &varName, FloatInit(), 0x26c, -1, -1);
        if (stringPool != NULL)
        {
            if (varName.value != NULL && varName.value != &stringPool->nullStr)
            {
                varName.value->uses -= 1;
                if (varName.value->uses == 0)
                {
                    ReleaseCStrId(stringPool, varName.value);
                }
            }
        }
        
        InitializeCStrId(&varName, "bActiveDamage", true);
        RegisterVariable(cls, &varName, BoolInit(), 0x268, -1, -1);
        if (stringPool != NULL)
        {
            if (varName.value != NULL && varName.value != &stringPool->nullStr)
            {
                varName.value->uses -= 1;
                if (varName.value->uses == 0)
                {
                    ReleaseCStrId(stringPool, varName.value);
                }
            }
        }

        return;
    }
};

void odr::rbx::Install()
{
    RBXFields::InstallAtOffset(0x8bd1e8);
}