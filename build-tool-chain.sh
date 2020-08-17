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

readonly BINUTILS_VERSION="2.35"
readonly BINUTILS_SOURCE="binutils-${BINUTILS_VERSION}"
readonly BINUTILS_ARCHIVE="${BINUTILS_SOURCE}.tar.gz"
readonly BINUTILS_URL="https://ftp.gnu.org/gnu/binutils/${BINUTILS_ARCHIVE}"

readonly GCC_VERSION="10.2.0"
readonly GCC_SOURCE="gcc-${GCC_VERSION}"
readonly GCC_ARCHIVE="${GCC_SOURCE}.tar.gz"
readonly GCC_URL="https://ftp.gnu.org/gnu/gcc/${GCC_SOURCE}/${GCC_ARCHIVE}"

readonly GRUB_REPO="https://git.savannah.gnu.org/git/grub.git"

readonly DEFAULT_PREFIX="${HOME}/tools"
readonly DEFAULT_MAKEOPTS="-j$(nproc)"

readonly INITIAL_DIRECTORY="$(pwd)"
readonly WORKING_DIRECTORY="$(mktemp -d)"

readonly LIBGCC_CFLAGS="-g -O2 -mcmodel=kernel -mno-red-zone"

JOBS=""
PREFIX=""

####
## Helper Functions
####

# Copied from http://fitnr.com/showing-a-bash-spinner.html
function spinner() {
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

main() {
	cd ${WORKING_DIRECTORY}

	## FIND REQUIRED TOOLS ##
	local __download=""
	local __make=""
	local __tar=""
	local __pkgmgr=""

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

	# PACKAGE MANAGER
	echo -n "checking for a system package manager... "
	case $(uname -s) in
		Darwin*)
			if hash brew 2>/dev/null; then
				echo "brew"
				__pkgmgr="brew"
			else
				echo "unkown"
			fi
			;;
		Linux*)
			if hash apt-get 2>/dev/null; then
				__pkgmgr="apt-get"
			else
				echo "unkown"
			fi
			;;
		*)
			echo "unkown"
			;;
	esac

	## GET USER OPTIONS ##
	echo
	echo "== Optional Settings =="

	local __prefix="${1:-${DEFAULT_PREFIX}}"
	local __makeopts="${2:-${DEFAULT_MAKEOPTS}}"

	export PATH="${__prefix}/bin:$PATH"

	echo
	echo "== Building Toolchain =="
	## BINUTILS ##
	echo -n "checking if ${TARGET}-as is already installed... "
	if [ -e ${__prefix}/bin/${TARGET}-as ]; then
		echo "already installed."
	else
		echo "not installed."
		run "${__download} ${BINUTILS_URL}" "Downloading binutils"
		run "${__tar} ${BINUTILS_ARCHIVE}" "Extracting binutils"

		mkdir binutils-build
		cd binutils-build

		local __binutils_config_cmd=$(
			cat <<-EOF
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
	echo -n "checking if ${TARGET}-gcc is already installed... "
	if [ -e ${__prefix}/bin/${TARGET}-gcc ]; then
		echo "already installed."
	else
		echo "not installed."
		run "${__download} ${GCC_URL}" "Downloading gcc"
		run "${__tar} ${GCC_ARCHIVE}" "Extracting gcc"

		cd ${GCC_SOURCE}
		run "contrib/download_prerequisites" "Downloading prerequisites"

		cd ..
		mkdir gcc-build
		cd gcc-build

		local __gcc_config_cmd=$(
			cat <<-EOF
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
		cd ..
	fi

	# GRUB MKRESCUE
	echo -n "checking if grub-mkrescue is already installed... "
	if hash grub-mkrescue 2>/dev/null; then
		echo "already installed."
	else
		echo "not installed."
		case "${__pkgmgr}" in
			apt-get)
				run "apt-get install -y grub-common" "Installing with apt-get"
				;;
			*)
				run "git clone ${GRUB_REPO} grub" "Cloning grub dev repo"
				cd grub
				run "./bootstrap" "Bootstraping grub"
				cd ..
				mkdir grub-build
				cd grub-build

				local __grub_config_cmd=$(
					cat <<-EOF
						../grub/configure \
						   --prefix=${__prefix} \
						   --target=${TARGET} \
						   --disable-werror \
						   TARGET_CC=${TARGET}-gcc \
						   TARGET_OBJCOPY=${TARGET}-objcopy \
						   TARGET_STRIP=${TARGET}-strip \
						   TARGET_NM=${TARGET}-nm \
						   TARGET_RANLIB=${TARGET}-ranlib
EOF
					  )
				run "${__grub_config_cmd}" "Configuring grub"
				run "${__make} ${__makeopts}" "Compiling grub"
				run "${__make} install" "Installing grub"
				cd ..
				;;
		esac
	fi

	# XORRISO
	echo -n "checking if xorriso is already installed... "
	if hash xorriso 2>/dev/null; then
		echo "already installed."
	else
		echo "not installed."
		case ${__pkgmgr} in
			brew*)
				run "brew install xorriso" "installing xorriso with brew"
				;;
			apt-get*)
				run "apt-get install -y xorriso" "Installing xorriso with apt-get"
				;;
			*)
				eexit "please install xorriso"
				;;
		esac
	fi

	# QEMU
	echo -n "checking if qemu-system-x86_64 is installed... "
	if hash qemu-system-x86_64 2>/dev/null; then
		echo "already installed."
	else
		echo "not installed."
		case ${__pkgmgr} in
			brew*)
				run "brew install qemu" "installing qemu with brew"
				;;
			*)
				eexit "please install qemu-system-x86_64"
				;;
		esac
	fi

	cd ${INITIAL_DIRECTORY}
	rm -rf ${WORKING_DIRECTORY}
	echo
	echo "Installation successful. Make sure that ${__prefix}/bin is in your PATH before running make"
	echo
	exit 0
}

ARGS=$(getopt -a -n build-tool-chain.sh -o p:j: --long jobs:,prefix: -- "$@")
if [ "$?" != "0" ] ; then
	echo "Invalid arguments"
fi

eval set -- "${ARGS}"

while :
do
	case "${1:-}" in
		-j | --jobs) JOBS="${2:-}" ; shift 2 ;;
		-p | --prefix) PREFIX="${2:-}" ; shift 2 ;;
		--) shift; break ;;
		*) echo "Unexpected option ${1:-}" ; exit 2 ;;
	esac
done

main "${PREFIX}" "-j${JOBS}"
