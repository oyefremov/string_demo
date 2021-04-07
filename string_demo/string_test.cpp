#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "string_api.h"
#include "simple_string.h"
#include "sso_string.h"

#include "test_allocator.h"

using string = sso::string;
//using string = std::string;

#ifdef DEBUG
#define SKIP_ALLOCATIONS_TEST 0
#else
#define SKIP_ALLOCATIONS_TEST 0
#endif // DEBUG

// test for short string optimization
const bool has_sso = string().capacity() > 0;

TEST(string, default_constructor) {
    string s;
    EXPECT_EQ(s.size(), 0u);
    EXPECT_STREQ(s.c_str(), "");
    EXPECT_EQ(s.begin(), s.end());
}

TEST(string, c_string_constructor) {
    string empty_string("");
    string short_string("short string");
    string long_string("loooooooooooooooooooooong string");
    if (has_sso) {
        EXPECT_GE(empty_string.capacity(), short_string.size());
        EXPECT_LT(empty_string.capacity(), long_string.size());
        EXPECT_EQ(empty_string.capacity(), short_string.capacity());
        EXPECT_GT(long_string.capacity(), empty_string.capacity());
    }

    EXPECT_STREQ(short_string.c_str(), "short string");
    EXPECT_STREQ(long_string.c_str(), "loooooooooooooooooooooong string");
    EXPECT_EQ(short_string.size(), 12);
    EXPECT_EQ(short_string.end() - short_string.begin(), 12);
    EXPECT_EQ(long_string.size(), 32);
    EXPECT_EQ(long_string.end() - long_string.begin(), 32);
}

TEST(string, copy_constructor) {
    string empty_string;
    string short_string("short string");
    string long_string("loooooooooooooooooooooong string");

    string empty_string_copy(empty_string);
    string short_string_copy(short_string);
    string long_string_copy(long_string);

    EXPECT_STREQ(empty_string.c_str(), empty_string_copy.c_str());
    EXPECT_STREQ(short_string.c_str(), short_string_copy.c_str());
    EXPECT_STREQ(long_string.c_str(), long_string_copy.c_str());
    EXPECT_EQ(empty_string.size(), empty_string_copy.size());
    EXPECT_EQ(short_string.size(), short_string_copy.size());
    EXPECT_EQ(long_string.size(), long_string_copy.size());
}

TEST(string, move_constructor) {
    string empty_string;
    string short_string("short string");
    string long_string("loooooooooooooooooooooong string");

    string empty_string_copy(std::move(empty_string));
    string short_string_copy(std::move(short_string));
    string long_string_copy(std::move(long_string));

    EXPECT_STREQ(empty_string.c_str(), "");
    EXPECT_STREQ(long_string.c_str(), "");
    EXPECT_STREQ(empty_string_copy.c_str(), "");
    EXPECT_STREQ(short_string_copy.c_str(), "short string");
    EXPECT_STREQ(long_string_copy.c_str(), "loooooooooooooooooooooong string");
    EXPECT_EQ(empty_string.size(), 0u);
    EXPECT_EQ(long_string.size(), 0u);
    EXPECT_EQ(empty_string_copy.size(), 0u);
    EXPECT_EQ(short_string_copy.size(), 12u);
    EXPECT_EQ(long_string_copy.size(), 32u);
    EXPECT_EQ(short_string.capacity(), empty_string.capacity());
    EXPECT_EQ(long_string.capacity(), empty_string.capacity());
    if (has_sso) {
        EXPECT_EQ(empty_string_copy.capacity(), empty_string.capacity());
        EXPECT_EQ(short_string_copy.capacity(), empty_string.capacity());
        EXPECT_GT(long_string_copy.capacity(), empty_string.capacity());
    }
}

TEST(string, copy_assignment) {
    string empty_string;
    string short_string("short string");
    string long_string("loooooooooooooooooooooong string");

    string empty_string_copy;
    string short_string_copy;
    string long_string_copy;

    empty_string_copy = empty_string;
    short_string_copy = short_string;
    long_string_copy = long_string;

    EXPECT_STREQ(empty_string.c_str(), empty_string_copy.c_str());
    EXPECT_STREQ(short_string.c_str(), short_string_copy.c_str());
    EXPECT_STREQ(long_string.c_str(), long_string_copy.c_str());
    EXPECT_EQ(empty_string.size(), empty_string_copy.size());
    EXPECT_EQ(short_string.size(), short_string_copy.size());
    EXPECT_EQ(long_string.size(), long_string_copy.size());

    // make string smaller
    long_string_copy = short_string;
    EXPECT_STREQ(long_string_copy.c_str(), short_string.c_str());
    EXPECT_EQ(long_string_copy.size(), short_string.size());
    EXPECT_EQ(long_string_copy.capacity(), long_string.capacity());

    // make string empty
    long_string_copy = empty_string;
    EXPECT_STREQ(long_string_copy.c_str(), empty_string.c_str());
    EXPECT_EQ(long_string_copy.size(), empty_string.size());
    EXPECT_EQ(long_string_copy.capacity(), long_string.capacity());
}

