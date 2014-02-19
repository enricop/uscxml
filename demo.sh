#!/bin/sh

if [ $# -lt 1 ]
then
        echo "Usage : $0 TimCommandID Write(1)/Read(0)"
        exit
fi

case "$1" in

-1) echo "Initializing Interpreter"
	ping 127.0.0.1
    ;;
0)  echo "Sending command $1"
	if [ "$2" -eq "1" ] ; then
		/usr/local/bin/modpoll -0 -1 -t0 -o4 -p 1502 -r1 -c1 127.0.0.1 1
	else
		/usr/local/bin/modpoll -0 -1 -t0 -o4 -p 1502 -r2 -c1 127.0.0.1
	fi
    ;;
1)  echo  "Sending command $1"
    	if [ "$2" -eq "1" ] ; then
		sleep 1				
	else

		/usr/local/bin/modpoll -0 -1 -t4 -o4 -p 1502 -r47 -c23 127.0.0.1
	fi
    ;;
2)  echo  "Sending command $1"
    	if [ "$2" -eq "1" ] ; then
		sleep 1	
	else
		/usr/local/bin/modpoll -0 -1 -t4 -o4 -p 1502 -r255 -c26 127.0.0.1
	fi
   ;;
3)  echo  "Sending command $1"
    	if [ "$2" -eq "1" ] ; then
		sleep 1
	else
		/usr/local/bin/modpoll -0 -1 -t4 -o4 -p 1502 -r463 -c114 127.0.0.1
	fi
   ;;
4)  echo  "Sending command $1"
    	if [ "$2" -eq "1" ] ; then
		sleep 1
	else
		/usr/local/bin/modpoll -0 -1 -t4 -o4 -p 1502 -r843 -c1 127.0.0.1
	fi
   ;;
5)  echo  "Sending command $1"
    	if [ "$2" -eq "1" ] ; then
		sleep 1
	else
		/usr/local/bin/modpoll -0 -1 -t4 -o4 -p 1502 -r844 -c23 127.0.0.1
	fi
   ;;
*) echo "Signal number $1 is not supported"
   ;;
esac

# comando 3 read

