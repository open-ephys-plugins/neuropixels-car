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

    desiredWidth = 170;

    probeTypeLabel = std::make_unique<Label>("Probe Type", "Probe Type");
    probeTypeLabel->setFont (FontOptions ("Inter", "Regular", 16.0f));
    probeTypeLabel->setJustificationType(Justification::centred);
    probeTypeLabel->setBounds(10, 30, 150, 30);
    addAndMakeVisible(probeTypeLabel.get());

    addMaskChannelsParameterEditor(Parameter::STREAM_SCOPE, "Channels", 35, 65);
    auto maskChannelsEditor = getParameterEditor("Channels");
    maskChannelsEditor->setSize(100, 40);
    maskChannelsEditor->setLayout(ParameterEditor::Layout::nameOnTop);
}


void NeuropixelsCAREditor::selectedStreamHasChanged()
{
    NeuropixelsCAR* processor = (NeuropixelsCAR*) getProcessor();

    probeTypeLabel->setText(processor->getDeviceName(getCurrentStream()), dontSendNotification);
}

