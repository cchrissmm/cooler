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

