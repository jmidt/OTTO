// Only include this file in tapedeck.cpp
#pragma once
#include <fmt/format.h>

#include "tapedeck.hpp"
#include "core/globals.hpp"

namespace otto::ui::vg {
  namespace Colours {
    const Colour Tape = Colour::bytes(60, 60, 59);
  }
}

namespace otto::modules {

  using namespace ui;
  using namespace ui::vg;

  struct TapeScreen : public ui::ModuleScreen<Tapedeck> {
    bool stopRecOnRelease = true;

    using ui::ModuleScreen<Tapedeck>::ModuleScreen;

    bool keypress(ui::Key key) override
    {
      bool shift = Globals::ui.keys[ui::K_SHIFT];
      switch (key) {
      case ui::K_REC:
        module->state.startRecord();
        return true;
      case ui::K_PLAY:
        if (Globals::ui.keys[ui::K_REC]) {
          stopRecOnRelease = false;
        }
        return false;
      case ui::K_TRACK_1:
        module->state.track = 0;
        return true;
      case ui::K_TRACK_2:
        module->state.track = 1;
        return true;
      case ui::K_TRACK_3:
        module->state.track = 2;
        return true;
      case ui::K_TRACK_4:
        module->state.track = 3;
        return true;
      case ui::K_LEFT:
        if (shift) module->goToBarRel(-1);
        else module->state.spool(-5);
        return true;
      case ui::K_RIGHT:
        if (shift) module->goToBarRel(1);
        else module->state.spool(5);
        return true;
      case ui::K_LOOP:
        module->state.looping = !module->state.looping;
        return true;
      case ui::K_LOOP_IN:
        if (shift) module->loopInHere();
        else module->goToLoopIn();
        return true;
      case ui::K_LOOP_OUT:
        if (shift) module->loopOutHere();
        else module->goToLoopOut();
        return true;
      case ui::K_CUT:
        if (module->state.doTapeOps()) {
          if (shift) {
            // TODO: Glue
          } else {
            module->tapeBuffer->slices[module->state.track]
              .cut(module->position());
          }
        }
          return true;
      case ui::K_LIFT:
        if (module->state.doTapeOps())
          // TODO:
          // module->tapeBuffer.lift(module->state.track);
          return true;
      case ui::K_DROP:
        if (module->state.doTapeOps())
          // TODO:
          // module->tapeBuffer.drop(module->state.track);
          return true;
      default:
        return false;
      }
    }

    void rotary(ui::RotaryEvent e) override
    {
      switch (e.rotary) {
      case ui::Rotary::Blue:
        break;
      case ui::Rotary::Green:
        break;
      case ui::Rotary::White:
        module->props.baseSpeed.step(e.clicks);
        break;
      case ui::Rotary::Red:
        module->props.gain.step(e.clicks);
        break;
      }
    }

    bool keyrelease(ui::Key key) override
    {
      switch (key) {
      case ui::K_REC:
        if (stopRecOnRelease) {
          module->state.stopRecord();
          return true;
        } else {
          stopRecOnRelease = true;
          return true;
        }
      case ui::K_LEFT:
      case ui::K_RIGHT:
        module->state.stop();
        return true;
      default:
        return false;
      }
    }

    std::string timeStr() const
    {
      return timeStr(module->position());
    }

    std::string timeStr(std::size_t position) const
    {
      double seconds = position/(1.0 * Globals::samplerate);
      double minutes = seconds / 60.0;
      return fmt::format("{:0>2}:{:0>5.2f}", (int) minutes, fmod(seconds, 60.0));
    }

    /* Drawing */

    void draw(Canvas& ctx) override
    {
      draw_tape(ctx);
      draw_timeline(ctx);
      draw_static_backround(ctx);
      draw_text(ctx);
      draw_state_icon(ctx);
      draw_speed_indicator(ctx);
      draw_slider(ctx);
    }

