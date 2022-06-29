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
    meanValues.clear();

    if (count == 32)
    {
        for (int i = 0; i < 384; i++)
        {
            channelGroups.add((i / 2) % 12);
        }

        meanValues.insertMultiple(0, 0.0f, 12);
    }
    else if (count == 24)
    {
        for (int i = 0; i < 384; i++)
        {
            channelGroups.add((i / 2) % 16);
        }

        meanValues.insertMultiple(0, 0.0f, 16);
    }

}

NeuropixelsCAR::NeuropixelsCAR()
    : GenericProcessor("Neuropixels CAR")
{

}


NeuropixelsCAR::~NeuropixelsCAR()
{

}


AudioProcessorEditor* NeuropixelsCAR::createEditor()
{
    editor = std::make_unique<NeuropixelsCAREditor>(this);
    return editor.get();
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
                std::cout << stream->device->getName() << std::endl;
                std::cout << "Num adcs: " << num_adcs << std::endl;

                settings[stream->getStreamId()]->setNumAdcs(num_adcs);
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
            NeuropixelsCARSettings* streamSettings = settings[stream->getStreamId()];

            if (streamSettings->numAdcs > 0)
            {

                const uint16 streamId = stream->getStreamId();
                const uint32 numSamples = getNumSamplesInBlock(streamId);

                for (int i = 0; i < numSamples; i++)
                {
                    streamSettings->meanValues.fill(0.0f);

                    // Sum sample values for each group
                    for (int ch = 0; ch < stream->getChannelCount(); ch++)
                    {
                        if (ch < 384)
                        {
                            int group = streamSettings->channelGroups[ch];
                            int globalChannelIndex = getGlobalChannelIndex(stream->getStreamId(), ch);

                            float currentValue = streamSettings->meanValues[group];

                            streamSettings->meanValues.set(group,
                                currentValue + *buffer.getReadPointer(globalChannelIndex, i));
                        }
                    }

                    // Calculate the mean sample value
                    for (int group = 0; group < streamSettings->meanValues.size(); group++)
                    {
                        float mean = streamSettings->meanValues[group] / streamSettings->numAdcs;
                        streamSettings->meanValues.set(group, mean);
                    }

                    // Subtract the mean sample value by group
                    for (int ch = 0; ch < stream->getChannelCount(); ch++)
                    {
                        if (ch < 384)
                        {
                            int group = streamSettings->channelGroups[ch];
                            int globalChannelIndex = getGlobalChannelIndex(stream->getStreamId(), ch);

                            float meanValue = streamSettings->meanValues[group];

                            float* writePointer = buffer.getWritePointer(globalChannelIndex, i);
                            *writePointer = *writePointer - meanValue;
                        }
                    }
                }

            }
        }
    }

}

