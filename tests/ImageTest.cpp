#include "../src/visual/Image.hpp"
#include <gtest/gtest.h>

using namespace std;
using namespace cv;

// auto img_file = "../../tests/resource/img.png";
auto img_file = "../../tests/resource/tulips.ppm";
auto jpegls_file = "../../tests/resource/jpegls_out.bin";


TEST(ImageTestSuite, ImageLoadTest) {
    Image image;
    Mat I = imread(img_file, IMREAD_COLOR);
    ASSERT_NO_THROW(image.load(img_file));
    image.display_image();
    ASSERT_EQ(image.size()[0], I.rows);
    ASSERT_EQ(image.size()[1], I.cols);
}

TEST(ImageTestSuite, ImageLoadFailTest) {
    Image im;
    auto gibb = "gibber-ish";
    ASSERT_ANY_THROW(im.load(gibb));
}

TEST(ImageTestSuite, PixelSetPersistTest) {
    auto tempImage = "../../tests/resource/persist.png";
    Image im;
    im.load(img_file);
    Vec3b values = {12, 54, 255};
    im.set_pixel(0, 0, values);
    im.save(tempImage);
    Image im2;
    im2.load(tempImage);
    ASSERT_EQ(im2.get_pixel(0, 0), values);
    remove(tempImage);
}

TEST(ImageTestSuite, PixelWiseCloning) {
    Image im1;
    im1.load(img_file);
    Image im2;
    im2._set_image_mat(Mat::zeros(im1.size()[0],
                                  im1.size()[1], im1.get_image_type()));
    for (auto it = im1.begin(); it != im1.end(); ++it) {
        im2.set_pixel(it.pos().y, it.pos().x, *it);
    }
    im2.save("../../tests/resource/pbp.png");
    ASSERT_EQ(im1.get_image_type(), im2.get_image_type());
    ASSERT_EQ(im1.get_pixel(0, 39), im2.get_pixel(0, 39));
    ASSERT_EQ(im1.get_pixel(345, 256), im2.get_pixel(345, 256));
    remove("../../tests/resource/pbp.png");
}

TEST(ImageTestSuite,JPEG_LS){
    Image im1,im2;
    double diffs;

    im1.load(img_file);
    im1.encode_JPEG_LS(jpegls_file,4);
    im2=Image::decode_JPEG_LS(jpegls_file);

    diffs=cv::norm(*im1._get_image_mat(),*im2._get_image_mat(),cv::NORM_L2);
    ASSERT_EQ(diffs,0);
}