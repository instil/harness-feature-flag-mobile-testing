#!/bin/bash

# Script to build (or download) the dependencies required for Surge to function

printHelp() {
	echo ""
	echo "Surge Install Dependencies script usage:"
	echo ""
	echo "./INSTALL-DEPENDENCIES.sh: Downloads all Surge Core dependencies, downloading precompiled binaries rather than building from source"
	echo "./INSTALL-DEPENDENCIES.sh --build: Downloads th source for all Surge Core dependencies, and builds them form source. This assumes you will ahev all the required tools installed."
	echo ""
}

BUILD=0

while [ "$1" != "" ]; do
	case $1 in
		-h | --help )
			printHelp
			shift
			exit 1
			;;

		-b | --build )
			shift
			BUILD=1
			;;

		*)
			echo "ERROR: Unrecognised parameter ${1}, ignoring..."
			echo ""
			shift
			;;

	esac
	shift
done


acquireDependency() {
	cd core/lib/$1

	if [[ $2 -eq 1 ]]; then
		./build.sh
	else
		./download.sh
	fi

	cd - > /dev/null
}

printf "	*** Checking out dependency submodules ***\n"
git submodule update --init --recursive

printf "\n	*** Acquiring OpenSSL & libuv ***\n\n"
acquireDependency openssl-mobile $BUILD
acquireDependency libuv-cmake $BUILD
