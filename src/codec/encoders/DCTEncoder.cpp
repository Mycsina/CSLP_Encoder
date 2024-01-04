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

void DCTEncoder::encode(){
    BitStream bs(dst, ios::out);
    Golomb g(&bs);
    Video vid(src);
    const vector<Frame *> frames = vid.generate_frames();
    const Frame sample = *frames[0];
    header.extract_info(sample);
    header.golomb_m = golomb_m;
    header.length = frames.size();
    header.block_size = 8;
    header.write_header(bs);
    g.set_m(golomb_m);

    for(Image img: vid.get_reel()){
        encode_frame(&img,&g);
    }
}

void DCTEncoder::encode_frame(Image *im, Golomb *g) {
    RLEEncoder rle(g);
    Mat *image_mat=im->get_image_mat();

    //for each channel
    for(int channel=0;channel<image_mat->channels();channel++){
        //for each block
        for(int row=0;row<(*image_mat).rows;row+=8){
            for(int col=0;col<(*image_mat).cols;col+=8){
                int block[8][8];
                double dct_matrix[8][8];
                //copy the block to a 8x8 int matrix
                for(int br=0;br<8;br++){
                    for(int bc=0;bc<8;bc++){
                        block[br][bc]=image_mat->at<Vec3b>(row+br,col+bc)[channel];
                    }
                }
                //get the dct of it into dct_matrix
                dct8x8(block,dct_matrix);

                //scan the dct_matrix in zigzag, do element-wise division and save using RLE (and golomb)
                for(int i=0;i<64;i++){
                    int q_val=1,zz_r,zz_c,result;
                    double dct_val;
                    zz_r=zigzag_order[i][0];
                    zz_c=zigzag_order[i][1];
                    dct_val=dct_matrix[zz_r][zz_c];
                    if(channel==0){
                        q_val=y_qmat[zz_r][zz_c];
                    }else{
                        q_val=uv_qmat[zz_r][zz_c];
                    }
                    result=(int)dct_val/q_val;
                    rle.push(result);
                }
            }
        }
    }
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
                    sum+=in[r][c] * cos((2*I+1)*r*PI/16) * cos((2*II+1)*c*PI/16) * ((r==0)?1/sqrt(2):1) * ((c==0)?1/sqrt(2):1); //I got this formula from the internet as Fourier transform is complicated
                }
            }
            out[I][II]=(int)sum/4;
        }
    }
}

void DCTEncoder::decode() {
    BitStream bs(src, ios::in);
    Golomb g(&bs);
    header=InterHeader::read_header(bs);

    g.set_m(header.golomb_m);
    RLEEncoder rle(&g);

    int rows=header.width;
    int cols=header.height;
    CHROMA_SUBSAMPLING cs=header.chroma_subsampling;
    COLOR_SPACE color = header.color_space;

    for(int i=0;i<header.length;i++){
        frames.push_back(decode_frame(&rle,&header));
    }
}

Frame DCTEncoder::decode_frame(RLEEncoder *rle, Header *h){
    Image im;
    int rows=header.height;
    int cols=header.width;
    Mat mat;

    im.set_color(header.color_space);
    im.set_chroma(header.chroma_subsampling);

    if(header.color_space==GRAY) {
        mat = Mat::zeros(rows, cols, CV_8UC1);
    }else{
        mat=Mat::zeros(rows,cols,CV_8UC3);
    }

    //for each channel
    for(int channel=0;channel<mat.channels();channel++){
        //for each block
        for(int row=0;row<mat.rows;row+=8){
            for(int col=0;col<mat.cols;col+=8){
                int block[8][8];
                double dct_matrix[8][8];


                for(int i=0;i<64;i++){
                    int r=zigzag_order[i][0]; //row to put value in
                    int c=zigzag_order[i][1]; //col to put value in
                    dct_matrix[r][c]=rle->pop();
                }

                //reverse the dct into block
                idct8x8(dct_matrix,block);

                //put the block into mat
                for(int br=0;br<8;br++){
                    for(int bc=0;bc<8;bc++){
                        mat.at<Vec3b>(row+br,col+bc)[channel]=block[br][bc];
                    }
                }
            }
        }
    }

    im.set_image_mat(mat);
    return Frame(im);
}