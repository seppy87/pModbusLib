#pragma once

#include"config.hpp"


namespace d0 {

	namespace ModBus {



		struct Header {
			unsigned int functionCode;
			unsigned int Length;
			unsigned int DeviceId;
			unsigned int Address;
		};

		//types

		typedef std::function<void(Poco::Net::StreamSocket&, const d0::ModBus::Header& header, const std::string& msg)> ModbusCallback;

		namespace fc {
			constexpr int MB_READCOILS = 1;
			constexpr int MB_READINPUTSTATUS = 2;
			constexpr int MB_READHOLDINGREGISTER = 3;
			constexpr int MB_READINPUTREGISTER = 4;
			constexpr int MB_WRITESINGLECOIL = 5;
			constexpr int MB_READEXCEPTIONSTATUS = 7;
			constexpr int MB_DIAGNOSTIC = 8;
			constexpr int MB_GETCOMEVENTCOUNTER = 11;
			constexpr int MB_GETCOMEVENTLOG = 12;
			constexpr int MB_WRITEMULTIPLECOILS = 15;
			constexpr int MB_WRITEMULTIPLEREGISTERS = 16;
			constexpr int MB_REPORTSERVERID = 17;
			constexpr int MB_READFILERECORD = 20;
			constexpr int MB_WRITEFILERECORD = 21;
			constexpr int MB_MASKWRITEREGISTER = 22;
			constexpr int MB_READWRITEMULTIPLEREGISTER = 23;
		}

		class ModbusServer : public Poco::Net::TCPServerConnection
		{
		public:
			ModbusServer(const Poco::Net::StreamSocket& s) : Poco::Net::TCPServerConnection(s) {
			}

			void run();

			void processMBAP(const std::string msg);

			void setupCallback(int functionCode, ModbusCallback callback, bool async = false);

		private:
			std::map<int, std::string> MBAP;
			Header header;
			std::map<int, ModbusCallback> callbacks;
		};

		

		typedef Poco::Net::TCPServerConnectionFactoryImpl<d0::ModBus::ModbusServer> TCPFactory;
	}

	inline
		int getHex(std::string hexstr) {
		return (int)strtol(hexstr.c_str(), 0, 16);
	}
}