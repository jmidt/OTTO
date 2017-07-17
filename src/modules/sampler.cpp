#include "sampler.h"

#include <algorithm>

#include "../utils.h"
#include "../ui/utils.h"
#include "../ui/icons.h"
#include "../globals.h"
#include "../util/sndfile.h"
#include "../util/match.h"

namespace module {

Sampler::Sampler() :
  SynthModule(&data),
  maxSampleSize (16 * GLOB.samplerate),
  sampleData (maxSampleSize),
  editScreen (new SampleEditScreen(this)) {

  for (uint i = 0; i < sampleData.size(); i++) {
    sampleData[i] = (i % 22050) / 22050.0;
  }

  GLOB.events.samplerateChanged.add([&] (uint sr) {
    maxSampleSize = 6 * sr;
  });

}

void Sampler::process(uint nframes) {
  for (auto &&nEvent : GLOB.midiEvents) {
    nEvent.match([&] (NoteOnEvent *e) {
      currentVoiceIdx = e->key % nVoices;
      auto &&voice = data.voiceData[currentVoiceIdx];
      voice.playProgress = (voice.fwd()) ? 0 : voice.length() - 1;
      voice.trigger = true;
    }, [] (MidiEvent *) {});
  }

  for (auto &&voice : data.voiceData) {

    // Process audio
    if (voice.playProgress >= 0) {
      if (voice.fwd()) {
        if (voice.loop() && voice.trigger) {
          for(int i = 0; i < nframes; ++i) {
            GLOB.audioData.proc[i] += sampleData[voice.in + voice.playProgress];
            voice.playProgress += 1;
            if (voice.playProgress >= voice.length()) {
              voice.playProgress = 0;
            }
          }
        } else {
          int frms = std::min<int>(nframes, voice.length() - voice.playProgress);
          for(int i = 0; i < frms; ++i) {
            GLOB.audioData.proc[i] += sampleData[voice.in + voice.playProgress];
            voice.playProgress += 1;
          }
          if (voice.playProgress >= voice.length()) {
            voice.playProgress = -1;
          }
        }
      } else {
        if (voice.loop() && voice.trigger) {
          for(int i = 0; i < nframes; ++i) {
            GLOB.audioData.proc[i] += sampleData[voice.in + voice.playProgress];
            voice.playProgress -= 1;
            if (voice.playProgress < 0) {
              voice.playProgress = voice.length() -1;
            }
          }
        } else {
          int frms = std::min<int>(nframes, voice.playProgress);
          for(int i = 0; i < frms; ++i) {
            GLOB.audioData.proc[i] += sampleData[voice.in + voice.playProgress];
            voice.playProgress -= 1;
          }
        }
      }
    }
  }

  for (auto &&nEvent : GLOB.midiEvents) {
    nEvent.match([&] (NoteOffEvent *e) {
      auto &&voice = data.voiceData[e->key % nVoices];
      voice.trigger = false;
      if (voice.stop()) {
        voice.playProgress = -1;
      }
    }, [] (MidiEvent *) {});
  };
}

void Sampler::display() {
  GLOB.ui.display(editScreen);
}

void Sampler::load() {
  top1::SndFile<1> sf (samplePath(data.sampleName));

  size_t rs = std::min(maxSampleSize, sf.size());

  sampleData.resize(rs);
  sf.read(sampleData.data(), rs);

  // Auto assign voices
  // for (uint i = 0; i < nVoices; ++i) {
  //   auto &&vd = data.voiceData[i];

  //   vd.in = i * (sampleData.size() / nVoices);
  //   vd.out = (i + 1) * sampleData.size() / nVoices;
  // }

  auto &mwf = editScreen->mainWF;
  mwf->clear();
  for (auto &&s : sampleData) {
    mwf->addFrame(s);
  }

  auto &wf = editScreen->topWF;
  wf->clear();
  for (auto &&s : sampleData) {
    wf->addFrame(s);
  }
  editScreen->topWFW.viewRange = {0, wf->size() - 1};

  for (auto &&v : data.voiceData) {
    v.in.max = sf.size();
    v.out.max = sf.size();
  }

  if (sf.size() == 0) LOGD << "Empty sample file";
  sf.close();
}

void Sampler::init() {
  load();
}
}

/****************************************/
/* SampleEditScreen                     */
/****************************************/

