#import "Current_Monitor.h"

bool CurrentMonitor::initialize(int a0, int a1, int a2, int a3){
    if(a0 == 1){
        address = address | 1;
    }
    if(a1 == 1){
        address = address | 2;
    }
    if(a2 == 1){
        address = address | 4;
    }
    if(a3 == 1){
        address = address | 8;
    }
    Wire.begin();
    byte command[7] = {header1, header2, deviceInfo, 0,0,0,0};
    byte apiCommand[8];
    calculateCheckSum(command, apiCommand, 7);
    deviceStatusReady = i2CWrite(apiCommand, 8);
    if(!deviceStatusReady){
        Serial.println("initialize command failed");
        return false;
    }
    byte deviceInfo[7];
    deviceStatusReady = i2CRead(7, deviceInfo);
    if(!deviceStatusReady){
        Serial.println("device info command read failed");
        return false;
    }
    sensorType = deviceInfo[0];
    maxCurrent = deviceInfo[1];
    numberOfChannels = deviceInfo[2];
    firmwareVersion = deviceInfo[3];
    
    
    return false;
}

int CurrentMonitor::readChannelCurrentMa(int channel){
    byte command[7] = {header1, header2, readCurrent, channel, channel, 0, 0};
    byte apiCommand[8];
    calculateCheckSum(command, apiCommand, 7);
    deviceStatusReady = i2CWrite(apiCommand, 8);
    if(!deviceStatusReady){
        return failedCommand;
    }
    
    byte currentReading[4];
    deviceStatusReady = i2CRead(4, currentReading);
    if(!deviceStatusReady){
        return failedCommand;
    }
    int current = convertToInt(currentReading[0], currentReading[1], currentReading[2]);
    Serial.printf("Current mA: %i", current);
    return current;
}

double CurrentMonitor::readChannelCurrent(int channel){
    double c = readChannelCurrentMa(channel);
    return c/1000;
}

bool CurrentMonitor::readAllChannelsMa(int *buffer){
    
    if(numberOfChannels > 10){
        //Read first 10 channels
        byte command[7] = {header1, header2, readCurrent, 1, 10, 0, 0};
        byte apiCommand[8];
        calculateCheckSum(command, apiCommand, 7);
        deviceStatusReady = i2CWrite(apiCommand, 8);
        if(!deviceStatusReady){
            return false;
        }
        byte currentReadings[31];
        deviceStatusReady = i2CRead(31, currentReadings);
        if(!deviceStatusReady){
            return false;
        }
        for(int i = 0; i < 10; i++){
            buffer[i] = convertToInt(currentReadings[(i*3)], currentReadings[(i*3)+1], currentReadings[(i*3)+2]);
        }
        //Read additional channels
        byte command2[7] = {header1, header2, readCurrent, 11, numberOfChannels, 0, 0};
        byte apiCommand2[8];
        calculateCheckSum(command2, apiCommand2, 7);
        deviceStatusReady = i2CWrite(apiCommand2, 8);
        if(!deviceStatusReady){
            return false;
        }
        int len = ((numberOfChannels - 10)*3)+1;
        byte currentReadings2[len];
        deviceStatusReady = i2CRead(len, currentReadings2);
        if(!deviceStatusReady){
            return false;
        }
        for(int i = 0; i < numberOfChannels-10; i++){
            buffer[i+10] = convertToInt(currentReadings2[(i*3)], currentReadings2[(i*3)+1], currentReadings2[(i*3)+2]);
        }
        //Logging
        for(int i = 0; i < numberOfChannels; i++){
            // Serial.printf("buffer[%i]: %i \n", i, buffer[i]);
        }
        return true;

    }else{
    
        byte command[7] = {header1, header2, readCurrent, 1, numberOfChannels, 0, 0};
        byte apiCommand[8];
        calculateCheckSum(command, apiCommand, 7);
        deviceStatusReady = i2CWrite(apiCommand, 8);
        if(!deviceStatusReady){
            return false;
        }
        int len = (numberOfChannels*3)+1;
        byte currentReadings[len];
        deviceStatusReady = i2CRead(len, currentReadings);
        if(!deviceStatusReady){
            return false;
        }
        // for(int i = 0; i < len; i++){
        //     Serial.printf("return byte[%i]: %i \n", i, currentReadings[i]);
        // }
    
    
        for(int i = 0; i < numberOfChannels; i++){
            buffer[i] = convertToInt(currentReadings[(i*3)], currentReadings[(i*3)+1], currentReadings[(i*3)+2]);
        }
        return true;
    }
}

bool CurrentMonitor::readAllChannels(double *buffer){
    int buf[numberOfChannels];
    if(!readAllChannelsMa(buf)){
        return false;
    }
    for(int i = 0; i < numberOfChannels; i++){
        buffer[i] = (double)buf[i]/1000;
    }
    return true;
}

bool CurrentMonitor::readCalibration(int startChannel, int stopChannel, int *buffer){
    int nChannels = stopChannel - startChannel + 1;
    int len = (nChannels*2)+1;
    byte command[7] = {header1, header2, readCal, startChannel, stopChannel, 0, 0};
    byte apiCommand[8];
    calculateCheckSum(command, apiCommand, 7);
    deviceStatusReady = i2CWrite(apiCommand, 8);
    if(!deviceStatusReady){
        return false;
    }
    byte calReadings[len];
    deviceStatusReady = i2CRead(len, calReadings);
    if(!deviceStatusReady){
        return false;
    }
    for(int i = 0; i < nChannels; i++){
        buffer[i] = (calReadings[(i*2)+1]*256)+calReadings[(i*2)+2];
    }
    return true;
}

void CurrentMonitor::calculateCheckSum(byte *command, byte *returnCommand, int commandLength){
    byte checkSum = 0;
    for(int i = 0; i < commandLength; i++){
        checkSum = checkSum+command[i];
        returnCommand[i] = command[i];
    }
    returnCommand[commandLength] = checkSum;
}

int CurrentMonitor::convertToInt(byte msb_1, byte msb, byte lsb){
    int r = (msb_1*65536)+(msb*256)+lsb;   
    return r;
}

bool CurrentMonitor::i2CWrite(byte *command, int commandLength){
    Wire.beginTransmission(address);
    // Serial.printf("Opening connection to address: %i \n", address);
    for(int i = 0; i < commandLength; i++){
        // Serial.printf("Writing: %i \n", command[i]);
        Wire.write(command[i]);
        // delay(wait);
    }
    // delay(40);
    byte status = Wire.endTransmission();
    if(status != 0){
        Serial.printf("endTransmission() returned %i \n", status);
        return false;
    }
    return true;
}

bool CurrentMonitor::i2CRead(int len, byte *buffer){
    unsigned long startTime = millis();
    Wire.requestFrom(address, len);
    
    while(Wire.available() < len && millis() < startTime+readTimeout);
    
    if(Wire.available() < len){
        Serial.println("Read timeout");
        Serial.printf("bytes available: %i \n", Wire.available());
        //read timed out, controller did not respond
        return false;
    }
    for(int i = 0; i < len; i++){
        buffer[i] = Wire.read();
        // Serial.printf("read byte[%i]: %i \n", i, buffer[i]);
    }
    return true;
}
