#include "PXXAnalyzer.h"
#include "PXXAnalyzerSettings.h"
#include <AnalyzerChannelData.h>

PXXAnalyzer::PXXAnalyzer()
:	Analyzer2(),  
	mSettings( new PXXAnalyzerSettings() ),
	mSimulationInitilized( false ),
  mPXX( NULL ),
  mSampleRateHz( 0 )
{
	SetAnalyzerSettings( mSettings.get() );
}

PXXAnalyzer::~PXXAnalyzer()
{
	KillThread();
}

void PXXAnalyzer::SetupResults()
{
	mResults.reset( new PXXAnalyzerResults( this, mSettings.get() ) );
	SetAnalyzerResults( mResults.get() );
	mResults->AddChannelBubblesWillAppearOn( mSettings->mInputChannel );
}

void PXXAnalyzer::newFrame(U64 data, U64 starting_sample) {
  Frame frame;
  frame.mData1 = data;
  frame.mFlags = 0;
  if (data == 0x7e) {
    frame.mType = 1;
    mResults->CommitPacketAndStartNewPacket();
  }
  frame.mStartingSampleInclusive = starting_sample;
  frame.mEndingSampleInclusive = mPXX->GetSampleNumber();

  mResults->AddFrame(frame);
  mResults->CommitResults();
  ReportProgress(frame.mEndingSampleInclusive);
}

void PXXAnalyzer::WorkerThread()
{
	mSampleRateHz = GetSampleRate();
  U32 samples_per_third = mSampleRateHz / 1000000 * 8;

  mPXX = GetAnalyzerChannelData( mSettings->mInputChannel );

	if( mPXX->GetBitState() == BIT_LOW )
		mPXX->AdvanceToNextEdge();

  U8 data = 0, bit_num = 7, ones_count = 0;
  U64 starting_sample = 1;
  U64 first_edge, second_edge = 0;
  U64 bit_count = 0;
	for( ; ; )
	{
    mPXX->AdvanceToNextEdge(); //falling edge -- beginning of a bit
    first_edge = mPXX->GetSampleNumber();
    // restart on unexpected timing
    if (second_edge && ((first_edge - second_edge) > (3 * (U64) samples_per_third))) {
      data = 0;
      bit_num = 7;
    }
    if (bit_num == 7) starting_sample = first_edge;
    mPXX->AdvanceToNextEdge();
    second_edge = mPXX->GetSampleNumber();
    U64 diff_edge = second_edge - first_edge;

    // basic glitch detector
    if ((diff_edge < ((U64) samples_per_third / 2)) || (diff_edge > (3 * (U64) samples_per_third))) {
      data = 0;
      bit_num = 7;
      continue;
    }

    bit_count++;

    if (diff_edge > samples_per_third) {
      // received 1
      data |= (1 << bit_num);
      ones_count += 1;
      ones_count %= 6;  // ignore 6 one bits - will recognize as 0x7e
    } else {
      // received 0
      if (ones_count == 5) {
        ones_count = 0;  // skip stuffed zero and mark with square
        mResults->AddMarker(second_edge, AnalyzerResults::X, mSettings->mInputChannel);
        continue;
      }
      ones_count = 0;
    }
    // put a dot where bit sampled
    mResults->AddMarker(second_edge, AnalyzerResults::Dot, mSettings->mInputChannel);

    if (bit_num == 0) {
      newFrame(data, starting_sample);
      //TODO      newFrame(bit_count, starting_sample);
      data = 0;
      bit_num = 7;
    } else {
      bit_num -= 1;
    }
	}
}

bool PXXAnalyzer::NeedsRerun()
{
	return false;
}

U32 PXXAnalyzer::GenerateSimulationData( U64 minimum_sample_index, U32 device_sample_rate, SimulationChannelDescriptor** simulation_channels )
{
	if( mSimulationInitilized == false )
	{
		mSimulationDataGenerator.Initialize( GetSimulationSampleRate(), mSettings.get() );
		mSimulationInitilized = true;
	}

	return mSimulationDataGenerator.GenerateSimulationData( minimum_sample_index, device_sample_rate, simulation_channels );
}

U32 PXXAnalyzer::GetMinimumSampleRateHz()
{
  return 1000000;
}

const char* PXXAnalyzer::GetAnalyzerName() const
{
	return "PXX Protocol";
}

const char* GetAnalyzerName()
{
	return "PXX Protocol";
}

Analyzer* CreateAnalyzer()
{
	return new PXXAnalyzer();
}

void DestroyAnalyzer( Analyzer* analyzer )
{
	delete analyzer;
}