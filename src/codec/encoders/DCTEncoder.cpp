#include "DCTEncoder.hpp"

using namespace std;
using namespace cv;

#ifndef PI
#define PI 3.14159265358979324
#endif

DCTEncoder::DCTEncoder(const char *src, const char *dst, const uint8_t golomb_m) : src(src), dst(dst), golomb_m(golomb_m) {}
DCTEncoder::DCTEncoder(const char *src, const char *dst) : src(src), dst(dst), golomb_m(0) {}

DCTEncoder::~DCTEncoder(){
    delete[] y_qmat;
    delete[] uv_qmat;
    delete[] zigzag_order;
}

void DCTEncoder::dct8x8(int (&in)[8][8], double (&out)[8][8]) {
    double sum;
    for(int I=0; I<8; I++){
        for(int II=0; II<8; II++){
            sum=0;
            for (int r=0; r<8; r++){
                for (int c=0; c<8; c++){
                    sum+=in[r][c] * cos((2*r+1)*I*PI/16) * cos((2*c+1)*II*PI/16) * ((I==0)?1/sqrt(2):1) * ((II==0)?1/sqrt(2):1); //I got this formula from the internet as Fourier transform is complicated
                }
            }
            out[I][II]=sum/4;
        }
    }
}

void DCTEncoder::idct8x8(double (&in)[8][8], int (&out)[8][8]) {
    double sum;

    for(int I=0; I<8; I++){
        for(int II=0; II<8; II++){
            sum=0;
            for (int r=0; r<8; r++){
                for (int c=0; c<8; c++){
                    sum+=in[r][c] * cos((2*I+1)*c*PI/16) * cos((2*II+1)*v*PI/16) * ((r==0)?1/sqrt(2):1) * ((c==0)?1/sqrt(2):1); //I got this formula from the internet as Fourier transform is complicated
                }
            }
            out[I][II]=(int)sum/4;
        }
    }
}