    void draw_static_backround(Canvas& ctx)
    {
      // tAPEDECK/CASETTETAPEGUIDELEFT
      ctx.save();
      ctx.beginPath();
      ctx.moveTo(49.6, 160.2);
      ctx.bezierCurveTo(49.6, 163.7, 46.8, 166.6, 43.2, 166.6);
      ctx.bezierCurveTo(39.6, 166.6, 36.8, 163.7, 36.8, 160.2);
      ctx.bezierCurveTo(36.8, 156.6, 39.6, 153.7, 43.2, 153.7);
      ctx.bezierCurveTo(46.8, 153.7, 49.6, 156.6, 49.6, 160.2);
      ctx.closePath();
      ctx.lineWidth(2.0);
      ctx.strokeStyle(Colour::bytes(60, 60, 59));
      ctx.lineCap(Canvas::LineCap::ROUND);
      ctx.lineJoin(Canvas::LineJoin::ROUND);
      ctx.fill(Colours::Black);
      ctx.stroke();

      // tAPEDECK/CASETTETAPEGUIDERIGHT
      ctx.beginPath();
      ctx.moveTo(284.3, 160.2);
      ctx.bezierCurveTo(284.3, 163.7, 281.4, 166.6, 277.9, 166.6);
      ctx.bezierCurveTo(274.3, 166.6, 271.4, 163.7, 271.4, 160.2);
      ctx.bezierCurveTo(271.4, 156.6, 274.3, 153.7, 277.9, 153.7);
      ctx.bezierCurveTo(281.4, 153.7, 284.3, 156.6, 284.3, 160.2);
      ctx.closePath();
      ctx.fill(Colours::Black);
      ctx.stroke();

      // tAPEDECK/CASETTEBORDERTOP
      ctx.beginPath();
      ctx.moveTo(21.7, 36.4);
      ctx.lineTo(21.7, 24.7);
      ctx.bezierCurveTo(21.7, 21.6, 24.2, 19.1, 27.3, 19.1);
      ctx.lineTo(292.2, 19.1);
      ctx.bezierCurveTo(295.3, 19.1, 297.8, 21.6, 297.8, 24.7);
      ctx.lineTo(297.8, 36.4);
      ctx.stroke();

      // tAPEDECK/CASETTEBORDERLEFTBOT
      ctx.beginPath();
      ctx.moveTo(21.7, 36.4);
      ctx.lineTo(21.7, 174.5);
      ctx.bezierCurveTo(21.7, 177.6, 24.1, 180.0, 27.2, 180.0);
      ctx.lineTo(49.2, 180.0);
      ctx.stroke();

      // tAPEDECK/CASETTEBORDERBOTMID
      ctx.beginPath();
      ctx.moveTo(74.4, 149.0);
      ctx.lineTo(246.4, 149.0);
      ctx.stroke();

      // tAPEDECK/CASETTEBORDERRIGHTCURVE
      ctx.beginPath();
      ctx.moveTo(256.5, 149.0);
      ctx.bezierCurveTo(259.9, 149.0, 262.8, 151.2, 263.8, 154.4);
      ctx.lineTo(272.1, 180.0);
      ctx.stroke();

      // tAPEDECK/CASETTEBORDERLEFTCURVE
      ctx.beginPath();
      ctx.moveTo(49.2, 180.0);
      ctx.lineTo(57.4, 154.4);
      ctx.bezierCurveTo(58.0, 152.8, 59.0, 151.4, 60.3, 150.5);
      ctx.stroke();

      // tAPEDECK/CASETTEBORDERRIGHTBOT
      ctx.beginPath();
      ctx.moveTo(272.1, 180.0);
      ctx.lineTo(292.2, 180.0);
      ctx.bezierCurveTo(295.3, 180.0, 297.8, 177.6, 297.8, 174.5);
      ctx.lineTo(297.8, 155.0);
      ctx.stroke();

      // tAPEDECK/CASETTEFELT
      ctx.beginPath();
      ctx.moveTo(177.9, 164.2);
      ctx.lineTo(177.9, 170.2);
      ctx.lineTo(143.9, 170.2);
      ctx.lineTo(143.9, 164.2);
      ctx.stroke();

      // tAPEDECK/TAPEHEAD
      ctx.beginPath();
      ctx.moveTo(174.9, 186.2);
      ctx.lineTo(174.9, 183.0);
      ctx.bezierCurveTo(174.9, 183.0, 166.4, 180.5, 160.4, 180.5);
      ctx.bezierCurveTo(154.3, 180.5, 146.2, 183.0, 146.2, 183.0);
      ctx.lineTo(146.2, 186.2);
      ctx.stroke();

      // tAPEDECK/TAPEGUIDELEFT
      ctx.beginPath();
      ctx.moveTo(99.3, 167.2);
      ctx.bezierCurveTo(99.3, 169.7, 97.3, 171.7, 94.8, 171.7);
      ctx.bezierCurveTo(92.3, 171.7, 90.2, 169.7, 90.2, 167.2);
      ctx.bezierCurveTo(90.2, 164.7, 92.3, 162.6, 94.8, 162.6);
      ctx.bezierCurveTo(97.3, 162.6, 99.3, 164.7, 99.3, 167.2);
      ctx.closePath();
      ctx.stroke();

      // tAPEDECK/TAPEGUIDERIGHT
      ctx.beginPath();
      ctx.moveTo(229.2, 167.2);
      ctx.bezierCurveTo(229.2, 169.7, 227.1, 171.7, 224.6, 171.7);
      ctx.bezierCurveTo(222.1, 171.7, 220.1, 169.7, 220.1, 167.2);
      ctx.bezierCurveTo(220.1, 164.7, 222.1, 162.6, 224.6, 162.6);
      ctx.bezierCurveTo(227.1, 162.6, 229.2, 164.7, 229.2, 167.2);
      ctx.closePath();
      ctx.stroke();

      // tAPEDECK/TOPONEPLACEHOLDER
      ctx.font(Fonts::Bold);
      ctx.font(18.0);
      ctx.fillStyle(Colour::bytes(60, 60, 59));
      ctx.fillText("OTTO", 29.1, 36.4);
      ctx.fillText("TAPE", 231.2, 36.4);

      // tAPEDECK/COLOURCODE/UNDERSCORE
      ctx.beginPath();
      ctx.moveTo(270.4, 42.0);
      ctx.lineTo(287.3, 42.0);
      ctx.lineWidth(2.0);
      ctx.strokeStyle(Colour::bytes(60, 60, 59));
      ctx.lineCap(Canvas::LineCap::ROUND);
      ctx.lineJoin(Canvas::LineJoin::ROUND);
      ctx.stroke();

      ctx.restore();
    }

