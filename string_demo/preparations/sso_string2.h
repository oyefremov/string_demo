#pragma once

#include <cstring>
#include <utility>

namespace sso2 {

    struct heap_string_data {
        char* _data;
        size_t _size;
        size_t _capacity;
    };

    static_assert(
        sizeof(heap_string_data) == 3 * sizeof(void*),
        "sizeof(heap_string_data) != 3*sizeof(void*)");

    enum {
        SSO_BUFFER_SIZE = sizeof(heap_string_data) - 1,
        SSO_CAPACITY = SSO_BUFFER_SIZE - 1
    };

    static_assert(SSO_CAPACITY == 22, "SSO_CAPACITY != 22");

    struct small_string_data {
        char _buffer[SSO_BUFFER_SIZE];
        char _size;
    };

    class string {

        union
        {
            small_string_data _small;
            heap_string_data _heap;
        };
        bool _use_heap;

        size_t calc_capacity(size_t required_size) const {
            if (required_size <= capacity()) return capacity();
            size_t res = 16u;
            while (res < required_size + 1) {
                res *= 2; // run over powers of two
            }
            return res - 1;
        }

        char* data() noexcept {
            return _use_heap ? _heap._data : _small._buffer;
        }

        const char* data() const noexcept {
            return _use_heap ? _heap._data : _small._buffer;
        }

    public:
        // default constructed
        string() noexcept {
            _small._size = 0;
            _small._buffer[0] = 0;
            _use_heap = false;
        }

        // construct from c-string
        string(const char* str) {
            auto new_size = std::strlen(str);
            if (new_size > SSO_CAPACITY) {
                _heap._size = new_size;
                _heap._capacity = new_size;
                _heap._data = new char[new_size + 1];
                std::memcpy(_heap._data, str, new_size + 1);
                _use_heap = true;
            }
            else {
                _small._size = (char)new_size;
                std::memcpy(_small._buffer, str, new_size + 1);
                _use_heap = false;
            }
        }

        string(const string& other) {
            auto new_size = other.size();
            if (new_size > SSO_CAPACITY) {
                _heap._size = new_size;
                _heap._capacity = new_size;
                _heap._data = new char[new_size + 1];
                std::memcpy(_heap._data, other.data(), new_size + 1);
                _use_heap = true;
            }
            else {
                _small._size = (char)new_size;
                std::memcpy(_small._buffer, other.data(), new_size + 1);
                _use_heap = false;
            }
        }

        string(string&& other) noexcept {
            if (other._use_heap) {
                _heap = other._heap;
                _use_heap = true;

                other._small._size = 0;
                other._small._buffer[0] = 0;
                other._use_heap = false;
            }
            else {
                _small = other._small;
                _use_heap = false;
            }
        }

        string& operator=(const string& other) {
            auto new_size = other.size();
            if (new_size > capacity()) {
                if (_use_heap) {
                    delete[] _heap._data;
                }
                _heap._capacity = new_size;
                _heap._size = new_size;
                _heap._data = new char[new_size + 1];
                std::memcpy(_heap._data, other.data(), new_size + 1);
                _use_heap = true;
            }
            else {
                if (_use_heap) {
                    _heap._size = new_size;
                    std::memcpy(_heap._data, other.data(), new_size + 1);
                }
                else {
                    _small._size = (char)new_size;
                    std::memcpy(_small._buffer, other.data(), new_size + 1);
                }
            }
            return *this;
        }
        string& operator=(string&& other) noexcept {
            if (this->_use_heap) {
                delete[] _heap._data;
            }

            if (other._use_heap) {
                this->_heap = other._heap;
                this->_use_heap = true;

                other._small._size = 0;
                other._small._buffer[0] = 0;
                other._use_heap = false;
            }
            else {
                this->_small = other._small;
                this->_use_heap = false;
            }

            return *this;
        }

        ~string() noexcept {
            if (_use_heap) {
                delete[] _heap._data;
            }
        }

        // useful and interesting
        void swap(string& other) noexcept {
            if (this->_use_heap && other._use_heap) {
                std::swap(this->_heap, other._heap);
            }
            else if (!this->_use_heap && !other._use_heap) {
                std::swap(this->_small, other._small);
            }
            else if (this->_use_heap && !other._use_heap) {
                auto tmp_heap = this->_heap;

                this->_small = other._small;
                this->_use_heap = false;

                other._heap = tmp_heap;
                other._use_heap = true;
            }
            else if (!this->_use_heap && other._use_heap) {
                other.swap(*this);
            }
        }

