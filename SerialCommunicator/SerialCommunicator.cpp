/*
 * @Description: serial encapsulation
 * @Version: 1.0
 * @Autor: dangwi
 * @Date: 2022-02-27
 * @LastEditTime: 2023-06-23 17:39:56
 */

#include <mutex>
#include <thread>
#include <glog/logging.h>

#include "SerialCommunicator.hpp"
#include "serial/serial.h"

using namespace serial;
using namespace std;

static mutex mtx;

#define CHECK_ENABLE(ret) \
    if (false == enable)  \
    {                     \
        return ret;       \
    }

#define CHECK_CONNECT()         \
    {                           \
        mtx.lock();             \
        if (false == connected) \
            connect();          \
        mtx.unlock();           \
    }

SerialCommunicator::SerialCommunicator(bool _enable, vector<string> _port_names, vector<int> _header)
    : Serial(), enable(_enable), port_names(_port_names), header(_header)
{
    CHECK_ENABLE()
    connect();
}

SerialCommunicator::~SerialCommunicator()
{
    close();
}

void SerialCommunicator::connect()
{
    CHECK_ENABLE()
    int sz = port_names.size();
    int idx = 0;
    while (true)
    {
        LOG(INFO) << "connecting to " << port_names[idx];
        try
        {
            setPort(port_names[idx]);
            setBaudrate(115200); // 设置波特率
            // setBaudrate(1500000);  // 设置波特率
            auto timeout = Timeout::simpleTimeout(100);
            setTimeout(timeout);
            open();
            if (!isOpen())
            {
                LOG(ERROR) << "serial open failed !";
            }
            port_names.erase(port_names.begin() + idx);
            connected = true;
            break;
        }
        catch (exception &e)
        {
            LOG(ERROR) << "serial open failed !";
            LOG(ERROR) << e.what();
            this_thread::sleep_for(chrono::milliseconds(1000));
        }
        idx = (idx + 1) % sz;
    }
}

void SerialCommunicator::send(const SerialPkg &pkg)
{
    CHECK_ENABLE()
    CHECK_CONNECT()
    try
    {
        write((uint8_t *)pkg.frame, pkg.size);
    }
    catch (std::exception &e)
    {
        LOG(ERROR) << e.what();
    }
}

int SerialCommunicator::receive(uint8_t *buffer, size_t n)
{
    CHECK_ENABLE(0)
    CHECK_CONNECT()
    try
    {
        uint8_t check_bit;
        // for(int i = 0 ; i < 13 ; i++){
        //     read(&check_bit, 1);
        //     LOG(INFO) << i << " : " << hex << (int)check_bit;
        // }
        // return 0;
        for (int i = 0; i < header.size(); i++)
        {
            read(&check_bit, 1);
            if (check_bit != header[i])
            {
                LOG(WARNING) << "Head is wrong!";
                return 0;
            }
        }
        read(buffer, n);
        return n;
    }
    catch (std::exception &e)
    {
        LOG(ERROR) << e.what();
    }
}
