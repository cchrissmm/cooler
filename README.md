# cooler

## Architecture
* high and low level sensor, capacitence
* wet pump
* fill pump
* dump pump
* fans

## Functional Flow
### Startup
* check level and dump if required

### Cooling requested by user
* check level and fill if required
  then
* start wet pump and wait xmin
  then
* start fan

### Off requested by user
* stop wet pump
wait x min
* stop fan
* dump water

### Fan requested by user
* start fan

### Long term dump
if wet running for >x then trigger a dump

## API

call coolOn() to start cooling
call coolOff() to stop cooling
call fanOn() to start fan
call fanOff() to stop fan

## Design
The system is built as two state machines.

A mode state machine which dictates the operating mode of the system.
Cooling, fan, idle.

And a State state machine which dictates which state the system is in. Such as filling, dumping, wetting, etc.




