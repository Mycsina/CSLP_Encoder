#include "Golomb.hpp"
#include <iostream>
#include <bitset>
#include <string>
#include <cmath>

using namespace std;

int Golomb::decode(){
    if(m<=0){
        m=bs->readBits(8);
    }
    int q=readUnary();
    int r=readBinaryTrunc();
    return q*m+r;
}

void Golomb::encode(int n){
    if(m<=0){
        throw std::invalid_argument("value of m unknown or invalid");
    }
    int r=n%m;
    int q=n/m;
    writeUnary(q);
    writeBinaryTrunc(r);
}

void Golomb::encode(int n, int m_){
    if(m<=0){
        m=m_;
        bs->writeBits(m_,8);
    }
    encode(n);
}

int Golomb::readUnary(){
    string buffer="";
    int bit_=-1;
    while(bit_!=0){
        std::cout << bs->readBit();
        buffer.append(std::bitset<1>(bit_).to_string());
    }
    return stoi(buffer,nullptr,2);
}

void Golomb::writeUnary(int n) {
    for(int i=0;i<n;i++){
        bs->writeBit(1);
    }
    bs->writeBit(0);
}

int Golomb::readBinaryTrunc() {
    int k=floor(log2(m));
    int u= (1<<(k+1))-m;
    int k_bits=bs->readBits(k);
    if(k_bits<u){
        return k_bits;
    }else{
        return ((k_bits<<1)+bs->readBit())-u;
    }
}

void Golomb::writeBinaryTrunc(int n){
    int k=floor(log2(m));
    int u = (1 << (k + 1)) - m;
    string temp; //because I can't just write the string directly

    if (n < u)
        temp=toBinary(n, k);
    else
        temp=toBinary(n + u, k + 1);

    for(int i=0;i<temp.length();i++){
        bs->writeBit(int(temp[i]));
    }
}

string Golomb::toBinary(int n, int len){
    string buffer = "";
    while (n != 0) {
        if (n%2==0)
            buffer = '0' + buffer;
        else  buffer = '1' + buffer;

        n >>= 1;
    }
    while (buffer.length() < len)
        buffer = '0' + buffer;
    return buffer;
}



