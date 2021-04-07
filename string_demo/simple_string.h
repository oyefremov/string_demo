#include <cstring>
#include <utility>
#include <cassert>

#pragma once

namespace simple {

    class string {
        char* _buffer = 0;
        size_t _size = 0;
        size_t _capacity = 0;

    public:
        // default constructed
        string() noexcept = default;
        // construct from c-string
        explicit string(const char* str)
        {
            assert(str);
            auto size = std::strlen(str);
            _buffer = new char[size + 1];
            std::memcpy(_buffer, str, size);
            _buffer[size] = '\0';
            _size = size;
            _capacity = size;
        }

        // rule of five
        string(const string& other) {
            auto size = other.size();
            if (size > 0) {
                _buffer = new char[size + 1];
                std::memcpy(_buffer, other._buffer, size);
                _buffer[size] = '\0';
                _size = size;
                _capacity = size;
            }
            else {
                _buffer = nullptr;
                _size = 0;
                _capacity = 0;
            }
        }
        string(string&& other) noexcept :
            _buffer(other._buffer),
            _size(other._size),
            _capacity(other._capacity) {
            other._buffer = nullptr;
            other._size = 0;
            other._capacity = 0;
        }
        string& operator=(const string& other) {
            if (this == &other) return *this;
            auto size = other.size();
            if (_capacity < size) {
                auto new_buffer = new char[size + 1];
                delete[] _buffer;
                _buffer = new_buffer;
                _capacity = size;
                std::memcpy(_buffer, other._buffer, size);
                _buffer[size] = '\0';
                _size = size;
            }
            else if (size > 0) {
                std::memcpy(_buffer, other._buffer, size);
                _buffer[size] = '\0';
                _size = size;
            }
            else {
                if (_buffer) _buffer[size] = '\0';
                _size = size;
            }
            return *this;
        }
        string& operator=(string&& other) noexcept {
            if (this == &other) return *this;
            delete[] _buffer;
            _buffer = other._buffer;
            _size = other._size;
            _capacity = other._capacity;
            other._buffer = nullptr;
            other._size = 0;
            other._capacity = 0;
            return*this;
        }
        ~string() noexcept {
            delete[] _buffer;
        }

        // useful and interesting
        void swap(string& other) noexcept {
            auto tmp = std::move(*this);
            *this = std::move(other);
            other = std::move(tmp);
        }

        // iterators
        char* begin() noexcept {
            return _buffer;
        }
        char* end() noexcept {
            return _buffer + _size;
        }

        // some modifications to have fun
        void insert(size_t index, size_t count, char ch) {
            if (_capacity < _size + count) {
                _capacity = _size + count;
                auto new_data = new char[_capacity + 1];
                std::memcpy(new_data, _buffer, index);
                std::memset(new_data + index, ch, count);
                std::memcpy(new_data + _size, _buffer + index, _size + 1 - index);
                delete _buffer;
                _buffer = new_data;
                _size += count;
            }
            else if (count > 0) {
                std::memmove(_buffer + index + count, _buffer + index, _size + 1 - index);
                std::memset(_buffer + index, ch, count);
                _size += count;
            }
        }
        void insert(size_t index, const char* str) {
            auto count = std::strlen(str);
            if (_capacity < _size + count) {
                _capacity = _size + count;
                auto new_data = new char[_capacity + 1];
                std::memcpy(new_data, _buffer, index);
                std::memcpy(new_data + index, str, count);
                std::memcpy(new_data + index + count, _buffer + index, _size - index);
                new_data[_size + count] = 0;
                delete _buffer;
                _buffer = new_data;
                _size += count;
            }
            else if (_buffer) {
                std::memmove(_buffer + index + count, _buffer + index, _size + 1 - index);
                if (str + count >= _buffer + index && str + count <= _buffer + _size) {
                    // some data pointed by str was moved with memmove
                    if (str < _buffer + index) {
                        auto first_part = _buffer + index - str;
                        // copy the first part that was not moved
                        std::memcpy(_buffer + index, str, first_part);
                        index += first_part;
                        str += count + first_part;
                        count -= first_part;
                    }
                    else {
                        str += count;
                    }
                }
                std::memcpy(_buffer + index, str, count);
                _size += count;
            }
        }

        // for printing
        const char* c_str() const noexcept {
            return _buffer ? _buffer : "";
        }

        size_t size() const noexcept {
            return _size;
        }
        void resize(size_t new_size, char ch = 0) {
            if (_capacity < new_size) {
                auto new_data = new char[new_size + 1];
                std::memcpy(new_data, _buffer, _size);
                delete _buffer;
                _buffer = new_data;
                std::memset(_buffer + _size, ch, new_size - _size);
                _buffer[new_size] = 0;
                _size = new_size;
                _capacity = new_size;
            }
            else if (new_size < _size) {
                _buffer[new_size] = 0;
                _size = new_size;
            }
            else if (new_size > _size) {
                std::memset(_buffer + _size, ch, new_size - _size);
                _buffer[new_size] = 0;
                _size = new_size;
            }

        }

        size_t capacity() const noexcept {
            return _capacity;
        }
        void reserve(size_t new_capacity) {
            if (new_capacity > _capacity) {
                auto new_buffer = new char[new_capacity + 1];
                std::memcpy(new_buffer, _buffer, _size);
                delete[] _buffer;
                _buffer = new_buffer;
                _capacity = new_capacity;
            }
        }
    };
}