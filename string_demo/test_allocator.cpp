//#include <cstdio>
//#include <cstdlib>
#include <new>
#include <map>

#include "test_allocator.h"

namespace {
    bool g_is_test_allocator_enabled = false;
    size_t g_total_allocations = 0;
    size_t g_total_used_memory = 0;
    size_t g_active_allocations = 0;
    size_t g_active_used_memory = 0;
    std::map<void*, size_t> g_allocation_sizes;
}

void* operator new(std::size_t sz) // no inline, required by [replacement.functions]/3
{
    // avoid std::malloc(0) which may return nullptr on success
    if (sz == 0) {
        sz = 1;
    }
    void* ptr = std::malloc(sz);
    if (!ptr) {
        throw std::bad_alloc{};
    }
    if (g_is_test_allocator_enabled) {
        g_active_allocations++;
        g_total_allocations++;
        g_active_used_memory += sz;
        g_total_used_memory += sz;

        g_is_test_allocator_enabled = false;
        g_allocation_sizes[ptr] = sz;
        g_is_test_allocator_enabled = true;
    }
    return ptr;
}
void operator delete(void* ptr) noexcept
{
    if (!ptr) return;

    if (g_is_test_allocator_enabled) {
        g_active_allocations--;

        auto size_ptr = g_allocation_sizes.find(ptr);
        if (size_ptr != g_allocation_sizes.end()) {
            g_active_used_memory -= size_ptr->second;
            g_is_test_allocator_enabled = false;
            g_allocation_sizes.erase(size_ptr);
            g_is_test_allocator_enabled = true;
        }
    }
    std::free(ptr);
}
namespace test_allocator {
    void enable_test_allocator() {
        g_is_test_allocator_enabled = true;
    }
    void disable_test_allocator() {
        g_is_test_allocator_enabled = false;
    }
    void clear_recorded_data() {
        g_total_allocations = 0;
        g_total_used_memory = 0;
        g_active_allocations = 0;
        g_active_used_memory = 0;
    }
    size_t total_allocations() {
        return g_total_allocations;
    }
    size_t total_used_memory() {
        return g_total_used_memory;
    }
    size_t active_allocations() {
        return g_active_allocations;
    }
    size_t active_used_memory() {
        return g_active_used_memory;
    }
};
