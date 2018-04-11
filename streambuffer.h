// streambuffer.h
//
// Defines the StreamBuffer class, which reads to and writes from a stream via
// a stream socket. The buffer is automatically allocated and deallocated.
//
// by: Justin Jo and Charles Wan

#ifndef _STREAMBUFFER_H_
#define _STREAMBUFFER_H_

#include <iostream>
#include <cstring>
#include <string>
#include <sstream>
#include <vector>
#include "c150streamsocket.h"
#include "c150debug.h"

using namespace std;
using namespace C150NETWORK;


class StreamBuffer {
private:
    char *buf;
    size_t buflen;

    void init() {
        buf = NULL;
        buflen = 0;
    }

    void clearBuffer() {
        if (buf != NULL) {
            delete [] buf;
            buf = NULL;
            buflen = 0;
        }
    }

    void readUntilNull(C150StreamSocket *sock) {
        vector<char> tmp;
        char lastRead;

        do {
            if (sock->read(&lastRead, 1) == 1) { // successfully read
                tmp.push_back(lastRead);
            } else { // no byte read before null, throw exception
                throw C150Exception(
                    "streabuffer: null byte not read before 0 length read"
                );
            }
        } while(lastRead != '\0');

        // save tmp to buf
        clearBuffer();
        buflen = tmp.size();
        buf = new char[buflen];
        memcpy(buf, &tmp[0], buflen);

        c150debug->printf(
            C150APPLICATION,
            "streambuffer.read: read %d bytes, including null term",
            buflen
        );
    }

public:
    StreamBuffer() {
        init();
    }

    StreamBuffer(const char *_buf, size_t _buflen) {
        init();
        setBuffer(_buf, _buflen);
    }

    ~StreamBuffer() {
        clearBuffer();
    }

    const char *c_str() {
        return buf;
    }

    size_t size() {
        return buflen;
    }

    void setBuffer(const char *_buf, size_t _buflen) {
        clearBuffer();
        buflen = _buflen;

        if (_buf == NULL || buflen == 0) {
            buf = NULL;
        } else {
            buf = new char[buflen];
            memcpy(buf, _buf, buflen);
        }
    }

    // reads lenToRead bytes from sock
    //  - if lenToRead is 0, read will read until null bytes is read
    //  - if bytes are successfully read, buf is cleared and replaced with the
    //    bytes read
    //  - if !lenToRead bytes read, exception is thrown, but existing buf is
    //    not overwritten
    void read(C150StreamSocket *sock, ssize_t lenToRead) {
        if (lenToRead == 0) {
            readUntilNull(sock);
            return;
        }

        char *tmp = new char[lenToRead];
        ssize_t readlen = sock->read(tmp, lenToRead);

        if (readlen == lenToRead) {
            clearBuffer();
            buf = tmp;
            buflen = readlen;

            c150debug->printf(
                C150APPLICATION,
                "streambuffer.read: read exactly %d bytes, as expected",
                readlen
            );
        } else {
            delete [] tmp;

            stringstream ss;
            ss << "streambuffer: " << lenToRead
               << " bytes could not be read, got " << readlen << " instead";
            throw C150Exception(ss.str());
        }
    }

    // writes the current buf to sock
    void write(C150StreamSocket *sock) {
        if (buflen > 0) {
            c150debug->printf(
                C150APPLICATION,
                "streambuffer.write: writing %d bytes",
                buflen
            );
            sock->write(buf, buflen);
        }
    }
};

#endif
