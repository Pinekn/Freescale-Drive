#!/bin/sh
function Execute
{
	echo "#define BMP_IN_TEST 		0 " > bpm.h
	echo "#define BMP_IN_NORMAL 	1 " >> bpm.h
	echo "#define BMP_MODE_IN_NORMAL " >> bpm.h
	make -f "makefile" clean
	make -j2 -f "makefile"
}

#Execute
