# Running the project
## Building
To build the project, run the following commands:
```bash
/usr/bin/cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_MAKE_PROGRAM=ninja -G Ninja -S . -B ./cmake-build-debug
```

## Running a demo
First, build the demos target.
```bash
/usr/bin/cmake --build ./cmake-build-debug --target Demos -j 6
cd ./cmake-build-debug/tests
```
Then, run any of the demos.
```bash
./Demos --gtest_filter='<test_suite_name>.<test_name>'
```
For example:
```bash
./Demos --gtest_filter='VideoTestSuite.VideoPlay'
```

### List of demos
#### Image
- Show a BGR image subsampled and converted back
    ```bash
    ./Demos --gtest_filter='ImageDemo.SubSampling_Demo'
    ```
- Show an images' color histograms
    ```bash
    ./Demos --gtest_filter='ImageDemo.ColorHist_Demo'
    ```
- Show a histogram equalized image and its color histogram
    ```bash
    ./Demos --gtest_filter='ImageDemo.EqualizedColorHist_Demo'
    ```
- Add a watermark to an image
    ```bash
    ./Demos --gtest_filter='ImageDemo.WatermarkDemo'
    ```
- Show an image subsampled without converting to RGB and then to YUV
    ```bash
    ./Demos --gtest_filter='ImageDemo.SubsamplingDemo'
    ```
- Show an image thresholded
    ```bash
    ./Demos --gtest_filter='ImageDemo.ThresholdDemo'
    ```
- Show an image with a Gaussian filter applied
    ```bash
    ./Demos --gtest_filter='ImageDemo.GaussianDemoFlat1'
    ```
- Show an image with a stronger Gaussian filter applied
    ```bash
    ./Demos --gtest_filter='ImageDemo.GaussianDemoFlat2'
    ```
- Show an image with a distance based filter applied
    ```bash
    ./Demos --gtest_filter='ImageDemo.GaussianDemoDistanceBased1'
    ```

#### Video
- Show a video from a file
    ```bash
    ./Demos --gtest_filter='VideoTestSuite.VideoPlay'
    ```
- Show a video from a Y4M file
    ```bash
    ./Demos --gtest_filter='VideoTestSuite.Y4MVideoTest'
    ```
- Show a video from a Y4M file converted to RGB
    ```bash
    ./Demos --gtest_filter='VideoTestSuite.Y4M2BGRVideoTest'
    ```

## Testing
Build the tests target.
```bash
/usr/bin/cmake --build ./cmake-build-debug --target tests_run -j 6
cd ./cmake-build-debug/tests
```
To run all tests, run the following command:
```bash
./tests_run --gtest_filter='*'
```
To run a specific test, run the following command:
```bash
./tests_run --gtest_filter='<test_suite_name>.<test_name>'
```
For example:
```bash
./tests_run --gtest_filter='ImageTest.GRAYEquivalenceTest'
```
