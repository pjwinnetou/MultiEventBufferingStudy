#include <iostream>
#include <queue>
#include <TRandom3.h>
#include <TFile.h>
#include <TStyle.h>
#include <TH1D.h>

struct TriggerEvent {
    uint64_t startTick;
    uint64_t processTick;
};

class TriggerSimMEB {
public:
    TriggerSimMEB(int totalevents, double clkRateMHz, double trgRatekHz, int deadForNval, int endatval, int MEBval, bool useRDTrig);
    void loop();
private:
    long int TotalEvents;
    double clockRateMHz;
    double triggerRatekHz;
    int deadForN;
    int endat;
    int MEB;
    uint64_t globalClockCounter;
    uint32_t triggerClockCounter;
    TRandom3 randGen;
    std::queue<TriggerEvent> buffer;
    bool useRDTrig;
    TH1D *hGL1vGTMClockDiff, *hvGTMClockDiff, *hGL1vGTMLocked;
};

TriggerSimMEB::TriggerSimMEB(int totalevents, double clkRateMHz, double trgRatekHz, int deadForNval, int endatval, int MEBval, bool useRDTrig = true)
    : TotalEvents(totalevents), clockRateMHz(clkRateMHz), triggerRatekHz(trgRatekHz),
      deadForN(deadForNval), endat(endatval), MEB(MEBval), useRDTrig(useRDTrig), randGen(0) {
    globalClockCounter = randGen.Integer(0xFFFF);
    triggerClockCounter = randGen.Integer(0xFFFF);
};
