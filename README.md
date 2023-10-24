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
```
Then, run any of the demos.
```bash
./cmake-build-debug/tests/Demos --gtest_filter='<test_suite_name>.<test_name>'
```
For example:
```bash
./cmake-build-debug/tests/Demos --gtest_filter='VideoTestSuite.VideoPlay'
```

## Testing
Build the tests target.
```bash
/usr/bin/cmake --build ./cmake-build-debug --target tests_run -j 6
```
To run all tests, run the following command:
```bash
./cmake-build-debug/tests/tests_run --gtest_filter='*'
```
To run a specific test, run the following command:
```bash
./cmake-build-debug/tests/tests_run --gtest_filter='<test_suite_name>.<test_name>'
```
For example:
```bash
./cmake-build-debug/tests/tests_run --gtest_filter='ImageTest.GRAYEquivalenceTest'
```
