#include "ImageProcessing.hpp"
#include <iostream>
#include <opencv2/highgui.hpp>

using namespace std;
using namespace cv;

void watermark(Image &im, Image mark, const Point2i coord1, const Point2i coord2, const double alpha) {
    const Mat image_mat = *im.get_image_mat();
    Mat markMat = *mark.get_image_mat();
    const int height = coord2.y - coord1.y;
    const int width = coord2.x - coord1.x;
    // Resize the watermark to fit the desired area
    InterpolationFlags flag;
    if (height < markMat.rows && width < markMat.cols) {
        flag = INTER_AREA;
    } else {
        flag = INTER_LINEAR;
    }
    resize(markMat, markMat, Size(width, height), 0, 0, flag);
    // Blend the watermark with the Image
    Mat roi = image_mat(Rect(coord1.x, coord1.y, width, height));
    addWeighted(roi, alpha, markMat, 1.0 - alpha, 0.0, roi);
}

Image convert_BGR_YUV444(Image &im) {
    auto image_mat_ = *im.get_image_mat();
    const int rows = image_mat_.rows;
    const int cols = image_mat_.cols;

    Mat yuv(rows, cols, CV_8UC3);

    image_mat_.forEach<Vec3b>([&yuv](const Vec3b &pixel, const int *position) {
        const int row = position[0];
        const int col = position[1];

        const double b = pixel.val[0];
        const double g = pixel.val[1];
        const double r = pixel.val[2];

        const double y = 0.299 * r + 0.587 * g + 0.114 * b;
        const double u = 0.492 * (b - y) + 128.0;
        const double v = 0.877 * (r - y) + 128.0;

        yuv.at<Vec3b>(row, col) = Vec3b(
                static_cast<uchar>(y), static_cast<uchar>(u), static_cast<uchar>(v));
    });

    Image result;
    result.set_image_mat(yuv);
    result.set_color(YUV);
    result.set_chroma(YUV444);
    return result;
}

Image convert_BGR_YUV422(Image &im) {
    auto image_mat_ = *im.get_image_mat();
    int rows = image_mat_.rows;
    int cols = image_mat_.cols;

    Mat yPlane(rows, cols, CV_8UC1);
    Mat uPlane(rows, cols / 2, CV_8UC1);
    Mat vPlane(rows, cols / 2, CV_8UC1);

    image_mat_.forEach<Vec3b>(
            [&yPlane, &uPlane, &vPlane](const Vec3b &pixel, const int *position) {
        const int row = position[0];
        const int col = position[1];

        const double b = pixel.val[0];
        const double g = pixel.val[1];
        const double r = pixel.val[2];

        const double y = 0.299 * r + 0.587 * g + 0.114 * b;
        const double u = 0.492 * (b - y) + 128.0;
        const double v = 0.877 * (r - y) + 128.0;

                yPlane.at<u_char>(row, col) = static_cast<uchar>(y);
                if (col % 2 == 0) {
                    uPlane.at<u_char>(row, col / 2) = static_cast<uchar>(u);
                    vPlane.at<u_char>(row, col / 2) = static_cast<uchar>(v);
                }
            });

    resize(uPlane, uPlane, Size(cols, rows));
    resize(vPlane, vPlane, Size(cols, rows));

    Mat yuv(rows, cols, CV_8UC3);
    vector<Mat> channels;
    channels.push_back(yPlane);
    channels.push_back(uPlane);
    channels.push_back(vPlane);

    merge(channels, yuv);
    Image result;
    result.set_image_mat(yuv);
    result.set_color(YUV);
    result.set_chroma(YUV422);
    return result;
}

Image convert_BGR_YUV420(Image &im) {
    auto mat = *im.get_image_mat();
    int rows = mat.rows;
    int cols = mat.cols;

    Mat yPlane(rows, cols, CV_8UC1);
    Mat uPlane(rows / 2, cols / 2, CV_8UC1);
    Mat vPlane(rows / 2, cols / 2, CV_8UC1);

    mat.forEach<Vec3b>(
            [&yPlane, &uPlane, &vPlane](const Vec3b &pixel, const int *position) {
        const int row = position[0];
        const int col = position[1];

        const double b = pixel.val[0];
        const double g = pixel.val[1];
        const double r = pixel.val[2];

        const double y = 0.299 * r + 0.587 * g + 0.114 * b;
        const double u = 0.492 * (b - y) + 128.0;
        const double v = 0.877 * (r - y) + 128.0;

                yPlane.at<u_char>(row, col) = static_cast<uchar>(y);
                if (row % 2 == 0 && col % 2 == 0) {
                    uPlane.at<u_char>(row / 2, col / 2) = static_cast<uchar>(u);
                    vPlane.at<u_char>(row / 2, col / 2) = static_cast<uchar>(v);
                }
            });

    resize(uPlane, uPlane, Size(cols, rows));
    resize(vPlane, vPlane, Size(cols, rows));

    Mat yuv(rows, cols, CV_8UC3);
    vector<Mat> channels;
    channels.push_back(yPlane);
    channels.push_back(uPlane);
    channels.push_back(vPlane);

    merge(channels, yuv);
    Image result;
    result.set_image_mat(yuv);
    result.set_color(YUV);
    result.set_chroma(YUV420);
    return result;
}

