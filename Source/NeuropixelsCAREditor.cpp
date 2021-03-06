/*
------------------------------------------------------------------

This file is part of the Open Ephys GUI
Copyright (C) 2022 Open Ephys

------------------------------------------------------------------

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "NeuropixelsCAREditor.h"

#include "NeuropixelsCAR.h"

NeuropixelsCAREditor::NeuropixelsCAREditor(GenericProcessor* parentNode) 
    : GenericEditor(parentNode)
{

    desiredWidth = 150;

    probeTypeLabel = std::make_unique<Label>("Probe Type", "Probe Type");
    probeTypeLabel->setBounds(20, 20, 110, 70);
    addAndMakeVisible(probeTypeLabel.get());

    addMaskChannelsParameterEditor("Channels", 30, 98);

}


void NeuropixelsCAREditor::selectedStreamHasChanged()
{
    NeuropixelsCAR* processor = (NeuropixelsCAR*) getProcessor();

    probeTypeLabel->setText(processor->getDeviceName(getCurrentStream()), dontSendNotification);
}

