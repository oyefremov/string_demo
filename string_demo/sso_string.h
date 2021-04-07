#pragma once

#include <cstring>
#include <utility>


using const_string = std::shared_ptr<const std::string>;
using mutable_string = std::string;

const_streing make_const(mutable_string&& str){
    return make_shared(std::move(str));
}

mutable_string make_mutable(const_string&& str){
    if (str.unique()){
        auto str = const_cast<std::string>(*str);
        return str;
    }else{
        return *str;
    }
}

namespace sso {
    class string {

        enum { SSO_BUFFER_SIZE = 16 };

        char* _data = _buffer;
        size_t _size = 0;
        size_t _capacity = SSO_BUFFER_SIZE - 1;
        char _buffer[SSO_BUFFER_SIZE] = { 0 };
        bool _use_heap = false;

        size_t calc_capacity(size_t required_size) const {
            if (required_size <= _capacity) return _capacity;
            size_t res = 16u;
            while (res < required_size + 1) {
                res *= 2; // run over powers of two
            }
            return res - 1;
        }

    public:
        // default constructed
        string() noexcept = default;
        // construct from c-string
        string(const char* str) {
            _size = std::strlen(str);
            if (_size > _capacity) {
                _capacity = _size;
                _data = new char[_capacity + 1];
                _use_heap = true;
            }
            std::memcpy(_data, str, _size + 1);
        }

        // rule of five
        string(const string& other) {
            _size = other._size;
            if (_size > _capacity) {
                _capacity = _size;
                _data = new char[_capacity + 1];
                _use_heap = true;
            }
            std::memcpy(_data, other._data, _size + 1);
        }

        string(string&& other) noexcept {
            if (other._use_heap) {
                _size = other._size;
                _capacity = other._capacity;
                _data = other._data;
                _use_heap = true;

                other._size = 0;
                other._capacity = SSO_BUFFER_SIZE - 1;
                other._data = other._buffer;
                other._buffer[0] = 0;
                other._use_heap = false;
            }
            else {
                _size = other._size;
                std::memcpy(_buffer, other._buffer, other._size + 1);
            }
        }

        string& operator=(const string& other) {
            if (other._size > _capacity) {
                if (_use_heap) {
                    delete[] _data;
                }
                _capacity = other._size;
                _data = new char[_capacity + 1];
                _use_heap = true;
            }
            _size = other._size;
            std::memcpy(_data, other._data, _size + 1);
            return *this;
        }
        string& operator=(string&& other) noexcept {
            if (this->_use_heap) {
                delete[] _data;
            }

            _size = other._size;
            _capacity = other._capacity;
            if (other._use_heap) {
                _data = other._data;
                _use_heap = true;

                other._size = 0;
                other._capacity = SSO_BUFFER_SIZE - 1;
                other._data = other._buffer;
                other._buffer[0] = 0;
                other._use_heap = false;
            }
            else {
                _data = _buffer;
                _use_heap = false;
                std::memcpy(_buffer, other._buffer, other._size + 1);
            }

            return *this;
        }

        ~string() noexcept {
            if (_use_heap) {
                delete[] _data;
            }
        }

        // useful and interesting
        void swap(string& other) noexcept {
            if (this->_use_heap && other._use_heap) {
                std::swap(this->_data, other._data);
            }
            else if (!this->_use_heap && !other._use_heap) {
                std::swap(this->_buffer, other._buffer);
            }
            else if (this->_use_heap && !other._use_heap) {
                other._data = this->_data;
                this->_data = this->_buffer;
                std::memcpy(this->_buffer, other._buffer, other._size + 1);
            }
            else if (!this->_use_heap && other._use_heap) {
                this->_data = other._data;
                other._data = other._buffer;
                std::memcpy(other._buffer, this->_buffer, this->_size + 1);
            }

            std::swap(this->_use_heap, other._use_heap);
            std::swap(this->_size, other._size);
            std::swap(this->_capacity, other._capacity);
        }

        // iterators
        char* begin() noexcept {
            return _data;
        }
        char* end() noexcept {
            return _data + _size;
        }

        // some modifications to have fun
        void insert(size_t index, size_t count, char ch) {
            if (_capacity < _size + count) {
                _capacity = calc_capacity(_size + count);
                auto new_data = new char[_capacity + 1];
                std::memcpy(new_data, _data, index);
                std::memset(new_data + index, ch, count);
                std::memcpy(new_data + _size, _data + index, _size + 1 - index);
                if (_use_heap) {
                    delete _data;
                }
                _use_heap = true;
                _data = new_data;
                _size += count;
            }
            else if (count > 0) {
                std::memmove(_data + index + count, _data + index, _size + 1 - index);
                std::memset(_data + index, ch, count);
                _size += count;
            }
        }
        void insert(size_t index, const char* str) {
            auto count = std::strlen(str);
            if (_capacity < _size + count) {
                _capacity = calc_capacity(_size + count);
                auto new_data = new char[_capacity + 1];
                std::memcpy(new_data, _data, index);
                std::memcpy(new_data + index, str, count);
                std::memcpy(new_data + index + count, _data + index, _size - index);
                new_data[_size + count] = 0;
                if (_use_heap) {
                    delete _data;
                }
                _use_heap = true;
                _data = new_data;
                _size += count;
            }
            else {
                // prefix__sufix
                //    ^^^^^^^
                // prefi^^^^^^^x__sufix
                std::memmove(_data + index + count, _data + index, _size + 1 - index);

                if (str + count >= _data + index && str + count <= _data + _size) {
                    // some data pointed by str was moved with memmove
                    if (str < _data + index) {
                        auto first_part = _data + index - str;
                        // copy the first part that was not moved
                        std::memcpy(_data + index, str, first_part);
                        index += first_part;
                        str += count + first_part;
                        count -= first_part;
                    }
                    else {
                        str += count;
                    }
                }
                std::memcpy(_data + index, str, count);
                _size += count;
            }
        }

        // for printing
        const char* c_str() const noexcept {
            return _data;
        }

        size_t size() const noexcept {
            return _size;
        }
        void resize(size_t new_size, char ch = 0) {
            if (_capacity < new_size) {
                auto new_data = new char[new_size + 1];
                std::memcpy(new_data, _data, _size);
                if (_use_heap) {
                    delete _data;
                }
                _use_heap = true;
                _data = new_data;
                std::memset(_data + _size, ch, new_size - _size);
                _data[new_size] = 0;
                _size = new_size;
                _capacity = new_size;
            }
            else if (new_size < _size) {
                _data[new_size] = 0;
                _size = new_size;
            }
            else if (new_size > _size) {
                std::memset(_data + _size, ch, new_size - _size);
                _data[new_size] = 0;
                _size = new_size;
            }
        }

        size_t capacity() const noexcept {
            return _capacity;
        }
        void reserve(size_t new_capacity) {
            if (new_capacity >= _capacity) {
                auto new_data = new char[new_capacity + 1];
                std::memcpy(new_data, _data, _size);
                new_data[_size] = 0;
                if (_use_heap) {
                    delete _data;
                }
                _use_heap = true;
                _data = new_data;
                _capacity = new_capacity;
            }
        }
    };
}
