#include "cmodbusconnect.h"

cModbusConnect::cModbusConnect()
{
    ModbusData.clear();
    isConnected = false;
}

void cModbusConnect::run()
{
    qDebug() << "Modbus Start";
    while(true)
    {
DISCONNECT:
        QThread::msleep(200);
        if(!ConnectMB())
        {
            qDebug() << "Connection Failed";
            isConnected = false;
        }
        else
            break;
    }

    uint16_t* m_getdata = (uint16_t*)malloc(256*sizeof(uint16_t));
    int AlarmCheck[16] = {
        1, 2, 4, 8, 16, 32, 64, 128, 256, 512,
        1024, 2048, 4096, 8192, 16384, 32768
    };
    ModBusDataType dataTemp;
    while(isRunningProgram)
    {
        int rc = modbus_read_registers(mb, 0x0000, 34, m_getdata);

        QString tempOut = "";
        for(int i = 0; i < 34; i++)
        {
            tempOut += QString::number(i);
            tempOut += ":";
            tempOut += QString::number(m_getdata[i]);
            tempOut += "/";
        }
        qDebug() << tempOut;

        if(rc == -1)
        {
            QThread::msleep(200);
            modbus_close(mb);
            modbus_free(mb);
            goto DISCONNECT;
        }

        if(m_getdata[18] != 0)
        {
            for(int i = 0; i < 16; i++)
            {
                if(m_getdata[18] == AlarmCheck[i])
                    m_getdata[18] = i + 1;
            }
        }

        while(true)
        {
            if(!isRunChamber && ModbusData.size() > 1)
                ModbusData.erase(ModbusData.begin());
            else
                break;
        }

        dataTemp.TempNow = m_getdata[0];
        if(dataTemp.TempNow >= 65335 && dataTemp.TempNow < 65535)
            dataTemp.TempNow = dataTemp.TempNow - 65536;

        dataTemp.TempTarget = m_getdata[1];
        if(dataTemp.TempTarget >= 65335 && dataTemp.TempTarget < 65535)
            dataTemp.TempTarget = dataTemp.TempTarget - 65536;

        dataTemp.HumidNow = m_getdata[4];
        dataTemp.HumidTarget = m_getdata[5];
        dataTemp.isRun = m_getdata[9];
        if(dataTemp.isRun == 1)
            isRunChamber = false;
        else
            isRunChamber = true;
        dataTemp.alarm = m_getdata[18];
        dataTemp.RunTimeH = m_getdata[23];
        dataTemp.RunTimeM = m_getdata[24];
        dataTemp.RunTimeS = m_getdata[25];
        dataTemp.StepNow = m_getdata[27];
        dataTemp.StepFull = m_getdata[28];
        dataTemp.CycleNow = m_getdata[30];
        dataTemp.CycleFull = m_getdata[31];
        dataTemp.RemainTimeH = m_getdata[32];
        dataTemp.RemainTimeM = m_getdata[33];
        ModbusData.push_back(dataTemp);
        QThread::msleep(300);
    }

    modbus_close(mb);
    modbus_free(mb);
    isRunningProgram = false;
}

bool cModbusConnect::send_datas(int writeAddr, uint16_t *default_datap, int num)
{
    if (modbus_write_registers(mb, writeAddr, num, default_datap) == -1)
    {
        qDebug() << "Send faild : " << modbus_strerror(errno);
        return false;
    }
    return true;
}

bool cModbusConnect::send(int writeAddr, uint16_t default_data)
{
    uint16_t sendData = default_data;

    if (modbus_write_register(mb, writeAddr, sendData) == -1)
    {
        qDebug() << "Send faild : " << modbus_strerror(errno);
        return false;
    }
    return true;
}

