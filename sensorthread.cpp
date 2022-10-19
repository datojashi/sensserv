#include "sensorthread.h"


SensorThread::SensorThread()
{

}

SensorThread::~SensorThread()
{

}


bool SensorThread::addCommand(COMMAND cmd, bool mode)
{
    mutex.lock();
    if(mode)
    {
        commands.push(cmd);
        std::cout << "Add cmd " << cmd.cmd << std::endl;
    }
    else if(commands.empty())
    {
        commands.push(cmd);
        std::cout << "Add cmd PING " << cmd.cmd << std::endl;
    }


    mutex.unlock();
    return true;
}

void SensorThread::onstart()
{
    ct=0;
    ct2=0;
    ct3=0;
    chunk.clear();
    messages.clear();
    std::cout << "started Sensor thread" << std::endl;
}


void SensorThread::onstop()
{
    std::cout << "stoped Sensor thread" << std::endl;
}



void SensorThread::processAudioData()
{
    awl::ByteArray adata=awl::Core::byteArrayRight(message,sizeof (MESSAGE));
    for(unsigned int i=0; i<adata.size(); i++)
    {

    }
}

void SensorThread::processCommand()
{
    COMMAND cmd;
    cmd.cmd=cmd_None;

    if(mutex.try_lock())
    {
        if(!commands.empty())
        {
            cmd=commands.front();
            commands.pop();
        }
        mutex.unlock();
    }
    if(cmd.cmd!=cmd_None)
    {
        sendMsg(cmd);
        msgState.store(ms_sent);
        if(cmd.cmd!=cmd_ping_request)
            setLastCommand(cmd);
        //std::cout << "msgState=" << msgState.load() << std::endl;
    }
}

time_t SensorThread::setCurrentTime()
{
    time_t t=time(0);
    struct tm *dt=localtime(&t);
    reqdata[3]=dt->tm_sec;
    reqdata[4]=dt->tm_min;
    reqdata[5]=dt->tm_hour;
    reqdata[6]=dt->tm_mday;
    reqdata[7]=dt->tm_mon+1;
    reqdata[8]=dt->tm_year-100;
    //9,10,11 - reserve
    return t;

}



void SensorThread::sendMsg(COMMAND cmd)
{
    memset(reqdata,0,64);
    reqdata[1]=cmd.cmd;
    reqdata[2]=send_ct++;



    awl::ByteArray ba;
    awl::Core::initba(ba,reqdata,64);
    //awl::Core::printhex(ba);
    socket->send(ba);
}

void SensorThread::onwork()
{
    if(msgState.load()==ms_none)
    {
        processCommand();
    }
}

void SensorThread::onmessage()
{
    MESSAGE* msg = (MESSAGE*)message.data();

    //std::cout << "=== ";
    //awl::Core::printhex(message);
}

void SensorThread::ontimeout()
{
    //std::cout << "---" << std::endl;
}

void SensorThread::getmessage()
{

    std::cout << awl::Core::byteArrayToString(tba) << std::endl;

}


bool SensorThread::getResponse(awl::ByteArray& resp, uint8_t to)
{
    int ct=0;
    bool result=false;
    //std::cout << "getResponse    " << msgState.load() << std::endl;
    while(ct < to)
    {
        if(msgState.load()==ms_responded)
        {
            resp = awl::Core::byteArrayRight(message,sizeof (MESSAGE));
            result = true;
            break;
        }
        ct++;
        usleep(100000);
    }
    msgState.store(ms_none);
    return result;
}


unsigned SensorThread::getMsgState()
{
    return msgState.load();
}

COMMAND SensorThread::getLastCommand()
{
    COMMAND cmd;
    mutex.lock();
    cmd=last_cmd;
    mutex.unlock();
    return cmd;
}
void SensorThread::setLastCommand(COMMAND cmd)
{
    mutex.lock();
    last_cmd=cmd;
    mutex.unlock();
}
