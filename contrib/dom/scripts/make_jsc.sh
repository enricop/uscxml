#!/bin/bash

find ../idl/ -name *.idl -exec ./generate-bindings.pl --outputDir=/Users/sradomski/Documents/TK/Code/uscxml/src/uscxml/plugins/datamodel/ecmascript/JavaScriptCore/dom --include=../idl/ --generator=ArabicaJSC {} \;
