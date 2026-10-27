#ifndef THERMOSTAT_SERVICE_H
#define THERMOSTAT_SERVICE_H

#include "device_base.h"

class ThermostatService : public DeviceService {
public:
    void start() override;
private:
    int temperature = 24; // Default temp
};

#endif // THERMOSTAT_SERVICE_H
