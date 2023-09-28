#include <gtest/gtest.h>
#include "../src/imageProcessing.cpp"

using namespace std;

auto img_name = "../../tests/resource/tulips.ppm";

TEST(ImageProcessingSuite, ConversionIdempotenceTest) {
    image im;
    im.load(img_name);
    image im2;
    im2.load(img_name);
    BGR2YUV(im._get_image_mat());
    im.display_image();
    YUV2BGR(im._get_image_mat());
    im.display_image();
    EXPECT_EQ(im._get_image_mat()->at<Vec3b>(0, 0), im2._get_image_mat()->at<Vec3b>(0, 0));
}

TEST(ImageProcessingSuite, WatermarkTest) {
    string mark_file = "../../tests/resource/lena.ppm";
    image im;
    im.load(img_name);
    image mark;
    mark.load(mark_file);
    watermark(im, &mark, {0, 0}, {mark.get_image_size()[1], mark.get_image_size()[0]}, 0.5);
    im.display_image();
    cout << "Is it the expected result?" << endl;
    auto ans = cin.get();
    EXPECT_EQ(ans, 'y');
}