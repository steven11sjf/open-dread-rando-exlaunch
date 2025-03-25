#include <lib.hpp>

#include "romfs_structs.hpp"
#include "lib/util/modules.hpp"

char* GetCStrId(TStringInstance* inst) {
    return inst->string;
}

inline uintptr_t GetTargetOffsetReverse(uintptr_t offset) {
    return offset - exl::util::GetMainModuleInfo().m_Total.m_Start;
}