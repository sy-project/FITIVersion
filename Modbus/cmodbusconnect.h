#ifndef CMODBUSCONNECT_H
#define CMODBUSCONNECT_H

#include "framework.h"
#include "cthread.h"

#include <QDebug>
#include <modbus.h>

class cModbusConnect : public Singleton<cModbusConnect>, public cThread
{
private:
    friend class Singleton;
    modbus_t* mb;
    bool isConnected;
    std::vector<ModBusDataType> ModbusData;

public:
    cModbusConnect();

    void run();

    bool send_datas(int writeAddr = 0, uint16_t* default_datap = 0, int num = 0);
    bool send(int writeAddr = 0, uint16_t default_data = 0);
    bool ProgramSet(std::vector<DBDataType> Recipe, int fullStep);
    bool GetConnect() { return isConnected; }
    ModBusDataType GetModbusData() {
        if(ModbusData.size() > 0)
            return ModbusData.back();
        else
            return GetModbusData();
    }
    int GetDataSize()
    {
        return ModbusData.size();
    }

private:
    bool ConnectMB();
};

#endif // CMODBUSCONNECT_H
