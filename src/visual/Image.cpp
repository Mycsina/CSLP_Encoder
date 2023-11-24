#include "Image.hpp"
#include <iostream>

using namespace std;
using namespace cv;

Histogram::Histogram(const int bins) {
    bins_ = bins;
    mat_ = Mat::zeros(bins_, 1, CV_32SC1);
}

Image::Image(const Mat &arr2d) {
    load(arr2d);
    c_space = BGR;
}
Image::Image(const char *filename) {
    load(filename);
    c_space = BGR;
}

Image *Image::load(const Mat &arr2d) {
    image_mat_ = arr2d.clone();
    return this;
}
void Image::set_image_mat(Mat mat) {
    image_mat_ = std::move(mat);
}
Mat *Image::get_image_mat() {
    return &image_mat_;
}
void Image::set_color(const COLOR_SPACE col) { c_space = col; }
COLOR_SPACE Image::get_color() const { return c_space; }
void Image::set_chroma(const CHROMA_SUBSAMPLING cs) { cs_ratio = cs; }
CHROMA_SUBSAMPLING Image::get_chroma() const { return cs_ratio; }
void Image::load(const char *filename, const ImreadModes mode) {
    // By default, cv:imread loads images in BGR format
    const Mat image = imread(filename, mode);
    c_space = BGR;
    if (!image.empty()) {
        Mat conv;
        image.convertTo(conv, CV_8UC3);
        image_mat_ = conv;
    } else {
        throw std::runtime_error("Image was not loaded");
    }
}
MatIterator_<Vec3b> Image::begin() { return image_mat_.begin<Vec3b>(); }
MatIterator_<Vec3b> Image::end() { return image_mat_.end<Vec3b>(); }
array<int, 2> Image::size() const { return {image_mat_.rows, image_mat_.cols}; }
int Image::getImageType() const { return image_mat_.type(); }
bool Image::loaded() const { return !image_mat_.empty(); }

void Image::save(const char *filename, const vector<int> &compression_params) const {
    if (loaded()) {
        imwrite(filename, image_mat_, compression_params);
    } else {
        throw std::runtime_error("Image hasn't been loaded");
    }
}

void Image::show(const bool vid_ctx) const {
    if (loaded()) {
        imshow("Image", image_mat_);
        if (!vid_ctx)
            waitKey(0);
        else {
            waitKey(25);
        }
    } else {
        throw std::runtime_error("Image hasn't been loaded");
    }
}

Vec3b Image::getPixel(const int row, const int col) const {
    if (loaded()) {
        if (row < 0 || row >= image_mat_.rows || col < 0 ||
            col >= image_mat_.cols) {
            throw std::out_of_range("Pixel out of bounds");
        }
        Vec3b color_values = image_mat_.at<Vec3b>(row, col);
        return color_values;
    }
    throw std::runtime_error("Image hasn't been loaded");
}

void Image::setPixel(const int row, const int col, const Vec3b &color_values) {
    if (row < 0 || row >= image_mat_.rows || col < 0 || col >= image_mat_.cols) {
        throw std::runtime_error("Pixel out of bounds");
    }
    image_mat_.at<Vec3b>(row, col) = color_values;
}

Image Image::clone() const {
    if (loaded()) {
        Image clone;
        clone.image_mat_ = image_mat_.clone();
        return clone;
    } else
        throw std::runtime_error("Image hasn't been loaded");
}

bool Image::operator==(Image &other) const {
    return norm(image_mat_, *other.get_image_mat(), cv::NORM_L2) == 0;
}

vector<Mat> Image::color_histograms(const int bins, const bool fill_hist, const int width,
                                    const int height) const {
    if (loaded()) {
        vector<Mat> histograms;
        vector<Mat> channels;
        // Array of colors for each channel
        Scalar colors[] = {Scalar(255, 0, 0), Scalar(0, 255, 0), Scalar(0, 0, 255)};
        split(image_mat_, channels);
        // If image is grayscale, set color to white
        if (channels.size() == 1) {
            colors[0] = Scalar(255, 255, 255);
        }
        for (int i = 0; i < channels.size(); i++) {
            Histogram hist = Histogram(bins);
            hist.color = colors[i];
            Mat backMat = histogram<uchar>(channels[i], bins);
            const int bin_w = cvRound(static_cast<double>(width) / 256);
            Vec3b backColor = (0, 0, 0);

            Mat histImage(height, width, CV_8UC3, backColor);

            normalize(backMat, backMat, 0, histImage.rows, NORM_MINMAX, -1, Mat());

            for (int j = 1; j < 256; j++) {
                line(histImage,
                     Point(bin_w * (j - 1), height - cvRound(backMat.at<float>(j - 1))),
                     Point(bin_w * (j), height - cvRound(backMat.at<float>(j))),
                     hist.color, 2, 8, 0);
            }

            if (fill_hist) {
                for (int j = 0; j < histImage.cols; j++) {
                    Mat col = histImage.col(j);
                    for (int k = col.rows - 1; k >= 0; k--) {
                        // Fill column with color until a non-<backColor> pixel is found
                        if (col.at<Vec3b>(k) != backColor) {
                            break;
                        }
                        const Vec3b color = {static_cast<uchar>(hist.color[0]), static_cast<uchar>(hist.color[1]),
                                             static_cast<uchar>(hist.color[2])};
                        col.at<Vec3b>(k) = color;
                        // If we reach end of column without finding a non-<backColor> pixel
                        // zero the column
                        if (k == 0) {
                            col = Mat::zeros(col.rows, 1, CV_8UC3);
                        }
                    }
                }
            }

            histograms.push_back(histImage);
        }
        return histograms;
    } else
        throw std::runtime_error("Image hasn't been loaded");
}

