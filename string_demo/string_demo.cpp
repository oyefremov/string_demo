// Intellias hiring day will happen on this Saturday
// You are wellcome to join ua.
// Additional; infor available at goo.gl/aasHG6


#include <iostream>
#include <algorithm>
#include <cstring>

namespace v0 {
    class string {
        char* _data = nullptr;
        size_t _size = 0;
        size_t _capacity = 0;

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

        // Don't forget to join our hairing day
        size_t capacity() const noexcept;
        void reserve(size_t new_capacity);

    };
}

namespace v1 {
    class string {
        char* _data = nullptr;
        size_t _size = 0;
        size_t _capacity = 0;

        void reserve_no_copy(size_t new_capacity) { reserve_impl<false>(new_capacity); }

        template<bool copy_data>
        void reserve_impl(size_t new_capacity) {
            if (_capacity >= new_capacity) return;

            char* new_data = new char[new_capacity + 1];
            if (copy_data) {
                if (_size > 0) {
                    std::copy_n(_data, _size + 1, new_data);
                }
                else {
                    new_data[0] = 0;
                }
            }
            delete[] _data;
            _data = new_data;
            _capacity = new_capacity;
        }

        size_t next_capacity(size_t new_size) const {
            if (new_size <= _capacity) return _capacity;
            const size_t MIN_CAPACITY = 16 - 1;
            const size_t MAX_CAPACITY = std::numeric_limits<size_t>::max() - 1;
            if (new_size > MAX_CAPACITY) throw std::bad_alloc();
            if (new_size > MAX_CAPACITY / 2) return MAX_CAPACITY;
            auto new_capacity_plus_one = _capacity + 1;
            if (new_capacity_plus_one < MIN_CAPACITY) new_capacity_plus_one = MIN_CAPACITY + 1;
            while (new_capacity_plus_one < (new_size + 1)) {
                new_capacity_plus_one *= 2;
            }
            return new_capacity_plus_one - 1;
        }

    public:
        string() = default;
        string(const string& other) { *this = other; }
        string(string&& other) noexcept { *this = other; }
        string& operator=(const string& other) {
            reserve_no_copy(other._size);
            if (other._size) {
                std::copy_n(other._data, other._size + 1, _data);
            }
            else if (_capacity > 0) {
                _data[0] = 0;
            }
            _size = other._size;
            return *this;
        }
        void swap(string& other) noexcept {
            std::swap(_data, other._data);
            std::swap(_capacity, other._capacity);
            std::swap(_size, other._size);
        }
        string& operator=(string&& other) noexcept {
            string().swap(*this); // clean resources
            this->swap(other);
            return *this;
        }
        ~string() {
            delete[] _data;
        }

        string(const char* str) {
            if (str == nullptr) return;
            auto count = std::strlen(str);
            reserve_no_copy(count);
            std::copy_n(str, count + 1, _data);
            _size = count;
        }

        void append(const char* str) {
            if (str == nullptr) return;
            auto count = std::strlen(str);
            reserve(next_capacity(_size + count));
            std::copy_n(str, count + 1, _data + _size);
            _size += count;
        }

        const char* c_str() const { return _data ? _data : ""; }

        size_t size() const { return _size; }
        void resize(size_t new_size) {
            if (_size == new_size) {
                // done
            }
            else if (_size > new_size) {
                _size = new_size;
                _data[_size] = 0;
            }
            else { // new_size > _size
                reserve(new_size);
                std::fill_n(_data + _size, new_size - _size + 1, 0);
                _size = new_size;
            }
        }

        size_t capacity() const { return _capacity; }

        void reserve(size_t new_capacity) { reserve_impl<true>(new_capacity); }
    };
}


namespace v2 {
    class string {
        enum {SMALL_BUFFER_SIZE = 16};
        char* _data = buffer;
        size_t _size = 0;
        size_t _capacity = SMALL_BUFFER_SIZE - 1;
        char buffer[SMALL_BUFFER_SIZE] = {0};
        bool use_small_buffer = true;

        void reserve_no_copy(size_t new_capacity) { reserve_impl<false>(new_capacity); }

        template<bool copy_data>
        void reserve_impl(size_t new_capacity) {
            if (_capacity >= new_capacity) return;

            char* new_data = new char[new_capacity + 1];
            if (copy_data) {
                if (_size > 0) {
                    std::copy_n(_data, _size + 1, new_data);
                }
                else {
                    new_data[0] = 0;
                }
            }
            if (!use_small_buffer) delete[] _data;
            _data = new_data;
            _capacity = new_capacity;
        }

        size_t next_capacity(size_t new_size) const {
            if (new_size <= _capacity) return _capacity;
            const size_t MIN_CAPACITY = 16 - 1;
            const size_t MAX_CAPACITY = std::numeric_limits<size_t>::max() - 1;
            if (new_size > MAX_CAPACITY) throw std::bad_alloc();
            if (new_size > MAX_CAPACITY / 2) return MAX_CAPACITY;
            auto new_capacity_plus_one = _capacity + 1;
            if (new_capacity_plus_one < MIN_CAPACITY) new_capacity_plus_one = MIN_CAPACITY + 1;
            while (new_capacity_plus_one < (new_size + 1)) {
                new_capacity_plus_one *= 2;
            }
            return new_capacity_plus_one - 1;
        }

    public:
        string() = default;
        string(const string& other) { *this = other; }
        string(string&& other) { *this = other; }
        string& operator=(const string& other) {
            reserve_no_copy(other._size);
            if (other._size) {
                std::copy_n(other._data, other._size + 1, _data);
            }
            else {
                _data[0] = 0;
            }
            _size = other._size;
            return *this;
        }
        string& operator=(string&& other) {
            if (other.use_small_buffer){
            }
            std::swap(_data, other._data);
            std::swap(_capacity, other._capacity);
            std::swap(_size, other._size);
            return *this;
        }
        ~string() {
            delete[] _data;
        }

        string(const char* str) {
            if (str == nullptr) return;
            auto count = std::strlen(str);
            reserve_no_copy(count);
            std::copy_n(str, count + 1, _data);
            _size = count;
        }

        void append(const char* str) {
            if (str == nullptr) return;
            auto count = std::strlen(str);
            reserve(next_capacity(_size + count));
            std::copy_n(str, count + 1, _data + _size);
            _size += count;
        }

        const char* c_str() const { return _data ? _data : ""; }

        size_t size() const { return _size; }
        void resize(size_t new_size) {
            if (_size == new_size) {
                // done
            }
            else if (_size > new_size) {
                _size = new_size;
                _data[_size] = 0;
            }
            else { // new_size > _size
                reserve(new_size);
                std::fill_n(_data + _size, new_size - _size + 1, 0);
                _size = new_size;
            }
        }

        size_t capacity() const { return _capacity; }

        void reserve(size_t new_capacity) { reserve_impl<true>(new_capacity); }
    };
}

#if 0
int main()
{
    using string = v1::string;
    string hello_world;
    hello_world = "Hello";
    hello_world.append(" ");
    hello_world.append("World");
    hello_world.append("\n");
    hello_world.append("With my just invented string");
    std::cout << hello_world.c_str() << "\n";
    std::cout << "size " << hello_world.size() << "\n";
    std::cout << "capacity " << hello_world.capacity() << "\n";
}


#endif