#!/usr/bin/env bash
set -e

VERSION="1.0.0"
[ -z "$BUILDER" ] && BUILDER=`whoami`
GENERATED_FILE_IOS="SURGE-iOS-$VERSION.md"
GENERATED_FILE_ANDROID="SURGE-android-$VERSION.md"
CURRENT_TIME=$(date "+%Y.%m.%d-%H.%M.%S")

if [[ "$1" = "--version" ]]; then
    echo "$VERSION"
fi
if [[ "$1" = "--builder" ]]; then
    echo "$BUILDER"
fi
if [[ "$1" = "--date" ]]; then
    echo "$CURRENT_TIME"
fi
