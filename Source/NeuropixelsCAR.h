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

#ifndef NeuropixelsCAR_H_DEFINED
#define NeuropixelsCAR_H_DEFINED

#include <ProcessorHeaders.h>

class NeuropixelsCARSettings
{

public:

	/** Constructor -- sets default values*/
	NeuropixelsCARSettings() : numAdcs(0) { }

	/** Holds the number of ADCs for this probe type*/
	int numAdcs;

	/** Channel groups inds for all channels*/
	Array<int> channelGroups;

	/** Mean values for each sample, for each channel group*/
	AudioBuffer<float> buffer;

	/** Number of channels active in each group*/
	Array<float> channelCounts;

	/** Set number of ADCs and initialize arrays */
	void setNumAdcs(int count);

	/** Reset channel counts to 0*/
	void resetCounts();

	/** Device name*/
	String name;

};

class NeuropixelsCAR : public GenericProcessor
{
public:
	/** The class constructor, used to initialize any members. */
	NeuropixelsCAR();

	/** The class destructor, used to deallocate memory */
	~NeuropixelsCAR();

	/** If the processor has a custom editor, this method must be defined to instantiate it. */
	AudioProcessorEditor* createEditor() override;

	/** Called every time the settings of an upstream plugin are changed.
		Allows the processor to handle variations in the channel configuration or any other parameter
		passed through signal chain. The processor can use this function to modify channel objects that
		will be passed to downstream plugins. */
	void updateSettings() override;

	/** Defines the functionality of the processor.
		The process method is called every time a new data buffer is available.
		Visualizer plugins typically use this method to send data to the canvas for display purposes */
	void process(AudioBuffer<float>& buffer) override;

	/** Returns the device name for a given stream*/
	String getDeviceName(uint16 stream);

private:

	StreamSettings<NeuropixelsCARSettings> settings;
};

#endif