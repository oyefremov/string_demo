#pragma once

namespace test_allocator {
    void enable_test_allocator();
    void disable_test_allocator();
    void clear_recorded_data();
    size_t total_allocations();
    size_t total_used_memory();
    size_t active_allocations();
    size_t active_used_memory();
};

struct allocations_recorder {
    allocations_recorder() {
        test_allocator::clear_recorded_data();
        test_allocator::enable_test_allocator();
    }
    ~allocations_recorder() { stop(); }
    void stop() { test_allocator::disable_test_allocator(); }
    size_t total_allocations() { return test_allocator::total_allocations(); }
    size_t total_used_memory() { return test_allocator::total_used_memory(); }
    size_t active_allocations() { return test_allocator::active_allocations(); }
    size_t active_used_memory() { return test_allocator::active_used_memory(); }
};
