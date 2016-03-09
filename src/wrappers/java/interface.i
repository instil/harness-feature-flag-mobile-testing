%module(directors="1") Surge

%feature("director", assumeoverride=1) RtspClientDelegate;

%include <arrays_java.i>
%include <typemaps.i>

%pragma(java) jniclasscode=%{
  static {
    try {
        System.loadLibrary("surge");
    } catch (UnsatisfiedLinkError e) {
        System.err.println("Native jni code library (libsurge.so) failed to load. \n" + e);
        System.exit(1);
    }
  }
%}

%apply (char *STRING, int LENGTH) { (const char *frameBuffer, size_t length) };

%{
#include "Surge.h"
#include "RtspClient.h"
#include "DelegateInterface.h"
%}

%include "Surge.h"
%include "RtspClient.h"
%include "DelegateInterface.h"

