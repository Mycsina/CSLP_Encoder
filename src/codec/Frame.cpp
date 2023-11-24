#include "Frame.hpp"

using namespace std;
using namespace cv;

MotionVector::MotionVector() : x(0), y(0) {}
MotionVector::MotionVector(int x, int y) : x(x), y(y) {}
ostream &operator<<(ostream &os, const MotionVector &vector) {
    os << "x: " << vector.x << " y: " << vector.y;
    return os;
}

ostream &operator<<(ostream &os, const vector<MotionVector> &vectors) {
    for (const auto &vector: vectors) {
        os << vector << endl;
    }
    return os;
}

Block::Block(const Image &img, int size, int row, int col) {
    size_ = size;
    row_ = row;
    col_ = col;
    block_mat_ = img.getSlice(row, col, size);
}

const cv::Mat &Block::getBlockMat() const {
    return block_mat_;
}

void Block::setBlockMat(const cv::Mat &blockMat) {
    block_mat_ = blockMat;
}

int Block::getSize() const {
    return size_;
}

int Block::getRow() const {
    return row_;
}

int Block::getCol() const {
    return col_;
}

array<int, 4> Block::getVertices() const {
    return {col_, row_, col_ + size_, row_ + size_};
}

Block::MAD::MAD(int threshold) {
    best_score = INFINITY;
    best_match = {0, 0};
    threshold = threshold;
}
double Block::MAD::block_diff(const Block &a, const Block &b) {
    int diff = 0;
    for (int i = 0; i < a.size_; i++) {
        for (int j = 0; j < a.size_; j++) {
            diff += abs(a.block_mat_.at<Vec3b>(i, j)[0] - b.block_mat_.at<Vec3b>(i, j)[0]);
            diff += abs(a.block_mat_.at<Vec3b>(i, j)[1] - b.block_mat_.at<Vec3b>(i, j)[1]);
            diff += abs(a.block_mat_.at<Vec3b>(i, j)[2] - b.block_mat_.at<Vec3b>(i, j)[2]);
        }
    }
    return floor(diff / (a.size_ * a.size_));
}
bool Block::MAD::isBetter(double score) {
    return score < best_score;
}

Block::MSE::MSE(int threshold) {
    best_score = INFINITY;
    best_match = {0, 0};
    threshold = threshold;
}
double Block::MSE::block_diff(const Block &a, const Block &b) {
    double diff = 0;
    for (int i = 0; i < a.size_; i++) {
        for (int j = 0; j < a.size_; j++) {
            diff += pow(a.block_mat_.at<Vec3b>(i, j)[0] - b.block_mat_.at<Vec3b>(i, j)[0], 2);
            diff += pow(a.block_mat_.at<Vec3b>(i, j)[1] - b.block_mat_.at<Vec3b>(i, j)[1], 2);
            diff += pow(a.block_mat_.at<Vec3b>(i, j)[2] - b.block_mat_.at<Vec3b>(i, j)[2], 2);
        }
    }
    return floor(diff / (a.size_ * a.size_));
}
bool Block::MSE::isBetter(double score) {
    return score < best_score;
}

Block::PSNR::PSNR(int threshold) {
    best_score = INFINITY;
    best_match = {0, 0};
    threshold = threshold;
}
double Block::PSNR::block_diff(const Block &a, const Block &b) {
    double mse_metric = MSE().block_diff(a, b);
    if (mse_metric == 0) return INFINITY;
    return 10 * log10(pow(255, 2) / mse_metric);
}
bool Block::PSNR::isBetter(double score) {
    return score > best_score;
}

Block::SAD::SAD(int threshold) {
    best_score = INFINITY;
    best_match = {0, 0};
    threshold = threshold;
}
double Block::SAD::block_diff(const Block &a, const Block &b) {
    double diff = 0;
    for (int i = 0; i < a.size_; i++) {
        for (int j = 0; j < a.size_; j++) {
            diff += abs(a.block_mat_.at<Vec3b>(i, j)[0] - b.block_mat_.at<Vec3b>(i, j)[0]);
            diff += abs(a.block_mat_.at<Vec3b>(i, j)[1] - b.block_mat_.at<Vec3b>(i, j)[1]);
            diff += abs(a.block_mat_.at<Vec3b>(i, j)[2] - b.block_mat_.at<Vec3b>(i, j)[2]);
        }
    }
    return diff;
}
bool Block::SAD::isBetter(double score) {
    return score > best_score;
}

