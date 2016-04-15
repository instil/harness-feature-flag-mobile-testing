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
