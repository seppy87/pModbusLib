#pragma once
#include"config.hpp"
#include"ModbusServer.h"

namespace d0 {
	namespace ModBus {
		class ModbusFactoryImpl : public Poco::Net::TCPServerConnectionFactoryImpl<d0::ModBus::ModbusServer>
		{
		public:
			Poco::Net::TCPServerConnection* createConnection(const Poco::Net::StreamSocket& s) override;
			void setAsync(bool t = true) {
				this->async = t;
			}

			void setupCallback(int functionCode, ModbusCallback functor);
		private:
			std::map<int, ModbusCallback> callbacks;
			bool async = false;
		};
	}

}