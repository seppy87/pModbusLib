#pragma once

#include"config.hpp"


namespace d0 {

	namespace ModBus {
		//types
		class ModbusServer;
		struct ModBusServerEventArgs;
		using ModbusCallback = std::function<void(const ModBus::ModbusServer* sender, ModBus::ModBusServerEventArgs& args)>;
		using HoldingRegister = UINT16[65];


		struct Header {
			unsigned int functionCode=0;
			unsigned int Length=0;
			unsigned int DeviceId=0;
			unsigned int Address=0;
			std::string rawHeader;
		};

		struct ModBusServerEventArgs {
			Poco::Net::StreamSocket& socket;
			const d0::ModBus::Header& header;
			const std::string& msg;
			HoldingRegister& reg;
			std::function<void(int)> errorFunc;
			std::function<bool(UINT, UINT)> testBit;
			std::function<void(std::optional<UINT16*>)> sendReply;
			ModBusServerEventArgs(Poco::Net::StreamSocket& socket, const ModBus::Header& header, const std::string& msg, d0::ModBus::HoldingRegister& reg) : socket(socket),header(header),msg(msg),reg(reg){}

		};

		
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

		namespace ec {
			constexpr int MB_UNSUPPORTED_FUNCTIONCODE = 1;
			constexpr int MB_INVALIDADDRESS = 2;
			constexpr int MB_INVALID_VALUES = 3;
			constexpr int MB_SERVERBUSY = 6;
			constexpr int MB_NORESPONSE = 11;
		}

		class ModbusServer : public Poco::Net::TCPServerConnection
		{
		public:
			ModbusServer(const Poco::Net::StreamSocket& s) : Poco::Net::TCPServerConnection(s) {
			}

			void run();

			void processMBAP(const std::string msg);

			void setupCallback(int functionCode, ModbusCallback callback, bool async = false);

			void sendErrorCode(int ErrorCode);
			void sendRegister(std::optional<UINT16*> args);

			bool testRegisterBit(UINT reg_number, UINT bitnumber);

			std::string uint16ToChar();

		private:
			std::map<int, std::string> MBAP;
			Header header;
			std::map<int, ModbusCallback> callbacks;
			HoldingRegister reg;
		};

		

		typedef Poco::Net::TCPServerConnectionFactoryImpl<d0::ModBus::ModbusServer> TCPFactory;
	}

	inline int getHex(const char* hexstr) {
		return (int)strtol(hexstr, 0, 16);
	}
}