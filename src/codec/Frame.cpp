#include "Frame.hpp"

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
    imshow("Block", block_mat_);
    waitKey(0);
    imshow("Other", other.getBlockMat());
    waitKey(0);
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

MotionVector Frame::match_block_es(const Blocks &block, int n, int search_radius) {
    Frame *reference = this;
    for (int i = 0; i < n; i++) {
        reference = reference->getPrevious();
    }
    MotionVector best_match;
    double best_pnsr = 0;
    int left = max(0, block.getCol() - search_radius);
    int up = max(0, block.getRow() - search_radius);
    for (int i = up; i <= search_radius; i++) {
        for (int j = left; j <= search_radius; j++) {
            Blocks ref_block = get_block(reference->getImage(), block.getSize(), block.getRow() + i, block.getCol() + j);
            double psnr = block.compare_block_psnr(ref_block);
            if (best_pnsr < psnr) {
                best_pnsr = psnr;
                best_match = MotionVector(i, j);
            }
        }
    }
    return best_match;
}

MotionVector Frame::match_block_arps(Blocks block, int n, int search_radius) {
    return {0, 0};
}

vector<MotionVector> Frame::match_all_blocks(int block_size, int n, int search_radius, bool fast) {
    vector<MotionVector> motion_vectors;
    for (int i = 0; i <= image_.get_image_size()[0] - block_size; i += block_size) {
        for (int j = 0; j <= image_.get_image_size()[1] - block_size; j += block_size) {
            Blocks block = get_block(image_, block_size, i, j);
            MotionVector mv;
            if (fast) {
                mv = match_block_arps(block, n, search_radius);
            } else {
                mv = match_block_es(block, n, search_radius);
            }
            motion_vectors.push_back(mv);
        }
    }
    return motion_vectors;
}
