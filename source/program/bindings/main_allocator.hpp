// provides bindings to use Mercury Engine's allocator

#pragma once

#include <cstddef>
#include <cstdint>

struct MainAllocator;

struct MainAllocator_Vtab {
    void (*dtor)(MainAllocator*);
    void (*deletor)(MainAllocator*);
    void (*InitializeSubHeaps)(MainAllocator*, void *);
    void (*unk0x18)(MainAllocator*);
    void (*unk0x20)(MainAllocator*);
    void (*unk0x28)(MainAllocator*);
    void (*unk0x30)(MainAllocator*);
    void (*unk0x38)(MainAllocator*);
    void* (*AllocateOnHeap)(MainAllocator*, size_t, size_t, int);
    void (*OnPointerDeleted)(MainAllocator*, void *);
    void* (*GetMemoryBlock)(MainAllocator*, void *);
    bool (*IsPointerManaged)(MainAllocator*, void *);
};

struct MainAllocator {
    MainAllocator_Vtab *vtab;
};

namespace Allocator{
    void* AllocateOnHeap(size_t totalSize, size_t entrySize = 8, uint32_t heapId = 2, MainAllocator *allocator = NULL);
    void OnPointerDeleted(void* ptr, MainAllocator *allocator = NULL);
}