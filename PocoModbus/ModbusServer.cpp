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
				ModBus::ModBusServerEventArgs args(ss, this->header, msg);
				args.errorFunc = [&](int errorCode) {this->sendErrorCode(errorCode); };
				this->callbacks[this->header.functionCode](this, args);
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
	this->header.rawHeader = msg.substr(0, 29);
}

void d0::ModBus::ModbusServer::setupCallback(int functionCode, ModbusCallback callback, bool async)
{
	if (async) {
		ModbusCallback mt = [&](const ModBus::ModbusServer * conn, d0::ModBus::ModBusServerEventArgs & args) { std::thread(callback,conn, std::ref(args)); };
		this->callbacks.insert(std::make_pair(functionCode, std::move(mt)));
		return;
	}
	this->callbacks.insert(std::make_pair(functionCode, callback));
}

void d0::ModBus::ModbusServer::sendErrorCode(int ErrorCode)
{
	char hexval[3];
	sprintf_s(hexval, "%0x", ErrorCode);
	std::string temp(hexval);
	for (UINT i = 0; i < temp.length();i++) {
		switch (temp[i]) {
		case 'a':
		case 'b':
		case 'c':
		case 'd':
		case 'e':
		case 'f':
			temp[i] = std::toupper(temp[i]);
			break;
		default:
			break;
		}
	}
	std::string errCode = "";
	if (temp.length() == 1) {
		errCode = '0' + temp;
	}
	else {
		errCode = temp;
	}
	char funcCode[5];
	int val = this->header.functionCode + 128;
	sprintf_s(funcCode, "%0x", val);
	std::string newHeader = "";
	newHeader = std::string(this->header.rawHeader);
	newHeader.replace(newHeader.size() - 2, newHeader.size(), std::string(funcCode));
	newHeader += (char)32;
	newHeader += errCode;
	socket().sendBytes(newHeader.c_str(), newHeader.size());
}