        // iterators
        char* begin() noexcept {
            return data();
        }
        char* end() noexcept {
            return data() + size();
        }

        // some modifications to have fun
        void insert(size_t index, size_t count, char ch) {
            auto data = this->data();
            auto size = this->size();
            if (capacity() < size + count) {
                auto new_capacity = calc_capacity(size + count);
                auto new_data = new char[new_capacity + 1];
                std::memcpy(new_data, data, index);
                std::memset(new_data + index, ch, count);
                std::memcpy(new_data + size, data + index, size + 1 - index);
                if (_use_heap) {
                    delete data;
                }
                _heap._data = new_data;
                _heap._size = size + count;
                _heap._capacity = new_capacity;
                _use_heap = true;
            }
            else if (count > 0) {
                std::memmove(data + index + count, data + index, size + 1 - index);
                std::memset(data + index, ch, count);
                if (_use_heap) {
                    _heap._size = size + count;
                }
                else {
                    _small._size = char(size + count);
                }
            }
        }
        void insert(size_t index, const char* str) {
            auto count = std::strlen(str);
            auto data = this->data();
            auto size = this->size();
            if (capacity() < size + count) {
                auto new_capacity = calc_capacity(size + count);
                auto new_data = new char[new_capacity + 1];
                std::memcpy(new_data, data, index);
                std::memcpy(new_data + index, str, count);
                std::memcpy(new_data + index + count, data + index, size - index);
                new_data[size + count] = 0;
                if (_use_heap) {
                    delete data;
                }
                _heap._data = new_data;
                _heap._size = size + count;
                _heap._capacity = new_capacity;
                _use_heap = true;
            }
            else {
                std::memmove(data + index + count, data + index, size + 1 - index);

                if (str + count >= data + index && str + count <= data + size) {
                    // some data pointed by str was moved with memmove
                    if (str < data + index) {
                        auto first_part = data + index - str;
                        // copy the first part that was not moved
                        std::memcpy(data + index, str, first_part);
                        index += first_part;
                        str += count + first_part;
                        count -= first_part;
                    }
                    else {
                        str += count;
                    }
                }
                std::memcpy(data + index, str, count);
                if (_use_heap) {
                    _heap._size = size + count;
                }
                else {
                    _small._size = char(size + count);
                }
            }
        }

        // for printing
        const char* c_str() const noexcept {
            return data();
        }

        size_t size() const noexcept {
            return _use_heap ? _heap._size : _small._size;
        }

        void resize(size_t new_size, char ch = 0) {
            auto old_size = size();
            if (capacity() < new_size) {
                auto new_data = new char[new_size + 1];
                std::memcpy(new_data, data(), old_size);
                if (_use_heap) {
                    delete _heap._data;
                }
                _use_heap = true;
                _heap._data = new_data;
                std::memset(new_data + old_size, ch, new_size - old_size);
                new_data[new_size] = 0;
                _heap._size = new_size;
                _heap._capacity = new_size;
            }
            else if (new_size < old_size) {
                if (_use_heap) {
                    _heap._data[new_size] = 0;
                    _heap._size = new_size;
                }
                else {
                    _small._buffer[new_size] = 0;
                    _small._size = (char)new_size;
                }
            }
            else if (new_size > old_size) {
                if (_use_heap) {
                    std::memset(_heap._data + old_size, ch, new_size - old_size);
                    _heap._data[new_size] = 0;
                    _heap._size = new_size;
                }
                else {
                    std::memset(_small._buffer + old_size, ch, new_size - old_size);
                    _small._buffer[new_size] = 0;
                    _small._size = (char)new_size;
                }
            }
        }

        size_t capacity() const noexcept {
            return _use_heap ? _heap._capacity : SSO_CAPACITY;
        }

        void reserve(size_t new_capacity) {
            auto size = this->size();
            if (new_capacity >= capacity()) {
                auto new_data = new char[new_capacity + 1];
                std::memcpy(new_data, data(), size);
                new_data[size] = 0;
                if (_use_heap) {
                    delete _heap._data;
                }
                _heap._size = size;
                _heap._data = new_data;
                _heap._capacity = new_capacity;
                _use_heap = true;
            }
        }
    };

    static_assert(
        sizeof(string) == (sizeof(small_string_data) + sizeof(void*)),
        "sizeof(string) != (sizeof(small_string_data) + sizeof(void*))");
}
