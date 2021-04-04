#pragma once

namespace v1 {

    class string {
    public:
        // rule of five
        string() noexcept;
        string(const string& other);
        string(string&& other) noexcept;
        string& operator=(const string& other);
        string& operator=(string&& other) noexcept;
        ~string() noexcept;

        void swap(string&& other) noexcept;

        // construct from c-string
        string(const char* str);

        // modifications
        void append(const char* str);

        // for printing
        const char* c_str() const noexcept;

        size_t size() const noexcept;
        void resize(size_t new_size);

        size_t capacity() const noexcept;
        void reserve(size_t new_capacity);

    };

}
