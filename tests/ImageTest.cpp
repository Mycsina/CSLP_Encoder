#include <gtest/gtest.h>
#include "../src/image.cpp"

using namespace std;

// auto img_file = "../../tests/resource/img.png";
auto img_file = "../../tests/resource/tulips.ppm";


TEST(ImageTestSuite, ImageLoadTest){
    image image;
    Mat I = imread(img_file, IMREAD_COLOR);
    ASSERT_NO_THROW(image.load(img_file));
    image.display_image();
    ASSERT_EQ(image.get_image_size()[0], I.rows);
    ASSERT_EQ(image.get_image_size()[1], I.cols);

}

TEST(ImageTestSuite, ImageLoadFailTest){
    image im;
    auto gibb = "gibber-ish";
    ASSERT_ANY_THROW(im.load(gibb));
}

TEST(ImageTestSuite, PixelSetPersistTest){
    auto tempImage = "../../tests/resource/persist.png";
    image im;
    im.load(img_file);
    Vec3b values = {12, 54, 255};
    im.set_pixel(0, 0, values);
    im.save(tempImage);
    image im2;
    im2.load(tempImage);
    ASSERT_EQ(im2.get_pixel(0, 0), values);
    remove(tempImage);
}

TEST(ImageTestSuite, PixelWiseCloning) {
    image im1;
    im1.load(img_file);
    image im2;
    // TODO perguntar ao professor porque é que isto causa um SIGILL
    im2._set_image_mat(Mat::zeros(im1.get_image_size()[0], im1.get_image_size()[1], im1.get_image_type()));
    for (auto it = im1.begin(); it != im1.end(); ++it) {
        im2.set_pixel(it.pos().y, it.pos().x, *it);
    }
    im2.save("../../tests/resource/pbp.png");
    ASSERT_EQ(im1.get_image_type(), im2.get_image_type());
    ASSERT_EQ(im1.get_pixel(0, 39), im2.get_pixel(0, 39));
    ASSERT_EQ(im1.get_pixel(345, 256), im2.get_pixel(345, 256));
    remove("../../tests/resource/pbp.png");
}