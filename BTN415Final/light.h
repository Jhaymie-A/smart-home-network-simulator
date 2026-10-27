// light_service.h
#ifndef LIGHT_SERVICE_H
#define LIGHT_SERVICE_H

#include "device_base.h"

class LightService : public DeviceService {
public:
    void start() override;
private:
    bool isOn = false;
};

#endif // LIGHT_SERVICE_H