
#include "NeuralPreamp.h"
#include "../FMR_Library/FMR_Lib/FMR_Neural.h"
#include "../FMR_Library/FMR_NeuralAmp/FMR_NeuralAmp.h"
#include "../FMR_Library/FMR_Lib/FMR_Resampler.h"
#include "../FMR_Library/FMR_Lib/FMR_ConvPot.h"
#include "../FMR_Library/Graphics/AluPlateControl.h"
#include "IPlug_include_in_plug_src.h"
#include "IControls.h"
#include <algorithm>

class ILevelColorControl : public IControl
{
public:
	ILevelColorControl(const IRECT& bounds)
		: IControl(bounds)
	{
		mIgnoreMouse = true;
		mLevel = 0.f;
	}

	void SetLevel(float level)
	{
		mLevel = std::clamp(level, 0.f, 1.f);
		SetDirty();
	}

	void Draw(IGraphics& g) override
	{
		// Palette de 6 couleurs
		static const IColor colors[] =
		{
			IColor(255,   0,  10,  60),  // Bleu très foncé
			IColor(255,   0,  40, 120),  // Bleu foncé
			IColor(255,   0,  90, 190),  // Bleu
			IColor(255,  30, 150, 240),  // Bleu clair
			IColor(255, 100, 200, 255),  // Bleu très clair
			IColor(255, 200, 235, 255)   // Presque blanc
		};

		constexpr int count = 6;

		const float position = mLevel * (count - 1);
		const int index = std::min(static_cast<int>(position), count - 2);
		const float fraction = position - index;

		const IColor& c1 = colors[index];
		const IColor& c2 = colors[index + 1];

		auto lerp = [fraction](int a, int b)
			{
				return static_cast<int>(a + (b - a) * fraction);
			};

		const IColor color(
			255,
			lerp(c1.R, c2.R),
			lerp(c1.G, c2.G),
			lerp(c1.B, c2.B)
		);

		// Rectangle de couleur homogène
		g.FillRoundRect(color, mRECT, 2.f);
	}

private:
	float mLevel = 0.f;
};

void NeuralPreamp::OnIdle()
{
	if (!GetUI())
		return;

	// ------------------------------------------------
	// LED : fonctionnement original
	// ------------------------------------------------
	auto updateLED = [](IControl* control,
		std::atomic<float>& peak,
		float fullScale)
		{
			auto* led = dynamic_cast<ILEDControl*>(control);

			if (!led)
				return;

			const float level = peak.exchange(
				0.0f, std::memory_order_relaxed);

			const float normalized = std::clamp(
				level / fullScale, 0.0f, 1.0f);

			if (normalized < 0.005f)
				return;

			// Vert, orange, rouge
			if (normalized < 0.355f)	// -9dB
				led->SetHue(0.33f);
			else if (normalized < 0.708f)	// -3dB
				led->SetHue(0.08f);
			else
				led->SetHue(0.00f);

			led->SetValue(1.0f);
			led->TriggerWithDecay(400);
		};

	// ------------------------------------------------
	// Rectangle bleu : fonctionnement indépendant
	// ------------------------------------------------
	auto updateColor = [](IControl* control,
		std::atomic<float>& peak,
		float fullScale)
		{
			auto* color = dynamic_cast<ILevelColorControl*>(control);

			if (!color)
				return;

			const float level = peak.exchange(0.0f, std::memory_order_relaxed);
			const float normalized = std::clamp(level / fullScale, 0.0f, 1.0f);

			color->SetLevel(normalized);
		};

	// ------------------------------------------------
	// INPUT
	// ------------------------------------------------
	updateLED(GetUI()->GetControlWithTag(kCtrlInputLED), mInputPeak,4.5f);

	// ------------------------------------------------
	// OUTPUT
	// ------------------------------------------------
	updateColor(GetUI()->GetControlWithTag(kCtrlNeurone), mOutputPeakColor,1.5f);
}