bool Block::isLeftEdge() const {
    return col_ == 0;
}

Block get_block(const Image &img, int size, int row, int col) {
    return {img, size, row, col};
}

Frame::Frame(const Image &img) {
    image_ = img;
    block_diff_ = new Block::SAD();
    motion_vectors_ = vector<MotionVector>();
}
Image Frame::getImage() const { return image_; }

bool Frame::isBlockDiff(Block::BlockDiff *blockDiff) const {
    if (block_diff_ == nullptr)
        return false;
    return block_diff_ == blockDiff;
}

void Frame::setBlockDiff(Block::BlockDiff *blockDiff) {
    if (!isBlockDiff(blockDiff))
        block_diff_ = blockDiff;
}


std::vector<MotionVector> Frame::getMotionVectors() const {
    return motion_vectors_;
}

const vector<int> &Frame::getIntraEncoding() const {
    return intra_encoding;
}

FrameType Frame::getType() const {
    return type_;
}

void Frame::setType(FrameType type) {
    type_ = type;
}

void Frame::show() {
    imshow("Frame", *image_.getImageMat());
    waitKey(0);
}

void Frame::encode_JPEG_LS() {
    type_ = I_FRAME;
    Mat image_mat_ = *image_.getImageMat();
    for (int r = 0; r < image_mat_.rows; r++) {
        for (int c = 0; c < image_mat_.cols; c++) {
            for (int channel = 0; channel < image_mat_.channels(); channel++) {
                int real = (int) image_mat_.at<Vec3b>(r, c)[channel];
                int predicted = (int) predict_JPEG_LS(image_mat_, r, c, channel);
                int diff = real - predicted;
                intra_encoding.push_back(diff);
            }
        }
    }
}

void Frame::encode_JPEG_LS(Golomb *g) {
    type_ = I_FRAME;
    Mat image_mat_ = *image_.getImageMat();
    for (int r = 0; r < image_mat_.rows; r++) {
        for (int c = 0; c < image_mat_.cols; c++) {
            for (int channel = 0; channel < image_mat_.channels(); channel++) {
                int real = (int) image_mat_.at<Vec3b>(r, c)[channel];
                int predicted = (int) predict_JPEG_LS(image_mat_, r, c, channel);
                int diff = real - predicted;
                g->encode(diff);
            }
        }
    }
}

void Frame::write_JPEG_LS(Golomb *g) {
    for (auto diff: intra_encoding) {
        g->encode(diff);
    }
}

Frame Frame::decode_JPEG_LS(Golomb *g, COLOR_SPACE c_space, CHROMA_SUBSAMPLING cs_ratio, int rows, int cols) {
    Mat mat;
    if (c_space == GRAY) {
        mat = Mat::zeros(rows, cols, CV_8UC1);
    } else {
        mat = Mat::zeros(rows, cols, CV_8UC3);
    }

    for (int r = 0; r < mat.rows; r++) {
        for (int c = 0; c < mat.cols; c++) {
            for (int channel = 0; channel < mat.channels(); channel++) {
                auto diff = g->decode();
                uchar predicted = Image::predict_JPEG_LS(mat, r, c, channel);
                uchar real = diff + predicted;
                if (mat.channels() > 1) {
                    mat.at<Vec3b>(r, c)[channel] = real;
                } else {
                    mat.at<uchar>(r, c) = real;
                }
            }
        }
    }
    Image im(mat);
    im.setColor(c_space);
    im.setChroma(cs_ratio);
    return Frame(im);
}

Frame Frame::decode_JPEG_LS(vector<int> encodings, COLOR_SPACE c_space, CHROMA_SUBSAMPLING cs_ratio, int rows, int cols) {
    Mat mat;
    if (c_space == GRAY) {
        mat = Mat::zeros(rows, cols, CV_8UC1);
    } else {
        mat = Mat::zeros(rows, cols, CV_8UC3);
    }
    int i = 0;
    for (int r = 0; r < mat.rows; r++) {
        for (int c = 0; c < mat.cols; c++) {
            for (int channel = 0; channel < mat.channels(); channel++) {
                uchar diff = encodings[i++];
                uchar predicted = Image::predict_JPEG_LS(mat, r, c, channel);
                uchar real = diff + predicted;
                if (mat.channels() > 1) {
                    mat.at<Vec3b>(r, c)[channel] = real;
                } else {
                    mat.at<uchar>(r, c) = real;
                }
            }
        }
    }
    Image im(mat);
    im.setColor(c_space);
    im.setChroma(cs_ratio);
    return Frame(im);
}

