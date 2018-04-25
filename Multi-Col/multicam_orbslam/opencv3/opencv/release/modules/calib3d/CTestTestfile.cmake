# CMake generated Testfile for 
# Source directory: /home/e4e/workspace/multicam_orbslam/opencv3/opencv/modules/calib3d
# Build directory: /home/e4e/workspace/multicam_orbslam/opencv3/opencv/release/modules/calib3d
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(opencv_test_calib3d "/home/e4e/workspace/multicam_orbslam/opencv3/opencv/release/bin/opencv_test_calib3d" "--gtest_output=xml:opencv_test_calib3d.xml")
set_tests_properties(opencv_test_calib3d PROPERTIES  LABELS "Main;opencv_calib3d;Accuracy" WORKING_DIRECTORY "/home/e4e/workspace/multicam_orbslam/opencv3/opencv/release/test-reports/accuracy")
add_test(opencv_perf_calib3d "/home/e4e/workspace/multicam_orbslam/opencv3/opencv/release/bin/opencv_perf_calib3d" "--gtest_output=xml:opencv_perf_calib3d.xml")
set_tests_properties(opencv_perf_calib3d PROPERTIES  LABELS "Main;opencv_calib3d;Performance" WORKING_DIRECTORY "/home/e4e/workspace/multicam_orbslam/opencv3/opencv/release/test-reports/performance")
add_test(opencv_sanity_calib3d "/home/e4e/workspace/multicam_orbslam/opencv3/opencv/release/bin/opencv_perf_calib3d" "--gtest_output=xml:opencv_perf_calib3d.xml" "--perf_min_samples=1" "--perf_force_samples=1" "--perf_verify_sanity")
set_tests_properties(opencv_sanity_calib3d PROPERTIES  LABELS "Main;opencv_calib3d;Sanity" WORKING_DIRECTORY "/home/e4e/workspace/multicam_orbslam/opencv3/opencv/release/test-reports/sanity")
