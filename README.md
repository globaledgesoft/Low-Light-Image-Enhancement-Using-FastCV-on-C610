# Low Light Image Enhancement Using FastCV on C610

## About the project

 In this project we are performing the image enhancement of the captured image under low light condition using Fastcv image filter api’s such as bi-lateral filter, median filter and guided filter. we have implemented the fastcv version of guided filter.

- Application source tree on the host system:

           lowLightEnhancement/
                                 inc/
                                        - fastcv.h
                                         opencv2/
                                               - contain opencv header files
								  lib / 
                                         - place opencv and fastcv library	   
                                  src/
                                         - ImageEnhance.cpp
                                  Images/
                                         Contain images for testing                       
                                  Makefile
                                  UbuntuARM.min
                                                                                      
## Dependencies
- Ubuntu System 18.04 or above
- Install Adb tool (Android debugging bridge) on host system
- setup an yocto build system(for opencv building)
- Setting up the Fastcv sdk on the host system.
- Build opencv library on target environment. 


## Prerequisites
### Install the FastCV SDK 
1)  Download FastCV SDK from url https://developer.qualcomm.com/software/fast-cv-sdk/tools and select particular version named v1.7.1 for Linux Embedded
2)  For FastCV installation and compilation, follow the below steps
  ```
      $ chmod 777 fastcv-installer-linuxembedded-1-7-1.bin
      $ ./fastcv-installer-linuxembedded-1-7-1.bin     
  ```
 3) To build application in fastcv:
    1. Download Hexagon DSP SDK from Qualcomm developer network using url https://developer.qualcomm.com/software/hexagon-dsp-sdk
    -To install Hexagon sdk follow below steps
    ```
       $ chmod 777 qualcomm_hexagon_sdk_3_5_2_eval.bin
       $ ./qualcomm_hexagon_sdk_3_5_2_eval.bin
    ```
    
     2. Download  gcc-linaro-7.5.0-2019.12-x86_64_arm-linux-gnueabihf.tar from https://releases.linaro.org/components/toolchain/binaries/latest-7/arm-linux-gnueabihf/
     - Extract the tar file 
     ```
        $ tar xvf gcc-linaro-7.5.0-2019.12-x86_64_arm-linux-gnueabihf.tar
     ```
     - copy folder gcc-linaro-7.5.0-2019.12-x86_64_arm-linux-gnueabihf to <Hexagon_SDK_ROOT>/tools/ folder.  
     ```
        $ cp -r gcc-linaro-7.5.0-2019.12-x86_64_arm-linux-gnueabihf/ <Hexagon_SDK_ROOT>/tools/
     ```
     
     - Rename gcc-linaro-7.5.0-2019.12-x86_64_arm-linux-gnueabihf folder to linaro
      ```
         $ mv gcc-linaro-7.5.0-2019.12-x86_64_arm-linux-gnueabihf linaro
      ```
     3. Keep the given application folder ‘fastcvSimpleTest’ in <Hexagon_SDK_ROOT>/examples/common/
     ```
        $ cp fastcv-1-7-1_LinuxEmbedded/samples/fastcvSimpleTest/ <Hexagon_SDK_ROOT>/examples/common/
     ```
     4. Copy 32-bit libfastcv.a from libs provided in the fastcv sdk (fastcv-1-7-1_LinuxEmbedded\lib\32-bit\hard\libfastcv.a) to <Hexagon_SDK_ROOT>/examples/common/fastcvSimpleTest/lib/ folder.
     ```
        cp  fastcv-1-7-1_LinuxEmbedded\lib\32-bit\hard\libfastcv.a <Hexagon_SDK_ROOT>/examples/common/fastcvSimpleTest/lib/
     ```
     5. Open a new terminal from the root directory <Hexagon_SDK_ROOT> of the  Hexagon SDK and run below command        
      ```
          $ cd < Hexagon SDK root directory>
          $ source setup_sdk_env.source
          $ cd examples/common/fastcvSimpleTest
          $ make tree V=UbuntuARM_Release
     ```
     6. A folder with the name "UbuntuARM_Release" should get generated in the fastcvSimpleTest folder. It will contain the application’s binary file ‘fastcvSimpleTest’.


### build opencv library 
- To install opencv library on the target board the required meta recipe for opencv is already present in folder “poky/meta-openembedded/meta-oe/recipes-support/opencv/opencv_3.4.5.bb” file. We need to follow the below steps to build.

-  Get into the yocto working directory

 ```sh
  $ cd  <yocto working directory>
 ```
 
