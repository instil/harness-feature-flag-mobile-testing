#!/bin/bash

# Script to build (or download) the dependencies required for Surge to function

printHelp() {
	echo ""
	echo "Surge FIRST-RUN script usage:"
	echo ""
	echo "./FIRST-RUN.sh: Checks out the repositories submodules and builds those that need compilation."
	echo "./FIRST-RUN.sh --no-build: Checks out the repositories submodules but downloads the binaries for the compilation-required libraries."
	echo ""
}

NO_BUILD=1

while [ "$1" != "" ]; do
    case $1 in

        -h | --help )
		printHelp
		shift
		exit 1
		;;

		-nb | --no-build )
		shift
		NO_BUILD=0
		;;

    	*)
		echo "ERROR: Unrecognised parameter ${1}, ignoring..."
		echo ""
    	shift
    	;;

    esac
    shift
done


buildOpenSSL() {
	cd core/lib/openssl-mobile

	if [[ $1 -eq 0 ]]; then
		./build-openssl.sh --no-build
	else
		./build-openssl.sh
	fi

	cd - > /dev/null
}

printf "	*** Checking out dependency submodules ***\n"
git submodule update --init --recursive

printf "\n	*** Building OpenSSL ***\n\n"
buildOpenSSL $NO_BUILD
