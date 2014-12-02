#ifndef BITSET_H
#define BITSET_H

#include <QString>
#include <stdarg.h>

int bitCount(int bits);
int highestBit(int bits);
int lowestBit(int bits);

class StaticDummy
{
public:
    StaticDummy();
};

extern StaticDummy staticDummyObject;

class BitSet;

class BitSetIterator
{
    friend class BitSet;
public:
    BitSetIterator():set(NULL),current(-1),currentArray(0){}
    BitSetIterator(const BitSet* s):set(s),current(-1),currentArray(0){}
    BitSetIterator(const BitSet* s,int c,int a):set(s),current(c),currentArray(a){}
    bool operator != (const BitSetIterator& other){return current!=other.current;}
    int operator * (){return current;}
    const BitSetIterator& operator ++ ();
private:
    const BitSet* set;
    int current;
    int currentArray;
};

class BitSet
{
    friend class BitSetIterator;
public:
    BitSet(int maxSize=1);
    BitSet(const BitSet& other);
    ~BitSet();
    void add(int element);
    void addM(int num,...);
    void remove(int element);
    void removeM(int num,...);
    bool contains(int element) const;
    void unite(const BitSet& other);
    void intersect(const BitSet& other);
    void subtract(const BitSet& other);
    void symmDifference(const BitSet& other);
    void complement();
    void copy(const BitSet& other);
    void operator |= (const BitSet& other){unite(other);}
    void operator &= (const BitSet& other){intersect(other);}
    void operator -= (const BitSet& other){subtract(other);}
    void operator ^= (const BitSet& other){symmDifference(other);}
    void operator = (const BitSet& other){copy(other);}
    void setFull();
    void setEmpty();
    void setRange(int lower,int upper,int step);
    bool full();
    bool empty();
    void writeToArray(int* array) const;
    void readFromArray(int* array,int size);
    bool operator == (const BitSet& other) const;
    BitSet operator | (const BitSet& other) const{BitSet newSet(*this); newSet.unite(other); return newSet;}
    BitSet operator & (const BitSet& other) const{BitSet newSet(*this); newSet.intersect(other); return newSet;}
    BitSet operator - (const BitSet& other) const{BitSet newSet(*this); newSet.subtract(other); return newSet;}
    BitSet operator ~ () const{BitSet newSet(*this); newSet.complement(); return newSet;}
    BitSet operator ^ (const BitSet& other) const{BitSet newSet(*this); newSet.symmDifference(other); return newSet;}
    int operator [] (int index);
    int size() {if(sizeChanged) reCalcSize(); return nSize;}
    int highest() const;
    int lowest() const;
    int indexOf(int element) const;
    operator QString() const;

    BitSetIterator begin() const{int h=highest(); BitSetIterator b(this,h,content[h/(8*sizeof(int))]); return b;}
    BitSetIterator end() const{BitSetIterator b(this,-1,0); return b;}

private:
    int nArraySizeInBits;
    int nArrayLength;
    int nSize;
    int nMaxSize;
    int* content;
    bool sizeChanged;

    void reCalcSize();
};


#endif // BITSET_H
