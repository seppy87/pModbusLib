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
#include<memory>
#include<optional>
#include<bitset>

namespace d0 {
	namespace ModBus {

		constexpr std::pair<UINT16,UINT16> splitUINT32(UINT32 val) {
			return std::make_pair((UINT16)(val >> 16), (UINT16)(val & 0x0000FFFFuL));
		}

		constexpr UINT32 joinUINT16(UINT16 x, UINT16 y) {
			return ((((UINT32)x) << 16) | ((UINT32)y));
		}

		constexpr std::pair<UINT8, UINT8> splitUINT16(UINT16 val) {
			return std::make_pair<UINT8,UINT8>((UINT8)(val >> 8), (UINT8)(val & 0xFFuL));
		}

		constexpr UINT16 joinUINT8(UINT8 x, UINT8 y) {
			return (((UINT16)x) << 8 | (UINT16)y);
		}

		constexpr std::pair<std::pair<UINT8, UINT8>, std::pair<UINT8, UINT8>> splitUINT32TOUINT8(UINT32 val) {
			auto x = splitUINT32(val);
			return std::make_pair(splitUINT16(x.first), splitUINT16(x.second));
		}

		constexpr UINT32 joinUINT8TOUINT32(UINT8 xl, UINT8 xh, UINT8 yl, UINT8 yh) {
			return joinUINT16(joinUINT8(xl, xh), joinUINT8(yl, yh));
		}

	}

	template<typename N>
	constexpr const char* toHex16(UINT16 val) {
		char ret[3];
		sprintf_s(ret, "%02X", val);
			return ret;
	}

	template<typename N>
	constexpr const char* toHex32(UINT32 val) {
		char ret[5];
		sprintf_s(ret, "%02X", val);
		return ret;
	}
}


