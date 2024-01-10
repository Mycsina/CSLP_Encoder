#include "../src/visual/Image.hpp"
#include <gtest/gtest.h>

using namespace std;
using namespace cv;

// auto img_file = "../../tests/resource/img.png";
auto img_file = "../../tests/resource/tulips.ppm";
auto jpegls_file = "../../tests/resource/jpegls_out.bin";


TEST(ImageTestSuite, ImageLoadTest) {
    Image image;
    const Mat I = imread(img_file, IMREAD_COLOR);
    ASSERT_NO_THROW(image.load(img_file));
    image.show();
    ASSERT_EQ(image.size().height, I.rows);
    ASSERT_EQ(image.size().width, I.cols);
}

TEST(ImageTestSuite, ImageLoadFailTest) {
    Image im;
    const auto gibb = "gibber-ish";
    ASSERT_ANY_THROW(im.load(gibb));
}

TEST(ImageTestSuite, PixelSetPersistTest) {
    const auto tempImage = "../../tests/resource/persist.png";
    Image im;
    im.load(img_file);
    const Vec3b values = {12, 54, 255};
    im.setPixel(0, 0, values);
    im.save(tempImage);
    Image im2;
    im2.load(tempImage);
    ASSERT_EQ(im2.getPixel(0, 0), values);
    remove(tempImage);
}

TEST(ImageTestSuite, PixelWiseCloning) {
    Image im1;
    im1.load(img_file);
    Image im2;
    im2.set_image_mat(Mat::zeros(im1.size(), im1.getImageType()));
    for (auto it = im1.begin(); it != im1.end(); ++it) {
        im2.setPixel(it.pos().y, it.pos().x, *it);
    }
    im2.save("../../tests/resource/pbp.png");
    ASSERT_EQ(im1.getImageType(), im2.getImageType());
    ASSERT_EQ(im1.getPixel(0, 39), im2.getPixel(0, 39));
    ASSERT_EQ(im1.getPixel(345, 256), im2.getPixel(345, 256));
    remove("../../tests/resource/pbp.png");
}
