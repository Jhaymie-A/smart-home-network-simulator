#ifndef SECURITY_SERVICE_H
#define SECURITY_SERVICE_H

#include "device_base.h"

class SecurityService : public DeviceService {
public:
    void start() override;
private:
    bool isOn = false;
};

#endif // SECURITY_SERVICE_H
