#include"ModbusServer.h"
#include"ModbusFactoryImpl.h"


#if defined(POCO_OS_FAMILY_WINDOWS)
Poco::NamedEvent terminator(Poco::ProcessImpl::terminationEventName(Poco::Process::id()));
#else
Event terminator;
#endif

void test(const d0::ModBus::ModbusServer* conn , d0::ModBus::ModBusServerEventArgs& args) {
	std::cout << "Hallo Welt\n";
	args.errorFunc(d0::ModBus::ec::MB_NORESPONSE);
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