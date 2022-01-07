#!/bin/bash

# shaderc and spirv-cross are not part of the bullseye repository.
debian_11()
{
  echo "Getting dependencies via apt"
  apt install g++ \
  	qtbase5-dev \
  	libopenal-data libopenal-dev libopenal1 \
  	libassimp-dev libassimp5 \
  	libglfw3 libglfw3-dev \
  	libsndfile1 libsndfile1-dev \
  	libglm-dev \
  	libbox2d-dev libbox2d2.3.0 \
  	libfreetype-dev libfreetype6 \
  	mono-complete \
    libcrypto++-dev
}

#Assumes /etc/os-release is present
source /etc/os-release

echo "OS: $PRETTY_NAME"

if [ "$ID" = "debian" ];
then
  if [ "$VERSION_ID" = "11" ];
  then
    debian_11
  else
    echo "Error: Unsupported host distribution"
  fi
else
  echo "Error: Unsupported host distribution"
fi