bool cModbusConnect::ProgramSet(std::vector<DBDataType> Recipe, int fullStep)
{
    int writeAddr = 1700;
    int SendDataNum = 1;
    int tempSendDataInt = 0;
    uint16_t* ChangedToSendData;
    ChangedToSendData = (uint16_t*)&Recipe.front().contRC.RECIPE_NO;
    if(modbus_write_registers(mb,writeAddr,SendDataNum,ChangedToSendData) == -1)
    {
        return false;
    }
    for(int step = 1; step <= fullStep; step++)
    {
        writeAddr = 1701;
        ChangedToSendData = (uint16_t*)&step;
        if(modbus_write_registers(mb,writeAddr,SendDataNum,ChangedToSendData) == -1)
            return false;

        writeAddr = 1724;
        SendDataNum = 1;
        for(int i = 0; i < 4; i++)
        {
            int j = 0;
            if(i == 0)
            {
                j = Recipe.front().contRC.TEMP * 10;
                ChangedToSendData = (uint16_t*)&j;
            }
            if(i == 1)
            {
                j = Recipe.front().contRC.HUMID * 10;
                ChangedToSendData = (uint16_t*)&j;
            }
            if(i == 2)
            {
                j = Recipe.front().contRC.TIME_DUR / 60;
                ChangedToSendData = (uint16_t*)&j;
            }
            if(i == 3)
            {
                j = (Recipe.front().contRC.TIME_DUR % 60) * 60;
                ChangedToSendData = (uint16_t*)&j;
            }
            if(modbus_write_registers(mb,writeAddr + i, SendDataNum, ChangedToSendData) == -1)
                return false;
        }

        writeAddr = 1706;
        SendDataNum = 1;
        tempSendDataInt = 3;
        ChangedToSendData = (uint16_t*)&(tempSendDataInt);
        if(modbus_write_registers(mb, writeAddr,SendDataNum,ChangedToSendData) == -1)
            return false;
        Recipe.erase(Recipe.begin());
    }

    writeAddr = 1700;
    SendDataNum = 1;
    ChangedToSendData = (uint16_t*)&Recipe.front().contRC.RECIPE_NO;
    if(modbus_write_registers(mb,writeAddr,SendDataNum,ChangedToSendData) == -1)
        return false;

    writeAddr = 1701;
    tempSendDataInt = 0;
    ChangedToSendData = (uint16_t*)&(tempSendDataInt);
    if(modbus_write_registers(mb,writeAddr,SendDataNum, ChangedToSendData) == -1)
        return false;

    writeAddr = 1745;
    SendDataNum = 1;
    for(int i = 0; i < 3; i++)
    {
        int j = 0;
        if(i == 0)
        {
            j = Recipe.front().reptRC.STEP_STRT;
            ChangedToSendData = (uint16_t*)&j;
        }
        if(i == 1)
        {
            j = Recipe.front().reptRC.STEP_END;
            ChangedToSendData = (uint16_t*)&j;
        }
        if(i == 2)
        {
            j = Recipe.front().reptRC.REPT_CYC;
            ChangedToSendData = (uint16_t*)&j;
        }
        if(modbus_write_registers(mb,writeAddr + i, SendDataNum, ChangedToSendData) == -1)
            return false;
    }

    writeAddr = 1706;
    SendDataNum = 1;
    tempSendDataInt = 3;
    ChangedToSendData = (uint16_t*)&(tempSendDataInt);
    if(modbus_write_registers(mb,writeAddr,SendDataNum, ChangedToSendData) == -1)
        return false;

    while(true)
    {
        writeAddr = 1707;
        ChangedToSendData = (uint16_t*)malloc(256*sizeof(uint16_t));
        modbus_read_registers(mb, writeAddr, 1, ChangedToSendData);
        if(ChangedToSendData[0] == 1)
            break;
    }

    return true;
}

bool cModbusConnect::ConnectMB()
{
    mb = modbus_new_rtu(COMPORT[2].c_str(), 38400, 'N', 8, 1);
    if(mb == NULL)
    {
        qDebug() << "libmodbus error : " << modbus_strerror(errno);
        return false;
    }
    if(modbus_set_slave(mb, 1) == -1)
    {
        qDebug() << "set slave id error : " << modbus_strerror(errno);
        return false;
    }
    modbus_set_debug(mb, TRUE);
    if (modbus_connect(mb) == -1)
    {
        qDebug() << "connect failed : " << modbus_strerror(errno);
        modbus_free(mb);
        return false;
    }
    struct timeval response;
    response.tv_sec = 0;
    response.tv_usec = 10000;
    modbus_set_response_timeout(mb,response.tv_sec,response.tv_usec);
    qDebug() << "Open Modbus Success";

    isConnected = true;
    return true;
}
