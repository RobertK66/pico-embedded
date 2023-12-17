## Build and Debug with Visaul Studio Code under Windows using a picoprobe interface

### 2023-12-17  setup with new VC versions

- Installed VC with ZIP Download and configured as 'Portable' 
  - Download and extract actaul version to C:\dev\vc_1_85_1_p 
  - Prior to first start create the 'portable data dir' C:\dev\vc_1_85_1_p\data
  - Prior to first start create the 'portable data dir' C:\dev\vc_1_85_1_p\data\tmp

  Starting VC now should store all local settings under data, and use the own tmp iso Systems temp dir.

- Plugins needed in VC:
  - C/C++ (ms-vscode.cpptools) -> (dependency: C/C++ ExtensionPack (ms-vscodecpptools-extension-pack))
  - CMAKE Tools (ms-vscode.cmake-tools) -> (dependency: CMake (
twxs.cmake))
  - Cortex Debug (marus25.cortex-debug) -> (4 dependencies ...)
  - Optional (recomended): C/C++ Themes (ms-vscode.cpptools-themes)

- Installed newer version of Cmake in C:\dev\cmake3280\
  - changed PATH in VC Startup.bat to use this version

- Make it work with
  - Download the git clone in new directory
  - When CMake first starts choose '_uncpecified_' as toolchain kit!!!


### 202x  original setup

- Pre-Conditions

You need following tools installed and available on your PATH:
(versions used at time of documentation)

```
ninja                       1.11.0
cmake                       version 3.25.1
arm-none-eabi-gcc-ar.exe    (xPack GNU Arm Embedded GCC x86_64) 2.37.20220122
c++.exe                     (Rev3, Built by MSYS2 project) 12.1.0                   !!!!
```

Make sure your Visual Studio Code has following Extensions installed:

```
dan-c-underwood.arm@1.7.4                                       ??? needed ???
marus25.cortex-debug@1.6.7
ms-dotnettools.csharp@1.25.2
ms-vscode.cmake-tools@1.12.27
ms-vscode.cpptools@1.13.6
ms-vscode.cpptools-extension-pack@1.3.0
ms-vscode.cpptools-themes@2.0.0
 ???? twxs.cmake@0.0.17
```

- open VC (with above tools in Path)
- Cloase all open workspaces/projects/folders
- Choose 'Clone Git repository' - choose a new empty local directory as target
- Open the repository - trust the authors

Check and change the settings in pico-embedded.code-workspace
```
// vvvv Change this to your needs vvvv
"myenv.armToolsBase": "C:/dev/arm-none-eabi-gcc/bin",
"myenv.openocdBase": "C:/dev/OpenOCD-20211118-0.11.0",
"myenv.openocdExe": "${config:myenv.openocdBase}/bin/openocd_rp2040.exe",
// ^^^^ Change this to your needs ^^^^
  ```
Make sure the paths are correct and use a openocd variant with RP2040 Support!
 
- Open the workspace choosing the pico-embedded.code-workspace file
- Goto the CMake tab - "Configure all projects" - Choose 'Lets Cmake guess the toolchain ...."

 -> sdk will be downloaded
 -> Build files will be written to <workspacebase>/build 
 
- Build all Projects 

 You should now find the <project>.uf2 file(s) under <workspaceroot>/build/<proj>
 
 - Dbug by using (one of the) the named launch configurations
 
This uses a picoprobe connected to your target raspberryPico as Debug hardware.





