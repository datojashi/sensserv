#include <iostream>

#include<signal.h>
#include<unistd.h>


#include "senstelnet.h"

using namespace std;


void sig_handler(int signum){
    switch(signum)
    {
    case SIGUSR1:
        break;
    case SIGUSR2:
        break;
    case SIGTERM:
        break;
    case SIGKILL:
        break;
    }

    std::cout << signum << std::endl;
}



class SensDispatcher : public awl::Net::TcpSeverDispatcher<SensorThread>
{
public:
    SensDispatcher(awl::Net::TcpServer<SensorThread>* ss) : awl::Net::TcpSeverDispatcher<SensorThread>(ss)
    {

    }

    void processConnection()
    {
        std::cout << "Processing sensor connection, total connections="  << connections.size() << '\t' << connection->ID << std::endl;
    }
};

int main()
{
    //awl::Core::test();

    signal(SIGUSR1,sig_handler);
    signal(SIGUSR2,sig_handler);
    signal(SIGUSR1,sig_handler);
    signal(SIGTERM,sig_handler);
    signal(SIGKILL,sig_handler);

    awl::Core::Config config("/home/dato/server.conf");

    config.readconfig();


    awl::Net::SockAddr senssockaddress( config.getValue("sensaddr"),std::stoi(config.getValue("sensport")));
    awl::Net::SockAddr telnetsockaddress( config.getValue("telnetaddr"),std::stoi(config.getValue("telnetport")));


    /*
    awl::Net::SockAddr cliaddress("127.0.0.1",19899);
    awl::Net::SockAddr appaddress("127.0.0.1",19000);
    AppThread appcli(cliaddress,appaddress);
    appcli.start(true);
    */

    std::cout << "sensor address=" << senssockaddress.addr()  << ':' << senssockaddress.port() << std::endl;
    std::cout << "telnet address=" << telnetsockaddress.addr() << ':' << telnetsockaddress.port() <<  std::endl;


    awl::Net::TcpServer<SensTelnet> telnetserver(telnetsockaddress);
    telnetserver.config=&config;

    if(telnetserver.get_socketstate()==awl::Net::ssListening)
    {
        telnetserver.start(true);
    }

    awl::Net::TcpServer<SensorThread> sensorserver(senssockaddress);

    //std::cout << sensorserver.get_socketstate() << endl;
    //if(sensorserver.get_socketstate()==awl::Net::ssListening)
    //    sensorserver.start(true);



    std::vector<SensTelnet*>* telnets;

    SensDispatcher dispatcher(&sensorserver);
    dispatcher.start(true);

    while(1)
    {
        telnets = telnetserver.lock();
        for(size_t i=0; i<telnets->size(); i++)
        {
            //(*telnets)[i]->setPrompt("sensserv>");
            (*telnets)[i]->sensorserver=&sensorserver;
        }
        telnetserver.unlock();


        //sensorserver.processConnections(&sensrProcessing);
        //sensorserver.clean();
        awl::Core::awl_delay_ms(1000);
    }

    return 0;
}
