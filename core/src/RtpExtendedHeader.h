// -*-c++-*-
// Copyright (c) 2016 Instil Software.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#ifndef __RTP_EXTENDED_HEADER_H__
#define __RTP_EXTENDED_HEADER_H__

#include <cstdlib>

namespace Surge {

    class RtpExtendedHeader {
    public:
        RtpExtendedHeader(const unsigned char *buffer, size_t length) {
            m_buffer = (unsigned char *)malloc(length);
            memcpy(m_buffer, buffer, length);
        }

        ~RtpExtendedHeader() {
            free(m_buffer);
        }

        const unsigned char *GetBuffer() const { return m_buffer; }

        size_t GetBufferLength() const { return m_length; }
        
    private:
        unsigned char *m_buffer;
        size_t m_length;
    };
    
}

#endif //__RTP_EXTENDED_HEADER_H__
