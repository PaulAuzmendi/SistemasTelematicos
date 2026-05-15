#pragma once
class Modbus
{
public:
    // ===== MBAP Header =====
    unsigned short transactionId;
    unsigned short protocolId;       // siempre 0 para Modbus
    unsigned short length;           // se calcula en los Build, no hace falta tocarlo
    unsigned char  unitId;           // típicamente 0x01 sobre TCP/IP

    // ===== PDU =====
    unsigned char  functionCode;     // lo ponen los Build, lo rellenan los Parse
    unsigned short address;
    unsigned short quantity;         // FC 0x03 request: cuántos registros pedir
    unsigned short value;            // FC 0x06: valor a escribir / FC 0x03 resp: primer registro leído

    // Para respuestas con varios registros
    unsigned short registers[125];
    int            regCount;

    Modbus();

    // ---- Lado MAESTRO ----
    int  BuildReadRequest(unsigned char* buf);
    int  BuildWriteRequest(unsigned char* buf);
    bool ParseResponse(const unsigned char* buf, int len);

    // ---- Lado ESCLAVO ----
    bool ParseRequest(const unsigned char* buf, int len);
    int  BuildReadResponse(unsigned char* buf, const unsigned short* vals, int count);
    int  BuildWriteResponseEcho(unsigned char* buf);
};
