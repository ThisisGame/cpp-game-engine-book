/**
Lightweight profiler library for c++
Copyright(C) 2016-2019  Sergey Yagovtsev, Victor Zarubkin

Licensed under either of
	* MIT license (LICENSE.MIT or http://opensource.org/licenses/MIT)
    * Apache License, Version 2.0, (LICENSE.APACHE or http://www.apache.org/licenses/LICENSE-2.0)
at your option.

The MIT License
	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights 
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies 
	of the Software, and to permit persons to whom the Software is furnished 
	to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all 
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
	INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR 
	PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE 
	LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, 
	TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE 
	USE OR OTHER DEALINGS IN THE SOFTWARE.


The Apache License, Version 2.0 (the "License");
	You may not use this file except in compliance with the License.
	You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.

**/
#ifndef EASY_PROFILER_SOCKET_H
#define EASY_PROFILER_SOCKET_H

#include <stdint.h>
#include <easy/details/easy_compiler_support.h>

#ifndef _WIN32

// Unix
# include <sys/types.h>
# include <sys/socket.h>
# include <netdb.h>
# include <stdio.h>
# include <unistd.h>
# include <fcntl.h>
# include <netinet/in.h> //for android-build

#else

// Windows
# define WIN32_LEAN_AND_MEAN
# include <winsock2.h>
# include <windows.h>
# include <ws2tcpip.h>
# include <stdlib.h>

#endif

class PROFILER_API EasySocket EASY_FINAL
{
public:

#ifdef _WIN32
    typedef SOCKET socket_t;
#else
    typedef int socket_t;
#endif

    enum class ConnectionState : int8_t
    {
        Disconnected = -1,
        Unknown,
        Connected,
        Connecting
    };

private:

    socket_t m_socket = 0;
    socket_t m_replySocket = 0;
    int m_wsaret = -1;

    struct hostent* m_server = nullptr;
    struct sockaddr_in m_serverAddress;

    ConnectionState m_state = ConnectionState::Unknown;

public:

    EasySocket();
    ~EasySocket();

    void setReceiveTimeout(int milliseconds);

    int send(const void* buf, size_t nbyte);
    int receive(void* buf, size_t nbyte);
    int listen(int count = 5);
    int accept();
    int bind(uint16_t portno);

    bool setAddress(const char* serv, uint16_t port);
    int connect();

    void flush();
    void init();

    ConnectionState state() const;
    bool isDisconnected() const;
    bool isConnected() const;

private:

    void checkResult(int result);
    bool checkSocket(socket_t s) const;
    void setBlocking(socket_t s, bool blocking);

}; // end of class EasySocket.

#endif // EASY_PROFILER_SOCKET_H
