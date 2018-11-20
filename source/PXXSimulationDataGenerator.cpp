#include "PXXSimulationDataGenerator.h"
#include "PXXAnalyzerSettings.h"

#include <AnalyzerHelpers.h>

PXXSimulationDataGenerator::PXXSimulationDataGenerator()
:	mSerialText( "PXX Protocol simulation data generator" ),
	mStringIndex( 0 )
{
}

PXXSimulationDataGenerator::~PXXSimulationDataGenerator()
{
}

void PXXSimulationDataGenerator::Initialize( U32 simulation_sample_rate, PXXAnalyzerSettings* settings )
{
	mSimulationSampleRateHz = simulation_sample_rate;
	mSettings = settings;

	mSerialSimulationData.SetChannel( mSettings->mInputChannel );
	mSerialSimulationData.SetSampleRate( simulation_sample_rate );
	mSerialSimulationData.SetInitialBitState( BIT_HIGH );
}

U32 PXXSimulationDataGenerator::GenerateSimulationData( U64 largest_sample_requested, U32 sample_rate, SimulationChannelDescriptor** simulation_channel )
{
	U64 adjusted_largest_sample_requested = AnalyzerHelpers::AdjustSimulationTargetSample( largest_sample_requested, sample_rate, mSimulationSampleRateHz );

	while( mSerialSimulationData.GetCurrentSampleNumber() < adjusted_largest_sample_requested )
	{
		CreateSerialByte();
	}

	*simulation_channel = &mSerialSimulationData;
	return 1;
}

void PXXSimulationDataGenerator::CreateSerialByte()
{
	U32 samples_per_third = mSimulationSampleRateHz / 125000;

	U8 byte = mSerialText[ mStringIndex ];
	mStringIndex++;
	if( mStringIndex == mSerialText.size() )
		mStringIndex = 0;

	U8 mask = 0x1 << 7;
	for( U32 i=0; i<8; i++ )
	{
    //we're currenty high
    //let's move forward a little
    mSerialSimulationData.Advance(samples_per_third);  // start third - 8us high
    mSerialSimulationData.Transition();
    mSerialSimulationData.Advance(samples_per_third);  // always 8us low
		if( byte & mask ) mSerialSimulationData.Advance( samples_per_third );  // another 8us low for a 1
    mSerialSimulationData.Transition();
    mask = mask >> 1;
	}

	mSerialSimulationData.TransitionIfNeeded( BIT_HIGH ); //we need to end high

	//lets pad the end a bit for the stop bit:
	mSerialSimulationData.Advance( samples_per_third );
}