- Execute source command for environment setting 

 ```sh
    $ source poky/qti-conf/set_bb_env.sh
 ```
- The pop up menu will be open for available machines that select “qcs610-odk” and press ok. Then one more pop up window will be open for distribution selection in that we need to select “qti-distro-fullstack-perf”. Run the bitbake command for installing packages.

 ```sh
 $ bitbake opencv 
 ```

- Once the build is complete the shared library and include file will be available in “./tmp-glibc/sysroots-components/armv7ahf-neon/opencv/usr”
Push the opencv shared library to the target board 

 ```sh
   $ cd  ./tmp-glibc/sysroots-components/armv7ahf-neon/opencv/usr/
   $ cp -r lib/. <Hexagon_SDK_ROOT>/examples/common/fastcvSimpleTest/
   $ cp -r include/opencv/ include/opencv2/  <Hexagon_SDK_ROOT>/examples/common/fastcvSimpleTest/inc/

 ```

**Note**: 
- For more reference refer to the “QCS610/QCS410 Linux Platform Development Kit Quick Start Guide document”.
- Also make sure install the all the dependency library from the yocto build to the system (ex: libgphoto2, libv4l-utils) 
- bb recipes of above  library are available inside meta-oe layer you can directly run the bitbake command

        
## To run application on the c610 board:

- Download a source repository which contains appliction code
     ```
        $ github clone <source repository>
        $ cd <source repository>
        $ cp lowLightEnhancement <Hexagon_SDK_ROOT>/examples/common/
        $ cp -r <Hexagon_SDK_ROOT>/examples/common/fastcvSimpleTest/lib/ <Hexagon_SDK_ROOT>/examples/common/lowLightEnhancement/
        $ cp -r <Hexagon_SDK_ROOT>/examples/common/fastcvSimpleTest/inc/ <Hexagon_SDK_ROOT>/examples/common/lowLightEnhancement/
      ```    
     
- source the setup_sdk_env.source 
    ```
      $ cd `< Hexagon SDK root directory>
      $ source setup_sdk_env.source
      $ cd examples/common/lowLightEnhancement
      $ make tree V=UbuntuARM_Release  
   ```
- **Note**: while integrating opencv code into fastcv, we may get following errors (Similar error)
   ```
    /home/admin/Hexagon_SDK/3.5.2/tools/linaro/arm-linux-gnueabihf/include/c++/7.5.0/cstdlib:75:15: fatal error: stdlib.h: No such file or directory
    #include_next <stdlib.h>
               ^~~~~~~~~~
     To Fix this:Replace **#include_next** with **#include** in the cstdlib file from the path above.
   ```  
- Remount the root directory writable:
  ```
           $ adb root
           $ adb remount
           $ adb shell mount -o remount,rw /
   ```
-  Push the lowLightEnhancement binary file and input image to the target:
   ```
          $ adb push UbuntuARM_Release/ship/lowLightEnhancement  /data/lowLight
          $ adb push Images/  /data/lowLight/
          $ adb push lib/  /data/lowLight/
          $ adb push inc/opencv/  /usr/include/
          $ adb push inc/opencv2/  /usr/include/

   ```
- Change bin permissions and execute the application executable:
   ```
          $ adb shell
          /# 
          /# chmod +x /data/lowLight/lowLightEnhancement
          /# export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/data/lowLight/lib/
          /#  cd /data/lowLight/
   ```
- To Enhance the camera capture image run below code
   ```
## syntax
         /# ./lowLightEnhancement  filtername   imagepath/capture
    ```
where, filtername will be ‘guided’ for guided filter, ‘median’ for median filter and ‘bilateral’ for bilateral filter. 
 second argument you can either pass the image file path or you can capture the camera image using ‘capture’ option
 ```
        /# ./lowLightEnhancement  guided  capture
        /# ./lowlightEnhanement bilateral  images/example.jpg
 ```
- after successful running this command enhanced “output.jpg” file is created. you can transfer the image using adb pull command.
   ```
         $ adb pull  /data/lowLight/output.jpg 
         
   ```
   
   ## Note 

   -- guided filter code implementation is adapted from below reference papers
   * [Guided Image Filtering](http://kaiminghe.com/eccv10/), by Kaiming He, Jian Sun, and Xiaoou Tang, in TPAMI 2013
   * [Fast Guided Filter](https://arxiv.org/abs/1505.00996), by Kaiming He and Jian Sun, arXiv 2015