    void draw_speed_indicator(Canvas& ctx)
    {
      // tAPEDECK/SPEEDINDICATOR
      ctx.save();

      // tAPEDECK/SPEEDINDICATOR/MID
      ctx.save();
      ctx.beginPath();
      ctx.moveTo(160.4, 154.8);
      ctx.lineTo(160.4, 160.0);
      ctx.lineWidth(2.0);
      ctx.strokeStyle(Colour::bytes(60, 60, 59));
      ctx.lineCap(Canvas::LineCap::ROUND);
      ctx.lineJoin(Canvas::LineJoin::ROUND);
      ctx.stroke();

      // tAPEDECK/SPEEDINDICATOR/L
      ctx.beginPath();
      ctx.moveTo(147.4, 156.3);
      ctx.lineTo(147.4, 158.5);
      ctx.stroke();

      // tAPEDECK/SPEEDINDICATOR/LL
      ctx.beginPath();
      ctx.moveTo(134.4, 155.8);
      ctx.lineTo(134.4, 159.3);
      ctx.stroke();

      // tAPEDECK/SPEEDINDICATOR/R
      ctx.beginPath();
      ctx.moveTo(173.5, 156.3);
      ctx.lineTo(173.5, 158.5);
      ctx.stroke();

      // tAPEDECK/SPEEDINDICATOR/RR
      ctx.beginPath();
      ctx.moveTo(186.5, 155.8);
      ctx.lineTo(186.5, 159.3);
      ctx.stroke();

      // tAPEDECK/SPEEDINDICATOR/INDICATOR
      float speed_amount = (module->state.playSpeed) / module->state.max_speed;
      float speed_x = 160.4 + speed_amount * 26;
      ctx.beginPath();
      ctx.moveTo(speed_x, 154.8);
      ctx.lineTo(speed_x, 160.0);
      ctx.strokeStyle(Colours::White);
      ctx.stroke();
    }

    void draw_text(Canvas& ctx)
    {
      ctx.save();

      // tAPEDECK/TIMESIGNATURE
      ctx.save();
      ctx.font(Fonts::Mono);
      ctx.font(14.9);
      ctx.textAlign(TextAlign::Center, TextAlign::Baseline);
      ctx.fillStyle(Colour::bytes(255, 255, 255));
      ctx.fillText(fmt::format("{: .2f}", module->props.baseSpeed), 160.5, 96.3);

      // tAPEDECK/TIMESTAMP
      ctx.font(Fonts::Mono);
      ctx.font(18);
      ctx.textAlign(TextAlign::Center, TextAlign::Baseline);
      ctx.fillStyle(Colour::bytes(255, 255, 255));
      ctx.fillText(timeStr(), 160.5, 43);
      ctx.restore();

      // tAPEDECK/TIMELINE

      // tAPEDECK/TIMELINECURRENT
      ctx.beginPath();
      ctx.moveTo(160.5, 187.9);
      ctx.lineTo(160.5, 219.6);
      ctx.lineWidth(2.0);
      ctx.strokeStyle(Colour::bytes(255, 255, 255));
      ctx.lineCap(Canvas::LineCap::ROUND);
      ctx.lineJoin(Canvas::LineJoin::ROUND);
      ctx.stroke();

      // tAPEDECK/TAPENUMBERPLACEHOLDER
      ctx.font(Fonts::Mono);
      ctx.font(17.9);
      ctx.fillStyle(Colour::bytes(255, 255, 255));
      ctx.fillText("64", 270.1, 36.4);

      // tAPEDECK/COLOURCODE/CODE/1
      ctx.beginPath();
      ctx.moveTo(231.6, 42.0);
      ctx.lineTo(236.7, 42.0);
      ctx.lineWidth(2.0);
      ctx.strokeStyle(Colours::Blue);
      ctx.lineCap(Canvas::LineCap::ROUND);
      ctx.lineJoin(Canvas::LineJoin::ROUND);
      ctx.stroke();

      // tAPEDECK/COLOURCODE/CODE/2
      ctx.beginPath();
      ctx.moveTo(240.6, 42.0);
      ctx.lineTo(245.7, 42.0);
      ctx.strokeStyle(Colours::Green);
      ctx.stroke();

      // tAPEDECK/COLOURCODE/CODE/3
      ctx.beginPath();
      ctx.moveTo(249.6, 42.0);
      ctx.lineTo(254.7, 42.0);
      ctx.strokeStyle(Colours::Blue);
      ctx.stroke();

      // tAPEDECK/COLOURCODE/CODE/4
      ctx.beginPath();
      ctx.moveTo(258.6, 42.0);
      ctx.lineTo(263.7, 42.0);
      ctx.strokeStyle(Colours::Blue);
      ctx.stroke();

      ctx.restore();
    }

