#pragma once
#define POCO_STATIC

#pragma comment(lib,"Ws2_32.lib")
#pragma comment(lib,"Iphlpapi.lib")
#include<Poco/Net/TCPServer.h>
#include<Poco/Net/TCPServerConnection.h>
#include<Poco/Net/TCPServerConnectionFactory.h>
#include<Poco/Net/StreamSocket.h>
#include<Poco/NumberParser.h>
#include<Poco/Process.h>
#include<Poco/Logger.h>
#include<Poco/NamedEvent.h>

#include<iostream>
#include<functional>
#include<future>
#include<thread>


