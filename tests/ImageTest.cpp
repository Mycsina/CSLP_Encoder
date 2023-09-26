#include <gtest/gtest.h>
#include "../src/image.cpp"

using namespace std;

auto img_file = "../../tests/resource/img.png";

TEST(ImageTestSuite, ImageLoadTest){
    image image;
    Mat I = imread(img_file, IMREAD_COLOR);
    ASSERT_NO_THROW(image.load(img_file));
    ASSERT_EQ(image.get_image_size()[0], I.rows);
    ASSERT_EQ(image.get_image_size()[1], I.cols);

}

TEST(ImageTestSuite, ImageLoadFailTest){
    image image;
    auto gibb = "gibber-ish";
    ASSERT_ANY_THROW(image.load(gibb));
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
    im2._get_image_mat().copySize(im1._get_image_mat());
    for (auto it = im1.begin(); it != im1.end(); ++it) {
        im2.set_pixel(it.pos().y, it.pos().x, *it);
    }
    ASSERT_EQ(im1.get_image_type(), im2.get_image_type());
    ASSERT_EQ(im1.get_pixel(0, 39), im2.get_pixel(0, 39));
    ASSERT_EQ(im1.get_pixel(345, 256), im2.get_pixel(345, 256));
    im2.save("../../tests/resource/pbp.png");
}