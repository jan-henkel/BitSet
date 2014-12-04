#ifndef BITSET_CPP
#define BITSET_CPP 1
#include "bitset.h"

#define LOOKUP_TABLE_BYTES 2

int bitCountLookupTable[1<<(8*LOOKUP_TABLE_BYTES)];
int highestBitLookupTable[1<<(8*LOOKUP_TABLE_BYTES)];
int lowestBitLookupTable[1<<(8*LOOKUP_TABLE_BYTES)];
StaticDummy staticDummyObject;

void initializeLookupTable()
{
    int bits,num,highest,lowest;
    for(int i=0;i<(1<<(8*LOOKUP_TABLE_BYTES));i++)
    {
        bits=i;
        num=0;
        highest=-1;
        lowest=-1;
        while(bits)
        {
            ++highest;
            if((bits%2) && num==0)
                lowest=highest;
            num+=bits%2;
            bits/=2;
        }
        bitCountLookupTable[i]=num;
        highestBitLookupTable[i]=highest;
        lowestBitLookupTable[i]=lowest;
    }
}

int bitCount(int bits)
{
    static const int mask=(1<<(8*LOOKUP_TABLE_BYTES))-1;
    static const int shiftmask=(1<<(8*(sizeof(int)-LOOKUP_TABLE_BYTES)))-1;
    int result=0;
    while(bits)
    {
        result+=bitCountLookupTable[bits&mask];
        bits>>=(8*LOOKUP_TABLE_BYTES);
        bits&=shiftmask;
    }
    return result;
}

int highestBit(int bits)
{
    static const int mask=(1<<(8*LOOKUP_TABLE_BYTES))-1;
    static const int shiftmask=(1<<(8*(sizeof(int)-LOOKUP_TABLE_BYTES)))-1;
    int offset=0;
    while(bits & ~mask)
    {
        offset+=8*LOOKUP_TABLE_BYTES;
        bits>>=(8*LOOKUP_TABLE_BYTES);
        bits&=shiftmask;
    }
    return highestBitLookupTable[bits]+offset;
}

int lowestBit(int bits)
{
    static const int mask=(1<<(8*LOOKUP_TABLE_BYTES))-1;
    static const int shiftmask=(1<<(8*(sizeof(int)-LOOKUP_TABLE_BYTES)))-1;
    int offset=0;
    while((bits & mask)==0)
    {
        offset+=8*LOOKUP_TABLE_BYTES;
        bits>>=(8*LOOKUP_TABLE_BYTES);
        bits&=shiftmask;
    }
    return lowestBitLookupTable[bits&mask]+offset;
}

StaticDummy::StaticDummy()
{
    initializeLookupTable();
}

const BitSetIterator& BitSetIterator::operator ++()
{
    int arrayIndex=current/(8*sizeof(int));
    currentArray&=~(1<<(current%(8*sizeof(int))));
    while(currentArray==0 && arrayIndex>0)
    {
        --arrayIndex;
        currentArray=set->content[arrayIndex];
    }
    current=highestBit(currentArray)+arrayIndex*8*sizeof(int);
    return *this;
}

BitSet::BitSet(int maxSize)
{
    nArrayLength=(maxSize/(sizeof(int)*8)+((maxSize%(sizeof(int)*8))!=0));
    nArraySizeInBits=nArrayLength*sizeof(int)*8;
    nSize=0;
    nMaxSize=maxSize;
    sizeChanged=false;
    content=new int[nArrayLength];
    for(int i=0;i<nArrayLength;++i)
        content[i]=0;
}

BitSet::BitSet(const BitSet &other)
{
    nSize=other.nSize;
    sizeChanged=other.sizeChanged;
    nArrayLength=other.nArrayLength;
    nArraySizeInBits=other.nArraySizeInBits;
    nMaxSize=other.nMaxSize;
    content=new int[nArrayLength];
    for(int i=0;i<nArrayLength;++i)
        content[i]=other.content[i];
}


BitSet::~BitSet()
{
    delete[] content;
}

