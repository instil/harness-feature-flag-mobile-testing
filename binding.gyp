# Copyright (c) 2016 Instil Software.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

{
    "targets": [
        {
            "target_name": "surge",

            "cflags" : [ "-std=c++1",
                         "-stdlib=libc++"
                       ],

            'cflags!': [ '-fno-exceptions' ],
            
            'cflags_cc!': [ '-fno-exceptions' ],
            
            "conditions": [
                [ 'OS!="win"', {
                    "cflags+": [ "-std=c++11" ],
                    "cflags_c+": [ "-std=c++11" ],
                    "cflags_cc+": [ "-std=c++11" ],
                    "defines": [ "BINDING_BUILD=1" ]
                }],
                [ 'OS=="mac"', {
                    "xcode_settings": {
                        "OTHER_CPLUSPLUSFLAGS" : [ "-std=c++11", "-stdlib=libc++" ],
                        "OTHER_LDFLAGS": [ "-stdlib=libc++" ],
                        "MACOSX_DEPLOYMENT_TARGET": "10.7",
                        'GCC_ENABLE_CPP_EXCEPTIONS': 'YES'
                    },
                }],
            ],

            'include_dirs': [
                './RxCpp/Rx/v2/src/',
            ],
            
            "sources": [
                
                "./src/Base64.cc",
                "./src/DateTime.cc",
                "./src/ErrorDispatcher.cc",
                "./src/FireableEvent.cc",
                "./src/H264Depacetizer.cc",
                "./src/Helpers.cc",
                "./src/InterleavedRtspTransport.cc",
                "./src/MJPEGDepacketizer.cc",
                "./src/MP4VDepacketizer.cc",
                "./src/Mutex.cc",
                "./src/RtpPacket.cc",
                "./src/RtspClient.cc",
                "./src/RtspCommandFactory.cc",
                "./src/SessionDescriptionFactory.cc",
                "./src/SessionDescriptionV0.cc",
                "./src/Surge.cc",
                "./src/Thread.cc",
                "./src/UdpTransport.cc",
                "./src/WaitableEvents.cc",
                
                "./src/wrappers/node/surge.cc"
            ]
        }
    ]
}