    void draw_timeline(Canvas& ctx)
    {
      // tAPEDECK/TIMELINEMARKERS
      ctx.save();

      // The amount of time to display on the timeline
      // TODO: Animate this?
      int timeline_time = 5 * Globals::samplerate;

      util::audio::Section<int> view_time {
        (int) module->position() - timeline_time/2,
        (int) module->position() + timeline_time/2};

      float left_edge = 23.2;
      float right_edge = 296.2;
      float timeline_width = right_edge - left_edge;

      float length_pr_time = timeline_width / timeline_time;
      auto time_to_coord = [&](int time) {
        time -= view_time.in;
        return left_edge + time * length_pr_time;
      };

      { // Bar Markers
        float max_x = std::min(right_edge, time_to_coord(tape_buffer::max_length));
        float min_x = std::max(left_edge, time_to_coord(0));

        ctx.lineWidth(1.5);
        ctx.strokeStyle(Colour::bytes(112, 126, 133));
        ctx.lineCap(Canvas::LineCap::ROUND);
        ctx.lineJoin(Canvas::LineJoin::ROUND);

        auto iter = Globals::metronome.iter(Globals::metronome.time_for_bar(
            std::min(0.f, Globals::metronome.bar_for_time(view_time.in) - 1)));

        while (true) {
          float x = time_to_coord(*iter);
          if (x < min_x) {
            iter++;
            continue;
          }
          if (x > max_x) break;
          ctx.beginPath();
          ctx.moveTo(x, 196.6);
          ctx.lineTo(x, 198.2);
          ctx.stroke();
          iter++;
        }
      }

      // Loop section
      auto ls = module->loopSect;
      if (ls.size() > 0) {
        if (view_time.overlaps(ls)) {
          ctx.beginPath();
          ctx.moveTo(std::max(left_edge, time_to_coord(ls.in)), 192.0);
          ctx.lineTo(std::min(right_edge, time_to_coord(ls.out)), 192.0);
          ctx.strokeStyle(Colour::bytes(58, 169, 53));
          ctx.stroke();
        }
      }

      // tAPEDECK/TIMELINE
      ctx.lineWidth(2.0);

      auto draw_slice = [&] (auto slice, int track) {
        if (slice.size() == 0) return;
        float y = 203 + 5 * track;
        ctx.beginPath();
        ctx.moveTo(std::max(left_edge,  time_to_coord(slice.in)), y);
        ctx.lineTo(std::min(right_edge, time_to_coord(slice.out - 1)), y);
        ctx.stroke();
      };

      ctx.strokeStyle(Colours::Tape);
      for (int track = 0; track < 4; track++) {
        for (auto&& slice : module->tapeBuffer->slices[track]
               .overlapping_slices(view_time)) {
          draw_slice(slice, track);
        }
      }

      // Recording or selected
      int cur_track = module->state.track;
      auto slice = module->recSect;
      if (slice.size() > 0 ) {
        ctx.strokeStyle(Colours::Red);
      } else {
        slice = module->tapeBuffer->slices[cur_track].current(module->position());
        ctx.strokeStyle(Colours::Blue);
      }
      draw_slice(slice, cur_track);

      // tAPEDECK/TIMELINE/TAPEINDICATORLEFT

      auto indicator_colour = [&] (int track) {
        return (module->state.track == track) ?
        Colour(Colours::White) : Colour::bytes(60, 60, 59);
      };

      // tAPEDECK/TIMELINE/TAPEINDICATORLEFT/1
      ctx.beginPath();
      ctx.moveTo(23.2, 203.0);
      ctx.bezierCurveTo(23.2, 203.9, 22.5, 204.6, 21.6, 204.6);
      ctx.bezierCurveTo(20.7, 204.6, 20.0, 203.9, 20.0, 203.0);
      ctx.bezierCurveTo(20.0, 202.2, 20.7, 201.5, 21.6, 201.5);
      ctx.bezierCurveTo(22.5, 201.5, 23.2, 202.2, 23.2, 203.0);
      ctx.closePath();
      ctx.fill(indicator_colour(0));

      // tAPEDECK/TIMELINE/TAPEINDICATORLEFT/2
      ctx.beginPath();
      ctx.moveTo(23.2, 208.3);
      ctx.bezierCurveTo(23.2, 209.2, 22.5, 209.9, 21.6, 209.9);
      ctx.bezierCurveTo(20.7, 209.9, 20.0, 209.2, 20.0, 208.3);
      ctx.bezierCurveTo(20.0, 207.4, 20.7, 206.7, 21.6, 206.7);
      ctx.bezierCurveTo(22.5, 206.7, 23.2, 207.4, 23.2, 208.3);
      ctx.closePath();
      ctx.fill(indicator_colour(1));

      // tAPEDECK/TIMELINE/TAPEINDICATORLEFT/3
      ctx.beginPath();
      ctx.moveTo(23.2, 213.0);
      ctx.bezierCurveTo(23.2, 213.9, 22.5, 214.6, 21.6, 214.6);
      ctx.bezierCurveTo(20.7, 214.6, 20.0, 213.9, 20.0, 213.0);
      ctx.bezierCurveTo(20.0, 212.2, 20.7, 211.5, 21.6, 211.5);
      ctx.bezierCurveTo(22.5, 211.5, 23.2, 212.2, 23.2, 213.0);
      ctx.closePath();
      ctx.fill(indicator_colour(2));

      // tAPEDECK/TIMELINE/TAPEINDICATORLEFT/4
      ctx.beginPath();
      ctx.moveTo(23.2, 217.8);
      ctx.bezierCurveTo(23.2, 218.6, 22.5, 219.4, 21.6, 219.4);
      ctx.bezierCurveTo(20.7, 219.4, 20.0, 218.6, 20.0, 217.8);
      ctx.bezierCurveTo(20.0, 216.9, 20.7, 216.2, 21.6, 216.2);
      ctx.bezierCurveTo(22.5, 216.2, 23.2, 216.9, 23.2, 217.8);
      ctx.closePath();
      ctx.fill(indicator_colour(3));

      // tAPEDECK/TIMELINE/TAPEINDICATORRIGHT

      // tAPEDECK/TIMELINE/TAPEINDICATORRIGHT/2
      ctx.beginPath();
      ctx.moveTo(299.4, 203.0);
      ctx.bezierCurveTo(299.4, 203.9, 298.7, 204.6, 297.8, 204.6);
      ctx.bezierCurveTo(296.9, 204.6, 296.2, 203.9, 296.2, 203.0);
      ctx.bezierCurveTo(296.2, 202.2, 296.9, 201.5, 297.8, 201.5);
      ctx.bezierCurveTo(298.7, 201.5, 299.4, 202.2, 299.4, 203.0);
      ctx.closePath();
      ctx.fill(indicator_colour(0));

      // tAPEDECK/TIMELINE/TAPEINDICATORRIGHT/3
      ctx.beginPath();
      ctx.moveTo(299.4, 208.3);
      ctx.bezierCurveTo(299.4, 209.2, 298.7, 209.9, 297.8, 209.9);
      ctx.bezierCurveTo(296.9, 209.9, 296.2, 209.2, 296.2, 208.3);
      ctx.bezierCurveTo(296.2, 207.4, 296.9, 206.7, 297.8, 206.7);
      ctx.bezierCurveTo(298.7, 206.7, 299.4, 207.4, 299.4, 208.3);
      ctx.closePath();
      ctx.fill(indicator_colour(1));

      // tAPEDECK/TIMELINE/TAPEINDICATORRIGHT/3
      ctx.beginPath();
      ctx.moveTo(299.4, 213.0);
      ctx.bezierCurveTo(299.4, 213.9, 298.7, 214.6, 297.8, 214.6);
      ctx.bezierCurveTo(296.9, 214.6, 296.2, 213.9, 296.2, 213.0);
      ctx.bezierCurveTo(296.2, 212.2, 296.9, 211.5, 297.8, 211.5);
      ctx.bezierCurveTo(298.7, 211.5, 299.4, 212.2, 299.4, 213.0);
      ctx.closePath();
      ctx.fill(indicator_colour(2));

      // tAPEDECK/TIMELINE/TAPEINDICATORRIGHT/4
      ctx.beginPath();
      ctx.moveTo(299.4, 217.8);
      ctx.bezierCurveTo(299.4, 218.6, 298.7, 219.4, 297.8, 219.4);
      ctx.bezierCurveTo(296.9, 219.4, 296.2, 218.6, 296.2, 217.8);
      ctx.bezierCurveTo(296.2, 216.9, 296.9, 216.2, 297.8, 216.2);
      ctx.bezierCurveTo(298.7, 216.2, 299.4, 216.9, 299.4, 217.8);
      ctx.closePath();
      ctx.fill(indicator_colour(3));

      ctx.restore();
    }

