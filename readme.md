# pico-embedded

### Build under Windows Commandline

- Pre-Conditions 
You need following tools installed and available on your PATH:
(versions used at time of documentation)

```
ninja                       1.11.0
cmake                       version 3.25.1
arm-none-eabi-gcc-ar.exe    (xPack GNU Arm Embedded GCC x86_64) 2.37.20220122
c++.exe                     (Rev3, Built by MSYS2 project) 12.1.0
python3                     Python 3.10.5
git                         version 2.38.1.windows.1
```

* Download this repo code
```
C:\dev>mkdir rasp_pico
C:\dev>cd rasp_pico
C:\dev\rasp_pico>git clone https://github.com/RobertK66/pico-embedded.git
...
C:\dev\rasp_pico>cd pico-embedded
```

* Set build options as env variables
```
set CMAKE_GENERATOR=Ninja
set PICO_SDK_FETCH_FROM_GIT=ON
```
If you already have an downloaded pico_sdk on your disk you can use the alternative setting
```
set PICO_SDK_PATH=C:/dev/pico-sdk
```
* Genrate the build files for the Ninja build system.
```
C:\dev\rasp_pico\pico-embedded>>cmake -S . -B ./build_cli
-- Building for: Ninja
Using PICO_SDK_FETCH_FROM_GIT from environment ('ON')
Downloading Raspberry Pi Pico SDK
...
-- Build files have been written to: C:/dev/rasp_pico/pico-embedded/build_cli
C:\dev\rasp_pico\pico-embedded>
```
* Execute the Build
```
C:\dev\rasp_pico\pico-embedded>cd build_cli
C:\dev\rasp_pico\pico-embedded\build_cli>ninja
...


### use with Visual Studio Code

### use with Visual Studio 2022

### use with Eclipse...