uchar Frame::predict_JPEG_LS(Mat mat, int row, int col, int channel) {
    if (row < 0 || row >= mat.rows || col < 0 || col >= mat.cols) {
        throw std::out_of_range("Pixel out of bounds");
    }

    uchar a, b, c;
    if (mat.channels() > 1) {
        if (row - 1 >= 0 && col >= 1) {
            a = mat.at<Vec3b>(row, col - 1)[channel];
            b = mat.at<Vec3b>(row - 1, col)[channel];
            c = mat.at<Vec3b>(row - 1, col - 1)[channel];
        } else if (row - 1 >= 0) {
            a = 0;
            b = mat.at<Vec3b>(row - 1, col)[channel];
            c = 0;
        } else if (col - 1 >= 0) {
            a = mat.at<Vec3b>(row, col - 1)[channel];
            b = 0;
            c = 0;
        } else {
            a = 0;
            b = 0;
            c = 0;
        }
    } else {
        if (row - 1 >= 0 && col >= 1) {
            a = mat.at<uchar>(row, col - 1);
            b = mat.at<uchar>(row - 1, col);
            c = mat.at<uchar>(row - 1, col - 1);
        } else if (row - 1 >= 0) {
            a = 0;
            b = mat.at<uchar>(row - 1, col);
            c = 0;
        } else if (col - 1 >= 0) {
            a = mat.at<uchar>(row, col - 1);
            b = 0;
            c = 0;
        } else {
            a = 0;
            b = 0;
            c = 0;
        }
    }


    if (c >= std::max(a, b)) {
        return std::min(a, b);
    } else if (c <= std::min(a, b)) {
        return std::max(a, b);
    } else {
        return a + b - c;
    }
}

std::array<int, 4> Frame::get_search_window(const Block &block, int search_radius) const {
    // block reference is top-left corner, so we need to account for that when calculating the search window
    array<int, 4> block_coords = block.getVertices();
    int x1 = max(block_coords[0] - search_radius, 0);
    int y1 = max(block_coords[1] - search_radius, 0);
    int x2 = min(block_coords[0] + search_radius, image_.size()[1] - block.getSize());
    int y2 = min(block_coords[1] + search_radius, image_.size()[0] - block.getSize());
    return {x1, y1, x2, y2};
}

std::array<Point, 5> Frame::get_rood_points(Point center, int arm_size, int block_size) const {
    // center is top left corner of block
    Point up = {center.x, max(center.y - arm_size, 0)};
    Point down = {center.x, min(center.y + arm_size, image_.size()[0] - block_size)};
    Point right = {min(center.x + arm_size, image_.size()[1] - block_size), center.y};
    Point left = {max(center.x - arm_size, 0), center.y};
    cout << "For point " << center << " the rood points are: " << endl;
    cout << up << endl;
    cout << left << endl;
    cout << down << endl;
    cout << right << endl;
    if (motion_vectors_.empty())
        return {up, right, down, left, center};
    Point MV_prediction = {center.x + motion_vectors_.back().x, center.y + motion_vectors_.back().y};
    return {up, right, down, left, MV_prediction};
}

bool Block::BlockDiff::compare(const Block &block, Frame *reference, cv::Point center) {
    auto block_coords = block.getVertices();
    Block ref_block = get_block(reference->getImage(), block.getSize(), center.y, center.x);
    double diff_value = block_diff(block, ref_block);
    if (isBetter(diff_value)) {
        MotionVector mv = {center.x - block_coords[0], center.y - block_coords[1]};
        mv.residual = Mat::zeros(block.getBlockMat().size(), CV_16SC3);
        for (int i = 0; i < mv.residual.rows; i++)
            for (int j = 0; j < mv.residual.cols; j++) {
                mv.residual.at<Vec3s>(i, j)[0] = block.getBlockMat().at<Vec3b>(i, j)[0] - ref_block.getBlockMat().at<Vec3b>(i, j)[0];
                mv.residual.at<Vec3s>(i, j)[1] = block.getBlockMat().at<Vec3b>(i, j)[1] - ref_block.getBlockMat().at<Vec3b>(i, j)[1];
                mv.residual.at<Vec3s>(i, j)[2] = block.getBlockMat().at<Vec3b>(i, j)[2] - ref_block.getBlockMat().at<Vec3b>(i, j)[2];
            }
        best_score = diff_value;
        previous_best = best_match;
        best_match = mv;
    }
    if (diff_value <= threshold)
        return true;
    return false;
}