Image Image::gaussian_blur(cv::Mat blur) {
    Image im = this->clone();
    Mat *m = im.get_image_mat();
    int totalBlur = static_cast<int>(sum(blur)[0]);

    // Check if the blur matrix has proper dimensions
    if (blur.rows % 2 == 0 || blur.cols % 2 == 0) {
        throw std::invalid_argument("blur matrix must have odd rows and columns");
    }
    int radiusR = blur.rows / 2;
    int radiusC = blur.cols / 2;

    // iterate every pixel to apply the filter
    image_mat_.forEach<Vec3b>([&blur, radiusC, radiusR, m, this,
                               totalBlur](Vec3b &pixel, const int *position) {
        const int row = position[0];
        const int col = position[1];

        const Mat blur_temp = cut(blur, row, col);           // cut the blur filter as needed
        Mat temp = get_neighbors(radiusR, radiusC, row, col);// get the neighbors

        temp = temp.mul(blur_temp);
        Scalar sum = cv::sum(temp);

        // normalize
        for (int a = 0; a < Scalar::rows; a++) {
            pixel[a] = static_cast<int>(sum[a] / totalBlur);
        }
        m->at<Vec3b>(row, col) = pixel;
    });
    return im;
}


Mat Image::get_neighbors(const int radiusR, const int radiusC, const int r, const int c) const {
    if (r < 0 || r >= image_mat_.rows || c < 0 || c >= image_mat_.cols) {
        throw std::out_of_range("Pixel out of bounds");
    }
    // get the min and max values, making sure to stay in bounds
    const int rMin = max(0, r - radiusR);
    const int rMax = min(image_mat_.rows - 1, r + radiusR);
    const int cMin = max(0, c - radiusC);
    const int cMax = min(image_mat_.cols - 1, c + radiusC);

    return image_mat_(Range(rMin, rMax + 1), Range(cMin, cMax + 1)).clone();
}

Mat Image::cut(const Mat &m, const int row, const int col) const {
    const int center_row = m.rows / 2;
    const int center_col = m.cols / 2;
    // check distance in image_mat from this pixel to the edge
    const int dist_up = row;
    const int dist_down = (image_mat_.rows - 1) - row;
    const int dist_left = col;
    const int dist_right = (image_mat_.cols - 1) - col;

    // check for need to cut
    const int r_min = max(center_row - dist_up, 0);
    const int r_max = min(center_row + dist_down, m.rows - 1);
    const int c_min = max(center_col - dist_left, 0);
    const int c_max = min(center_col + dist_right, m.cols - 1);

    return m(Range(r_min, r_max + 1), Range(c_min, c_max + 1));
}

Mat Image::getSlice(const int row, const int col, const int size) const {
    if (row < 0 || row + size > image_mat_.rows || col < 0 || col + size > image_mat_.cols) {
        throw std::out_of_range("Slice out of bounds");
    }
    return image_mat_(Rect(col, row, size, size));
}

void setSlice(const Mat &mat, const Mat &slice, const int row, const int col) {
    if (row < 0 || row + slice.rows > mat.rows || col < 0 || col + slice.cols > mat.cols) {
        throw std::out_of_range("Slice out of bounds");
    }
    slice.copyTo(mat(Rect(col, row, slice.cols, slice.rows)));
}

void Image::encode_JPEG_LS(const std::string &path, const int m = 4) {
    auto *bs = new BitStream(path, std::ios::out);

    Golomb g(bs);

    bs->writeBits(c_space, 4);
    bs->writeBits(cs_ratio, 4);
    bs->writeBits(image_mat_.cols, 8 * sizeof(int));
    bs->writeBits(image_mat_.rows, 8 * sizeof(int));
    bs->writeBits(m, 8 * sizeof(int));
    g.set_m(m);

    encode_JPEG_LS(&g);
    delete bs;
}

void Image::encode_JPEG_LS(Golomb *g) {
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

Image Image::decode_JPEG_LS(const std::string &path) {
    ifstream file;
    file.open(path);
    if (!file) {
        file.close();
        throw std::runtime_error("File does not exist");
    }
    file.close();

    BitStream bs(path, std::ios::in);
    Golomb g(&bs);

    //read header
    auto c_space = static_cast<COLOR_SPACE>(bs.readBits(4));
    auto cs_ratio = static_cast<CHROMA_SUBSAMPLING>(bs.readBits(4));
    int cols = bs.readBits(8 * sizeof(int));
    int rows = bs.readBits(8 * sizeof(int));
    int m = bs.readBits(8 * sizeof(int));
    g.set_m(m);

    return decode_JPEG_LS(&g, c_space, cs_ratio, rows, cols);
}

Image Image::decode_JPEG_LS(Golomb *g, const COLOR_SPACE c_space, const CHROMA_SUBSAMPLING cs_ratio, const int rows, const int cols) {
    Mat mat;
    if (c_space == GRAY) {
        mat = Mat::zeros(rows, cols, CV_8UC1);
    } else {
        mat = Mat::zeros(rows, cols, CV_8UC3);
    }

    for (int r = 0; r < mat.rows; r++) {
        for (int c = 0; c < mat.cols; c++) {
            for (int channel = 0; channel < mat.channels(); channel++) {
                const auto diff = static_cast<uchar>(g->decode());
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
    return im;
}

uchar Image::predict_JPEG_LS(Mat mat, const int row, const int col, const int channel = 0) {
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