NeuralPreamp::NeuralPreamp(const InstanceInfo& info)
: iplug::Plugin(info, MakeConfig(kNumParams, kNumPresets))
{
#if IPLUG_DSP
  mAmp = std::make_unique<NeuralAmp>();
  mRate = std::make_unique<FMR_RateAdapter<sample>>();
#endif

  GetParam(kPreGain)->InitInt("PreGain", 2048, 0, 4095);

  GetParam(kVolume)->InitDouble("Volume", 0.55, 0.1, 0.999, 0.001);

  GetParam(kBass)->InitDouble("Bass", 0.5, 0.001, 0.999, 0.001);
  GetParam(kMiddle)->InitDouble("Middle", 0.5, 0.001, 0.999, 0.001);
  GetParam(kTreble)->InitDouble("Treble", 0.5, 0.001, 0.999, 0.001);

  GetParam(kMaster)->InitInt("Master", 2048, 0, 4095);

  // Arguments : ID du paramètre, index par défaut, liste des choix entre accolades
  GetParam(kParamAmpSelect)->InitEnum("Mode Ampli", 11, {
	  "Bogner Triple Giant Clean",
	  "Bogner Triple Giant Brown",
	  "Bogner Triple Giant Sharp",
	  "Diezel VH4 ch1",
	  "Diezel VH4 ch2",
	  "Diezel VH4 ch3",
	  "Diezel VH4 ch4",
	  "Dumble ODS Clean",
	  "Dumble ODS Overdrive",
	  "Fender Bassman 5F6",
	  "Fender Blackface AB763",
	  "Fender Blackface AB763 Bright",
	  "Framus Cobra V1&2 Clean",
	  "Framus Cobra V1&2 Crunch",
	  "Framus Cobra V1&2 Lead",
	  "HiWatt DR103 Normal",
	  "HiWatt DR103 Bright",
	  "Marshall JCM600",
	  "Marshall JCM800",
	  "Marshall JMP1959 Input 2",
	  "Marshall JMP1959 Input 1",
	  "Marshall JTM45 Input 2",
	  "Marshall JTM45 Input 1",
	  "Mesa Dual Rectifier Or. Clean",
	  "Mesa Dual Rectifier Or. Normal",
	  "Orange Rockerverb Clean",
	  "Orange Rockerverb Dirty",
	  "Peavey EVH5150 Clean",
	  "Peavey EVH5150 Crunch",
	  "Peavey EVH5150 Lead",
	  "Revv Channel 2 Green",
	  "Revv Channel 2 Crunch",
	  "Revv Channel 3 Purple",
	  "Revv Channel 4 Red",
	  "Soldano SLO100 OD Clean",
	  "Soldano SLO100 OD Crunch",
	  "Soldano SLO100 OD Lead",
	  "Vox AC30C2 Normal",
	  "Vox AC30C2 Top Boost",
	  });

#if IPLUG_EDITOR
  mMakeGraphicsFunc = [&]() {
    return MakeGraphics(*this, PLUG_WIDTH, PLUG_HEIGHT, PLUG_FPS,
                        GetScaleForScreen(PLUG_WIDTH, PLUG_HEIGHT));
  };

#define SIZE_TITLE 55
#define SIZE_LABEL 16
#define SIZE_MENU 30
#define SIZE_FMR 50

  mLayoutFunc = [&](IGraphics* pGraphics) {
	  pGraphics->AttachCornerResizer(EUIResizerMode::Scale, false);
	  pGraphics->AttachPanelBackground(COLOR_GRAY);
	  pGraphics->LoadFont("Lintsec", LINTSEC_FN);
	  pGraphics->LoadFont("BlackOpsOne", BLACKOPS_FN);

	  const IBitmap bleuKnobBitmap = pGraphics->LoadBitmap(KNOB_FN, 101);
	  const IBitmap fondBitmap = pGraphics->LoadBitmap(BACKGROUND_FN, 1);

	  const IRECT bounds = pGraphics->GetBounds();
	  const IRECT innerBounds = bounds.GetPadded(-10.f);

	  // fond bleu qui clignote
	  pGraphics->AttachControl(new ILevelColorControl(IRECT(234, 48, 566, 120)), kCtrlNeurone);
	  // background
	  pGraphics->AttachControl(new IBitmapControl(pGraphics->GetBounds(), fondBitmap));

	  // liste déroulante
	  IRECT zoneMenu = IRECT(175, 155, 625, 195);
	  pGraphics->AttachControl(new IVMenuButtonControl(zoneMenu, kParamAmpSelect, "PREAMP"));
	  // affichage
	  pGraphics->AttachControl(new ICaptionControl(zoneMenu, kParamAmpSelect, IText(SIZE_MENU, COLOR_BLACK, "BlackOpsOne")));

#define POT_YH 204
#define POT_YB (POT_YH+100)
#define decalage_box (-100)
#define TXT_YH (POT_YH-decalage_box)
#define TXT_YB (TXT_YH+26)
#define PLQ_YH (POT_YH-(decalage_box+5))
#define PLQ_YB (PLQ_YH+29)

	  pGraphics->AttachControl(new iplug::igraphics::ILEDControl(IRECT(30, POT_YH - 20, 170, POT_YB + 20), COLOR_GREEN), kCtrlInputLED);
	  const IRECT preGainKnob(50, POT_YH, 150, POT_YB);
	  pGraphics->AttachControl(new IBKnobControl(preGainKnob, bleuKnobBitmap, kPreGain));
	  pGraphics->AttachControl(new ITextControl(IRECT(50, TXT_YH, 150, TXT_YB), "PreGain", IText(SIZE_LABEL, COLOR_BLACK, "Lintsec")));

	  const IRECT volumeKnob(170, POT_YH, 270, POT_YB);
	  pGraphics->AttachControl(new IBKnobControl(volumeKnob, bleuKnobBitmap, kVolume));
	  pGraphics->AttachControl(new ITextControl(IRECT(170, TXT_YH, 270, TXT_YB), "Volume", IText(SIZE_LABEL, COLOR_BLACK, "Lintsec")));

	  const IRECT bassKnob(290, POT_YH, 390, POT_YB);
	  pGraphics->AttachControl(new IBKnobControl(bassKnob, bleuKnobBitmap, kBass));
	  pGraphics->AttachControl(new ITextControl(IRECT(290, TXT_YH, 390, TXT_YB), "Bass", IText(SIZE_LABEL, COLOR_BLACK, "Lintsec")));

	  const IRECT middleKnob(410, POT_YH, 510, POT_YB);;
	  pGraphics->AttachControl(new IBKnobControl(middleKnob, bleuKnobBitmap, kMiddle));
	  pGraphics->AttachControl(new ITextControl(IRECT(410, TXT_YH, 510, TXT_YB), "Middle", IText(SIZE_LABEL, COLOR_BLACK, "Lintsec")));

	  const IRECT trebleKnob(530, POT_YH, 630, POT_YB);;
	  pGraphics->AttachControl(new IBKnobControl(trebleKnob, bleuKnobBitmap, kTreble));
	  pGraphics->AttachControl(new ITextControl(IRECT(530, TXT_YH, 630, TXT_YB), "Treble", IText(SIZE_LABEL, COLOR_BLACK, "Lintsec")));

	  // pGraphics->AttachControl(new iplug::igraphics::ILEDControl(IRECT(630, POT_YH - 20, 770, POT_YB + 20), COLOR_GREEN), kCtrlOutputLED);
	  const IRECT masterKnob(650, POT_YH, 750, POT_YB);
	  pGraphics->AttachControl(new IBKnobControl(masterKnob, bleuKnobBitmap, kMaster));
	  pGraphics->AttachControl(new ITextControl(IRECT(650, TXT_YH, 750, TXT_YB), "Master", IText(SIZE_LABEL, COLOR_BLACK, "Lintsec")));

	  pGraphics->AttachControl(new AluPlateControl(IRECT(671, 107, 781, 152), 3.f, 3.f, 4));
	  pGraphics->AttachControl(new ITextControl(IRECT(676, 102, 776, 162), "FMR", IText(SIZE_FMR, COLOR_BLACK, "BlackOpsOne")));
	  };
#endif
}

