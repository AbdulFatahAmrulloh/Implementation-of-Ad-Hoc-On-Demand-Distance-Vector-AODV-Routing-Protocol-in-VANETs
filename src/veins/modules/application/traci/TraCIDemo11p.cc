#include "veins/modules/application/traci/TraCIDemo11p.h"
#include "veins/modules/application/traci/TraCIDemo11pMessage_m.h"

using namespace veins;

Define_Module(veins::TraCIDemo11p);

int color = 0; // 0=red; 1=green; 2=blue

void TraCIDemo11p::initialize(int stage)
{
    DemoBaseApplLayer::initialize(stage);
    if (stage == 0) {
        sentMessage = false;
        lastDroveAt = simTime();
        currentSubscribedServiceId = -1;

        // Inisialisasi statistik EED
        eedStats.setName("End-to-End Delay");
    } else {
        if (myId % 5 == 0) {
            TraCIDemo11pMessage* wsm = new TraCIDemo11pMessage();
            populateWSM(wsm, 10, 1);
            wsm->setSenderAddress(myId);
            wsm->setDemoData("ping");
            scheduleAt(simTime() + 10, wsm);
        }
    }
}

void TraCIDemo11p::onWSM(BaseFrame1609_4* frame)
{
    TraCIDemo11pMessage* wsm = check_and_cast<TraCIDemo11pMessage*>(frame);

    if (strcmp(wsm->getDemoData(), "pong") == 0) {
        // Hitung End-to-End Delay
        simtime_t eed = simTime() - wsm->getCreationTime();
        eedStats.collect(eed); // Kumpulkan data EED

        // Rotasi warna
        if (color == 0) {
            findHost()->getDisplayString().updateWith("r=30,red");
            color = 1;
        } else if (color == 1) {
            findHost()->getDisplayString().updateWith("r=30,green");
            color = 2;
        } else if (color == 2) {
            findHost()->getDisplayString().updateWith("r=30,blue");
            color = 0;
        }

        msgReceived = 1;
        std::cout << "CAR " << myId << " | *pong* received from RSU!"
                  << " | SimTime: " << simTime() << " | EED: " << eed << endl;
    }
}

void TraCIDemo11p::handleSelfMsg(cMessage* msg)
{
    if (TraCIDemo11pMessage* wsm = dynamic_cast<TraCIDemo11pMessage*>(msg)) {
        if (msgReceived == 0) {
            sendDown(wsm->dup());
            scheduleAt(simTime() + 2, wsm);
            std::cout << "CAR " << myId << " | *ping* sent to RSU..."
                      << " | SimTime: " << simTime() << endl;
        } else {
            delete wsm;
        }
    } else {
        DemoBaseApplLayer::handleSelfMsg(msg);
    }
}

void TraCIDemo11p::handlePositionUpdate(cObject* obj)
{
    DemoBaseApplLayer::handlePositionUpdate(obj);

    if (mobility->getSpeed() < 1) {
        if (simTime() - lastDroveAt >= 10 && sentMessage == false) {
            findHost()->getDisplayString().setTagArg("i", 1, "red");
        }
    } else {
        lastDroveAt = simTime();
    }
}

void TraCIDemo11p::finish()
{
    // Catat durasi simulasi
    recordScalar("Simulation duration", simTime());

    // Catat statistik EED
    eedStats.recordAs("End-to-End Delay");
}
