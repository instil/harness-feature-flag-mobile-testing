#!/usr/bin/env python

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