void Block::BlockDiff::reset() {
    best_score = INFINITY;
    best_match = {0, 0};
}
void Block::PSNR::reset() {
    best_score = -(double) INFINITY;
    best_match = {0, 0};
}

MotionVector Frame::match_block_es(const Block &block, Frame *reference, int search_radius) {
    bool finished;
    block_diff_->reset();
    finished = block_diff_->compare(block, reference, {block.getCol(), block.getRow()});
    if (finished)
        return block_diff_->best_match;
    auto block_coords = block.getVertices();
    auto search_bounds = get_search_window(block, search_radius);
    int left = search_bounds[0];
    int upper = search_bounds[1];
    int right = search_bounds[2];
    int down = search_bounds[3];
    for (int i = upper; i < down; i++) {
        for (int j = left; j < right; j++) {
#ifdef _VISUALIZE
            Mat canvas = this->getImage().getImageMat()->clone();
            rectangle(canvas, Point(block_coords[0], block_coords[1]), Point(block_coords[2], block_coords[3]), Scalar(255, 255, 255));
            rectangle(canvas, Point(j, i), Point(j + block.getSize(), i + block.getSize()), Scalar(0, 0, 255));
            imshow("Canvas", canvas);
            waitKey(1);
#endif
            finished = block_diff_->compare(block, reference, {j, i});
            if (finished)
                return block_diff_->best_match;
        }
    }
    return block_diff_->best_match;
}


MotionVector Frame::match_block_arps(const Block &block, Frame *reference, int threshold) {
    throw runtime_error("This function mustn't be used");
    bool finished;
    block_diff_->reset();
    double self_sad = block_diff_->compare(block, reference, {block.getCol(), block.getRow()});
    if (self_sad == block_diff_->threshold)
        return block_diff_->best_match;
    auto block_coords = block.getVertices();
    int size;
    if (block.isLeftEdge())
        size = 2;
    else
        size = max(motion_vectors_.back().x, motion_vectors_.back().y);
    array<Point, 5> initial_points;
    if (size == 0)
        initial_points = {Point(block_coords[0], block_coords[1]), {0, 0}, {0, 0}, {0, 0}, {0, 0}};
    initial_points = get_rood_points({block_coords[0], block_coords[1]}, size, block.getSize());
    for (auto point: initial_points) {
#ifdef _VISUALIZE
        Mat canvas = this->getImage().getImageMat()->clone();
        rectangle(canvas, Point(block_coords[0], block_coords[1]), Point(block_coords[2], block_coords[3]), Scalar(255, 255, 255));
        rectangle(canvas, Point(point.x, point.y), Point(point.x + block.getSize(), point.y + block.getSize()), Scalar(0, 0, 255));
        imshow("Canvas", canvas);
        waitKey(1);
#endif
        finished = block_diff_->compare(block, reference, point);
        if (finished)
            return block_diff_->best_match;
    }
    do {
        auto new_points = get_rood_points({block_coords[0] + block_diff_->best_match.x, block_coords[1] + block_diff_->best_match.y}, size, block.getSize());
        for (auto point: new_points) {
#ifdef _VISUALIZE
            Mat canvas = this->getImage().getImageMat()->clone();
            rectangle(canvas, Point(block_coords[0], block_coords[1]), Point(block_coords[2], block_coords[3]), Scalar(255, 255, 255));
            rectangle(canvas, Point(point.x, point.y), Point(point.x + block.getSize(), point.y + block.getSize()), Scalar(0, 0, 255));
            imshow("Canvas", canvas);
            waitKey(1);
#endif
            finished = block_diff_->compare(block, reference, point);
        }
    } while (!finished);
    return block_diff_->best_match;
}

