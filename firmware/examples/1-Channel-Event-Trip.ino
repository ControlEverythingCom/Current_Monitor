// This #include statement was automatically added by the Particle IDE.
#include "Current_Monitor/Current_Monitor.h"

CurrentMonitor current;

//cloud variables
double currentReading;
double kwh;
int firmware;
int maxCurrent;
int numberOfChannels;
int sensorType;
double ACVoltageD;

//local variables
float ACVoltage = 120.00;
unsigned long upTime = 0;
unsigned long lastReadTime;
int acVoltageStorageIndex = 0;
int kwhStorageStartIndex = 4;
int tripLimitIndex = 12;
double tripLimit = 100.00;
double lastReading = 0;


//Cloud Functions
int setACVoltagte(String voltage);
int clearKWH(String channel);
int setTripLimit(String limit);

void setup() {
    if(!current.initialize(0,0,0,0)){
        Serial.println("Initialize failed");
    }
    firmware = current.firmwareVersion;
    maxCurrent = current.maxCurrent;
    numberOfChannels = current.numberOfChannels;
    sensorType = current.sensorType;
    ACVoltageD = (double)ACVoltage;
    Serial.println(ACVoltageD);
    getInfoFromStorage();
    Particle.variable("Firmware", firmware);
    Particle.variable("Max_Current", maxCurrent);
    Particle.variable("Sensor_type", sensorType);
    Particle.variable("Channels", numberOfChannels);
    Particle.variable("Current", currentReading);
    Particle.variable("KWH_Readings", kwh);
    Particle.variable("ACVoltage", ACVoltageD);
    Particle.function("SetACVoltage", setACVoltagte);
    Particle.function("ClearKWHs", clearKWH);
    Particle.function("Trip", setTripLimit);
}

void loop() {
    if(current.deviceStatusReady){
        while(current.deviceStatusReady){
            //Read current on channel 1
            double c = current.readChannelCurrent(1);
            if(c != current.failedCommand){
                //Publish most recent current reading
                currentReading = c;
                if(currentReading > tripLimit && lastReading < currentReading){
                    Particle.publish("Tripped", "light_on", 60, PRIVATE);
                }
                lastReading = currentReading;
                // Calculate Kilowatt hours
                // Calculate Wattage
                double wattage = currentReading*ACVoltage;
                //Calculate hours
                upTime = millis();
                double hours = (upTime - lastReadTime)/ (60.00 * 60.00 * 1000.00);
                lastReadTime = millis();
                //Calculate Kilowatt hours
                kwh = kwh + ((wattage * hours)/1000);
                //Store kwh in eeprom so we do not loose it on power loss.
                EEPROM.put(kwhStorageStartIndex, kwh);
            }else{
                Serial.println("Reading of current failed");
            }
            //We read current on the circuit once per second.
            delay(1000);
        }
    }else{
        Serial.println("Device not ready");
        delay(1000);
    }
}

int setACVoltagte(String voltage){
    ACVoltage = voltage.toFloat();
    ACVoltageD = (double)ACVoltage;
    EEPROM.put(acVoltageStorageIndex, ACVoltage);
    return 1;
}

int clearKWH(String channelNumber){
    kwh = 0.00;
    EEPROM.put(kwhStorageStartIndex, kwh);
    return 1;
    
}

int setTripLimit(String limit){
    float trip = limit.toFloat();
    tripLimit = (double)trip;
    EEPROM.put(tripLimitIndex, tripLimit);
    
}

void getInfoFromStorage(){
    float tACVoltage;
    EEPROM.get(acVoltageStorageIndex, tACVoltage);
    String emptyCheck = String(tACVoltage);
    if(emptyCheck.equalsIgnoreCase("0.000000")){
        Serial.println("No ACVoltage reading stored");
    }else{
        ACVoltage = tACVoltage;
        ACVoltageD = (double)ACVoltage;
    }

    double tKWHReading;
    EEPROM.get(kwhStorageStartIndex, tKWHReading);
    String eCheck = String(tKWHReading);
    if(eCheck.equalsIgnoreCase("0.000000")){
        Serial.println("No Stored kWH readings for channel 1 \n");
    }else{
        Serial.printf("%.4f stored for Channel %i \n", tKWHReading, 1);
        kwh = tKWHReading;
    }
    
    double tTripLimit;
    EEPROM.get(tripLimitIndex, tTripLimit);
    String eCheck1 = String(tTripLimit);
    if(eCheck1.equalsIgnoreCase("0.000000")){
        Serial.println("No Stored trip limit for channel 1 \n");
    }else{
        Serial.printf("%.4f stored for Channel %i trip limit \n", tTripLimit, 1);
        tripLimit = tTripLimit;
    }
}