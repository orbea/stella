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

#ifndef CARTRIDGEFE_WIDGET_HXX
#define CARTRIDGEFE_WIDGET_HXX

class CartridgeFE;
class PopUpWidget;

#include "CartDebugWidget.hxx"

class CartridgeFEWidget : public CartDebugWidget
{
  public:
    CartridgeFEWidget(GuiObject* boss, const GUI::Font& lfont,
                      const GUI::Font& nfont,
                      int x, int y, int w, int h,
                      CartridgeFE& cart);
    virtual ~CartridgeFEWidget() = default;

  private:
    CartridgeFE& myCart;
    PopUpWidget* myBank;

    enum { kBankChanged = 'bkCH' };

  private:
    // No implementation for non-bankswitched ROMs
    void loadConfig() override;
    void handleCommand(CommandSender* sender, int cmd, int data, int id) override;

    string bankState() override;

    // Following constructors and assignment operators not supported
    CartridgeFEWidget() = delete;
    CartridgeFEWidget(const CartridgeFEWidget&) = delete;
    CartridgeFEWidget(CartridgeFEWidget&&) = delete;
    CartridgeFEWidget& operator=(const CartridgeFEWidget&) = delete;
    CartridgeFEWidget& operator=(CartridgeFEWidget&&) = delete;
};

#endif
