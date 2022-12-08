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
set PICO_SDK_FETCH_FROM_GIT=ON
set PICO_SDK_FETCH_FROM_GIT_PATH=./../pico-sdk
```
If you already have an downloaded pico_sdk on your disk you can use the alternative setting
```
set PICO_SDK_PATH=C:/dev/pico-sdk
```
* Genrate the build files for the Ninja build system.
```
C:\dev\rasp_pico\pico-embedded>cmake -L -G Ninja -B ./build
-- Building for: Ninja
Using PICO_SDK_FETCH_FROM_GIT from environment ('ON')
Downloading Raspberry Pi Pico SDK
...
-- Build files have been written to: C:/dev/rasp_pico/pico-embedded/build
... <list of cmake cache values set by config>  ...

C:\dev\rasp_pico\pico-embedded>
```
* Execute the Build
```
C:\dev\rasp_pico\pico-embedded>cd build
C:\dev\rasp_pico\pico-embedded\build>ninja
...
[93/93] Completed 'PioasmBuild'

C:\dev\rasp_pico\pico-embedded\build>
```

* If everything succeded, you can find the .uf2 files for each sub-project under \<root>/build/\<proj>
e.g.:
```
C:\dev\rasp_pico\pico-embedded\build>dir cli
 Datenträger in Laufwerk C: ist Windows
 Volumeseriennummer: B62B-F21E

 Verzeichnis von C:\dev\rasp_pico\pico-embedded\build\cli

06.12.2022  11:25    <DIR>          .
06.12.2022  11:25    <DIR>          ..
06.12.2022  11:25            39 348 cli.bin
06.12.2022  11:25           714 889 cli.dis
06.12.2022  11:25            87 052 cli.elf
06.12.2022  11:25           110 740 cli.hex
06.12.2022  11:25            78 848 cli.uf2
06.12.2022  11:25    <DIR>          CMakeFiles
06.12.2022  11:24             1 218 cmake_install.cmake
06.12.2022  11:24    <DIR>          elf2uf2
               6 Datei(en),      1 032 095 Bytes
               4 Verzeichnis(se), 131 400 880 128 Bytes frei
```




### use with Visual Studio Code

### use with Visual Studio 2022

### use with Eclipse...