    void draw_slider(Canvas& ctx)
    {
      float graph = module->procGraph.clip();
      float setting = module->props.gain.mode.normalize();
      auto colour = (Colour::bytes(60, 60, 59).mix(Colours::Red,
          std::min(1.f, graph / setting * 3.f)));
      ctx.save();
      // tAPEDECK/SYNTH
      ctx.beginPath();
      ctx.moveTo(290.6, 138.4);
      ctx.lineTo(290.6, 142.1);
      ctx.bezierCurveTo(290.6, 143.1, 291.4, 143.9, 292.4, 143.9);
      ctx.bezierCurveTo(293.4, 143.9, 294.2, 143.1, 294.2, 142.1);
      ctx.lineTo(294.2, 134.8);
      ctx.bezierCurveTo(294.2, 133.8, 295.0, 133.0, 296.0, 133.0);
      ctx.bezierCurveTo(297.0, 133.0, 297.8, 133.8, 297.8, 134.8);
      ctx.lineTo(297.8, 142.1);
      ctx.bezierCurveTo(297.8, 143.1, 298.6, 143.9, 299.6, 143.9);
      ctx.bezierCurveTo(300.6, 143.9, 301.4, 143.1, 301.4, 142.1);
      ctx.lineTo(301.4, 134.8);
      ctx.bezierCurveTo(301.4, 133.8, 302.2, 133.0, 303.2, 133.0);
      ctx.bezierCurveTo(304.2, 133.0, 305.0, 133.8, 305.0, 134.8);
      ctx.lineTo(305.0, 138.4);
      ctx.lineWidth(2.0);
      ctx.lineCap(Canvas::LineCap::ROUND);
      ctx.lineJoin(Canvas::LineJoin::ROUND);
      ctx.stroke(colour);

      // tAPEDECK/METERRIGHT/METERRIGHTSTATIC
      ctx.beginPath();
      ctx.moveTo(297.8, 120.6);
      ctx.lineTo(297.8, 42.4);
      ctx.stroke(Colour::bytes(60, 60, 59));

      // Graph
      if (graph > 0.005) {
        ctx.beginPath();
        ctx.moveTo(297.8, 120.6);
        float y = 120.6 - (120.6 - 42.4) * graph;
        ctx.lineTo(297.8, y);
        ctx.stroke(Colours::Red);
      }

      // Setting
      ctx.beginPath();
      float y = 120.6 - (120.6 - 42.4) * setting;
      ctx.circle(297.8, y, 2.f);
      ctx.fill(Colours::Red);

      ctx.restore();
    }

