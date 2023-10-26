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
    parent_ = img;
    size_ = size;
    row_ = row;
    col_ = col;
    block_mat_ = img.get_slice(row, col, size);
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
Block::BlockDiff* Block::MAD::reset(int threshold) {
    return new MAD(threshold);
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
void Block::MAD::update(double score, const MotionVector &vector) {
    if (score < best_score) {
        previous_best = best_match;
        best_score = score;
        best_match = vector;
    }
}

Block::MSE::MSE(int threshold) {
    best_score = INFINITY;
    best_match = {0, 0};
    threshold = threshold;
}
Block::BlockDiff* Block::MSE::reset(int threshold) {
    return new MSE(0);
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
void Block::MSE::update(double score, const MotionVector &vector) {
    if (score < best_score) {
        previous_best = best_match;
        best_score = score;
        best_match = vector;
    }
}

Block::PSNR::PSNR(int threshold) {
    best_score = INFINITY;
    best_match = {0, 0};
    threshold = threshold;
}
Block::BlockDiff* Block::PSNR::reset(int threshold) {
    return new PSNR(threshold);
}
double Block::PSNR::block_diff(const Block &a, const Block &b) {
    double mse_metric = MSE().block_diff(a, b);
    if (mse_metric == 0) return INFINITY;
    return 10 * log10(pow(255, 2) / mse_metric);
}
void Block::PSNR::update(double score, const MotionVector &vector) {
    if (score > best_score) {
        best_score = score;
        best_match = vector;
    }
}

Block::SAD::SAD(int threshold) {
    best_score = INFINITY;
    best_match = {0, 0};
    threshold = threshold;
}
Block::BlockDiff* Block::SAD::reset(int threshold) {
    return new SAD(threshold);
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
void Block::SAD::update(double score, const MotionVector &vector) {
    if (score < best_score) {
        previous_best = best_match;
        best_score = score;
        best_match = vector;
    }
}

bool Block::isLeftEdge() const {
    return col_ == 0;
}

Block get_block(const Image &img, int size, int row, int col) {
    return {img, size, row, col};
}

Frame::Frame(Image img) {
    image_ = img;
    frame_mat_ = img._get_image_mat()->clone();
    previous_ = nullptr;
    next_ = nullptr;
    block_diff_ = new Block::SAD();
    motion_vectors_ = vector<MotionVector>();
}
Image Frame::getImage() const { return image_; }
Mat *Frame::getFrameMat() { return &frame_mat_; }

bool Frame::isBlockDiff(Block::BlockDiff *blockDiff) const {
    if (block_diff_ == nullptr)
        return false;
    return block_diff_ == blockDiff;
}

void Frame::setBlockDiff(Block::BlockDiff *blockDiff) {
    if (!isBlockDiff(blockDiff))
        block_diff_ = blockDiff;
    if (next_ != nullptr)
        next_->setBlockDiff(blockDiff);
}

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

std::vector<MotionVector> Frame::getMotionVectors() const {
    return motion_vectors_;
}

void Frame::display_frame() {
    imshow("Frame", frame_mat_);
    waitKey(0);
}

void Frame::display_frame_original() {
    imshow("Frame", *image_._get_image_mat());
    waitKey(0);
}

Mat Frame::get_difference() {
    Mat diff;
    absdiff(*image_._get_image_mat(), frame_mat_, diff);
    return diff;
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
    MotionVector mv = {center.x - block_coords[0], center.y - block_coords[1]};
    update(diff_value, mv);
    cout << "Threshold: " << threshold << endl;
    cout << "Best match: " << best_score << endl;
    cout << "Best vector: " << best_match << endl;
    if (diff_value <= threshold)
        return true;
    return false;
}

MotionVector Frame::match_block_es(const Block &block, Frame *reference, int search_radius) {
    bool finished;
    block_diff_ = block_diff_->reset(block_diff_->threshold);
    finished = block_diff_->compare(block, reference, {block.getCol(), block.getRow()});
    if (finished)
        return block_diff_->best_match;;
    auto block_coords = block.getVertices();
    auto search_bounds = get_search_window(block, search_radius);
    int left = search_bounds[0];
    int upper = search_bounds[1];
    int right = search_bounds[2];
    int down = search_bounds[3];
    for (int i = upper; i < down; i++) {
        for (int j = left; j < right; j++) {
            // TODO preprocessor macro for demo
            Mat canvas = this->getImage()._get_image_mat()->clone();
            rectangle(canvas, Point(block_coords[0], block_coords[1]), Point(block_coords[2], block_coords[3]), Scalar(255, 255, 255));
            rectangle(canvas, Point(j, i), Point(j + block.getSize(), i + block.getSize()), Scalar(0, 0, 255));
            imshow("Canvas", canvas);
            waitKey(1);
            // TODO end
            finished = block_diff_->compare(block, reference, {j, i});
            if (finished)
                break;
        }
    }
    return block_diff_->best_match;
}


MotionVector Frame::match_block_arps(const Block &block, Frame *reference, int threshold) {
    bool finished;
    block_diff_ = block_diff_->reset(block_diff_->threshold);
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
        // TODO preprocessor macro for demo
        Mat canvas = this->getImage()._get_image_mat()->clone();
        rectangle(canvas, Point(block_coords[0], block_coords[1]), Point(block_coords[2], block_coords[3]), Scalar(255, 255, 255));
        rectangle(canvas, Point(point.x, point.y), Point(point.x + block.getSize(), point.y + block.getSize()), Scalar(0, 0, 255));
        imshow("Canvas", canvas);
        waitKey(1);
        // TODO end
        finished = block_diff_->compare(block, reference, point);
        if (finished)
            return block_diff_->best_match;
    }
    do {
        auto new_points = get_rood_points({block_coords[0] + block_diff_->best_match.x, block_coords[1] + block_diff_->best_match.y}, size, block.getSize());
        for (auto point: new_points) {
            // TODO preprocessor macro for demo
            Mat canvas = this->getImage()._get_image_mat()->clone();
            rectangle(canvas, Point(block_coords[0], block_coords[1]), Point(block_coords[2], block_coords[3]), Scalar(255, 255, 255));
            rectangle(canvas, Point(point.x, point.y), Point(point.x + block.getSize(), point.y + block.getSize()), Scalar(0, 0, 255));
            imshow("Canvas", canvas);
            waitKey(1);
            // TODO end
            finished = block_diff_->compare(block, reference, point);
        }
    } while (!finished);
    return block_diff_->best_match;
}

void Frame::match_all_blocks(int block_size, int n, int search_radius, bool fast) {
    Frame *reference = this;
    setBlockDiff(new Block::MSE());
    for (int i = 0; i < n; i++) {
        reference = reference->getPrevious();
    }
    vector<MotionVector> motion_vectors;
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

Frame Frame::reconstruct_frame(Frame *reference, const vector<MotionVector> &motion_vectors) {
    return Frame();
}