#pragma once

#include "IPlug_include_in_plug_hdr.h"
#include <memory>
#include <atomic>
#include "../FMR_Library/FMR_Lib/FMR_DSP_float.h"

const int kNumPresets = 1;

enum EParams
{
  kVolume = 0,
  kPreGain,
  kMaster,
  kBass,
  kMiddle,
  kTreble,
  kParamAmpSelect,
  kNumParams
};

enum EControlTags
{
	kCtrlInputLED = 100,
	//kCtrlOutputLED,
	kCtrlNeurone
};

using namespace iplug;
using namespace igraphics;

class NeuralAmp;
template <typename T> class FMR_RateAdapter;

class NeuralPreamp final : public Plugin
{
public:
  NeuralPreamp(const InstanceInfo& info);
  ~NeuralPreamp();
  void OnIdle() override;

#if IPLUG_DSP
  void ProcessBlock(sample** inputs, sample** outputs, int nFrames) override;
  void OnReset() override;

private:
  void ProcessEngine(sample** inputs, sample** outputs, int nFrames);
  std::unique_ptr<NeuralAmp> mAmp;
  std::unique_ptr<FMR_RateAdapter<sample>> mRate;
  
  std::atomic<float> mInputPeak{ 0.0f };
  //std::atomic<float> mOutputPeak{ 0.0f };
  std::atomic<float> mOutputPeakColor{ 0.0f };

  FilterFloat_t filterPreGain, filterVolume, filterMaster;
  float currentPreGain = 0.0f, currentVolume = 0.0f, currentMaster = 0.0f;

  FilterFloat_t filterBass, filterMiddle, filterTreble;
  float currentBass = 0.0f, currentMiddle = 0.0f, currentTreble = 0.0f;

#endif
};
