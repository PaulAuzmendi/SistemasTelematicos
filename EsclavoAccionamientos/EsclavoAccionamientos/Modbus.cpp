#include "pch.h"
#include "Modbus.h"

Modbus::Modbus()
    : transactionId(0)
    , protocolId(0)
    , length(0)
    , unitId(0x01)
    , functionCode(0)
    , address(0)
    , quantity(0)
    , value(0)
    , regCount(0)
{
    for (int i = 0; i < 125; i++) registers[i] = 0;
}

// ---------- Lado MAESTRO ----------

int Modbus::BuildReadRequest(unsigned char* buf)
{
    functionCode = 0x03;
    length = 6;  // unitId + fc + addr + qty

    buf[0] = (unsigned char)(transactionId >> 8);
    buf[1] = (unsigned char)(transactionId & 0xFF);
    buf[2] = (unsigned char)(protocolId >> 8);
    buf[3] = (unsigned char)(protocolId & 0xFF);
    buf[4] = (unsigned char)(length >> 8);
    buf[5] = (unsigned char)(length & 0xFF);
    buf[6] = unitId;
    buf[7] = 0x03;
    buf[8] = (unsigned char)(address >> 8);
    buf[9] = (unsigned char)(address & 0xFF);
    buf[10] = (unsigned char)(quantity >> 8);
    buf[11] = (unsigned char)(quantity & 0xFF);
    return 12;
}

int Modbus::BuildWriteRequest(unsigned char* buf)
{
    functionCode = 0x06;
    length = 6;

    buf[0] = (unsigned char)(transactionId >> 8);
    buf[1] = (unsigned char)(transactionId & 0xFF);
    buf[2] = (unsigned char)(protocolId >> 8);
    buf[3] = (unsigned char)(protocolId & 0xFF);
    buf[4] = (unsigned char)(length >> 8);
    buf[5] = (unsigned char)(length & 0xFF);
    buf[6] = unitId;
    buf[7] = 0x06;
    buf[8] = (unsigned char)(address >> 8);
    buf[9] = (unsigned char)(address & 0xFF);
    buf[10] = (unsigned char)(value >> 8);
    buf[11] = (unsigned char)(value & 0xFF);
    return 12;
}

bool Modbus ::ParseResponse(const unsigned char* buf, int len)
{
    if (len < 8) return false;

    transactionId = (buf[0] << 8) | buf[1];
    protocolId = (buf[2] << 8) | buf[3];
    length = (buf[4] << 8) | buf[5];
    unitId = buf[6];
    functionCode = buf[7];

    if (functionCode == 0x03)
    {
        if (len < 9) return false;
        unsigned char byteCount = buf[8];
        regCount = byteCount / 2;
        if (regCount > 125) return false;
        if (len < 9 + byteCount) return false;

        for (int i = 0; i < regCount; i++) {
            registers[i] = (buf[9 + 2 * i] << 8) | buf[9 + 2 * i + 1];
        }
        if (regCount > 0) value = registers[0];   // atajo para una sola lectura
        return true;
    }
    else if (functionCode == 0x06)
    {
        if (len < 12) return false;
        address = (buf[8] << 8) | buf[9];
        value = (buf[10] << 8) | buf[11];
        return true;
    }
    return false;   // function code desconocido o respuesta de excepción
}

// ---------- Lado ESCLAVO ----------

bool Modbus::ParseRequest(const unsigned char* buf, int len)
{
    if (len < 12) return false;

    transactionId = (buf[0] << 8) | buf[1];
    protocolId = (buf[2] << 8) | buf[3];
    length = (buf[4] << 8) | buf[5];
    unitId = buf[6];
    functionCode = buf[7];
    address = (buf[8] << 8) | buf[9];

    if (functionCode == 0x03) {
        quantity = (buf[10] << 8) | buf[11];
        return true;
    }
    else if (functionCode == 0x06) {
        value = (buf[10] << 8) | buf[11];
        return true;
    }
    return false;
}

int Modbus::BuildReadResponse(unsigned char* buf, const unsigned short* vals, int count)
{
    unsigned char byteCount = (unsigned char)(2 * count);
    length = 3 + byteCount;   // unitId + fc + byteCount + data

    buf[0] = (unsigned char)(transactionId >> 8);
    buf[1] = (unsigned char)(transactionId & 0xFF);
    buf[2] = (unsigned char)(protocolId >> 8);
    buf[3] = (unsigned char)(protocolId & 0xFF);
    buf[4] = (unsigned char)(length >> 8);
    buf[5] = (unsigned char)(length & 0xFF);
    buf[6] = unitId;
    buf[7] = 0x03;
    buf[8] = byteCount;
    for (int i = 0; i < count; i++) {
        buf[9 + 2 * i] = (unsigned char)(vals[i] >> 8);
        buf[9 + 2 * i + 1] = (unsigned char)(vals[i] & 0xFF);
    }
    return 9 + byteCount;
}

int Modbus::BuildWriteResponseEcho(unsigned char* buf)
{
    length = 6;

    buf[0] = (unsigned char)(transactionId >> 8);
    buf[1] = (unsigned char)(transactionId & 0xFF);
    buf[2] = (unsigned char)(protocolId >> 8);
    buf[3] = (unsigned char)(protocolId & 0xFF);
    buf[4] = (unsigned char)(length >> 8);
    buf[5] = (unsigned char)(length & 0xFF);
    buf[6] = unitId;
    buf[7] = 0x06;
    buf[8] = (unsigned char)(address >> 8);
    buf[9] = (unsigned char)(address & 0xFF);
    buf[10] = (unsigned char)(value >> 8);
    buf[11] = (unsigned char)(value & 0xFF);
    return 12;
}