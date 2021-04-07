#pragma once

#include <cstring>
#include <utility>

namespace sso3 {

    const size_t USE_HEAP_BIT = (size_t)1 << (sizeof(size_t) * 8 - 1);
    const size_t CAPACITY_MASK = ~USE_HEAP_BIT;

    struct heap_string_data {
        char* _data;
        size_t _size;
        size_t _capacity_and_heap_flag;
        size_t capacity() const {
            return _capacity_and_heap_flag & CAPACITY_MASK;
        }
        bool use_heap() const {
            return _capacity_and_heap_flag & USE_HEAP_BIT;
        }
        void set_capacity_and_heap_flag(size_t capacity) {
            _capacity_and_heap_flag = capacity | USE_HEAP_BIT;
        }
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
        char _size_and_heap_flag;
        size_t size() const {
            return _size_and_heap_flag;
        }
        bool use_heap() const {
            return _size_and_heap_flag & 128;
        }
        void set_size_and_reset_heap_flag(size_t size) {
            assert(size <= SSO_CAPACITY);
            _size_and_heap_flag = (char)size;
        }
    };

    class string {

        union
        {
            small_string_data _small;
            heap_string_data _heap;
        };
        bool use_heap() const {
            return _small.use_heap();
        }

        void set_heap_data(const heap_string_data& src) {
            _heap = src;
        }

        void set_heap_data(size_t size, size_t capacity, char* data) {
            _heap._size = size;
            _heap._data = data;
            _heap.set_capacity_and_heap_flag(capacity);
        }

        void set_small_data(const small_string_data& src) {
            _small = src;
        }

        void set_small_data(size_t size, const char* src) {
            std::memcpy(_small._buffer, src, size + 1);
            assert(size <= SSO_CAPACITY);
            _small.set_size_and_reset_heap_flag(size);
        }

        void clear_small_data() {
            _small._buffer[0] = 0;
            _small.set_size_and_reset_heap_flag(0);
        }

        size_t calc_capacity(size_t required_size) const {
            if (required_size <= capacity()) return capacity();
            size_t res = 16u;
            while (res < required_size + 1) {
                res *= 2; // run over powers of two
            }
            return res - 1;
        }

        char* data() noexcept {
            return use_heap() ? _heap._data : _small._buffer;
        }

        const char* data() const noexcept {
            return use_heap() ? _heap._data : _small._buffer;
        }

    public:
        // default constructed
        string() noexcept {
            clear_small_data();
        }

        // construct from c-string
        string(const char* str) {
            auto new_size = std::strlen(str);
            if (new_size > SSO_CAPACITY) {
                auto new_data = new char[new_size + 1];
                std::memcpy(new_data, str, new_size + 1);
                set_heap_data(new_size, new_size, new_data);
            }
            else {
                set_small_data(new_size, str);
            }
        }

        string(const string& other) {
            auto new_size = other.size();
            if (new_size > SSO_CAPACITY) {
                auto new_data = new char[new_size + 1];
                std::memcpy(new_data, other.data(), new_size + 1);
                set_heap_data(new_size, new_size, new_data);
            }
            else {
                set_small_data(new_size, other.data());
            }
        }

        string(string&& other) noexcept {
            if (other.use_heap()) {
                set_heap_data(other._heap);
                other.clear_small_data();
            }
            else {
                set_small_data(other._small);
            }
        }

        string& operator=(const string& other) {
            auto new_size = other.size();
            if (new_size > capacity()) {
                if (use_heap()) {
                    delete[] _heap._data;
                }
                auto new_data = new char[new_size + 1];
                std::memcpy(new_data, other.data(), new_size + 1);
                set_heap_data(new_size, new_size, new_data);
            }
            else {
                if (use_heap()) {
                    _heap._size = new_size;
                    std::memcpy(_heap._data, other.data(), new_size + 1);
                }
                else {
                    _small.set_size_and_reset_heap_flag(new_size);
                    std::memcpy(_small._buffer, other.data(), new_size + 1);
                }
            }
            return *this;
        }
        string& operator=(string&& other) noexcept {
            if (this->use_heap()) {
                delete[] _heap._data;
            }

            if (other.use_heap()) {
                set_heap_data(other._heap);
                other.clear_small_data();
            }
            else {
                set_small_data(other._small);
            }

            return *this;
        }

        ~string() noexcept {
            if (use_heap()) {
                delete[] _heap._data;
            }
        }

        // useful and interesting
        void swap(string& other) noexcept {
            if (this->use_heap() && other.use_heap()) {
                std::swap(this->_heap, other._heap);
            }
            else if (!this->use_heap() && !other.use_heap()) {
                std::swap(this->_small, other._small);
            }
            else if (this->use_heap() && !other.use_heap()) {
                auto tmp_heap = this->_heap;
                set_small_data(other._small);
                other.set_heap_data(tmp_heap);
            }
            else if (!this->use_heap() && other.use_heap()) {
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
                if (use_heap()) {
                    delete data;
                }
                set_heap_data(size + count, new_capacity, new_data);
            }
            else if (count > 0) {
                std::memmove(data + index + count, data + index, size + 1 - index);
                std::memset(data + index, ch, count);
                if (use_heap()) {
                    _heap._size = size + count;
                }
                else {
                    _small.set_size_and_reset_heap_flag(size + count);
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
                if (use_heap()) {
                    delete data;
                }
                set_heap_data(size + count, new_capacity, new_data);
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
                if (use_heap()) {
                    _heap._size = size + count;
                }
                else {
                    _small.set_size_and_reset_heap_flag(size + count);
                }
            }
        }

        // for printing
        const char* c_str() const noexcept {
            return data();
        }

        size_t size() const noexcept {
            return use_heap() ? _heap._size : _small.size();
        }

        void resize(size_t new_size, char ch = 0) {
            auto old_size = size();
            if (capacity() < new_size) {
                auto new_data = new char[new_size + 1];
                std::memcpy(new_data, data(), old_size);
                if (use_heap()) {
                    delete _heap._data;
                }
                std::memset(new_data + old_size, ch, new_size - old_size);
                new_data[new_size] = 0;
                set_heap_data(new_size, new_size, new_data);
            }
            else if (new_size < old_size) {
                if (use_heap()) {
                    _heap._data[new_size] = 0;
                    _heap._size = new_size;
                }
                else {
                    _small._buffer[new_size] = 0;
                    _small.set_size_and_reset_heap_flag(new_size);
                }
            }
            else if (new_size > old_size) {
                if (use_heap()) {
                    std::memset(_heap._data + old_size, ch, new_size - old_size);
                    _heap._data[new_size] = 0;
                    _heap._size = new_size;
                }
                else {
                    std::memset(_small._buffer + old_size, ch, new_size - old_size);
                    _small._buffer[new_size] = 0;
                    _small.set_size_and_reset_heap_flag(new_size);
                }
            }
        }

        size_t capacity() const noexcept {
            return use_heap() ? _heap.capacity() : SSO_CAPACITY;
        }

        void reserve(size_t new_capacity) {
            auto size = this->size();
            if (new_capacity >= capacity()) {
                auto new_data = new char[new_capacity + 1];
                std::memcpy(new_data, data(), size);
                new_data[size] = 0;
                if (use_heap()) {
                    delete _heap._data;
                }
                set_heap_data(size, new_capacity, new_data);
            }
        }
    };

    static_assert(
        sizeof(string) == sizeof(small_string_data),
        "sizeof(string) != sizeof(small_string_data)");
}
