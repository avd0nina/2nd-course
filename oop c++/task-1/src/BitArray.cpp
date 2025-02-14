#include "BitArray.h"

BitArray::BitArray() : m_size(0) {
}

BitArray::~BitArray() {
}

BitArray::BitArray(int num_bits, unsigned long value) : m_size(num_bits) {
    if (num_bits < 0)
        throw std::invalid_argument("Number of bits cannot be negative");
    int num_blocks = (num_bits + BITS_PER_BLOCK - 1) / BITS_PER_BLOCK;
    m_bits.resize(num_blocks, 0);
    if (num_bits > 0) {
        m_bits[0] = value;
        if (num_bits < BITS_PER_BLOCK) {
            unsigned long mask = (1UL << num_bits) - 1;
            m_bits[0] &= mask;
        }
    }
}

BitArray::BitArray(const BitArray& b) : m_bits(b.m_bits), m_size(b.m_size) {
}

void BitArray::swap(BitArray& b) {
    std::swap(m_bits, b.m_bits);
    std::swap(m_size, b.m_size);
}

BitArray& BitArray::operator=(const BitArray& b) {
    if (this != &b) {
        m_bits = b.m_bits;
        m_size = b.m_size;
    }
    return *this;
}

void BitArray::resize(int num_bits, bool value) {
    if (num_bits < 0)
        throw std::invalid_argument("Number of bits cannot be negative");
    int old_num_blocks = (m_size + BITS_PER_BLOCK - 1) / BITS_PER_BLOCK;
    int new_num_blocks = (num_bits + BITS_PER_BLOCK - 1) / BITS_PER_BLOCK; 
    m_bits.resize(new_num_blocks, value ? ~0UL : 0UL); 
    if (num_bits < m_size) { 
        int last_full_block = num_bits / BITS_PER_BLOCK;
        int bits_in_last_block = num_bits % BITS_PER_BLOCK; 
        if (bits_in_last_block != 0) {
            unsigned long mask = (1UL << bits_in_last_block) - 1; 
            m_bits[last_full_block] &= mask; 
        }
    }
    else if (value && (num_bits > m_size)) { 
        int bits_in_old_last_block = m_size % BITS_PER_BLOCK; 
        if (bits_in_old_last_block != 0) {
            int block = m_size / BITS_PER_BLOCK;
            unsigned long mask = (~0UL) << bits_in_old_last_block;
            m_bits[block] |= mask;
        }
    }
    m_size = num_bits;
}

void BitArray::clear() {
    m_bits.clear();
    m_size = 0;
}

void BitArray::push_back(bool bit) {
    int index = m_size;
    resize(m_size + 1);
    set(index, bit);
}

BitArray& BitArray::operator&=(const BitArray& b) {
    if (m_size != b.m_size)
        throw std::invalid_argument("Sizes of BitArrays must match for bitwise AND operation");
    for (size_t i = 0; i < m_bits.size(); ++i) {
        m_bits[i] &= b.m_bits[i];
    }
    return *this;
}

BitArray& BitArray::operator|=(const BitArray& b) {
    if (m_size != b.m_size)
        throw std::invalid_argument("Sizes of BitArrays must match for bitwise OR operation");
    for (size_t i = 0; i < m_bits.size(); ++i) {
        m_bits[i] |= b.m_bits[i];
    }
    return *this;
}

BitArray& BitArray::operator^=(const BitArray& b) {
    if (m_size != b.m_size)
        throw std::invalid_argument("Sizes of BitArrays must match for bitwise XOR operation");
    for (size_t i = 0; i < m_bits.size(); ++i) {
        m_bits[i] ^= b.m_bits[i];
    }
    return *this;
}

BitArray& BitArray::operator<<=(int n) {
    if (n < 0)
        return (*this) >>= -n;
    if (n >= m_size) {
        reset();
        return *this;
    }
    for (int i = m_size - 1; i >= n; --i) {
        set(i, (*this)[i - n]);
    }
    for (int i = 0; i < n; ++i) {
        set(i, false);
    }
    return *this;
}

BitArray& BitArray::operator>>=(int n) {
    if (n < 0)
        return (*this) <<= -n;
    if (n >= m_size) {
        reset();
        return *this;
    }
    for (int i = 0; i < m_size - n; ++i) {
        set(i, (*this)[i + n]);
    }
    for (int i = m_size - n; i < m_size; ++i) {
        set(i, false);
    }
    return *this;
}

BitArray BitArray::operator<<(int n) const {
    BitArray result(*this);
    result <<= n;
    return result;
}

