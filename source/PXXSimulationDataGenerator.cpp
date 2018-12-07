#include "PXXSimulationDataGenerator.h"
#include "PXXAnalyzerSettings.h"

#include <AnalyzerHelpers.h>

PXXSimulationDataGenerator::PXXSimulationDataGenerator()
: mPXXData{ 0x7E,0x0A,0x00,0x00,0xF9,0x73,0x3F,0x2D,0xB1,0x40,
            0xEF,0x06,0x40,0x00,0x74,0x3F,0x18,0x12,0x7E,0x7E },
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

  U8 ones_count = 0;
  adjusted_largest_sample_requested -= 10000;
	while( mPXXSimulationData.GetCurrentSampleNumber() < adjusted_largest_sample_requested )
	{
		ones_count = CreatePXXByte(ones_count);
	}

	*simulation_channel = &mPXXSimulationData;
	return 1;
}

U8 PXXSimulationDataGenerator::CreatePXXByte(U8 ones_count)
{
	U32 samples_per_third = mSimulationSampleRateHz / 1000000 * 8;

  U8 byte = mPXXData[ mStringIndex ];
	U8 mask = 1 << 7;
	for( U32 i=0; i<8; i++ )
	{
    mPXXSimulationData.Advance(samples_per_third);  // start third - 8us high
    mPXXSimulationData.Transition();
    mPXXSimulationData.Advance(samples_per_third);  // always 8us low
    if (byte & mask) {
      mPXXSimulationData.Advance(samples_per_third);  // another 8us low for a 1
      if (++ones_count == 5) {  // stuff a 0
        mPXXSimulationData.Transition();
        mPXXSimulationData.Advance(samples_per_third);  // start third - 8us high
        mPXXSimulationData.Transition();
        mPXXSimulationData.Advance(samples_per_third);  // always 8us low
        ones_count = 0;
      }
    }
    else {
      ones_count = 0;
    }

// THIS REQUIRED STATEMENT CAUSES CRASH, DON'T KNOW WHY
    //mPXXSimulationData.Transition();

    mask = mask >> 1;
	}

  mStringIndex++;
  if (mStringIndex == 20) {
    mStringIndex = 0;
    mPXXSimulationData.Advance(50 * samples_per_third);
  }

	mPXXSimulationData.TransitionIfNeeded( BIT_HIGH );

  return ones_count;
}
