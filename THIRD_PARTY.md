# Third-party components and assets

The MIT license of this repository covers the source code only. The items below keep their own licenses. Fonts and images are **not included** in this repository; they are embedded in the compiled plugin.

## Code

| Component | Use | License |
|---|---|---|
| [iPlug2](https://github.com/iPlug2/iPlug2) | Plugin framework and UI | See the license files in the iPlug2 repository |
| WDL (Cockos), bundled with iPlug2 | `resample.cpp` (sample rate conversion), IGraphics helpers | See `WDL/` license file in iPlug2 |
| VST3 SDK, bundled with iPlug2 | VST3 interface | License depends on the SDK version bundled with your iPlug2 copy. VST is a trademark of Steinberg Media Technologies GmbH. |

## Fonts (embedded in the plugin, not in this repository)

| Font | Used for | Author | License |
|---|---|---|---|
| Black Ops One | "FMR" logo text and amp selection menu | Sorkin Type Co (James Grieshaber, Eben Sorkin) | [SIL Open Font License 1.1](https://scripts.sil.org/OFL), Reserved Font Names "Black Ops" and "Black Ops One". Copyright line as shown by Font Squirrel: Copyright (c) 2011 by Sorkin Type Co (www.sorkintype.com). The copyright line in your font file may differ (for example "The PinyonScript Project Authors"); copy the one from the file you ship. |
| Lintsec | Control labels | Typographer Mediengestaltung (Dieter Steffmann), 2000. Lintsec (c) 1992 David Rakowski; full character set added by Dieter Steffmann. | [1001Fonts Free For Commercial Use License (FFC)](https://www.1001fonts.com/licenses/ffc.html). Embedded unmodified in the plugin; not offered as a font file. |

Both fonts are used unmodified. Black Ops One: when it is redistributed, the OFL text and the copyright line must accompany it. Lintsec: the font file is not published (see `.gitignore`), as the FFC license forbids publishing it without the author's written permission.

## Images (embedded in the plugin, not in this repository)

| Image | Origin |
|---|---|
| Knob bitmap strip | Designed by the author with [WebKnobMan](https://www.g200kg.com/en/webknobman) (g200kg). |
| Background (`new_preamp_800_v6.png`) | Generated with Gemini (Google), then retouched by the author. |

These images are not covered by the MIT license. All rights reserved by the author, except that they may be used as part of the compiled plugin binaries distributed by FMR Labs.

## Trademarks

All amplifier, pedal and brand names are trademarks of their respective owners. They are used for identification purposes only; FMR Labs is not affiliated with, endorsed or sponsored by any of them.