NeuralPreamp::~NeuralPreamp() = default;

#if IPLUG_DSP
// Fréquence du host != 48 kHz : le moteur tourne à 48 kHz (resampling WDL) ; latence déclarée au host
void NeuralPreamp::OnReset()
{
  mRate->Init(GetSampleRate(), std::max(GetBlockSize(), 64));
  SetLatency(mRate->Latency());

#define potCutFrequency 5.0f
  InitFilter(&filterPreGain, potCutFrequency, LP_1st_order, 1.0f);
  InitFilter(&filterVolume, potCutFrequency, LP_1st_order, 1.0f);
  InitFilter(&filterMaster, potCutFrequency, LP_1st_order, 1.0f);
  InitFilter(&filterBass, potCutFrequency, LP_1st_order, 1.0f);
  InitFilter(&filterMiddle, potCutFrequency, LP_1st_order, 1.0f);
  InitFilter(&filterTreble, potCutFrequency, LP_1st_order, 1.0f);
}

void NeuralPreamp::ProcessBlock(sample** inputs, sample** outputs, int nFrames)
{
  const int nChans = std::min(NOutChansConnected(), NA_NB_CHANNELS);

  if (mRate->Active())
  {
    // host -> 48 kHz -> moteur -> host, par tranches <= MaxBlock()
    for (int done = 0; done < nFrames; )
    {
      const int n = std::min(nFrames - done, mRate->MaxBlock());
      sample* in[NA_NB_CHANNELS];
      sample* out[NA_NB_CHANNELS];
      for (int c = 0; c < nChans; c++)
      {
        in[c] = inputs[c] + done;
        out[c] = outputs[c] + done;
      }
      const int n48 = mRate->ToEngine(in, n, nChans);
      ProcessEngine(mRate->Planar(), mRate->Planar(), n48);
      mRate->FromEngine(n48, out, n, nChans);
      done += n;
    }
  }
  else
  {
    ProcessEngine(inputs, outputs, nFrames);
  }
}

