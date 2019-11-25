#!/bin/bash

# Exit when any command fails
set -e

# Save current working directory
pushd $(pwd) &>/dev/null
# On exit, return to previous working directory
trap "popd &>/dev/null" EXIT

# Move into a temp directory
cd $(mktemp -d)

####
## Constant Values
####

TARGET="x86_64-elf"

BINUTILS_VERSION="2.33.1"
BINUTILS_SOURCE="binutils-${BINUTILS_VERSION}"
BINUTILS_ARCHIVE="${BINUTILS_SOURCE}.tar.gz"
BINUTILS_URL="https://ftp.gnu.org/gnu/binutils/${BINUTILS_ARCHIVE}"

GCC_VERSION="9.2.0"
GCC_SOURCE="gcc-${GCC_VERSION}"
GCC_ARCHIVE="${GCC_SOURCE}.tar.gz"
GCC_URL="https://ftp.gnu.org/gnu/gcc/${GCC_SOURCE}/${GCC_ARCHIVE}"

DEFAULT_PREFIX="${HOME}/tools"
DEFAULT_MAKE_OPTS="-j$(nproc)"

####
## Helper Functions
####

# Copied from http://fitnr.com/showing-a-bash-spinner.html
function spinner()
{
    local pid=$1
    local delay=0.25
    local spinstr='|/-\'

    while [ "$(ps a | awk '{print $1}' | grep $pid)" ]; do
	local temp=${spinstr#?}
	printf " [%c] " "$spinstr"
	local spinstr=$temp${spinstr%"$temp"}
	sleep $delay
	printf "\b\b\b\b\b"
    done

    printf "\b\b\b\b\b\n"
}

####
## Get an install prefix
####

echo -n "Prefix to install to (${DEFAULT_PREFIX}): "
read PREFIX

PREFIX="${PREFIX:=${DEFAULT_PREFIX}}"

####
## Get make options
####

echo -n "Enter make options (${DEFAULT_MAKE_OPTS}): "
read MAKE_OPTS
n
MAKE_OPTS="${MAKE_OPTS:=${DEFAULT_MAKE_OPTS}}"

MAKE="make ${MAKE_OPTS}"

####
## Download source files
####

echo -n "Downloading binutils..."
wget -q -O ${BINUTILS_ARCHIVE} ${BINUTILS_URL} &
spinner $!

echo -n "Downloading gcc..."
wget -q -O ${GCC_ARCHIVE} ${GCC_URL} &
spinner $!

exit 0

####
## Binutils
####

tar xvf ${BINUTILS_ARCHIVE}

mkdir binutils-build
cd binutils-build

../${BINUTILS_SOURCE}/configure \
   --prefix=${PREFIX} \
   --target=${TARGET} \
   --disable-nls \
   --disable-werror \
   --with-sysroot

${MAKE}
${MAKE} install

cd ..

####
## GCC
####

tar xvf ${GCC_ARCHIVE}

mkdir gcc-build
cd gcc-build

../${GCC_SOURCE}/configure \
   --prefix=${PREFIX} \
   --target=${TARGET} \
   --disable-nls \
   --disable-werror \
   --enable-languages=c

${MAKE} all-gcc

${MAKE} all-target-libgcc CFLAGS_FOR_TARGET="-g -O2 -mcmodel=kernel -mno-red-zone" || true
sed -i 's/PICFLAG/DISABLED_PICFLAG/g' ${TARGET}/libgcc/Makefile
${MAKE} all-target-libgcc CFLAGS_FOR_TARGET="-g -O2 -mcmodel=kernel -mno-red-zone"

${MAKE} install-gcc
${MAKE} install-target-libgcc
