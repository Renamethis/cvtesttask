git clone https://github.com/opencv/opencv
cd opencv
mkdir build && cd build
cmake ..
make -j4
sudo make install
cd ..
cd ..
git clone https://github.com/nlohmann/json.git
cd json
mkdir build && cd build
cmake ..
make -j4
sudo make install
cd ..
cd ..
mkdir build && cd build
cmake ..
make -j4

