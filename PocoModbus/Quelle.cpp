#include"ModbusServer.h"
#include"ModbusFactoryImpl.h"

//typedef Poco::Net::TCPServerConnectionFactoryImpl<d0::ModbusServer> TCPFactory;
#if defined(POCO_OS_FAMILY_WINDOWS)
Poco::NamedEvent terminator(Poco::ProcessImpl::terminationEventName(Poco::Process::id()));
#else
Event terminator;
#endif

void test(Poco::Net::StreamSocket& s, const d0::ModBus::Header& header, const std::string& msg) {
	std::cout << "Hallo Welt\n";
}

int main(int argc, char** argv) {
	Poco::Net::initializeNetwork();
	auto t = new d0::ModBus::ModbusFactoryImpl();
	
	t->setupCallback(d0::ModBus::fc::MB_READHOLDINGREGISTER, &test);
	Poco::Net::TCPServer srv(t, 502);
	
	srv.start();
	terminator.wait();
	system("pause");
	Poco::Net::uninitializeNetwork();
	return 0;
}