void NeuralPreamp::ProcessEngine(sample** inputs, sample** outputs, int nFrames)
{
  const float volume = GetParam(kVolume)->Value();

  const float bass = GetParam(kBass)->Value();
  const float middle = GetParam(kMiddle)->Value();
  const float treble = GetParam(kTreble)->Value();

  // 0 to 1.0
  const float master = PotCurve_0_23[(unsigned short int)(GetParam(kMaster)->Value())];
  // 0.1 to 10 => -20dB to +20dB
  const float preGain = 0.1f + 9.9f * PotCurve_0_23[(unsigned int)(GetParam(kPreGain)->Value())];

  const int nChans = std::min(NOutChansConnected(), NA_NB_CHANNELS);

  float input, output;
  float inputPeak = 0.0f;
  float outputPeak = 0.0f;

  unsigned int chosenPreAmp = static_cast<int>(GetParam(kParamAmpSelect)->Value());
  mAmp->Request(static_cast<int>(chosenPreAmp));

  for (int s = 0; s < nFrames; s++)
  {
	  currentPreGain = Filter1st(&filterPreGain, preGain);
	  currentVolume = Filter1st(&filterVolume, volume);

	  currentBass = Filter1st(&filterBass, bass);
	  currentMiddle = Filter1st(&filterMiddle, middle);
	  currentTreble = Filter1st(&filterTreble, treble);

	  currentMaster = Filter1st(&filterMaster, master);

	  mAmp->Tick();
	  mAmp->SetEQ(currentBass, currentMiddle, currentTreble);

	  for (int c = 0; c < nChans; c++)
	  {
		  input = currentPreGain * inputs[c][s];
		  inputPeak = std::max(inputPeak, std::fabs(input));

		  output = currentMaster * mAmp->Process(c, (float)std::clamp(input, -4.5f, 4.5f), currentVolume);
		  outputPeak = std::max(outputPeak, std::fabs(output));
		  outputs[c][s] = output;
	  }
  }
  // Publication thread-safe des crêtes vers l'interface
  float current = mInputPeak.load(std::memory_order_relaxed);
  while (inputPeak > current &&
	  !mInputPeak.compare_exchange_weak(
		  current, inputPeak,
		  std::memory_order_relaxed))
  {
  }
/*
  current = mOutputPeak.load(std::memory_order_relaxed);
  while (outputPeak > current &&
	  !mOutputPeak.compare_exchange_weak(
		  current, outputPeak,
		  std::memory_order_relaxed))
  {
  }
*/ 
  // Peak rectangle (indépendant)
  current = mOutputPeakColor.load(std::memory_order_relaxed);
  while (outputPeak > current &&
	  !mOutputPeakColor.compare_exchange_weak(
		  current, outputPeak,
		  std::memory_order_relaxed))
  {
  }
}
#endif