void BitSet::add(int element)
{
    if(element<nMaxSize && element>=0)
    {
        int subArray=element/(sizeof(int)*8);
        int index=element%(sizeof(int)*8);
        if(!sizeChanged && (content[subArray]&(1<<index))==0)
            ++nSize;
        content[subArray]|=(1<<index);
    }
}

void BitSet::addM(int num,...)
{
    va_list valist;
    int arg;
    va_start(valist,num);
    for(int i=0;i<num;++i)
    {
        arg=va_arg(valist,int);
        add(arg);
    }
    va_end(valist);
}

void BitSet::remove(int element)
{
    if(element<nMaxSize && element>=0)
    {
        int subArray=element/(sizeof(int)*8);
        int index=element%(sizeof(int)*8);
        if(!sizeChanged &&(content[subArray]&(1<<index)))
            --nSize;
        content[subArray]&=~(1<<index);
    }
}

void BitSet::removeM(int num,...)
{
    va_list valist;
    int arg;
    va_start(valist,num);
    for(int i=0;i<num;++i)
    {
        arg=va_arg(valist,int);
        remove(arg);
    }
    va_end(valist);
}

bool BitSet::contains(int element) const
{
    if(element<nArraySizeInBits && element>=0)
    {
        int subArray=element/(sizeof(int)*8);
        int index=element%(sizeof(int)*8);
        return ((content[subArray]&(1<<index))!=0);
    }
    return false;
}

void BitSet::unite(const BitSet &other)
{
    /*if(other.nMaxSize>nMaxSize)
        nMaxSize=other.nMaxSize;
    if(other.nArrayLength>nArrayLength)
    {
        int* contentNew=new int[other.nArrayLength];
        int i;
        for(i=0;i<nArrayLength;++i)
            contentNew[i]=content[i];
        for(i=nArrayLength;i<other.nArrayLength;i++)
            contentNew[i]=other.content[i];
        delete[] content;
        content=contentNew;
        nArrayLength=other.nArrayLength;
        nMaxElements=nArrayLength*sizeof(int)*8;
    }*/
    for(int j=0;j<nArrayLength;++j)
        content[j]|=other.content[j];
    sizeChanged=true;
}


void BitSet::intersect(const BitSet &other)
{
    for(int j=0;j<nArrayLength;++j)
        content[j]&=other.content[j];
    sizeChanged=true;
}

void BitSet::subtract(const BitSet &other)
{
    for(int j=0;j<nArrayLength;++j)
        content[j]&=~other.content[j];
    sizeChanged=true;
}

void BitSet::symmDifference(const BitSet &other)
{
    for(int j=0;j<nArrayLength;++j)
        content[j]^=other.content[j];
    sizeChanged=true;
}

void BitSet::copy(const BitSet &other)
{
    delete[] content;
    nSize=other.nSize;
    sizeChanged=other.sizeChanged;
    nArrayLength=other.nArrayLength;
    nArraySizeInBits=other.nArraySizeInBits;
    nMaxSize=other.nMaxSize;
    content=new int[nArrayLength];
    for(int i=0;i<nArrayLength;++i)
        content[i]=other.content[i];
}

bool BitSet::operator ==(const BitSet& other) const
{
    if(nArrayLength!=other.nArrayLength || nMaxSize!=other.nMaxSize)
        return false;
    for(int i=0;i<nArrayLength;++i)
    {
        if(content[i]!=other.content[i])
            return false;
    }
    return true;
}

void BitSet::complement()
{
    for(int j=0;j<nArrayLength;++j)
        content[j]=~content[j];
    if(nMaxSize<nArraySizeInBits)
        content[nArrayLength-1]&=((1<<(nMaxSize%(8*sizeof(int))))-1);
    if(!sizeChanged)
        nSize=nMaxSize-nSize;
}

void BitSet::reCalcSize()
{
    nSize=0;
    for(int i=0;i<nArrayLength;++i)
        nSize+=bitCount(content[i]);
    sizeChanged=false;
}