TEST(string, move_assignment) {
    string empty_string;
    string short_string("short string");
    string long_string("loooooooooooooooooooooong string");

    string empty_string_copy;
    string short_string_copy;
    string long_string_copy;

    empty_string_copy = std::move(empty_string);
    short_string_copy = std::move(short_string);
    long_string_copy = std::move(long_string);

    EXPECT_STREQ(empty_string.c_str(), "");
    EXPECT_STREQ(long_string.c_str(), "");
    EXPECT_STREQ(empty_string_copy.c_str(), "");
    EXPECT_STREQ(short_string_copy.c_str(), "short string");
    EXPECT_STREQ(long_string_copy.c_str(), "loooooooooooooooooooooong string");
    EXPECT_EQ(empty_string.size(), 0u);
    EXPECT_EQ(long_string.size(), 0u);
    EXPECT_EQ(empty_string_copy.size(), 0u);
    EXPECT_EQ(short_string_copy.size(), 12u);
    EXPECT_EQ(long_string_copy.size(), 32u);
    EXPECT_EQ(short_string.capacity(), empty_string.capacity());
    EXPECT_EQ(long_string.capacity(), empty_string.capacity());
    if (has_sso) {
        EXPECT_EQ(empty_string_copy.capacity(), empty_string.capacity());
        EXPECT_EQ(short_string_copy.capacity(), empty_string.capacity());
        EXPECT_GT(long_string_copy.capacity(), empty_string.capacity());
    }

    // make string smaller
    long_string_copy = string("short string");
    EXPECT_STREQ(long_string_copy.c_str(), "short string");
    EXPECT_EQ(long_string_copy.size(), 12u);
    if (has_sso) {
        EXPECT_EQ(long_string_copy.capacity(), empty_string.capacity());
    }

    // make string empty
    long_string_copy = string();
    EXPECT_STREQ(long_string_copy.c_str(), "");
    EXPECT_EQ(long_string_copy.size(), 0u);
    EXPECT_EQ(long_string_copy.capacity(), empty_string.capacity());
}

TEST(string, default_string_allocations) {
    if (SKIP_ALLOCATIONS_TEST) return;

    allocations_recorder memory;
    {
        string empty_string;
    }
    memory.stop();
    EXPECT_EQ(memory.total_allocations(), 0u);
    EXPECT_EQ(memory.total_used_memory(), 0);
    EXPECT_EQ(memory.active_allocations(), 0u);
    EXPECT_EQ(memory.active_used_memory(), 0u);
}

TEST(string, short_string_allocations) {
    if (SKIP_ALLOCATIONS_TEST || !has_sso) return;

    allocations_recorder memory;
    {
        string short_string("short string");
    }
    memory.stop();
    EXPECT_EQ(memory.total_allocations(), 0u);
    EXPECT_EQ(memory.total_used_memory(), 0);
    EXPECT_EQ(memory.active_allocations(), 0u);
    EXPECT_EQ(memory.active_used_memory(), 0u);
}

TEST(string, long_string_allocations) {
    if (SKIP_ALLOCATIONS_TEST) return;

    allocations_recorder memory;
    size_t used_capacity;
    {
        string long_string("loooooooooooooooooooooong string");
        used_capacity = long_string.capacity();
    }
    memory.stop();
    EXPECT_EQ(memory.total_allocations(), 1u);
    EXPECT_EQ(memory.total_used_memory(), used_capacity + 1);
    EXPECT_EQ(memory.active_allocations(), 0u);
    EXPECT_EQ(memory.active_used_memory(), 0u);
}

TEST(string, copy_constructor_allocations_no_sso) {
    if (SKIP_ALLOCATIONS_TEST) return;

    allocations_recorder memory;
    size_t used_capacity;
    {
        string empty_string;
        string long_string("loooooooooooooooooooooong string");

        string empty_string_copy(empty_string);
        string long_string_copy(long_string);

        used_capacity = long_string_copy.capacity();
    }
    memory.stop();
    EXPECT_EQ(memory.total_allocations(), 2u);
    EXPECT_EQ(memory.total_used_memory(), 2 * (used_capacity + 1));
    EXPECT_EQ(memory.active_allocations(), 0u);
    EXPECT_EQ(memory.active_used_memory(), 0u);
}

TEST(string, copy_constructor_allocations) {
    if (SKIP_ALLOCATIONS_TEST || !has_sso) return;

    allocations_recorder memory;
    size_t used_capacity;
    {
        string empty_string;
        string short_string("short string");
        string long_string("loooooooooooooooooooooong string");

        string empty_string_copy(empty_string);
        string short_string_copy(short_string);
        string long_string_copy(long_string);

        used_capacity = long_string_copy.capacity();
    }
    memory.stop();
    EXPECT_EQ(memory.total_allocations(), 2u);
    EXPECT_EQ(memory.total_used_memory(), 2 * (used_capacity + 1));
    EXPECT_EQ(memory.active_allocations(), 0u);
    EXPECT_EQ(memory.active_used_memory(), 0u);
}

