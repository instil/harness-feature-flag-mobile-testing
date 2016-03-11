%module(directors="1") Surge

%include <arrays_java.i>
%include <typemaps.i>
%include <std_string.i>
%include <std_vector.i>
%include <cdata.i>

%feature("director", assumeoverride=1) LoggingDelegate;
%feature("director", assumeoverride=1) RtspClientDelegate;

%apply (char *STRING, int LENGTH) { (const char *frameBuffer, size_t length) };

%apply int *OUTPUT { struct RtspSessionDescriptions* const palettes };

%ignore SurgeJava::RtspClientProxy::RtspSessionDescriptions::palettes; // We will wrap this another way
%typemap(javacode) SurgeJava::RtspClientProxy::RtspSessionDescriptions %{
  public RtspPalette[] getPaletteEntries() {
      RtspPalette[] ret = new RtspPalette[(int)getLength()];
      for (int i = 0; i < ret.length; ++i) {
          ret[i] = getPaletteEntry(i);
      }
      return ret;
  }
%}

%extend SurgeJava::RtspClientProxy::RtspSessionDescriptions {
  RtspPalette* getPaletteEntry(size_t pos) {
      return $self->palettes + pos;
  }
}

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

%{
#include "DelegateInterface.h"
#include "ErrorDispatcher.h"
#include "RtspResponse.h"
#include "DescribeResponse.h"
#include "SetupResponse.h"
#include "SessionDescription.h"
#include "SessionDescriptionV0.h"
#include "Logging.h"
#include "LoggingDelegate.h"    
#include "Surge.h"
#include "RtspClient.h"
#include "RtspClientProxy.h"

using namespace Surge;
using namespace SurgeUtil;
using namespace SurgeJava::LoggerProxy;
using namespace SurgeJava::RtspClientProxy;
%}

%include "ErrorDispatcher.h"
%include "DelegateInterface.h"
%include "Surge.h"
%include "RtspClient.h"
%include "RtspResponse.h"
%include "DescribeResponse.h"
%include "SetupResponse.h"
%include "SessionDescription.h"
%include "SessionDescriptionV0.h"
%include "Logging.h"
%include "LoggingDelegate.h"
%include "RtspClientProxy.h"
