#include "PXXSimulationDataGenerator.h"
#include "PXXAnalyzerSettings.h"

#include <AnalyzerHelpers.h>

PXXSimulationDataGenerator::PXXSimulationDataGenerator()
:	mPXXText( "PXX Protocol simulation data generator" ),
	mStringIndex( 0 ),
	mSimulationSampleRateHz( 0 ),
  mSettings( 0 )
{
}

PXXSimulationDataGenerator::~PXXSimulationDataGenerator()
{
}

void PXXSimulationDataGenerator::Initialize( U32 simulation_sample_rate, PXXAnalyzerSettings* settings )
{
	mSimulationSampleRateHz = simulation_sample_rate;
	mSettings = settings;

	mPXXSimulationData.SetChannel( mSettings->mInputChannel );
	mPXXSimulationData.SetSampleRate( simulation_sample_rate );
	mPXXSimulationData.SetInitialBitState( BIT_HIGH );
  mPXXSimulationData.Advance(mSimulationSampleRateHz / 1000000 * 16);
}

U32 PXXSimulationDataGenerator::GenerateSimulationData( U64 largest_sample_requested, U32 sample_rate, SimulationChannelDescriptor** simulation_channel )
{
	U64 adjusted_largest_sample_requested = AnalyzerHelpers::AdjustSimulationTargetSample( largest_sample_requested, sample_rate, mSimulationSampleRateHz );

	while( mPXXSimulationData.GetCurrentSampleNumber() < adjusted_largest_sample_requested )
	{
		CreatePXXByte();
	}

	*simulation_channel = &mPXXSimulationData;
	return 1;
}

void PXXSimulationDataGenerator::CreatePXXByte()
{
	U32 samples_per_third = mSimulationSampleRateHz / 1000000 * 8;

	U8 byte = mPXXText[ mStringIndex ];
	mStringIndex++;
	if( mStringIndex == mPXXText.size() )
		mStringIndex = 0;

	U8 mask = 0x1 << 7;
	for( U32 i=0; i<8; i++ )
	{
    //we're currenty high
    //let's move forward a little
    mPXXSimulationData.Advance(samples_per_third);  // start third - 8us high
    mPXXSimulationData.Transition();
    mPXXSimulationData.Advance(samples_per_third);  // always 8us low
		if( byte & mask ) mPXXSimulationData.Advance( samples_per_third );  // another 8us low for a 1
    mPXXSimulationData.Transition();
    mask = mask >> 1;
	}

	mPXXSimulationData.TransitionIfNeeded( BIT_HIGH ); //we need to end high

	//lets pad the end a bit for the last high period
	mPXXSimulationData.Advance( samples_per_third );
}
