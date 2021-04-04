#pragma once

namespace api_only {
    class string {
    public:
        // default constructed
        string() noexcept;
        // construct from c-string
        string(const char* str);

        // rule of five
        string(const string& other);
        string(string&& other) noexcept;
        string& operator=(const string& other);
        string& operator=(string&& other) noexcept;
        ~string() noexcept;

        // useful and interesting
        void swap(string& other) noexcept;

        // iterators
        char* begin() noexcept;
        char* end() noexcept;

        // some modifications to have fun
        void insert(size_t index, size_t count, char ch);
        void insert(size_t index, const char* str);

        // for printing
        const char* c_str() const noexcept;

        size_t size() const noexcept;
        void resize(size_t new_size, char ch = 0);

        size_t capacity() const noexcept;
        void reserve(size_t new_capacity);
    };
}