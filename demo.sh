#!/bin/sh

if [ $# -lt 1 ]
then
        echo "Usage : $0 TimCommandID Write(1)/Read(0)"
        exit
fi

TIM="127.0.0.1"
PORT="1502"

case "$1" in

-1) echo "Initializing Interpreter"
	ping $TIM
    ;;
0)  echo "Sending command $1"
	if [ "$2" -eq "1" ] ; then
		/usr/local/bin/modpoll -0 -1 -t0 -o4 -p $PORT -r1 -c1 $TIM 1
	else
		/usr/local/bin/modpoll -0 -1 -t0 -o4 -p $PORT -r2 -c1 $TIM
	fi
    ;;
1)  echo  "Sending command $1"
    	if [ "$2" -eq "1" ] ; then
		echo "Write Mode is Not Supported"		
	else
		/usr/local/bin/modpoll -0 -1 -t4 -o4 -p $PORT -r76 -c76 $TIM
	fi
    ;;
2)  echo  "Sending command $1"
    	if [ "$2" -eq "1" ] ; then
		echo "Write Mode is Not Supported"		
	else
		/usr/local/bin/modpoll -0 -1 -t4 -o4 -p $PORT -r385 -c56 $TIM
	fi
   ;;
3)  echo  "Sending command $1"
    	if [ "$2" -eq "1" ] ; then
		echo "Write Mode is Not Supported"		
	else
		/usr/local/bin/modpoll -0 -1 -t4 -o4 -p $PORT -r663 -c114 $TIM
	fi
   ;;
4)  echo  "Sending command $1"
    	if [ "$2" -eq "1" ] ; then
		echo "Write Mode is Not Supported"		
	else
		/usr/local/bin/modpoll -0 -1 -t4 -o4 -p $PORT -r1043 -c1 $TIM
	fi
   ;;
5)  echo  "Sending command $1"
    	if [ "$2" -eq "1" ] ; then
		echo "Write Mode is Not Supported"		
	else
		/usr/local/bin/modpoll -0 -1 -t4 -o4 -p $PORT -r1044 -c23 $TIM
	fi
   ;;
6)  echo  "Sending command $1"
    	if [ "$2" -eq "1" ] ; then
		/usr/local/bin/modpoll -0 -1 -t4 -o4 -p $PORT -r1123 -c1 $TIM 18
	else
		/usr/local/bin/modpoll -0 -1 -t4 -o4 -p $PORT -r1067 -c29 $TIM
	fi
   ;;
7)  echo  "Sending command $1"
	if [ "$2" -eq "1" ] ; then
		/usr/local/bin/modpoll -0 -1 -t4 -o4 -p $PORT -r1459 -c1 $TIM 1
	else
		/usr/local/bin/modpoll -0 -1 -t4 -o4 -p $PORT -r1377 -c84 $TIM
	fi
   ;;
8)  echo  "Sending command $1"
    	if [ "$2" -eq "1" ] ; then
		/usr/local/bin/modpoll -0 -1 -t4 -o4 -p $PORT -r1687 -c1 $TIM 1
	else
		/usr/local/bin/modpoll -0 -1 -t4 -o4 -p $PORT -r1677 -c23 $TIM
	fi
   ;;
9)  echo  "Sending command $1"
    	if [ "$2" -eq "1" ] ; then
		echo "Write Mode is Not Supported"	
	else
		/usr/local/bin/modpoll -0 -1 -t4 -o4 -p $PORT -r1802 -c29 $TIM
	fi
   ;;
10)  echo  "Sending command $1"
    	if [ "$2" -eq "1" ] ; then
		echo "Write Mode is Not Supported"	
	else
		/usr/local/bin/modpoll -0 -1 -t4 -o4 -p $PORT -r2089 -c32 $TIM
	fi
   ;;
*) echo "Signal number $1 is not supported"
   ;;
esac

# comando 3 read

