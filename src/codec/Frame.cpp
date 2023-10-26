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

Frame::Frame(Image img) {
    image_ = img;
    frame_mat_ = img._get_image_mat()->clone();
    previous_ = nullptr;
    next_ = nullptr;
}
Image Frame::getImage() const { return image_; }
Mat *Frame::getFrameMat() { return &frame_mat_; }
void Frame::setFrameMat(const Mat &frameMat) { frame_mat_ = frameMat; }
Frame *Frame::getPrevious() const { return previous_; }

void Frame::setPrevious(Frame *previous) {
    previous_ = previous;
    if (previous->getNext() != this)
        previous->setNext(this);
}

Frame *Frame::getNext() const {
    return next_;
}

void Frame::setNext(Frame *next) {
    next_ = next;
    if (next->getPrevious() != this)
        next->setPrevious(this);
}

void Frame::display_frame() {
    imshow("Frame", frame_mat_);
    waitKey(0);
}

void Frame::display_frame_original() {
    imshow("Frame", *image_._get_image_mat());
    waitKey(0);
}

Blocks get_block(const Image &img, int size, int row, int col) {
    return {img, size, row, col};
}

Mat Frame::get_difference() {
    Mat diff;
    absdiff(*image_._get_image_mat(), frame_mat_, diff);
    return diff;
}

double Blocks::compare_block_mal(const Blocks &other) const {
    int diff = 0;
    for (int i = 0; i < size_; i++) {
        for (int j = 0; j < size_; j++) {
            diff += abs(block_mat_.at<Vec3b>(i, j)[0] - other.getBlockMat().at<Vec3b>(i, j)[0]);
            diff += abs(block_mat_.at<Vec3b>(i, j)[1] - other.getBlockMat().at<Vec3b>(i, j)[1]);
            diff += abs(block_mat_.at<Vec3b>(i, j)[2] - other.getBlockMat().at<Vec3b>(i, j)[2]);
        }
    }
    return floor(diff / (size_ * size_));
}

double Blocks::compare_block_mse(const Blocks &other) const {
    double diff = 0;
    for (int i = 0; i < size_; i++) {
        for (int j = 0; j < size_; j++) {
            diff += pow(block_mat_.at<Vec3b>(i, j)[0] - other.getBlockMat().at<Vec3b>(i, j)[0], 2);
            diff += pow(block_mat_.at<Vec3b>(i, j)[1] - other.getBlockMat().at<Vec3b>(i, j)[1], 2);
            diff += pow(block_mat_.at<Vec3b>(i, j)[2] - other.getBlockMat().at<Vec3b>(i, j)[2], 2);
        }
    }
    return floor(diff / (size_ * size_));
}

double Blocks::compare_block_psnr(const Blocks &other) const {
    double mse = compare_block_mse(other);
    if (mse == 0) return INFINITY;
    return 10 * log10(pow(255, 2) / mse);
}

// TODO currently search is right biased, fix this
MotionVector Frame::match_block_es(const Blocks &block, Frame *reference, int search_radius) {
    MotionVector best_match;
    double best_mse = -(double) INFINITY;
    Blocks future_self = get_block(reference->getImage(), block.getSize(), block.getRow(), block.getCol());
    double self_mse = block.compare_block_mse(future_self);
    if (self_mse == 0)
        return best_match;
    int block_coords[] = {block.getCol(), block.getRow(), block.getSize() + block.getCol(), block.getSize() + block.getRow()};
    int left = max(block_coords[0] - search_radius, 0);
    int upper = max(block_coords[1] - search_radius, 0);
    int right = min(block_coords[2] + search_radius, image_.size()[1] - block.getSize());
    int down = min(block_coords[3] + search_radius, image_.size()[0] - block.getSize());
    for (int i = upper; i < down; i++) {
        for (int j = left; j < right; j++) {
            // TODO preprocessor macro for demo
            Mat canvas = this->getImage()._get_image_mat()->clone();
            rectangle(canvas, Point(block_coords[0], block_coords[1]), Point(block_coords[2], block_coords[3]), Scalar(255, 255, 255));
            rectangle(canvas, Point(j, i), Point(j + block.getSize(), i + block.getSize()), Scalar(0, 0, 255));
            imshow("Canvas", canvas);
            waitKey(1);
            Blocks ref_block = get_block(reference->getImage(), block.getSize(), i, j);
            double mse = block.compare_block_mse(ref_block);
            MotionVector mv = {j - block_coords[0], i - block_coords[1]};
            if (mse == 0)
                return mv;
            if (best_mse < mse) {
                best_mse = mse;
                best_match = mv;
            }
        }
    }
    return best_match;
}

MotionVector Frame::match_block_arps(const Blocks &block, Frame *reference, int search_radius) {
    return {0, 0};
}

vector<MotionVector> Frame::match_all_blocks(int block_size, int n, int search_radius, bool fast) {
    Frame *reference = this;
    for (int i = 0; i < n; i++) {
        reference = reference->getPrevious();
    }
    vector<MotionVector> motion_vectors;
    for (int i = 0; i < image_.size()[0] - block_size; i += block_size) {
        for (int j = 0; j < image_.size()[1] - block_size; j += block_size) {
            Blocks block = get_block(image_, block_size, i, j);
            MotionVector mv;
            if (fast) {
                mv = match_block_arps(block, reference, search_radius);
            } else {
                mv = match_block_es(block, reference, search_radius);
            }
            motion_vectors.push_back(mv);
        }
    }
    return motion_vectors;
}
Frame Frame::reconstruct_frame(Frame *reference, const vector<MotionVector> &motion_vectors) {
    return Frame();
}
