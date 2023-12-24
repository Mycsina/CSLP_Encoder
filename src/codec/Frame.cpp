#include "Frame.hpp"

using namespace std;
using namespace cv;

MotionVector::MotionVector() : x(0), y(0) {}
MotionVector::MotionVector(const int x, const int y) : x(x), y(y) {}
bool MotionVector::operator==(const MotionVector &rhs) const {
    return x == rhs.x && y == rhs.y;
}
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

Block::Block(const Image &img, const int size, const int row, const int col) {
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
bool Block::MAD::isBetter(const double score) {
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
bool Block::MSE::isBetter(const double score) {
    return score < best_score;
}

Block::PSNR::PSNR(int threshold) {
    best_score = INFINITY;
    best_match = {0, 0};
    threshold = threshold;
}
double Block::PSNR::block_diff(const Block &a, const Block &b) {
    const double mse_metric = MSE().block_diff(a, b);
    if (mse_metric == 0) return INFINITY;
    return 10 * log10(pow(255, 2) / mse_metric);
}
bool Block::PSNR::isBetter(const double score) {
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
bool Block::SAD::isBetter(const double score) {
    return score < best_score;
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
Image Frame::get_image() const { return image_; }

bool Frame::is_block_diff(const Block::BlockDiff *blockDiff) const {
    if (block_diff_ == nullptr)
        return false;
    return block_diff_ == blockDiff;
}

void Frame::set_block_diff(Block::BlockDiff *blockDiff) {
    if (!is_block_diff(blockDiff))
        block_diff_ = blockDiff;
}


std::vector<MotionVector> Frame::get_motion_vectors() const {
    return motion_vectors_;
}

const vector<int> &Frame::get_intra_encoding() const {
    return intra_encoding;
}

FrameType Frame::get_type() const {
    return type_;
}

void Frame::setType(const FrameType type) {
    type_ = type;
}

void Frame::show() {
    imshow("Frame", *image_.get_image_mat());
    waitKey(0);
}

void Frame::encode_JPEG_LS() {
    type_ = I_FRAME;
    Mat image_mat_ = *image_.get_image_mat();
    for (int r = 0; r < image_mat_.rows; r++) {
        for (int c = 0; c < image_mat_.cols; c++) {
            for (int channel = 0; channel < image_mat_.channels(); channel++) {
                const int real = image_mat_.at<Vec3b>(r, c)[channel];
                const int predicted = predict_JPEG_LS(image_mat_, r, c, channel);
                int diff = real - predicted;
                intra_encoding.push_back(diff);
            }
        }
    }
}

void Frame::encode_JPEG_LS(Golomb *g) {
    type_ = I_FRAME;
    Mat image_mat_ = *image_.get_image_mat();
    for (int r = 0; r < image_mat_.rows; r++) {
        for (int c = 0; c < image_mat_.cols; c++) {
            for (int channel = 0; channel < image_mat_.channels(); channel++) {
                const int real = image_mat_.at<Vec3b>(r, c)[channel];
                const int predicted = predict_JPEG_LS(image_mat_, r, c, channel);
                const int diff = real - predicted;
                g->encode(diff);
            }
        }
    }
}

void Frame::write_JPEG_LS(Golomb *g) const {
    for (const auto diff: intra_encoding) {
        g->encode(diff);
    }
}

Frame Frame::decode_JPEG_LS(Golomb *g, const Header header) {
    Mat mat;
    if (header.color_space == GRAY) {
        mat = Mat::zeros(header.height, header.width, CV_8UC1);
    } else {
        mat = Mat::zeros(header.height, header.width, CV_8UC3);
    }
    for (int r = 0; r < mat.rows; r++) {
        for (int c = 0; c < mat.cols; c++) {
            for (int channel = 0; channel < mat.channels(); channel++) {
                const auto diff = g->decode();
                const uchar predicted = predict_JPEG_LS(mat, r, c, channel);
                const uchar real = diff + predicted;
                if (mat.channels() > 1) {
                    mat.at<Vec3b>(r, c)[channel] = real;
                } else {
                    mat.at<uchar>(r, c) = real;
                }
            }
        }
    }
    Image im(mat);
    im.set_color(header.color_space);
    im.set_chroma(header.chroma_subsampling);
    return Frame(im);
}

Frame Frame::decode_JPEG_LS(const vector<int> &encodings, const COLOR_SPACE c_space, const CHROMA_SUBSAMPLING cs_ratio, const int rows, const int cols) {
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
                const uchar diff = encodings[i++];
                const uchar predicted = predict_JPEG_LS(mat, r, c, channel);
                const uchar real = diff + predicted;
                if (mat.channels() > 1) {
                    mat.at<Vec3b>(r, c)[channel] = real;
                } else {
                    mat.at<uchar>(r, c) = real;
                }
            }
        }
    }
    Image im(mat);
    im.set_color(c_space);
    im.set_chroma(cs_ratio);
    return Frame(im);
}

uchar Frame::predict_JPEG_LS(Mat mat, const int row, const int col, const int channel) {
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
    }
    if (c <= std::min(a, b)) {
        return std::max(a, b);
    }
    return a + b - c;
}

