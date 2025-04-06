#pragma once

#include "veins/modules/application/ieee80211p/DemoBaseApplLayer.h"
#include <omnetpp/cstddev.h> // Tambahkan ini

namespace veins {

class VEINS_API TraCIDemo11p : public DemoBaseApplLayer {
public:
    void initialize(int stage) override;

protected:
    simtime_t lastDroveAt;
    bool sentMessage;
    int currentSubscribedServiceId;

protected:
    void onWSM(BaseFrame1609_4* wsm) override;

    void handleSelfMsg(cMessage* msg) override;
    void handlePositionUpdate(cObject* obj) override;

protected:
    int msgReceived = 0; // 0 = message not received yet; 1 = message received
    cStdDev eedStats;    // Statistik End-to-End Delay

    virtual void finish() override; // Fungsi untuk mencatat hasil statistik
};

} // namespace veins
