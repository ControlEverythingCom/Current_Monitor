#include "spark_wiring_constants.h"
#include "spark_wiring_i2c.h"
#include "spark_wiring.h"
#include "spark_wiring_usbserial.h"
class CurrentMonitor{
public:
    //Device info and device Status populated during initialization
    bool deviceStatusReady = false;
    byte sensorType = 0xFF;
    byte maxCurrent = 0xFF;
    byte numberOfChannels = 0xFF;
    byte firmwareVersion = 0xFF;
    
    //Int returned on failure to read channel current
    int failedCommand = 2147483646;
    
    //Must be called prior to using any other functions of the CurrentMonitor class.
    bool initialize(int a0, int a1, int a2, int a3);
    
    //returns current of given channel in milliamps
    int readChannelCurrentMa(int channel);
    
    //returns current of given channel in amps
    double readChannelCurrent(int channel);
    
    bool readAllChannelsMa(int *buffer);
    
    bool readAllChannels(double *buffer);
    
    bool readCalibration(int startChannel, int stopChannel, int *buffer);

private:
    int address = 42;
    byte header1 = 146;
    byte header2 = 106;
    byte readCurrent = 1;
    byte deviceInfo = 2;
    byte readCal = 3;
    byte setCal = 4;
    
    unsigned long readTimeout = 1000;
    int wait = 10;
    
    //calculates checksum and appends to a new byte array.  Must pass command to be wrapped and the length of the command being wrapped.  Will return the length of the new command.
    void calculateCheckSum(byte *command, byte *returnCommand, int commandLength);
    
    int convertToInt(byte msb_1, byte msb, byte lsb);
    
    bool i2CWrite(byte *command, int commandLength);
    bool i2CRead(int len, byte *buffer);
};