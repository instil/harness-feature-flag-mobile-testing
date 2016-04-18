#!/usr/bin/env python

from distutils.core import setup
from Cython.Build import cythonize
from distutils.extension import Extension

extensions = [Extension("Surge",
                        language = "c++",
                        extra_compile_args = ['--std=c++11'],
                        
                        define_macros = [('CYTHON', '1'),
                                         ('BINDING_BUILD', '1'),
                                         ('IS_BIG_ENDIAN', '0')],

                        include_dirs = ['./RxCpp/Rx/v2/src/'],
                        
                        sources = [
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
                            
                            "./src/wrappers/python/Surge.pyx",
                        ]
                    )]

setup(
    name='Surge',
    version='1.0',
    description='RTSP implementation supporting TCP-interleaved and UDP',
    author='Instil Software',
    url='https://github.com/instil/surge',
    ext_modules=cythonize(extensions)
)
