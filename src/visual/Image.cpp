#include "Image.hpp"

using namespace std;
using namespace cv;

Image *Image::load(const Mat &arr) {
    image_mat_ = arr.clone();
    return this;
}

void Image::load(const char *filename, ImreadModes mode) {
    Mat image, conv;
    // By default, cv:imread loads images in BGR format
    image = imread(filename, mode);
    c_space = BGR;
    if (!image.empty()) {
        image.convertTo(conv, CV_8UC3);
        image_mat_ = conv;
    } else {
        throw std::runtime_error("Image was not loaded");
    }
}

void Image::save(const char *filename, const vector<int> &compression_params) {
    if (loaded()) {
        imwrite(filename, image_mat_, compression_params);
    } else {
        throw std::runtime_error("Image hasn't been loaded");
    }
}

void Image::display_image(bool vid_ctx) {
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

Vec3b Image::get_pixel(int row, int col) const {
    if (loaded()) {
        if (row < 0 || row >= image_mat_.rows || col < 0 ||
            col >= image_mat_.cols) {
            throw std::runtime_error("Pixel out of bounds");
        }
        Vec3b color_values = image_mat_.at<Vec3b>(row, col);
        return color_values;
    }
    throw std::runtime_error("Image hasn't been loaded");
}

void Image::set_pixel(int row, int col, const Vec3b &color_values) {
    if (row < 0 || row >= image_mat_.rows || col < 0 || col >= image_mat_.cols) {
        throw std::runtime_error("Pixel out of bounds");
    }
    image_mat_.at<Vec3b>(row, col) = color_values;
}

Image Image::clone() {
    if (loaded()) {
        Image clone;
        clone.image_mat_ = image_mat_.clone();
        return clone;
    } else
        throw std::runtime_error("Image hasn't been loaded");
}

vector<Mat> Image::color_histograms(int bins, bool fill_hist, int width,
                                    int height) {
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
            int bin_w = cvRound((double) width / 256);
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
                        Vec3b color = {(uchar) hist.color[0], (uchar) hist.color[1],
                                       (uchar) hist.color[2]};
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
    Mat *m = im._get_image_mat();
    int totalBlur = int(sum(blur)[0]);
    int radiusR, radiusC;

    // Check if the blur matrix has proper dimensions
    if (blur.rows % 2 == 0 || blur.cols % 2 == 0) {
        throw std::invalid_argument("blur matrix must have odd rows and columns");
    }
    radiusR = blur.rows / 2;
    radiusC = blur.cols / 2;

    // iterate every pixel to apply the filter
    image_mat_.forEach<Vec3b>([&blur, radiusC, radiusR, m, this,
                               totalBlur](Vec3b &pixel, const int *position) {
        int row = position[0];
        int col = position[1];

        Mat blur_temp = cut(blur, row, col);                 // cut the blur filter as needed
        Mat temp = get_neighbors(radiusR, radiusC, row, col);// get the neighbors

        temp = temp.mul(blur_temp);
        cv::Scalar sum = cv::sum(temp);

        // normalize
        for (int a = 0; a < sum.rows; a++) {
            pixel[a] = (int) (sum[a] / totalBlur);
        }
        m->at<Vec3b>(row, col) = pixel;
    });
    return im;
}


Mat Image::get_neighbors(int radiusR, int radiusC, int r, int c) const {
    if (r < 0 || r >= image_mat_.rows || c < 0 || c >= image_mat_.cols) {
        throw std::out_of_range("Pixel out of bounds");
    }
    // get the min and max values, making sure to stay in bounds
    int rMin = max(0, r - radiusR);
    int rMax = min(image_mat_.rows - 1, r + radiusR);
    int cMin = max(0, c - radiusC);
    int cMax = min(image_mat_.cols - 1, c + radiusC);

    return image_mat_(Range(rMin, rMax + 1), Range(cMin, cMax + 1)).clone();
}

Mat Image::cut(const Mat &m, int row, int col) const {
    int centerRow = m.rows / 2;
    int centerCol = m.cols / 2;
    // check distance in image_mat from this pixel to the edge
    int distUp = row;
    int distDown = (image_mat_.rows - 1) - row;
    int distLeft = col;
    int distRight = (image_mat_.cols - 1) - col;

    // check for need to cut
    int rMin = max((centerRow - distUp), 0);
    int rMax = min((centerRow + distDown), m.rows - 1);
    int cMin = max((centerCol - distLeft), 0);
    int cMax = min((centerCol + distRight), m.cols - 1);

    return m(Range(rMin, rMax + 1), Range(cMin, cMax + 1));
}

Mat Image::get_slice(int row, int col, int size) const {
    if (row < 0 || row + size > image_mat_.rows || col < 0 || col + size > image_mat_.cols) {
        throw std::out_of_range("Slice out of bounds");
    }
    return image_mat_(Rect(col, row, size, size));
}
