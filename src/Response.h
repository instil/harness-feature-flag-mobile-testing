// -*-c++-*-
#ifndef __RESPONSE_H__
#define __RESPONSE_H__

#include <cstdlib>

namespace Surge {

    class Response {
    public:
        Response(unsigned char *buffer, size_t length, bool copy):
            m_buffer(buffer),
            m_length(length),
            m_copy(copy) {
            if (copy) {
                unsigned char *copy = (unsigned char*)malloc(length);
                memcpy((void*)copy, (const void *)buffer, length);
                m_buffer = copy;
            }
        }

        ~Response() {
            if (m_copy) {
                free(m_buffer);
            }
        }

        const unsigned char *BytesPointer() const { return m_buffer; }
        
        const size_t PointerLength() const { return m_length; }

    private:
        unsigned char *m_buffer;
        size_t m_length;
        bool m_copy;
    };  
    
}

#endif //__RESPONSE_H__
