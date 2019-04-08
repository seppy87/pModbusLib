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
				ModBus::ModBusServerEventArgs args(ss, this->header, msg,this->reg);
				args.errorFunc = [&](int errorCode) {this->sendErrorCode(errorCode); };
				args.testBit = [&](UINT reg_number, UINT bitnumber) { return this->testRegisterBit(reg_number, bitnumber); };
				args.sendReply = [&](std::optional<UINT16*> arg) { this->sendRegister(arg); };
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
	header.Length = d0::getHex(hex.c_str());
	std::cout << "Length = " << header.Length<<'\n';//msg[40]<<msg[41] << '\n';
	hex.clear();
	hex += msg[24];
	hex += msg[25];
	header.DeviceId = d0::getHex(hex.c_str());
	std::cout << "Device ID = " << header.DeviceId << '\n';
	hex.clear();
	hex += msg[31];
	hex += msg[32];
	hex += msg[34];
	hex += msg[35];
	header.Address = d0::getHex(hex.c_str());
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
	sprintf_s(funcCode, "%02X", val);
	std::string newHeader = "";
	newHeader = std::string(this->header.rawHeader);
	newHeader.replace(newHeader.size() - 2, newHeader.size(), std::string(funcCode));
	newHeader += (char)32;
	newHeader += errCode;
	socket().sendBytes(newHeader.c_str(), newHeader.size());
}

void d0::ModBus::ModbusServer::sendRegister(std::optional<UINT16*> args)
{

	UINT16* val = args.value_or(this->reg);
	std::string RegStr = "";
	for (UINT i = 0; i < 65; i++) {
		char c[2];
		memcpy_s(c, sizeof(c), (const void*)&val[i], 2);
		RegStr += std::string(c);
	}
	socket().sendBytes(RegStr.c_str(), RegStr.size());
}

bool d0::ModBus::ModbusServer::testRegisterBit(UINT reg_number, UINT bitnumber)
{
	return (std::bitset<16>(this->reg[reg_number])).test(bitnumber);
}

std::string d0::ModBus::ModbusServer::uint16ToChar()
{
	std::string result = "";
	for (UINT i = 0; i < 65; i++) {
		auto cont = reg[i];
		char c[5];
		sprintf_s(c, "%02X", cont);
		result += std::string(c) + (char)32;
	}
	return result;
}
