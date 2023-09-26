#include <gtest/gtest.h>
#include "../src/image.cpp"

using namespace std;

auto filename = "../../tests/resource/img.png";

TEST(ImageTestSuite, ImageLoadTest){
    image image;
    Mat I = imread(filename, IMREAD_COLOR);
    ASSERT_NO_THROW(image.load(filename));
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
    im.load(filename);
    Vec3b values = {12, 54, 255};
    im.set_pixel(0, 0, values);
    im.save(tempImage);
    image im2;
    im2.load(tempImage);
    ASSERT_EQ(im2.get_pixel(0, 0), values);
    remove(tempImage);
}

TEST(ImageTestSuite, PixelWiseCloning) {
    image image;
    image.load(filename);
    image image2;
    image2._get_image_mat().copySize(image._get_image_mat());
    for (auto it = image.begin(); it != image.end(); ++it) {
        image2.set_pixel(it.pos().y, it.pos().x, *it);
    }
    ASSERT_EQ(image.get_image_type(), image2.get_image_type());
    ASSERT_EQ(image.get_pixel(0, 39), image2.get_pixel(0, 39));
    ASSERT_EQ(image.get_pixel(345, 256), image2.get_pixel(345, 256));
    image2.save("../../tests/resource/pbp.png");
}