TEST(string, move_constructor_allocations_no_sso) {
    if (SKIP_ALLOCATIONS_TEST) return;

    allocations_recorder memory;
    size_t used_capacity;
    {
        string empty_string;
        string long_string("loooooooooooooooooooooong string");

        string empty_string_copy(std::move(empty_string));
        string long_string_copy(std::move(long_string));

        used_capacity = long_string_copy.capacity();
    }
    memory.stop();
    EXPECT_EQ(memory.total_allocations(), 1u);
    EXPECT_EQ(memory.total_used_memory(), used_capacity + 1u);
    EXPECT_EQ(memory.active_allocations(), 0u);
    EXPECT_EQ(memory.active_used_memory(), 0u);
}

TEST(string, move_constructor_allocations) {
    if (SKIP_ALLOCATIONS_TEST || !has_sso) return;

    allocations_recorder memory;
    size_t used_capacity;
    {
        string empty_string;
        string short_string("short string");
        string long_string("loooooooooooooooooooooong string");

        string empty_string_copy(std::move(empty_string));
        string short_string_copy(std::move(short_string));
        string long_string_copy(std::move(long_string));

        used_capacity = long_string_copy.capacity();
    }
    memory.stop();
    EXPECT_EQ(memory.total_allocations(), 1u);
    EXPECT_EQ(memory.total_used_memory(), used_capacity + 1u);
    EXPECT_EQ(memory.active_allocations(), 0u);
    EXPECT_EQ(memory.active_used_memory(), 0u);
}

TEST(string, insert) {
    string str;
    str.insert(0, "");
    EXPECT_STREQ(str.c_str(), "");
    EXPECT_EQ(str.size(), 0u);
    EXPECT_EQ(str.capacity(), string().capacity());
    str.insert(0, "a");
    EXPECT_STREQ(str.c_str(), "a");
    str.insert(0, "abc");
    EXPECT_STREQ(str.c_str(), "abca");
    str.insert(4, "123");
    EXPECT_STREQ(str.c_str(), "abca123");
    EXPECT_EQ(str.size(), 7u);
    EXPECT_LT(str.size(), str.capacity());
    str.insert(0, str.capacity() - str.size(), 'x');
    EXPECT_EQ(str.size(), str.capacity());
    str.insert(0, "y");
    EXPECT_LT(str.size(), str.capacity());
}

TEST(string, insert_self) {
    string str("01234");
    str.reserve(16);
    str.insert(3, str.c_str() + 1);
    EXPECT_STREQ(str.c_str(), "012123434");
}

TEST(string, insert_self_2) {
    string str("01234");
    str.reserve(16);
    str.insert(1, str.c_str() + 3);
    EXPECT_STREQ(str.c_str(), "0341234");
}

TEST(string, insert_allocations) {
    if (SKIP_ALLOCATIONS_TEST) return;

    allocations_recorder memory;
    auto count = string().capacity() + 5;
    size_t used_capacity;
    {
        string str;
        for (size_t i = 0; i < count; ++i) {
            str.insert(i, "a");
        }
        used_capacity = str.capacity();
    }
    memory.stop();
    EXPECT_EQ(memory.total_allocations(), 1u);
    EXPECT_EQ(memory.total_used_memory(), used_capacity + 1u);
    EXPECT_EQ(memory.active_allocations(), 0u);
    EXPECT_EQ(memory.active_used_memory(), 0u);
}
#if 0
TEST(sso4_string, small_buffer_size_22) {
    if (!has_sso) return;

    sso4::string str22("1234567890123456789012");
    EXPECT_EQ(str22.size(), 22);
    EXPECT_EQ(str22.capacity(), string().capacity());
    EXPECT_STREQ(str22.c_str(), "1234567890123456789012");
}

TEST(sso4_string, small_buffer_size_23) {
    if (!has_sso) return;

    sso4::string str23("12345678901234567890123");
    EXPECT_EQ(str23.size(), 23);
    EXPECT_EQ(str23.capacity(), string().capacity());
    EXPECT_STREQ(str23.c_str(), "12345678901234567890123");
}

TEST(sso3_string, test_heap_flag){
    sso3::small_string_data data;
    auto ptr = reinterpret_cast<sso3::heap_string_data*>(&data);
    
    data.set_size_and_reset_heap_flag(7);
    EXPECT_FALSE(data.use_heap());
    EXPECT_FALSE(ptr->use_heap());
    EXPECT_EQ(data.size(), 7);    
    ptr->set_capacity_and_heap_flag(1'000'000);
    EXPECT_TRUE(data.use_heap());
    EXPECT_TRUE(ptr->use_heap());
    EXPECT_EQ(ptr->capacity(), 1'000'000);    
}

TEST(sso4_string, test_heap_flag){
    sso4::small_string_data data;
    auto ptr = reinterpret_cast<sso4::heap_string_data*>(&data);
    
    data.set_size_and_reset_heap_flag(7);
    EXPECT_FALSE(data.use_heap());
    EXPECT_FALSE(ptr->use_heap());
    EXPECT_EQ(data.size(), 7);    
    ptr->set_capacity_and_heap_flag(1'000'001);
    EXPECT_TRUE(data.use_heap());
    EXPECT_TRUE(ptr->use_heap());
    EXPECT_EQ(ptr->capacity(), 1'000'001);    
}
#endif