BitArray BitArray::operator>>(int n) const {
    BitArray result(*this);
    result >>= n;
    return result;
}

BitArray& BitArray::set(int n, bool val) {
    check_index(n);
    int block = n / BITS_PER_BLOCK;
    int offset = n % BITS_PER_BLOCK;
    if (val)
        m_bits[block] |= (1UL << offset);
    else
        m_bits[block] &= ~(1UL << offset);
    return *this;
}

BitArray& BitArray::set() {
    std::fill(m_bits.begin(), m_bits.end(), ~0UL);
    int extra_bits = m_bits.size() * BITS_PER_BLOCK - m_size;
    if (extra_bits > 0) {
        unsigned long mask = (1UL << (m_size % BITS_PER_BLOCK)) - 1;
        m_bits.back() &= mask;
    }
    return *this;
}

BitArray& BitArray::reset(int n) {
    return set(n, false);
}

BitArray& BitArray::reset() {
    std::fill(m_bits.begin(), m_bits.end(), 0UL);
    return *this;
}

bool BitArray::any() const {
    for (size_t i = 0; i < m_bits.size(); ++i) {
        if (m_bits[i])
            return true;
    }
    return false;
}

bool BitArray::none() const {
    return !any();
}

BitArray BitArray::operator~() const {
    BitArray result(*this);
    for (size_t i = 0; i < result.m_bits.size(); ++i) {
        result.m_bits[i] = ~result.m_bits[i];
    }
    int extra_bits = result.m_bits.size() * BITS_PER_BLOCK - m_size;
    if (extra_bits > 0) {
        unsigned long mask = (1UL << (m_size % BITS_PER_BLOCK)) - 1;
        result.m_bits.back() &= mask;
    }
    return result;
}

int BitArray::count() const {
    int cnt = 0;
    for (size_t i = 0; i < m_bits.size(); ++i) {
        unsigned long x = m_bits[i];
        cnt += __builtin_popcountl(x);
    }
    return cnt;
}

void BitArray::check_index(int i) const {
    if (i < 0 || i >= m_size)
        throw std::out_of_range("BitArray index out of range");
}

BitArray::reference BitArray::operator[](int i) {
    check_index(i);
    return reference(*this, i);
}

bool BitArray::operator[](int i) const {
    check_index(i);
    int block = i / BITS_PER_BLOCK;
    int offset = i % BITS_PER_BLOCK;
    return (m_bits[block] & (1UL << offset)) != 0;
}

int BitArray::size() const {
    return m_size;
}

bool BitArray::empty() const {
    return m_size == 0;
}

std::string BitArray::to_string() const {
    std::string result;
    result.reserve(m_size);
    for (int i = 0; i < m_size; ++i)
    {
        result.push_back((*this)[i] ? '1' : '0');
    }
    return result;
}

bool operator==(const BitArray& a, const BitArray& b) {
    if (a.size() != b.size())
        return false;
    return a.m_bits == b.m_bits;
}

bool operator!=(const BitArray& a, const BitArray& b) {
    return !(a == b);
}

BitArray operator&(const BitArray& b1, const BitArray& b2) {
    if (b1.size() != b2.size())
        throw std::invalid_argument("Sizes of BitArrays must match for bitwise AND operation");
    BitArray result(b1);
    result &= b2;
    return result;
}

BitArray operator|(const BitArray& b1, const BitArray& b2) {
    if (b1.size() != b2.size())
        throw std::invalid_argument("Sizes of BitArrays must match for bitwise OR operation");
    BitArray result(b1);
    result |= b2;
    return result;
}

BitArray operator^(const BitArray& b1, const BitArray& b2) {
    if (b1.size() != b2.size())
        throw std::invalid_argument("Sizes of BitArrays must match for bitwise XOR operation");
    BitArray result(b1);
    result ^= b2;
    return result;
}

BitArray::reference::reference(BitArray& bitarray, int index) : m_bitarray(bitarray), m_index(index) {}

BitArray::reference& BitArray::reference::operator=(bool x) {
    m_bitarray.set(m_index, x);
    return *this;
}

BitArray::reference& BitArray::reference::operator=(const reference& x) {
    bool value = static_cast<bool>(x);
    m_bitarray.set(m_index, value);
    return *this;
}

BitArray::reference::operator bool() const {
    return static_cast<const BitArray&>(m_bitarray).operator[](m_index);
}

BitArray::reference& BitArray::reference::flip() {
    bool current = static_cast<bool>(*this);
    m_bitarray.set(m_index, !current);
    return *this;
}
