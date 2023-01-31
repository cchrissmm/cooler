# cooler

## Architecture
* high and low level sensor, capacitence
* wet pump
* fill pump
* dump pump
* fans

## Functional Flow
### System Startup
* check level and dump if required

### Cooling requested by user
* dump if required
* check level and fill if required
  then
* start wet pump and wait xmin
  then
* start fan

### Off requested by user
* stop wet pump
* dump water
wait x min
* stop fan

### Fan requested by user
* start fan

### Long term dump
if wet running for >x then trigger a dump

## API

### Methods
runCooler() to start cooling

stopCOoler() to stop cooling and fan (whatever is on)

setFanSpeed(int fanSpeed) to set fan speed (0-255) / turn the fan on. default is 255



## Design
The system is built as state machine which dictates which state the system is in. Such as filling, dumping, wetting, etc.

Based on the state the various system functions are started and stopped




