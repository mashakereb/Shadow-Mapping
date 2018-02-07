Shadow Mapping Demo 
==

## Working example of shadowing using OpenGL/GLUT

After installing OpenGL and freeglut3 libraries,
build the project with cmake.
Please use ```CMake 3.5``` or higher.

### Install OpenGl and Glut

```shell
sudo apt install mesa-utils
sudo apt-get install freeglut3-dev
sudo apt-get install libxmu-dev libxi-dev
```

#### Clone the project
```shell
git clone https://github.com/MashaKereb/Shadow-Mapping.git
```
### Build
```shell
 cd Shadow-Mapping
 mkdir build
 cd build
 cmake ..
 cmake --build .
```
### Run
```shell
 ./Shadow-Mapping 
```
 
### Result (Demo)

 ![Demo Scene](https://raw.githubusercontent.com/MashaKereb/Shadow_mapping/master/result.gif)