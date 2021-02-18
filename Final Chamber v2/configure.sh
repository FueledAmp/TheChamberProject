#!/bin/bash
a=`whoami`

if  test $a != "root"
then
	echo "Please execute this script as root"
	kill $$
fi

echo "Installing lib GTK"
sudo apt-get install libgtk2.0-dev
gcc -w -o main main.c `pkg-config --libs --cflags gtk+-2.0`
echo "Done!"
