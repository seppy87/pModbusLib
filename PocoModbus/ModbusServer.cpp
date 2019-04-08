#include "ModbusServer.h"

void d0::ModBus::ModbusServer::run()
{
	Poco::Net::StreamSocket& ss = socket();
	try
	{
		char buffer[256];
		int n = ss.receiveBytes(buffer, sizeof(buffer));
		while (n > 0)
		{
			std::cout << "SENDER = " << ss.peerAddress().toString() << '\n';
			std::cout << "Received " << n << " bytes:" << std::endl;
			std::string msg;
			Poco::Logger::formatDump(msg, buffer, n);
			std::cout << msg << std::endl;
			this->processMBAP(msg);
			if (this->callbacks.find(this->header.functionCode) != this->callbacks.end()) {
				this->callbacks[this->header.functionCode](ss, this->header, msg);
			}
			n = ss.receiveBytes(buffer, sizeof(buffer));
			
		}
		
	}
	catch (Poco::Exception & exc)
	{
		std::cerr << "ClientConnection: " << exc.displayText() << std::endl;
	}
	
}

void d0::ModBus::ModbusServer::processMBAP(const std::string msg)
{
	std::cout << "Function Code = " << msg[28] << '\n';
	header.functionCode = (int)msg[28]-48;
	std::string hex = "";
	hex += msg[40];
	hex += msg[41];
	header.Length = d0::getHex(hex);
	std::cout << "LEngth = " << header.Length<<'\n';//msg[40]<<msg[41] << '\n';
	hex.clear();
	hex += msg[24];
	hex += msg[25];
	header.DeviceId = d0::getHex(hex);
	std::cout << "Device ID = " << header.DeviceId << '\n';
	hex.clear();
	hex += msg[31];
	hex += msg[32];
	hex += msg[34];
	hex += msg[35];
	header.Address = d0::getHex(hex);
	std::cout << "Address=" << header.Address << '\n';
}

void d0::ModBus::ModbusServer::setupCallback(int functionCode, ModbusCallback callback, bool async)
{
	if (async) {
		//std::packaged_task<void(Poco::Net::StreamSocket&, const d0::ModBus::Header&, const std::string&)> task(std::move(callback));
		ModbusCallback mt = [&](Poco::Net::StreamSocket & s, const d0::ModBus::Header & header, const std::string & msg) { std::thread(callback, std::ref(s), std::ref(header), std::ref(msg)); };
		this->callbacks.insert(std::make_pair(functionCode, std::move(mt)));
		return;
	}
	this->callbacks.insert(std::make_pair(functionCode, callback));
}