    void draw_state_icon(Canvas& ctx)
    {

      auto& state = module->state;
      ctx.save();

      ctx.lineCap(Canvas::LineCap::ROUND);
      ctx.lineJoin(Canvas::LineJoin::MITER);
      ctx.lineWidth(2.0);

      ctx.translate(148.65, 120);

      if (state.readyToRec) {
        ctx.strokeStyle(Colours::Red); // Red means recording
        ctx.fillStyle(Colours::Red);
        // } else if (state.playing()) {
        //   ctx.strokeStyle(Colours::Green);
        //   ctx.fillStyle(Colours::Green);
      } else {
        ctx.strokeStyle(Colours::White);
        ctx.fillStyle(Colours::White);
      }

      if (state.recording()){ // Record
        ctx.beginPath();
        ctx.moveTo(17.4, 6.9);
        ctx.bezierCurveTo(17.4, 9.9, 14.9, 12.4, 11.9, 12.4);
        ctx.bezierCurveTo(8.8, 12.4, 6.3, 9.9, 6.3, 6.9);
        ctx.bezierCurveTo(6.3, 3.8, 8.8, 1.4, 11.9, 1.4);
        ctx.bezierCurveTo(14.9, 1.4, 17.4, 3.8, 17.4, 6.9);
        ctx.closePath();
        ctx.fill();
        ctx.stroke();
      } else if (state.spooling() && state.playSpeed > 0) { // Spool FWD
        ctx.beginPath();
        ctx.moveTo(11.9, 2.0);
        ctx.lineTo(22.0, 6.9);
        ctx.lineTo(11.9, 11.7);
        ctx.lineTo(11.9, 2.0);
        ctx.closePath();
        ctx.fill();
        ctx.stroke();

        // icons/fforward/1
        ctx.beginPath();
        ctx.moveTo(1.0, 2.0);
        ctx.lineTo(11.1, 6.9);
        ctx.lineTo(1.0, 11.7);
        ctx.lineTo(1.0, 2.0);
        ctx.closePath();
        ctx.fill();
        ctx.stroke();
      } else if (state.spooling() && state.playSpeed < 0) { // Spool BWD
        // icons/rewind/1
        ctx.beginPath();
        ctx.moveTo(11.3, 11.7);
        ctx.lineTo(1.1, 6.9);
        ctx.lineTo(11.3, 2.0);
        ctx.lineTo(11.3, 11.7);
        ctx.closePath();
        ctx.fill();
        ctx.stroke();

        // icons/rewind/2
        ctx.beginPath();
        ctx.moveTo(22.1, 11.7);
        ctx.lineTo(12.0, 6.9);
        ctx.lineTo(22.1, 2.0);
        ctx.lineTo(22.1, 11.7);
        ctx.closePath();
        ctx.fill();
        ctx.stroke();
      } else if (state.looping) { // Loop
        if (module->props.baseSpeed > 0) {
          // icons/loopforward/Pad
          ctx.beginPath();
          ctx.moveTo(9.3, 3.9);
          ctx.lineTo(4.4, 3.9);
          ctx.bezierCurveTo(2.5, 3.9, 1.0, 5.5, 1.0, 7.4);
          ctx.lineTo(1.0, 8.1);
          ctx.bezierCurveTo(1.0, 10.0, 2.5, 11.5, 4.4, 11.5);
          ctx.lineTo(18.6, 11.5);
          ctx.bezierCurveTo(20.5, 11.5, 22.0, 10.0, 22.0, 8.1);
          ctx.lineTo(22.0, 7.4);
          ctx.bezierCurveTo(22.0, 5.5, 20.5, 3.9, 18.6, 3.9);
          ctx.lineTo(15.5, 3.9);
          ctx.stroke();

          // icons/loopforward/Pad
          ctx.beginPath();
          ctx.moveTo(8.1, 6.9);
          ctx.lineTo(11.1, 3.9);
          ctx.lineTo(8.1, 1.0);
          ctx.stroke();

        } else {
          ctx.beginPath();
          ctx.moveTo(13.7, 3.9);
          ctx.lineTo(18.6, 3.9);
          ctx.bezierCurveTo(20.5, 3.9, 22.0, 5.5, 22.0, 7.4);
          ctx.lineTo(22.0, 8.1);
          ctx.bezierCurveTo(22.0, 10.0, 20.5, 11.5, 18.6, 11.5);
          ctx.lineTo(4.4, 11.5);
          ctx.bezierCurveTo(2.5, 11.5, 1.0, 10.0, 1.0, 8.1);
          ctx.lineTo(1.0, 7.4);
          ctx.bezierCurveTo(1.0, 5.5, 2.5, 3.9, 4.4, 3.9);
          ctx.lineTo(7.5, 3.9);
          ctx.stroke();

          // icons/loopbackward/Pad
          ctx.beginPath();
          ctx.moveTo(14.9, 6.9);
          ctx.lineTo(11.9, 3.9);
          ctx.lineTo(14.9, 1.0);
          ctx.stroke();
        }
      } else if (state.playing()) { // Play
        if (module->props.baseSpeed > 0) {
          // icons/play
          ctx.beginPath();
          ctx.moveTo(7.7, 1.4);
          ctx.lineTo(17.8, 6.9);
          ctx.lineTo(7.7, 12.5);
          ctx.lineTo(7.7, 1.4);
          ctx.closePath();
          ctx.fill();
          ctx.stroke();
        } else {
          // icons/playbackward
          ctx.beginPath();
          ctx.moveTo(16.2, 12.5);
          ctx.lineTo(6.1, 6.9);
          ctx.lineTo(16.2, 1.4);
          ctx.lineTo(16.2, 12.5);
          ctx.closePath();
          ctx.fill();
          ctx.stroke();
        }
      } else { // Stop
        // icons/stop
        // ctx.beginPath();
        // ctx.moveTo(16.5, 2.2);
        // ctx.lineTo(7.2, 2.2);
        // ctx.lineTo(7.2, 11.5);
        // ctx.lineTo(16.5, 11.5);
        // ctx.lineTo(16.5, 2.2);
        // ctx.closePath();
        // ctx.fill();
        // ctx.stroke();
      }

      ctx.restore();
    }

