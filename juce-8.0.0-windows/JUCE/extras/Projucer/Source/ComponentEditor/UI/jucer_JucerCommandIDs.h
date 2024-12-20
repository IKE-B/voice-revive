/*
  ==============================================================================

   This file is part of the JUCE framework.
   Copyright (c) Raw Material Software Limited

   JUCE is an open source framework subject to commercial or open source
   licensing.

   By downloading, installing, or using the JUCE framework, or combining the
   JUCE framework with any other source code, object code, content or any other
   copyrightable work, you agree to the terms of the JUCE End User Licence
   Agreement, and all incorporated terms including the JUCE Privacy Policy and
   the JUCE Website Terms of Service, as applicable, which will bind you. If you
   do not agree to the terms of these agreements, we will not license the JUCE
   framework to you, and you must discontinue the installation or download
   process and cease use of the JUCE framework.

   JUCE End User Licence Agreement: https://juce.com/legal/juce-8-licence/
   JUCE Privacy Policy: https://juce.com/juce-privacy-policy
   JUCE Website Terms of Service: https://juce.com/juce-website-terms-of-service/

   Or:

   You may also use this code under the terms of the AGPLv3:
   https://www.gnu.org/licenses/agpl-3.0.en.html

   THE JUCE FRAMEWORK IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL
   WARRANTIES, WHETHER EXPRESSED OR IMPLIED, INCLUDING WARRANTY OF
   MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE, ARE DISCLAIMED.

  ==============================================================================
*/

#pragma once


//==============================================================================
/**
    A namespace to hold all the possible command IDs.
*/
namespace JucerCommandIDs
{
    enum
    {
        test                   = 0xf20009,
        toFront                = 0xf2000a,
        toBack                 = 0xf2000b,

        group                  = 0xf20017,
        ungroup                = 0xf20018,

        showGrid               = 0xf2000e,
        enableSnapToGrid       = 0xf2000f,

        editCompLayout         = 0xf20010,
        editCompGraphics       = 0xf20011,

        bringBackLostItems     = 0xf20012,

        zoomIn                 = 0xf20013,
        zoomOut                = 0xf20014,
        zoomNormal             = 0xf20015,
        spaceBarDrag           = 0xf20016,

        compOverlay0           = 0xf20020,
        compOverlay33          = 0xf20021,
        compOverlay66          = 0xf20022,
        compOverlay100         = 0xf20023,

        newDocumentBase        = 0xf32001,
        newComponentBase       = 0xf30001,
        newElementBase         = 0xf31001,

        alignTop               = 0xf33000,
        alignRight             = 0xf33001,
        alignBottom            = 0xf33002,
        alignLeft              = 0xf33003,
    };
}