int BitSet::operator [](int index)
{
    if(index<size() && index>=0)
    {
        ++index;
        int i=0,j=bitCount(content[0]),k=8*sizeof(int),l=k/2,mask=-1;
        while(index>j && i<nArrayLength-1)
        {
            index-=j;
            ++i;
            j=bitCount(content[i]);
        }
        while(index!=j)
        {
            k= index<j ? k-l : k+l;
            if(l>1)
                l/=2;
            mask=(1<<k)-1;
            j=bitCount(content[i]&mask);
        }
        return highestBit(content[i]&mask)+8*sizeof(int)*i;
    }
    return -1;
}

int BitSet::highest() const
{
    for(int i=nArrayLength-1;i>=0;--i)
    {
        if(content[i])
            return highestBit(content[i])+i*8*sizeof(int);
    }
    return -1;
}

int BitSet::lowest() const
{
    for(int i=0;i<nArrayLength;++i)
    {
        if(content[i])
            return lowestBit(content[i])+i*8*sizeof(int);
    }
    return -1;
}

int BitSet::indexOf(int element) const
{
    if(element<nArraySizeInBits && element>=0)
    {
        int subArray=element/(sizeof(int)*8);
        int index=element%(sizeof(int)*8);
        if(content[subArray]&(1<<index))
        {
            int result=bitCount(content[subArray]&((1<<index)-1));
            for(int i=subArray-1;i>=0;--i)
                result+=bitCount(content[i]);
            return result;
        }
    }
    return -1;
}

BitSet::operator QString() const
{
    QString str="";
    for(int i:*this)
    {
        str=","+QString::number(i)+str;
    }
    str="{"+str.remove(0,1)+"}";
    return str;
}

void BitSet::setEmpty()
{
    for(int i=0;i<nArrayLength;++i)
        content[i]=0;
    nSize=0;
    sizeChanged=false;
}

void BitSet::setFull()
{
    for(int i=0;i<nArrayLength-1;++i)
        content[i]=-1;
    int j=nMaxSize%(8*sizeof(int));
    if(j)
        content[nArrayLength-1]=(1<<j)-1;
    else
        content[nArrayLength-1]=-1;
    nSize=nMaxSize;
    sizeChanged=false;
}

bool BitSet::empty()
{
    return size()==0;
}

bool BitSet::full()
{
    return size()==nMaxSize;
}

void BitSet::writeToArray(int *array) const
{
    int j=-1;
    for(int i:*this)
        array[++j]=i;
}

void BitSet::readFromArray(int *array, int size)
{
    setEmpty();
    for(int i=0;i<size;++i)
        add(array[i]);
}

void BitSet::setRange(int lower, int upper, int step)
{
    setEmpty();
    for(int i=lower;i<=upper;i+=step)
        add(i);
}

void BitSet::resize(int newMaxSize, bool preserve)
{
    int nNewArrayLength=(newMaxSize/(sizeof(int)*8)+((newMaxSize%(sizeof(int)*8))!=0));
    if(nNewArrayLength!=nArrayLength)
    {
        int* newContent=new int[nNewArrayLength];
        int minLength=nNewArrayLength>nArrayLength?nArrayLength:nNewArrayLength;
        if(preserve)
        {
            for(int i=0;i<minLength;++i)
                newContent[i]=content[i];
            for(int i=minLength;i<nNewArrayLength;++i)
                newContent[i]=0;
        }
        else
        {
            for(int i=0;i<nNewArrayLength;++i)
                newContent[i]=0;
            nSize=0;
            sizeChanged=false;
        }
        delete[] content;
        content=newContent;
        nArrayLength=nNewArrayLength;
        nArraySizeInBits=nNewArrayLength*sizeof(int)*8;
    }
    if(preserve && newMaxSize<nMaxSize)
    {
        content[nArrayLength-1]&=(1<<(newMaxSize%(sizeof(int)*8)))-1;
        sizeChanged=true;
    }
    nMaxSize=newMaxSize;
}

#endif
