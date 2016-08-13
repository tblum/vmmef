//
// Created by troels on 8/10/16.
//

#ifndef VMMEF_ADCITERATOR_H
#define VMMEF_ADCITERATOR_H


#include <iterator>
#include <cassert>
#include <arpa/inet.h>

class ADCiterator : public std::iterator<std::random_access_iterator_tag, uint16_t >
{
private:
    const uint16_t *p;
public:
    typedef std::iterator<std::random_access_iterator_tag, uint16_t >::difference_type difference_type;

    // Constructors
    ADCiterator(const uint16_t* x) :p(x) {assert ((uintptr_t)p % 2 == 0);}
    ADCiterator(const uint32_t* x) :p((const uint16_t*)x) {}
    ADCiterator(const ADCiterator& mit) : p(mit.p) {}

    // Manipulators
    inline ADCiterator& operator++() {++p;return *this;}
    inline ADCiterator& operator--() {--p;return *this;}
    inline ADCiterator operator++(int) {ADCiterator tmp(*this); ++p; return tmp;}
    inline ADCiterator operator--(int) {ADCiterator tmp(*this); --p; return tmp;}
    inline ADCiterator& operator+=(difference_type i) {p += i; return *this;}
    inline ADCiterator& operator-=(difference_type i) {p -= i; return *this;}

    // Iterator arithmatic
    inline difference_type operator-(const ADCiterator& other) const {return p-other.p;}
    inline ADCiterator operator+(difference_type i) const {return ADCiterator(p+i);}
    inline ADCiterator operator-(difference_type i) const {return ADCiterator(p-i);}

    // Comparison
    inline bool operator==(const ADCiterator& other) const {return p == other.p;}
    inline bool operator!=(const ADCiterator& other) const {return p != other.p;}
    inline bool operator>(const ADCiterator& other) const {return p > other.p;}
    inline bool operator<(const ADCiterator& other) const {return p < other.p;}
    inline bool operator>=(const ADCiterator& other) const {return p >= other.p;}
    inline bool operator<=(const ADCiterator& other) const {return p <= other.p;}

    // Value operators
    inline uint16_t operator*() const {return ((uintptr_t)p % sizeof(uint32_t) == 0) ? ntohs(*(p+1)) : ntohs(*(p-1));}
    inline uint16_t operator[](difference_type i) const {return *(*this+i);}

};


#endif //VMMEF_ADCITERATOR_H
