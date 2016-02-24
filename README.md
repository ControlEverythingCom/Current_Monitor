# About

This Library is intended for use with ControlEverything Current Monitoring products on the Particle Core/Photon/Electron platform.

The intention of this library is to make use of the ControlEverything Current Monitor Products with Particle development web IDE as simple as possible for users.

Current Monitoring products from ControlEverything may be purchased [here] [currentmonitorcategory].

###Developer information
NCD has been designing and manufacturing computer control products since 1995.  We specialize in hardware design and manufacturing of sensory input and output control devices.  We pride ourselves as being the industry leader of computer control relay products as well as sensory reading products.  Our products are proven reliable and we are very excited to support Particle.  For more information on NCD please visit www.controleverything.com

###Requirements
- [Particle Compatible Current Monitor][sht30store].
- Particle Core, Photon, or Electron module
- Knowledge base for developing and programming with Particle Core/PhotonElectron modules.

### Version
1.0.0

### How to use this library

The libary must be imported into your application.  This can be done through the Particle WEB IDE by searching community Libraries on Particle's WEB IDE(Build) for Current_Monitor.  Click Include in App button.  Select the App you want to include the library in.  Finally click Add to this app.  For more information see [Particle's documentation] [sparkIncludeLibrary].
You can also use the example applications from the library by making sure the example ino file you want to use is selected, then click on the Use This Example button.

### Example use

Once the Library is included in your applicaiton you should see an include statement at the top like this:
```cpp
//This #include statement was automatically added by the Particle IDE.
#include "Current_Monitor/Current_Monitor.h"
```

Now you need to instanciate an object of the library for use in your application like this:
```cpp
CurrentMonitor current;
```

Here is an example use case for the class:
```cpp
// This #include statement was automatically added by the Particle IDE.
#include "Current_Monitor/Current_Monitor.h"

CurrentMonitor current;
double current;

void setup() {
if(!current.initialize(0,0,0,0)){
Serial.println("Initialize failed");
}
Particle.variable("Current", current);
}

void loop() {
if(current.deviceStatusReady){
double c = current.readChannelCurrent(1);
if(c != current.failedCommand){
current = c;
}
}
delay(1000);
}
```

###Public accessible methods
```cpp
bool CurrentMonitor::initialize(int a0, int a1, int a2, int a3);
```
>Must be called first before using the object.  This method should also be called any time communication with
>the controller is lost or broken to recover communication  This method accepts four int argument.  This
>tells the Library what address to direct commands to.  a0, a1, a2, and a3 re representations of the the Address
>jumpers on the Current Monitor board.  If the jumper is
>installed then send a 1, if the jumper is not installed send a 0 like this ```current.initialize(0,0,0,0);```
This function will return true if the device responds or false if it does not.


```cpp
int readChannelCurrentMa(int channel);
```
>This method accepts 1 int argument specifying the channel to be read.  This method will return the current reading on that channel in milliamps.
>

```cpp
double readChannelCurrent(int channel);
```
>This method accepts 1 int argument specifying the channel to be read.  This method will return the current reading on that channel in amps as a double variable.
>

```cpp
bool readAllChannelsMa(int *buffer);
```
>This method accepts 1 int pointer argument which should be an int array the size of the number of channels on the board.  This method will read the current of all channels on the board in milliamps and will populate those readings in the int array.  The function will return true if the process completes successfully or false if something goes wrong.
>

```cpp
bool readAllChannels(double *buffer);
```
>This method accepts 1 double pointer argument which should be a double array the size of the number of channels on the board.  This method will read the current of all channels on the board in amps and will populate those readings in the double array.  The function will return true if the process completes successfully or false if something goes wrong.
>

```cpp
bool readCalibration(int startChannel, int stopChannel, int *buffer);
```
>This method accepts 2 int arguments and 1 int pointer argument.  This method will read the calibration settings out of the board for multiple channls.  The startChannel argument specifies the first channel calibration to be read.  The stopChannel argument specifies the last channel calibration to be read.  The int pointer should be an int array the same size as the number of channels to be read.  The method will populate the int array with the calibration readings for the specified channels.  If the user wishes to read channels 1, 2, 3, and 4 then they should pass 1 as the startChannel, 4 as the stopChannel, and they should pass a pointer to an int array with a size of 4.  The function will return true if the process completes successfully or false if something goes wrong.
>
>A function will be added in the future to alter calibration values.

###Public accessible Variables

```cpp
bool deviceStatusReady
```
>This boolean variable will indicate the device's status as either ready or not ready.  This variable should be checked before sending any command to the library.

```cpp
byte sensorType
```
>This variable indicates the type of sensor used on the controller.

```cpp
byte maxCurrent
```
>This variable indicates the maximum current which can be read on each channel of the controller.

```cpp
byte numberOfChannels
```
>This variable indicates the number of channels on the controller.

```cpp
byte firmwareVersion
```
>This variable indicates the version of firmware installed on the controller's CPU.  Note that this has nothing to do with the firmware running on the Particle Module.

```cpp
int failedCommand = 2147483646;
```
>This variable variable is used as the return if a function which is to return an int variable fails.

License
----

GNU
[sparkIncludeLibrary]:https://docs.particle.io/guide/getting-started/build/photon/
[currentmonitorcategory]:https://www.controleverything.com/products?f[0]=field_assigned_categories%3A6172&f[1]=field_assigned_categories%3A6223