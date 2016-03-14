#!/usr/bin/env python

from distutils.core import setup
from Cython.Build import cythonize
from distutils.extension import Extension

extensions = [Extension("Surge",
                        language="c++",
                        extra_compile_args=['--std=c++11'],
                        define_macros=[('CYTHON', '1'),
                                       ('IS_BIG_ENDIAN', '0')],
                        sources=['src/wrappers/python/Surge.pyx',
                                 'src/Base64.cc',
                                 'src/DateTime.cc',
                                 'src/ErrorDispatcher.cc',
                                 'src/FireableEvent.cc',
                                 'src/H264Depacetizer.cc',               
                                 'src/Helpers.cc',
                                 'src/Mutex.cc',
                                 'src/RtpPacket.cc',
                                 'src/RtspClient.cc',
                                 'src/SessionDescriptionFactory.cc',
                                 'src/SocketHandler.cc',
                                 'src/Surge.cc',
                                 'src/Thread.cc',
                                 'src/WaitableEvents.cc']
                    )]

setup(
    ext_modules = cythonize(extensions)
)
