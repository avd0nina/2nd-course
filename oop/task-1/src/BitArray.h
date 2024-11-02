#ifndef BITARRAY_H
#define BITARRAY_H

#include <vector>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <climits>

class BitArray {
public:
    BitArray();
    ~BitArray();
    explicit BitArray(int num_bits, unsigned long value = 0);
    BitArray(const BitArray& b);
    void swap(BitArray& b);
    BitArray& operator=(const BitArray& b);
    void resize(int num_bits, bool value = false);
    void clear();
    void push_back(bool bit);
    BitArray& operator&=(const BitArray& b);
    BitArray& operator|=(const BitArray& b);
    BitArray& operator^=(const BitArray& b);
    BitArray& operator<<=(int n);
    BitArray& operator>>=(int n);
    BitArray operator<<(int n) const;
    BitArray operator>>(int n) const;
    BitArray& set(int n, bool val = true);
    BitArray& set();
    BitArray& reset(int n);
    BitArray& reset();
    bool any() const;
    bool none() const;
    BitArray operator~() const;
    int count() const;

    class reference {
    public:
        reference(BitArray& bitarray, int index);
        reference& operator=(bool x);
        reference& operator=(const reference& x);
        operator bool() const;
        reference& flip();
    private:
        BitArray& m_bitarray;
        int m_index;
    };

    reference operator[](int i);
    bool operator[](int i) const;
    int size() const;
    bool empty() const;
    std::string to_string() const;
    friend bool operator==(const BitArray& a, const BitArray& b);
    friend bool operator!=(const BitArray& a, const BitArray& b);
    friend BitArray operator&(const BitArray& b1, const BitArray& b2);
    friend BitArray operator|(const BitArray& b1, const BitArray& b2);
    friend BitArray operator^(const BitArray& b1, const BitArray& b2);

private:
    std::vector<unsigned long> m_bits;
    int m_size;
    static const int BITS_PER_BLOCK = sizeof(unsigned long) * CHAR_BIT;
    void check_index(int i) const;
};

#endif // BITARRAY_H
