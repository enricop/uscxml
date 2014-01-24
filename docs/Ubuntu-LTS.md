Install Ubuntu LTS 12

For core functionality without fancy plugins:

$ sudo apt-get install cmake build-essential libxml2-dev libcurl4-openssl-dev

For OpenSceneGraph invoker (version in repositories lacks vrml plugin):

$ sudo apt-get install libpng12-dev libjpeg-dev libboost-all-dev libltdl-dev
$ sudo apt-get install libopenscenegraph-dev # just for the dependencies
$ sudo apt-get remove libopenscenegraph-dev

$ wget http://sourceforge.net/projects/openvrml/files/latest/download
$ tar xvjf openvrml*
$ cd openvrml*
$ ./configure --disable-render-text-node --disable-script-node-javascript --disable-script-node-java --disable-gl-renderer --disable-xembed --disable-player --disable-examples --disable-mozilla-plugin
$ sudo make install
$ cd ..

for mavericks:
PNG_CFLAGS="-I/opt/local/include" PNG_LIBS="-L/opt/local/lib" XML_CFLAGS="-I`xcrun --show-sdk-path`/usr/include/libxml2" XML_LIBS="-L`xcrun --show-sdk-path`/usr/lib" CPPFLAGS="--sysroot=`xcrun --show-sdk-path` -I/opt/local/include -I`xcrun --show-sdk-path`/usr/include/libxml2" LDFLAGS="--sysroot=`xcrun --show-sdk-path` -L/opt/local/lib -lboost_system-mt -lxml2 -lpng" ./configure --disable-render-text-node --disable-script-node-javascript --disable-script-node-java --disable-gl-renderer --disable-xembed --disable-player --disable-examples --disable-mozilla-plugin


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