Install Ubuntu LTS 12

For core functionality without fancy plugins:

$ sudo apt-get install cmake
$ sudo apt-get install build-essential
$ sudo apt-get install libxml2-dev
$ sudo apt-get install libcurl4-openssl-dev

For OpenSceneGraph invoker (version in repositories lacks vrml plugin):

$ sudo apt-get install libpng12-dev
$ sudo apt-get install libjpeg-dev
$ sudo apt-get install libboost-all-dev
$ sudo apt-get install libltdl-dev
$ sudo apt-get install libopenscenegraph-dev # just for the dependencies
$ sudo apt-get remove libopenscenegraph-dev

$ wget http://sourceforge.net/projects/openvrml/files/latest/download
$ tar xvjf openvrml*
$ cd openvrml*
$ ./configure --disable-render-text-node --disable-script-node-javascript --disable-script-node-java --disable-gl-renderer --disable-xembed --disable-player --disable-examples --disable-mozilla-plugin
$ sudo make install
$ cd ..

$ sudo apt-get install git
$ git clone https://github.com/openscenegraph/osg
$ cd osg
$ nano src/osgPlugins/vrml/IndexedFaceSet.cpp # Edit line 207 and remove the else block

$ mkdir build && cd build
$ cmake ..
$ make -j4
$ sudo make install

For ffmpeg invoker:

Follow http://ffmpeg.org/trac/ffmpeg/wiki/UbuntuCompilationGuide for a version of recent version of ffmpeg
on ubuntu. The various libav* packages that come with the distribution are inadequate.

Install uscxml:
$ git clone https://github.com/tklab-tud/uscxml.git
$ cd uscxml
$ mkdir build && cd build
$ cmake ..
$ make