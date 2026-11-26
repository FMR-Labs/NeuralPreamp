#define PLUG_NAME "Neural Preamp"
#define PLUG_MFR "FMR-Labs"
#define PLUG_VERSION_HEX 0x00010000
#define PLUG_VERSION_STR "1.0.0"
#define PLUG_UNIQUE_ID 'NlPr'
#define PLUG_MFR_ID 'FMRL'
#define PLUG_URL_STR "https://github.com/FMR-Labs"
#define PLUG_EMAIL_STR "FMRLabsVST@gmail.com"
#define PLUG_COPYRIGHT_STR "Copyright 2026 FMR"
#define PLUG_CLASS_NAME NeuralPreamp

#define BUNDLE_NAME "Neural Preamp"
#define BUNDLE_MFR "FMR-Labs"
#define BUNDLE_DOMAIN "com"

#define SHARED_RESOURCES_SUBPATH "Neural Preamp"

#define PLUG_CHANNEL_IO "1-1 2-2"

#define PLUG_LATENCY 0
#define PLUG_TYPE 0
#define PLUG_DOES_MIDI_IN 0
#define PLUG_DOES_MIDI_OUT 0
#define PLUG_DOES_MPE 0
#define PLUG_DOES_STATE_CHUNKS 0
#define PLUG_HAS_UI 1
#define PLUG_WIDTH 800
#define PLUG_HEIGHT 372
#define PLUG_FPS 30
#define PLUG_SHARED_RESOURCES 0
#define PLUG_HOST_RESIZE 0
#define PLUG_RESIZABLE 0

#define AUV2_ENTRY NeuralPreamp_Entry
#define AUV2_ENTRY_STR "NeuralPreamp_Entry"
#define AUV2_FACTORY NeuralPreamp_Factory
#define AUV2_VIEW_CLASS NeuralPreamp_View
#define AUV2_VIEW_CLASS_STR "NeuralPreamp_View"

#define VST3_SUBCATEGORY "Fx|Distortion"

#define APP_NUM_CHANNELS 2
#define APP_N_VECTOR_WAIT 0
#define APP_MULT 1
#define APP_COPY_AUV3 0
#define APP_SIGNAL_VECTOR_SIZE 64

#define LINTSEC_FN "Lintsec.ttf"
#define BLACKOPS_FN "BlackOpsOne-Regular.ttf"

#define KNOB_FN "../FMR_Library/img/MetalicKnobBlue.png"
#define BACKGROUND_FN "../FMR_Library/img/new_preamp_800_v6.png"
