#include <iostream>
#include "TROOT.h"
#include <vector>
#include "TriggerSimMEB.h"

void TriggerSimMEB::loop() {
  TFile file(Form("output_nEvents%ld_isRandomTrig%d_TrigRate%.1f_MEB%d.root",TotalEvents,useRDTrig,triggerRatekHz,MEB), "RECREATE");
  hGL1vGTMClockDiff = new TH1D("hGL1vGTMClockDiff", "GL1 vs vGTM Difference", 10000, 0, 0xFFFFFFFF);
  hGL1vGTMLocked = new TH1D("hGL1vGTMLocked", "Locked with GL1", 2, -0.5,1.5);
  hvGTMClockDiff = new TH1D("hvGTMClockDiff", "Clock Difference of triggers in vGTM",3*endat, 0, endat*5); 

  int triggerCount = 0;
  uint64_t tick = 0;
  uint64_t lastTriggerTick = 0;
  uint32_t clockDiffTrigger= 0;
  uint32_t previousClock= 0;
  uint32_t previousDiff = 0;
  uint32_t clockDiff = 0;

  int triggerInterval = static_cast<int>(clockRateMHz * 1000 / triggerRatekHz);
  std::cout << "triggerInterval : " << triggerInterval << std::endl;
  std::cout << "useRDTrig : " << useRDTrig << std::endl;

  while (triggerCount < TotalEvents) {
    if(globalClockCounter%1000000000==0) std::cout << "Events " << globalClockCounter << " has been processed / " << triggerCount << " events recorded (" << (float) triggerCount/TotalEvents*100. << "%) has been processed out of " << TotalEvents << std::endl;
    globalClockCounter++;
    triggerClockCounter++;
    if (tick - lastTriggerTick <= deadForN) {
      tick++;
      continue;
    }

    bool IsTrigger = (useRDTrig) ? randGen.Uniform() < triggerRatekHz / (clockRateMHz * 1000) : tick % triggerInterval == 0;

    if(IsTrigger){
      lastTriggerTick = tick;
      bool bufferHasSpace = buffer.size() < MEB;
      if (bufferHasSpace) {
        TriggerEvent newEvent{tick, tick}; 
        buffer.push(newEvent);

        if(triggerCount==0){
          previousDiff = (uint32_t)((globalClockCounter) & 0xFFFFFFFF) - (triggerClockCounter);
          previousClock = triggerClockCounter;
        }
        clockDiff = (uint32_t)(globalClockCounter & 0xFFFFFFFF) - triggerClockCounter;
        clockDiffTrigger = triggerClockCounter - previousClock;
        uint32_t clockDiffDiff = (triggerCount==0) ? 0 : clockDiff - previousDiff;
        if(clockDiffDiff!=0 ) std::cout << "Clock diff not aligned! clockDiff : " << clockDiff << " - previousDiff : " << previousDiff << std::endl;
        hGL1vGTMClockDiff ->Fill(clockDiff);
        
        if(triggerCount>0) hvGTMClockDiff ->Fill(clockDiffTrigger);

        bool islocked = (clockDiffDiff ==0 ) ? 1 : 0;
        if(triggerCount>0) hGL1vGTMLocked->Fill(islocked);
        previousClock = triggerClockCounter;
        previousDiff = clockDiff;
      }
    }

    if (!buffer.empty() && tick >= buffer.front().processTick + endat) {
      //std::cout << "tick : " << tick << std::endl;
      buffer.pop();
      triggerCount++;
    }

    tick++;
  }

  hGL1vGTMLocked->Write();
  hvGTMClockDiff->Write();
  hGL1vGTMClockDiff->Write();
  file.Close();
}

int main(int argc, char *argv[]) {
  int nMEB = 1;
  long int nEvents = 1e+6;
  bool isRandom=false;
  if(argc<2) {
    std::cout << "default option : nMEB = 1, nEvents = 1M, no Random trigger" << std::endl;
  }
  else{
    nEvents = atoi(argv[1]);
    nMEB = atoi(argv[2]);
    isRandom = atoi(argv[3]);
    std::cout << "use setting for MEB = " << nMEB << std::endl;
    std::cout << "required nEvents = " << nEvents << std::endl;
    std::cout << "Use random trigger : " << isRandom << std::endl;
  }

  ROOT::EnableImplicitMT();
  TriggerSimMEB t(nEvents, 9.388, 15, 4, 500, nMEB,isRandom);
  t.loop();
  return 0;
}

