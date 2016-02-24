#!/usr/bin/env bash
set -e

VERSION="1.7.3"
[ -z "$BUILDER" ] && BUILDER=`whoami`
GENERATED_FILE_IOS="RTSP-iOS-$VERSION.md"
GENERATED_FILE_ANDROID="RTSP-android-$VERSION.md"
GENERATED_FILE_XAMARIN="RTSP-xamarin-$VERSION.md"
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
