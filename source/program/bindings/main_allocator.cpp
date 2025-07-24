#include "bindings/main_allocator.hpp"
#include "globals.hpp"

namespace Allocator {
    void* AllocateOnHeap(size_t totalSize, size_t entrySize /* = 8 */, uint32_t heapId /* = 2 */, MainAllocator *allocator /* = NULL */) {
        if (!allocator) {
            allocator = Globals::GetMainAllocator();
        }
        return allocator->vtab->AllocateOnHeap(allocator, totalSize, entrySize, heapId);
    }

    void OnPointerDeleted(void* ptr, MainAllocator *allocator /* = NULL */) {
        if (!allocator) {
            allocator = Globals::GetMainAllocator();
        }
        return allocator->vtab->OnPointerDeleted(allocator, ptr);

    }
}