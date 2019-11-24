#!/bin/bash

# Detect uninitialised variables
set -o nounset
# Exit when any command fails
set -o errexit
# Exit on error inside any functions or subshells
set -o errtrace
# Detect errors before piping
set -o pipefail

####
## Constant Values
####

readonly TARGET="x86_64-elf"

readonly BINUTILS_VERSION="2.33.1"
readonly BINUTILS_SOURCE="binutils-${BINUTILS_VERSION}"
readonly BINUTILS_ARCHIVE="${BINUTILS_SOURCE}.tar.gz"
readonly BINUTILS_URL="https://ftp.gnu.org/gnu/binutils/${BINUTILS_ARCHIVE}"

readonly GCC_VERSION="9.2.0"
readonly GCC_SOURCE="gcc-${GCC_VERSION}"
readonly GCC_ARCHIVE="${GCC_SOURCE}.tar.gz"
readonly GCC_URL="https://ftp.gnu.org/gnu/gcc/${GCC_SOURCE}/${GCC_ARCHIVE}"

readonly DEFAULT_PREFIX="${HOME}/tools"
readonly DEFAULT_MAKEOPTS="-j$(nproc)"

readonly INITIAL_DIRECTORY="$(pwd)"
readonly WORKING_DIRECTORY="$(mktemp -d)"

readonly LIBGCC_CFLAGS="-g -O2 -mcmodel=kernel -mno-red-zone"

####
## Helper Functions
####

# Copied from http://fitnr.com/showing-a-bash-spinner.html
function spinner()
{
    local pid=$1
    local delay=0.1
    local spinstr='|/-\'

    while [ "$(ps a | awk '{print $1}' | grep $pid)" ]; do
	local temp=${spinstr#?}
	printf "[%c]" "$spinstr"
	local spinstr=$temp${spinstr%"$temp"}
	sleep $delay
	printf "\b\b\b"
    done

    printf "   \b\b\b"
}


run() {
    local __cmd=$1
    local __msg=$2
    local __logfile="${WORKING_DIRECTORY}/build-toold-chain.log"

    eval "${__cmd}" &>${__logfile} &
    local __pid=$!

    echo -n "${__msg}... "
    spinner $__pid

    if wait $__pid; then
	echo "done."
	rm $__logfile
    else
	eexit "Failed to run '${__cmd}'. A logfile has been saved at ${__logfile}"
    fi
}

####
## Clean up functions
####

## Called to exit with an error message
eexit() {
    local __message=${1:-"An unkown error occured!"}
    echo ${__message}
    kill_subprocesses
    exit -1
}

## Called on interrupt
kill_subprocesses() {
    jobs -p | xargs kill &>/dev/null
}

## Always called on exit
finish() {
    cd ${INITIAL_DIRECTORY}
}

####
## Setting Traps
####

trap 'kill_subprocesses' INT
trap 'finish' EXIT

####
## Entry point
####

main () {
    cd ${WORKING_DIRECTORY}

    ## FIND REQUIRED TOOLS ##
    local __download=""
    local __make=""
    local __tar=""

    echo "== Looking For Required Tools =="

    # WGET or CURL
    echo -n "checking for wget... "
    if hash wget 2>/dev/null; then
	echo "ok"
	__download="wget -q"
    else
	echo "not found"
	echo -n "checking for curl... "
	if hash curl 2>/dev/null; then
	    __download="curl -sO"
	    echo "ok"
	else
	    eexit "not found"
	fi
    fi

    # MAKE
    echo -n "checking for make... "
    hash make 2>/dev/null || eexit "not found"
    __make="make"
    echo "ok"

    # TAR
    echo -n "checking for tar... "
    hash tar 2>/dev/null || eexit "not found"
    __tar="tar -xf"
    echo "ok"

    ## GET USER OPTIONS ##
    echo
    echo "== Optional Settings =="

    local __prefix=""
    local __makeopts=""

    echo -n "Prefix to install to (${DEFAULT_PREFIX}): "
    read __prefix
    __prefix="${__prefix:-${DEFAULT_PREFIX}}"

    echo -n "Enter make options (${DEFAULT_MAKEOPTS}): "
    read __makeopts
    __makeopts="${__makeopts:-${DEFAULT_MAKEOPTS}}"

    export PATH="${__prefix}/bin:$PATH"

    echo
    echo "== Building Toolchain =="
    ## BINUTILS ##
    echo -n "checking if ${TARGET}-as is already installed... "
    if [ -e ${__prefix}/bin/${TARGET}-as ]; then
	echo "already installed."
    else
	run "${__download} ${BINUTILS_URL}" "Downloading binutils"
	run "${__tar} ${BINUTILS_ARCHIVE}" "Extracting binutils"

	mkdir binutils-build
	cd binutils-build

	local __binutils_config_cmd=$(cat <<- EOF
    ../${BINUTILS_SOURCE}/configure \
       --prefix=${__prefix} \
       --target=${TARGET} \
       --disable-nls \
       --disable-werror \
       --with-sysroot
EOF
	      )

	run "${__binutils_config_cmd}" "Configuring binutils"
	run "${__make} ${__makeopts}" "Compiling binutils"
	run "${__make} install" "Installing binutils"

    cd ..
    fi

    ## GCC ##
    echo -n "check if ${TARGET}-gcc is already installed... "
    if [ -e ${__prefix}/bin/${TARGET}-gcc ]; then
	echo "already installed."
    else
	run "${__download} ${GCC_URL}" "Downloading gcc"
	run "${__tar} ${GCC_ARCHIVE}" "Extracting gcc"

	cd ${GCC_SOURCE}
	run "contrib/download_prerequisites" "Downloading prerequisites"

	cd ..
	mkdir gcc-build
	cd gcc-build

	local __gcc_config_cmd=$(cat <<- EOF
    ../${GCC_SOURCE}/configure \
       --prefix=${__prefix} \
       --target=${TARGET} \
       --disable-nls \
       --disable-werror \
       --enable-languages=c
EOF
	      )

	run "${__gcc_config_cmd}" "Configuring gcc"
	run "${__make} ${__makeopts} all-gcc" "Compiling gcc"

	local __libgcc_compile_cmd='${__make} ${__makeopts} all-target-libgcc CFLAGS_FOR_TARGET="${LIBGCC_CFLAGS}"'

	eval "${__libgcc_compile_cmd}" &>/dev/null &
	local __libgcc_configure_pid=$!
	echo -n "Configuring libgcc... "
	spinner $__libgcc_configure_pid
	sed -i -e 's/PICFLAG/DISABLED_PICFLAG/g' ${TARGET}/libgcc/Makefile
	echo "done."

	run "${__libgcc_compile_cmd}" "Compile libgcc"

	run "${__make} install-gcc" "Installing gcc"
	run "${__make} install-target-libgcc" "Installing libgcc"
    fi

    cd ${INITIAL_DIRECTORY}
    rm -rf ${WORKING_DIRECTORY}
    echo
    echo "Installation successful. Make sure that ${__prefix}/bin is in your PATH before running make"
    echo
    exit 0
}
main