    void draw_tape(Canvas& ctx)
    {
      Point l_center = {93.4, 90.1};
      Point r_center = {224.6, 90.1};
      Colour colour  = module->state.readyToRec ? Colour(Colours::Red) : Colour::bytes(112, 126, 133);

      float min_r = 26;
      float max_r = 42;
      float pos_amount = (module->position() / float(tape_buffer::max_length));
      float l_radius = min_r + pos_amount * (max_r - min_r);
      float r_radius = min_r + (1 - pos_amount) * (max_r - min_r);
      draw_tape_half(ctx, colour, {37, 160}, l_center, l_radius);
      draw_tape_half(ctx, colour, {284, 160}, r_center, r_radius);

      draw_static_tape(ctx, colour);

      float reel_rotation = module->position() / 44100.0 * M_PI;
      draw_reel_wheel(ctx, l_center, reel_rotation, colour);
      draw_reel_wheel(ctx, r_center, reel_rotation, colour);
    }

    void draw_tape_half(Canvas& ctx, Colour col, Point p, Point c, float r)
    {
      // Circle C1 has center c and radius r
      // p is a point outside circle C
      // Calculate point ip on C, for which a line between ip and p is tangent
      // to C
      // TODO: Some of this could be done constexpr or simply by hand
      util::math::vec cp = p - c;
      float l = std::sqrt(cp.x*cp.x + cp.y*cp.y);
      float v = std::asin(r/l);
      float u = std::asin(std::abs(cp.x)/l);
      float w = u - v;
      float m = std::cos(v) * l;
      Point ip = p - Point{(cp.x > 0 ? 1 : -1) * std::sin(w) * m, std::cos(w) * m};

      ctx.lineWidth(2);
      ctx.beginPath();
      ctx.circle(c, r);
      ctx.moveTo(ip);
      ctx.lineTo(p);
      ctx.stroke(col);
    }

