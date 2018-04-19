echo "Building ROS nodes"

cd Examples/ROS/MultiColSLAM
mkdir build
cd build
cmake .. -DROS_BUILD_TYPE=Release
make -j