Image convert_YUV_BGR(Image &im) {
    auto image_mat_ = *im.get_image_mat();
    const int rows = image_mat_.rows;
    const int cols = image_mat_.cols;
    Mat bgr(rows, cols, CV_8UC3);
    image_mat_.forEach<Vec3b>([&bgr](const Vec3b &pixel, const int *position) -> void {
        const int row = position[0];
        const int col = position[1];

        const double y = pixel.val[0];
        const double u = pixel.val[1] - 128.0;
        const double v = pixel.val[2] - 128.0;

        double r = y + 1.13983 * v;
        double g = y - 0.39465 * u - 0.58060 * v;
        double b = y + 2.03211 * u;

        // Ensure values are within [0, 255]
        r = std::min(255.0, std::max(0.0, r));
        g = std::min(255.0, std::max(0.0, g));
        b = std::min(255.0, std::max(0.0, b));

        // Set the BGR pixel values
        bgr.at<Vec3b>(row, col) = Vec3b(
                static_cast<uchar>(b), static_cast<uchar>(g), static_cast<uchar>(r));
    });
    Image result;
    result.set_image_mat(bgr);
    result.set_color(BGR);
    return result;
}

Image convert_BGR_GRAY(Image &im) {
    Mat *matrix = im.get_image_mat();
    Mat gray(matrix->rows, matrix->cols, CV_8UC1);
    if (matrix->channels() != 3) {
        throw runtime_error("Original matrix must have 3 channels");
    }
    if (matrix->depth() != CV_8U) {
        throw runtime_error(
                "Original matrix must have 8-bit unsigned integers");
    }
    for (int i = 0; i < matrix->rows; i++) {
        for (int j = 0; j < matrix->cols; j++) {
            auto color = matrix->at<Vec3b>(i, j);
            const int R = color[2];
            const int G = color[1];
            const int B = color[0];
            const auto Y = static_cast<uchar>(0.299 * R + 0.587 * G + 0.114 * B);
            gray.at<uchar>(i, j) = Y;
        }
    }
    Image result;
    result.set_image_mat(gray);
    result.set_color(GRAY);
    return result;
}

void subsample(Image &im, const CHROMA_SUBSAMPLING cs) {
    Mat *matrix = im.get_image_mat();
    Mat channels[3];
    split(*matrix, channels);
    const Size target_size(channels[0].size[1], channels[0].size[0]);
    float scaling[2];
    if (matrix->cols % 2 != 0) {
        throw std::runtime_error("Matrix must have an even number of columns");
    }
    switch (cs) {
        case YUV444:
            cout << "No subsampling has been performed" << endl;
            break;
        case YUV422:
            // 1/2 horizontal sampling, full vertical sampling
            scaling[0] = 0.5;
            scaling[1] = 1;
            break;
        case YUV420:
            // 1/2 horizontal sampling, 1/2 vertical sampling
            scaling[0] = 0.5;
            scaling[1] = 0.5;
            break;
        default:
            throw std::runtime_error("Invalid subsampling ratio");
    }
    resize(channels[1], channels[1], target_size, scaling[0], scaling[1],
           INTER_LINEAR);
    resize(channels[2], channels[2], target_size, scaling[0], scaling[1],
           INTER_LINEAR);
    merge(channels, 3, *matrix);
}

void equalize_hist(Image &im) {
    Mat *matrix = im.get_image_mat();
    vector<Mat> channels;
    split(*matrix, channels);
    if (matrix->depth() != CV_8U) {
        throw std::runtime_error(
                "Original matrix must have 8-bit unsigned integers");
    }
    for (auto &channel: channels) {
        equalizeHist(channel, channel);
    }
    merge(channels, *matrix);
}

//! Calculates the [empirical cumulative distribution function](https://en.wikipedia.org/wiki/Empirical_distribution_function) of a histogram
Mat ecdf(const Mat &histogram, const int total) {
    Mat res = Mat::zeros(histogram.rows, histogram.cols, CV_32F);
    float cumsum = 0;
    for (int j = 0; j < histogram.cols; j++) {
        cumsum += histogram.at<float>(j);
        const float val = cumsum / static_cast<float>(total);
        res.at<float>(j) = val;
    }
    return res;
}

void binarize(Image &im) {
    Mat matrix = *im.get_image_mat();
    const int pix_num = matrix.rows * matrix.cols;
    constexpr int bins = 256;
    if (matrix.depth() != CV_8U) {
        throw std::runtime_error(
                "Original matrix must have 8-bit unsigned integers");
    }
    if (matrix.channels() == 3) {
        cout << "Converting to grayscale" << endl;
        auto gray = convert_BGR_GRAY(im);
        matrix = *gray.get_image_mat();
    }
    Mat hist = histogram<uchar>(matrix, bins);
    // Get empirical cumulative distribution function
    Mat distrib = ecdf(hist, pix_num);
    // Get best threshold
    int best_threshold = 0;
    double max_variance = 0;
    for (int thresh = 1; thresh < bins - 1; thresh++) {
        // Calculate percentage of pixels above and below threshold (foreground and
        // background)

        const double fg_prob = distrib.at<float>(thresh);// w0
        const double bg_prob = 1 - fg_prob;              // w1
        // Skip if one of the percentages is 0
        if (fg_prob == 0 || bg_prob == 0) {
            continue;
        }
        double fg_mean = 0;// μ0
        double bg_mean = 0;// μ1
        for (int i = 0; i < thresh; i++) {
            fg_mean += i * static_cast<double>(hist.at<float>(i));
        }
        for (int i = thresh; i < bins; i++) {
            bg_mean += i * static_cast<double>(hist.at<float>(i));
        }
        fg_mean /= fg_prob;
        bg_mean /= bg_prob;
        const double inter_variance =
                fg_prob * bg_prob * ((fg_mean - bg_mean) * (fg_mean - bg_mean));
        if (inter_variance > max_variance) {
            max_variance = inter_variance;
            best_threshold = thresh;
        }
    }
    // Binarize image
    matrix.forEach<uchar>(
            [&best_threshold](uchar &pixel, const int *) -> void {
                if (pixel > best_threshold) {
                    pixel = 255;
                } else {
                    pixel = 0;
                }
            });
    im.set_image_mat(matrix);
}
