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

#include "NeuropixelsCAR.h"

#include "NeuropixelsCAREditor.h"


void NeuropixelsCARSettings::setNumAdcs(int count)
{
    channelGroups.clear();
    channelCounts.clear();

    numAdcs = count;

    if (numAdcs == 32)
    {
        for (int i = 0; i < 384; i++)
        {
            channelGroups.add((i / 2) % 12);
        }

        channelCounts.insertMultiple(0, 0, 12);
        buffer.setSize(12, 10000);
    }
    else if (numAdcs == 24)
    {
        for (int i = 0; i < 384; i++)
        {
            channelGroups.add((i / 2) % 16);
        }

        channelCounts.insertMultiple(0, 0, 16);
        buffer.setSize(16, 10000);
    }

}

void NeuropixelsCARSettings::resetCounts()
{
    for (int i = 0; i < channelCounts.size(); i++)
    {
        channelCounts.set(i, 0);
    }
}

NeuropixelsCAR::NeuropixelsCAR()
    : GenericProcessor("Neuropixels CAR")
{
    addMaskChannelsParameter(Parameter::STREAM_SCOPE, "Channels", "Channels to use for this stream");
}


NeuropixelsCAR::~NeuropixelsCAR()
{

}


AudioProcessorEditor* NeuropixelsCAR::createEditor()
{
    editor = std::make_unique<NeuropixelsCAREditor>(this);
    return editor.get();
}

String NeuropixelsCAR::getDeviceName(uint16 streamId)
{
    if (streamId > 0)
        return settings[streamId]->name;
    else
        return "No device detected.";
}

void NeuropixelsCAR::updateSettings()
{

    settings.update(getDataStreams());

    for (auto stream : dataStreams)
    {
        if (stream->device != nullptr)
        {
            int adcMetadataIndex = stream->device->findMetadata(
                MetadataDescriptor::MetadataType::UINT16,
                1,
                "neuropixels.adcs");
            
            if (adcMetadataIndex > -1)
            {
                const MetadataValue* value = stream->device->getMetadataValue(adcMetadataIndex);
                uint16 num_adcs;
                value->getValue(&num_adcs);

                settings[stream->getStreamId()]->setNumAdcs(num_adcs);
                settings[stream->getStreamId()]->name = stream->device->getName();
            }
            else {
                settings[stream->getStreamId()]->name = "No Neuropixels detected.";
            }
        }
    }

}


void NeuropixelsCAR::process(AudioBuffer<float>& buffer)
{

    for (auto stream : getDataStreams())
    {

        if ((*stream)["enable_stream"])
        {

            const uint16 streamId = stream->getStreamId();

            NeuropixelsCARSettings* streamSettings = settings[streamId];

            if (streamSettings->numAdcs > 0)
            {

                const uint32 numSamples = getNumSamplesInBlock(streamId);

                if (numSamples > 0)
                {

                    streamSettings->buffer.clear();
                    streamSettings->resetCounts();

                    // Sum sample values for each group
                    for (auto localChannelIndex : *((*stream)["Channels"].getArray()))
                    {
                        int ch = int(localChannelIndex);

                        if (ch < 384)
                        {
                            int group = streamSettings->channelGroups[ch];
                            int globalChannelIndex = getGlobalChannelIndex(streamId, ch);

                            streamSettings->buffer.addFrom(group,
                                0,
                                buffer.getReadPointer(globalChannelIndex, 0),
                                numSamples);

                            streamSettings->channelCounts.set(group, streamSettings->channelCounts[group] + 1.0f);
                        }
                    }

                    // Calculate the mean sample values
                    for (int group = 0; group < streamSettings->buffer.getNumChannels(); group++)
                    {
                        streamSettings->buffer.applyGain(group, 0, numSamples, 1.0f / streamSettings->channelCounts[group]);
                    }

                    // Subtract the mean sample value by group
                    for (auto localChannelIndex : *((*stream)["Channels"].getArray()))
                    {
                        
                        int ch = int(localChannelIndex);

                        if (ch < 384)
                        {
                            int group = streamSettings->channelGroups[ch];
                            int globalChannelIndex = getGlobalChannelIndex(streamId, ch);

                            buffer.addFrom(globalChannelIndex, 0, streamSettings->buffer.getReadPointer(group), numSamples, -1.0f);
                        }
                    }
                }
            }
        }
    }

}

