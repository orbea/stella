//============================================================================
//
//   SSSS    tt          lll  lll
//  SS  SS   tt           ll   ll
//  SS     tttttt  eeee   ll   ll   aaaa
//   SSSS    tt   ee  ee  ll   ll      aa
//      SS   tt   eeeeee  ll   ll   aaaaa  --  "An Atari 2600 VCS Emulator"
//  SS  SS   tt   ee      ll   ll  aa  aa
//   SSSS     ttt  eeeee llll llll  aaaaa
//
// Copyright (c) 1995-2019 by Bradford W. Mott, Stephen Anthony
// and the Stella Team
//
// See the file "License.txt" for information on usage and redistribution of
// this file, and for a DISCLAIMER OF ALL WARRANTIES.
//============================================================================

#ifndef SETTINGS_LIBRETRO_HXX
#define SETTINGS_LIBRETRO_HXX

class OSystem;

#include "Settings.hxx"

class SettingsLIBRETRO : public Settings
{
  public:
    /**
      Create a new LIBRETRO settings object
    */
    explicit SettingsLIBRETRO();
    virtual ~SettingsLIBRETRO() = default;

  private:
    // Following constructors and assignment operators not supported
    SettingsLIBRETRO(const SettingsLIBRETRO&) = delete;
    SettingsLIBRETRO(SettingsLIBRETRO&&) = delete;
    SettingsLIBRETRO& operator=(const SettingsLIBRETRO&) = delete;
    SettingsLIBRETRO& operator=(SettingsLIBRETRO&&) = delete;
};

#endif
