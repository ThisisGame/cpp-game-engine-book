# How to build

You can create projects to build the tests for the uuid library using CMake.

If you don't have CMake installed you can get it from https://cmake.org/.

## Windows

Do the Following:

* Create a folder called **build**
```
mkdir build
cd build
```

* Run the following CMake command from the **build** folder to generate projects to target the **x86** platform.
```
   cmake -G "Visual Studio 15 2017" ..
```

To generate projects to target **x64** use the generator **"Visual Studio 15 2017 Win64"**.

To generate projects to target **ARM** use the generator **"Visual Studio 15 2017 ARM"**.


## Mac

Do the Following:

* Create a folder called **build**
```
mkdir build
cd build
```

* Run the following CMake command from the **build** folder to generate projects to target the **x86** platform.
```
   cmake -G Xcode ..
```