std::array<int, 4> Frame::get_search_window(const Block &block, const int search_radius) const {
    // Block reference is top-left corner, so we need to account for that when calculating the search window
    const array<int, 4> block_coords = block.getVertices();
    const int x1 = max(block_coords[0] - search_radius, 0);
    const int y1 = max(block_coords[1] - search_radius, 0);
    const int x2 = min(block_coords[0] + search_radius, image_.size().width - block.getSize());
    const int y2 = min(block_coords[1] + search_radius, image_.size().height - block.getSize());
    return {x1, y1, x2, y2};
}

vector<Point> Frame::get_rood_points(const Point center, const int arm_size, const int block_size) const {
    // center is top left corner of block
    const Point up = {center.x, max(center.y - arm_size, 0)};
    const Point down = {center.x, min(center.y + arm_size, image_.size().height - block_size)};
    const Point right = {min(center.x + arm_size, image_.size().width - block_size), center.y};
    const Point left = {max(center.x - arm_size, 0), center.y};
    if (motion_vectors_.empty() || arm_size == 1)
        return {up, right, down, left, center};
    const Point MV_prediction = {center.x + motion_vectors_.back().x, center.y + motion_vectors_.back().y};
    return {up, right, down, left, MV_prediction};
}

bool Block::BlockDiff::compare(const Block &block, const Frame *reference, const Point center) {
    const auto block_coords = block.getVertices();
    const Block ref_block = get_block(reference->get_image(), block.getSize(), center.y, center.x);
    const double diff_value = block_diff(block, ref_block);
    if (isBetter(diff_value)) {
        MotionVector mv = {center.x - block_coords[0], center.y - block_coords[1]};
        mv.residual = Mat::zeros(block.getBlockMat().size(), CV_16SC3);
        for (int i = 0; i < mv.residual.rows; i++)
            for (int j = 0; j < mv.residual.cols; j++) {
                mv.residual.at<Vec3s>(i, j)[0] = block.getBlockMat().at<Vec3b>(i, j)[0] - ref_block.getBlockMat().at<Vec3b>(i, j)[0];
                mv.residual.at<Vec3s>(i, j)[1] = block.getBlockMat().at<Vec3b>(i, j)[1] - ref_block.getBlockMat().at<Vec3b>(i, j)[1];
                mv.residual.at<Vec3s>(i, j)[2] = block.getBlockMat().at<Vec3b>(i, j)[2] - ref_block.getBlockMat().at<Vec3b>(i, j)[2];
            }
        double best_score = diff_value;
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
    best_score = -static_cast<double>(INFINITY);
    best_match = {0, 0};
}

MotionVector Frame::match_block_es(const Block &block, const Frame *reference, const int search_radius) const {
    block_diff_->reset();
    bool finished = block_diff_->compare(block, reference, {block.getCol(), block.getRow()});
    if (finished)
        return block_diff_->best_match;
    auto block_coords = block.getVertices();
    const auto search_bounds = get_search_window(block, search_radius);
    const int left = search_bounds[0];
    const int upper = search_bounds[1];
    const int right = search_bounds[2];
    const int down = search_bounds[3];
    for (int i = upper; i < down; i++) {
        for (int j = left; j < right; j++) {
#ifdef _VISUALIZE
            Mat canvas = this->get_image().get_image_mat()->clone();
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


MotionVector Frame::match_block_arps(const Block &block, Frame *reference) const {
    bool finished = false;
    vector<Point> visited;
    block_diff_->reset();
    auto block_coords = block.getVertices();
    int size;
    if (block.isLeftEdge())
        size = 2;
    else
        size = max(abs(motion_vectors_.back().x), abs(motion_vectors_.back().y));
    vector<Point> initial_points;
    if (size == 0)
        initial_points = {Point(block_coords[0], block_coords[1])};
    initial_points = get_rood_points({block_coords[0], block_coords[1]}, size, block.getSize());
    for (auto point: initial_points) {
#ifdef _VISUALIZE
        Mat canvas = this->get_image().get_image_mat()->clone();
        rectangle(canvas, Point(block_coords[0], block_coords[1]), Point(block_coords[2], block_coords[3]), Scalar(255, 255, 255));
        rectangle(canvas, Point(point.x, point.y), Point(point.x + block.getSize(), point.y + block.getSize()), Scalar(0, 0, 255));
        imshow("Canvas", canvas);
        waitKey(1);
#endif
        if (find(visited.begin(), visited.end(), point) != visited.end())
            continue;
        finished = block_diff_->compare(block, reference, point);
        visited.push_back(point);
        if (finished)
            return block_diff_->best_match;
    }
    MotionVector mv = block_diff_->best_match;
    do {
        auto new_points = get_rood_points({block_coords[0] + block_diff_->best_match.x, block_coords[1] + block_diff_->best_match.y}, 1, block.getSize());
        int found = new_points.size();
        for (auto point: new_points) {
#ifdef _VISUALIZE
            Mat canvas = this->get_image().get_image_mat()->clone();
            rectangle(canvas, Point(block_coords[0], block_coords[1]), Point(block_coords[2], block_coords[3]), Scalar(255, 255, 255));
            rectangle(canvas, Point(point.x, point.y), Point(point.x + block.getSize(), point.y + block.getSize()), Scalar(0, 0, 255));
            imshow("Canvas", canvas);
            waitKey(1);
#endif
            if (find(visited.begin(), visited.end(), point) != visited.end()) {
                found--;
                if (found == 0)
                    return block_diff_->best_match;
                continue;
            }
            finished = block_diff_->compare(block, reference, point);
            visited.push_back(point);
        }
    } while (!finished && !(mv == block_diff_->best_match));
    return block_diff_->best_match;
}

void Frame::calculate_MV(Frame *reference, const int block_size, const int search_radius, const bool fast) {
    type_ = P_FRAME;
    vector<MotionVector> motion_vectors;
    for (int i = 0; i + block_size <= image_.size().height; i += block_size) {
        for (int j = 0; j + block_size <= image_.size().width; j += block_size) {
            Block block = get_block(image_, block_size, i, j);
            MotionVector mv;
            if (block_diff_ == nullptr)
                block_diff_ = new Block::SAD();
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
    Mat reconstructed = Mat::zeros(reference->get_image().size(), CV_8UC3);
    for (int i = 0; i + block_size <= reference->get_image().size().height; i += block_size) {
        for (int j = 0; j + block_size <= reference->get_image().size().width; j += block_size) {
            MotionVector mv = motion_vectors[i / block_size * (reference->get_image().size().width / block_size) + j / block_size];
            Block block = get_block(reference->get_image(), block_size, i + mv.y, j + mv.x);
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

void Frame::visualize_MV(const Frame *reference, const int block_size) const {
    int i = 0;
    int j = 0;
    Mat res = Mat::zeros(reference->get_image().size(), CV_8UC3);
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

void Frame::write(Golomb *g) const {
    for (const auto &mv: get_motion_vectors()) {
        g->encode(mv.x);
        g->encode(mv.y);
        Mat residual = mv.residual;
        for (int row = 0; row < residual.rows; row++) {
            for (int col = 0; col < residual.cols; col++) {
                for (int channel = 0; channel < residual.channels(); channel++) {
                    g->encode(residual.at<Vec3s>(row, col)[channel]);
                }
            }
        }
    }
}

Frame Frame::decode_inter(Golomb *g, Frame *reference, InterHeader header) {
    vector<MotionVector> mvs;
    const int block_size = header.block_size;
    const int rows = header.height;
    const int cols = header.width;
    for (int block_num = 0; block_num < (rows / block_size) * (cols / block_size); block_num++) {
        MotionVector mv;
        Mat residual;
        if (header.color_space == GRAY) {
            residual = Mat::zeros(block_size, block_size, CV_16SC1);
        } else {
            residual = Mat::zeros(block_size, block_size, CV_16SC3);
        }
        mv.x = g->decode();
        mv.y = g->decode();
        for (int row = 0; row < block_size; row++) {
            for (int col = 0; col < block_size; col++) {
                for (int channel = 0; channel < residual.channels(); channel++) {
                    residual.at<Vec3s>(row, col)[channel] = static_cast<short>(g->decode());
                }
            }
        }
        mv.residual = residual;
        mvs.push_back(mv);
    }
    return reconstruct_frame(reference, mvs, block_size);
}