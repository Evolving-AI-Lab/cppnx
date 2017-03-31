## CPPN-Explorer

This repository contains the source code for the CPPN-Explorer (CPPN-X) tool, which allows the user to examine Compositional Pattern Producing Networks (CPPNs) such as Picbreeder.com genomes. For the accompanying publication, see http://www.evolvingai.org/PicbreederCanalization.

## 1. Installation

This files contains the installation instructions for cppn-x. 
These installations have been tested on Windows XP (SP 3) and Mac OS X 10.6.8,
but they might work on other systems as well.

Please chose the instructions for the system that most closely resembles yours
and mail to joost.hui@gmail.com, providing as much information as possible, 
if you encounter any problems.


### UNIX and MAC systems
These are the installation instructions for MAC and UNIX based systems.
Because of a recent MAC specific fix, there is a good chance that it will not compile on UNIX.
If this is the case, open compile.sh, remove the arguments to qmake, and try again.

#### Install QT 4.8
For Mac:
* Download 'Qt libraries 4.8.4 for Mac (185 MB)' from http://qt-project.org/downloads.
For UNIX:
* Download 'Qt libraries 4.8.4 for embedded Linux (226 MB)' from http://qt-project.org/downloads.
For both:
* Execute the installer and follow the instructions.

#### Install GSL-1.16
* Download GSL-1.16 from http://www.gnu.org/software/gsl/
* Follow the installation instruction provided in the README and INSTALL files.

#### (optional) Install FFmpeg 1.2
* Download the FFmpeg 1.2.1 "Magic" release from: http://www.ffmpeg.org/download.html (Make sure it is the source code and not a precompiled binary)
* Install the libraries according the the instruction found in INSTALL. These instructions are usually: 
	- ./configure 
	- make 
	- make install 
* If the there are missing packages, install them separately. 
The following site lists the commands to install most of ffmpegs dependencies, but your probably don't need all of them: http://hunterford.me/compiling-ffmpeg-on-mac-os-x/


#### Compile cppn-x
* Open compile.sh and check that all paths are correct. The compile.sh will have additional comments for setting.
* Open cppnx.pro to set or change deployment target for OSX 
* Run compile.sh

For UNIX:
* The executable 'cppn-x' should be placed in the 'build' folder.
For MAC:
* The app 'cppn-x.app' should be placed in the 'build' folder.

#### Run cppn-x
For UNIX:
* Run 'cppn-x' in the 'build' folder.
For MAC:
* Open 'cppn-x.app' in the 'build' folder from a finder window or execute build/cppn.app/Contents/MacOS/cppn-x.

### Windows systems
These instructions use the open source 'MinGW' (Minimalist GNU for windows) to install both QT and cppn-x.
It is probably possible to install everything using Visual Studio 2008 or Visual Studio 2010,
but this has not been tested and no instructions are available.

#### Install MinGW 4.4
* Download MinGW-gcc-4.4.0 (MinGW-gcc440_1.zip) from: http://nosymbolfound.blogspot.com/2012/12/since-until-now-qt-under-windows-is.html
* Extract where you wish to install MinGW (there will not be an installer) and remember where you put it.

The MinGW variant of QT explicitly states that it needs MinGW 4.4 in order to function properly when using the pre-compiled binaries.
More recent versions are reported to function properly when compiling QT yourself, but these instructions use the pre-compiled binaries.

#### Install QT 4.8.4
* Download 'Qt libraries 4.8.4 for Windows (minGW 4.4, 317 MB)' from: http://qt-project.org/downloads
* Execute the installer (qt-win-opensource-4.8.4-mingw.exe) and follow the instructions.
	- You can install QT anywhere but you have to remember the installation directory.
	- When, during the installation of QT, you are asked where MinGW is installed,
	  make sure you provide the installation directory you remembered in the step above. 
	  You can now forget this directory.

#### (optional) Install FFmpeg 1.2
* Download the FFmpeg 1.2 shared and the FFmpeg 1.2 dev builds from: http://ffmpeg.zeranoe.com/builds/
	- For 32 bit:
		- Shared build: ffmpeg.zeranoe.com/builds/win32/shared/ffmpeg-1.2-win32-shared.7z
		- Dev build: ffmpeg.zeranoe.com/builds/win32/dev/ffmpeg-1.2-win32-dev.7z
	- For 64 bit:
		- Shared build: ffmpeg.zeranoe.com/builds/win64/shared/ffmpeg-1.2-win64-shared.7z
		- Dev build: ffmpeg.zeranoe.com/builds/win64/dev/ffmpeg-1.2-win64-dev.7z
* You can extract these libraries anywhere, but you have to remember where they are installed.
* Open (edit) qt-path.bat and change the paths after QT_DEV_PATH and QT_SHARED_PATH to the directories of the builds you just extracted. 
  Make sure the QT_DEV_PATH contains the 'include' and 'lib' directories.
  Make sure the QT_SHARED_PATH contains the 'bin' directory.
* In the instructions below, run compile-mpeg.bat instead of compile.bat
* This build will probably only run using run.bat
	  
#### Compile cppn-x
* Open (edit) qt-path.bat and make sure the path points to the bin directory in the QT installation directory you remembered in the previous step.
  As such, the second line should read something like: set PATH=C:\path\to\qt\4.8.4\bin
  Don't forget to save this file.
* Run compile.bat
* An executable (cppn-x.exe) should now be present in build\release

#### Run cppn-x
Usually you can just run build\release\cppn-x.exe
If that doesn't work due to dll errors, try running it using run.bat


## 2. Usage

To get started, you can either load an existing CPPN, or start a new CPPN from scratch. Example CPPN genome files are available at: http://www.evolvingai.org/PicbreederCanalization.

### Examining connections

Once a CPPN is loaded, click on any connection to select it. You can now use the slider at the bottom of the screen to change the weight of the selected connection and observe its effect on the final image. Alternatively, you can press the "start scan" button to automatically sweep over the entire range of values for that connection. Changing the values in the step, start, and end boxes will affect where the scan starts and ends, as well as its step size. Once you are done, you can click "reset" to change the weight back to its original value.

### Labeling connections

To label a connection, simply select a connection, type the name of the label in the top right box, and press "add label". Once the label is created, other connections can be labeled by selecting the connection, and then pressing the colored button of the relevant label. Lastly, the first 9 labels can also be assigned via the shortcuts Alt+1 through Alt+9.

### View and positioning

Many different options are available for changing how networks are displayed in the "view" menu, including showing or hiding labels, showing or hiding the sign (red for negative, green for positive) of each connection, showing or hiding the intermediate thumbnails, and showing a leged for all labels. In addition, some positioning options are available in the "Position" menu. Current positioning options are "Layered", "Circle", and "ONP" (Optimal Neuron Placement). The "Layered" and "Circle" options are self-explanatory. The "ONP" option attempts to minimize connection lenght while keeping the input and output in fixed positions. This mode can be helpful for discovering modules within the network.

### Other

CPPN-X has many other experimental functionalities under the "Experimental" options, including adding nodes and edges, changing acitvation functions, comparing different files. These functionalities are still in development, and not fully documented, so use them at your own risk.


## 3. Licenses
The code in this repository is licensed under the MIT License. The CPPN genomes available at http://www.evolvingai.org/PicbreederCanalization are for non-commercial use only, and UCFRF retains the copyright for the genomes and their respective generated images, including derivative images. See http://picbreeder.org/legalterms.php for more information regarding these CPPN genomes.


## 4. Questions?
If you have questions/suggestions, please feel free to [email](mailto:joost.hui@gmail.com) or create github issues. 