bool module::SampleEditScreen::keypress(ui::Key key) {
  using namespace ui;
  auto& voice = module->data.voiceData[module->currentVoiceIdx];
  switch (key) {
  case K_GREEN_UP: voice.in.inc(); return true;
  case K_GREEN_DOWN: voice.in.dec(); return true;
  case K_BLUE_UP: voice.out.inc(); return true;
  case K_BLUE_DOWN: voice.out.dec(); return true;
  case K_RED_UP: voice.mode.inc(); return true;
  case K_RED_DOWN: voice.mode.dec(); return true;
  }
}

namespace drawing {

const static drawing::Size topWFsize = {240, 20};
const static drawing::Point topWFpos = {20, 20};
const static drawing::Size arrowSize = {30, 20};
const static drawing::Point arrowPos = {270, 20};
const static drawing::Size mainWFsize = {280, 170};
const static drawing::Point mainWFpos = {20, 50};

namespace Colours {

const Colour TopWF = Blue.dim(0.2);
const Colour TopWFCur = Blue.brighten(0.5);
const Colour TopWFActive = White;
const Colour WFGrid = 0x303040;
}
}

module::SampleEditScreen::SampleEditScreen(Sampler *m) :
  ModuleScreen (m),
  topWF (new Waveform(
     module->sampleData.size() / drawing::topWFsize.w / 4.0, 1.0)
         ),
  topWFW (topWF, drawing::topWFsize),
  mainWF (new Waveform(50, 1.0)),
  mainWFW (mainWF, drawing::mainWFsize) {}

void module::SampleEditScreen::draw(drawing::Canvas &ctx) {
  using namespace drawing;

  ctx.callAt(topWFpos, [&] () {
    topWFW.drawRange(ctx, topWFW.viewRange, Colours::TopWF);
    for (uint i = 0; i < Sampler::nVoices; ++i) {
      auto& voice = module->data.voiceData[i];
      bool isActive = voice.playProgress >= 0;
      bool isCurrent = i == module->currentVoiceIdx;
      if (isActive && !isCurrent) {
        Colour baseColour = Colours::TopWF;
        float mix = voice.playProgress / float(voice.out - voice.in);

        if (mix < 0) mix = 1;
        if (voice.fwd()) mix = 1 - mix; //voice is not reversed

        Colour colour = baseColour.mix(Colours::TopWFActive, mix);
        topWFW.drawRange(ctx, {
           std::size_t(std::round(voice.in / topWF->ratio)),
             std::size_t(std::round(voice.out / topWF->ratio))
             }, colour);
      }
    }
    {
      auto& voice = module->data.voiceData[module->currentVoiceIdx];
      Colour baseColour = Colours::TopWFCur;
      float mix = voice.playProgress / float(voice.out - voice.in);

      if (mix < 0) mix = 1;
      if (voice.fwd()) mix = 1 - mix; //voice is not reversed

      Colour colour = baseColour.mix(Colours::TopWFActive, mix);
      topWFW.drawRange(ctx, {
          std::size_t(std::round(voice.in / topWF->ratio)),
            std::size_t(std::round(voice.out / topWF->ratio))
            }, colour);
    }
  });

  auto& voice = module->data.voiceData[module->currentVoiceIdx];

  icons::Arrow icon;

  if (voice.fwd()) {
    icon.dir = icons::Arrow::Right;
  } else {
    icon.dir = icons::Arrow::Left;
  }

  if (voice.stop()) {
    icon.stopped = true;
  }

  if (voice.loop()) {
    icon.looping = true;
  }

  icon.size = arrowSize;
  icon.colour = Colours::Red;
  ctx.drawAt(arrowPos, icon);

  ctx.callAt(mainWFpos, [&] () {

    mainWFW.minPx = 5;
    mainWFW.viewRange = {
      std::size_t(std::round(voice.in / mainWF->ratio)),
      std::size_t(std::round(voice.out / mainWF->ratio))};

    ctx.beginPath();
    ctx.roundedCurve(mainWFW.begin(), mainWFW.end(), -1);
    ctx.stroke(Colours::TopWFCur);

    ctx.beginPath();
    ctx.circle(mainWFW.point(mainWFW.viewRange.in), 2);
    ctx.fill(Colours::Blue);

    ctx.beginPath();
    ctx.circle(mainWFW.point(mainWFW.viewRange.in), 5);
    ctx.stroke(Colours::Blue);

    ctx.beginPath();
    ctx.circle(mainWFW.point(mainWFW.viewRange.out), 2);
    ctx.fill(Colours::Green);

    ctx.beginPath();
    ctx.circle(mainWFW.point(mainWFW.viewRange.out), 5);
    ctx.stroke(Colours::Green);
  });

}
