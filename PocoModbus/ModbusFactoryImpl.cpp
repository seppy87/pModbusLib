#include "ModbusFactoryImpl.h"

Poco::Net::TCPServerConnection* d0::ModBus::ModbusFactoryImpl::createConnection(const Poco::Net::StreamSocket& s)
{
	auto ret = new ModbusServer(s);
	for (auto cb : callbacks) {
		ret->setupCallback(cb.first, cb.second, async);
	}
	return ret;
}

void d0::ModBus::ModbusFactoryImpl::setupCallback(int functionCode, ModbusCallback functor)
{
	this->callbacks.insert(std::make_pair(functionCode, functor));
}