void Frame::calculate_MV(Frame *reference, int block_size, int search_radius, bool fast) {
    type_ = P_FRAME;
    setBlockDiff(new Block::MSE());
    for (int i = 0; i + block_size <= image_.size()[0]; i += block_size) {
        for (int j = 0; j + block_size <= image_.size()[1]; j += block_size) {
            Block block = get_block(image_, block_size, i, j);
            MotionVector mv;
            if (fast) {
                mv = match_block_arps(block, reference);
            } else {
                block_diff_->threshold = 0;
                mv = match_block_es(block, reference, search_radius);
            }
            motion_vectors_.push_back(mv);
        }
    }
}

Frame Frame::reconstruct_frame(Frame *reference, const vector<MotionVector> &motion_vectors, int block_size) {
    Mat reconstructed = Mat::zeros(reference->getImage().size()[0], reference->getImage().size()[1], CV_8UC3);
    for (int i = 0; i + block_size <= reference->getImage().size()[0]; i += block_size) {
        for (int j = 0; j + block_size <= reference->getImage().size()[1]; j += block_size) {
            MotionVector mv = motion_vectors[i / block_size * (reference->getImage().size()[1] / block_size) + j / block_size];
            Block block = get_block(reference->getImage(), block_size, i, j);
            Mat reconstructed_block = Mat::zeros(block.getBlockMat().size(), CV_8UC3);
            for (int k = 0; k < block.getBlockMat().rows; k++)
                for (int l = 0; l < block.getBlockMat().cols; l++) {
                    reconstructed_block.at<Vec3b>(k, l)[0] = block.getBlockMat().at<Vec3b>(k, l)[0] + mv.residual.at<Vec3s>(k, l)[0];
                    reconstructed_block.at<Vec3b>(k, l)[1] = block.getBlockMat().at<Vec3b>(k, l)[1] + mv.residual.at<Vec3s>(k, l)[1];
                    reconstructed_block.at<Vec3b>(k, l)[2] = block.getBlockMat().at<Vec3b>(k, l)[2] + mv.residual.at<Vec3s>(k, l)[2];
                }
            block.setBlockMat(reconstructed_block);
            setSlice(reconstructed, block.getBlockMat(), i, j);
        }
    }
    Image im(reconstructed);
    Frame frame(im);
    frame.setType(P_FRAME);
    return frame;
}

void Frame::visualize_MV(Frame *reference, int block_size) {
    int i = 0;
    int j = 0;
    Mat res = Mat::zeros(reference->getImage().size()[0], reference->getImage().size()[1], CV_8UC3);
    for (const auto &v: motion_vectors_) {
        setSlice(res, v.residual, j, i);
        arrowedLine(res, Point(i + block_size / 2, j + block_size / 2), Point(i + v.x + block_size / 2, j + v.y + block_size / 2), Scalar(0, 0, 255), 1, 8, 0);
        i += block_size;
        if (i >= res.cols) {
            i = 0;
            j += block_size;
        }
    };
    imshow("res", res);
    waitKey(0);
}

void Frame::encode_inter(Golomb *g, Frame *reference, int search_radius,int block_size){
    calculate_MV(reference,block_size,search_radius,false);
    for(const auto& mv: getMotionVectors()){
        g->encode(mv.x);
        g->encode(mv.y);
        Mat residual=mv.residual;
        for(int row=0;row<residual.rows;row++){
            for(int col=0;col<residual.cols;col++){
                for(int channel=0;channel<residual.channels();channel++){
                    g->encode((int)residual.at<Vec3s>(row,col)[channel]);
                }
            }
        }
    }
}

Frame Frame::decode_inter(Golomb *g, Frame *reference, COLOR_SPACE c_space, CHROMA_SUBSAMPLING cs_ratio, int rows,int cols, int search_radius, int block_size) {
    vector<MotionVector> mvs;
    for(int block_num=0;block_num<(rows/block_size)*(cols/block_size);block_num++){
        MotionVector mv;
        Mat residual;
        if(c_space==GRAY){
            residual=Mat::zeros(block_size,block_size,CV_16SC1);
        }else{
            residual=Mat::zeros(block_size,block_size,CV_16SC3);
        }
        mv.x=g->decode();
        mv.y=g->decode();
        for(int row=0;row<block_size;row++){
            for(int col=0;col<block_size;col++){
                for(int channel=0;channel<residual.channels();channel++){
                    residual.at<Vec3s>(row,col)[channel]=(short)g->decode();
                }
            }
        }
        mv.residual=residual;
        mvs.push_back(mv);
    }
    return Frame::reconstruct_frame(reference,mvs,block_size);
}