    void draw_static_tape(Canvas& ctx, Colour colour)
    {
      // tAPEDECK/TAPEBOTTOM
      ctx.save();
      ctx.beginPath();
      ctx.moveTo(43.2, 166.6);
      ctx.lineTo(92.8, 174.6);
      ctx.lineTo(224.9, 174.6);
      ctx.lineTo(277.9, 166.6);
      ctx.lineWidth(2.0);
      ctx.strokeStyle(colour);
      ctx.lineCap(Canvas::LineCap::ROUND);
      ctx.lineJoin(Canvas::LineJoin::ROUND);
      ctx.stroke();

      // tAPEDECK/LEFTREEL

      // tAPEDECK/LEFTREEL/OUTTERCIRCLE
      ctx.beginPath();
      ctx.moveTo(119.0, 90.1);
      ctx.bezierCurveTo(119.0, 104.2, 107.7, 115.5, 93.6, 115.5);
      ctx.bezierCurveTo(79.6, 115.5, 68.2, 104.2, 68.2, 90.1);
      ctx.bezierCurveTo(68.2, 76.1, 79.6, 64.7, 93.6, 64.7);
      ctx.bezierCurveTo(107.7, 64.7, 119.0, 76.1, 119.0, 90.1);
      ctx.closePath();
      ctx.fill(Colours::Black);
      ctx.stroke();

      // tAPEDECK/RIGHTREEL/OUTTERCIRCLE
      ctx.beginPath();
      ctx.moveTo(250.5, 90.3);
      ctx.bezierCurveTo(250.5, 104.4, 239.0, 115.9, 224.9, 115.9);
      ctx.bezierCurveTo(210.8, 115.9, 199.3, 104.4, 199.3, 90.3);
      ctx.bezierCurveTo(199.3, 76.2, 210.8, 64.7, 224.9, 64.7);
      ctx.bezierCurveTo(239.0, 64.7, 250.5, 76.2, 250.5, 90.3);
      ctx.closePath();
      ctx.fill(Colours::Black);
      ctx.stroke();

      // tAPEDECK/TIMELINE
      ctx.restore();
    }

    void draw_reel_wheel(Canvas& ctx, Point center, float rotation, Colour colour)
    {
      // rEELWHEEL/LEFTREEL
      ctx.save();

      ctx.rotateAround(rotation, center);
      // Reel size = 53x53
      ctx.translate(center - Point{26.5, 26.5});

      // rEELWHEEL/LEFTREEL/OUTTERCIRCLE
      ctx.beginPath();
      ctx.circle(26.5, 26.5, 25.5);
      ctx.strokeStyle(colour);
      ctx.stroke();

      // rEELWHEEL/LEFTREEL/RED
      ctx.beginPath();
      ctx.moveTo(44.7, 36.9);
      ctx.bezierCurveTo(41.0, 43.2, 34.2, 47.5, 26.4, 47.5);
      ctx.strokeStyle(Colour::bytes(230, 51, 42));
      ctx.stroke();

      // White parts
      ctx.lineWidth(2.0);
      ctx.strokeStyle(Colour::bytes(255, 255, 255));

      // rEELWHEEL/LEFTREEL/2
      ctx.beginPath();
      ctx.moveTo(35.1, 31.5);
      ctx.lineTo(38.3, 33.3);
      ctx.stroke();

      // rEELWHEEL/LEFTREEL/CIRCLE
      ctx.beginPath();
      ctx.circle(26.5, 26.5, 13);
      ctx.stroke();

      ctx.beginPath();
      ctx.strokeStyle(Colour::bytes(255, 255, 255));
      ctx.moveTo(26.4, 16.4);
      ctx.lineTo(26.4, 12.7);
      ctx.moveTo(17.7, 31.5);
      ctx.lineTo(14.5, 33.3);
      ctx.moveTo(26.4, 36.5);
      ctx.lineTo(26.4, 40.2);
      ctx.moveTo(17.7, 21.4);
      ctx.lineTo(14.5, 19.6);
      ctx.moveTo(35.1, 21.4);
      ctx.lineTo(38.3, 19.6);
      ctx.stroke();

      ctx.restore();
    }
  };

}
