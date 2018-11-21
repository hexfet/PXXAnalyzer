#ifndef PXX_ANALYZER_H
#define PXX_ANALYZER_H

#include <Analyzer.h>
#include "PXXAnalyzerResults.h"
#include "PXXSimulationDataGenerator.h"

class PXXAnalyzerSettings;
class ANALYZER_EXPORT PXXAnalyzer : public Analyzer2
{
public:
	PXXAnalyzer();
	virtual ~PXXAnalyzer();

	virtual void SetupResults();
	virtual void WorkerThread();

	virtual U32 GenerateSimulationData( U64 newest_sample_requested, U32 sample_rate, SimulationChannelDescriptor** simulation_channels );
	virtual U32 GetMinimumSampleRateHz();

	virtual const char* GetAnalyzerName() const;
	virtual bool NeedsRerun();

protected: //vars
	std::auto_ptr< PXXAnalyzerSettings > mSettings;
	std::auto_ptr< PXXAnalyzerResults > mResults;
	AnalyzerChannelData* mPXX;

	PXXSimulationDataGenerator mSimulationDataGenerator;
	bool mSimulationInitilized;

	//PXX analysis vars:
	U32 mSampleRateHz;
	U32 mStartOfStopBitOffset;
	U32 mEndOfStopBitOffset;

private:
  void newFrame(U64 data, U64 starting_sample);
};

extern "C" ANALYZER_EXPORT const char* __cdecl GetAnalyzerName();
extern "C" ANALYZER_EXPORT Analyzer* __cdecl CreateAnalyzer( );
extern "C" ANALYZER_EXPORT void __cdecl DestroyAnalyzer( Analyzer* analyzer );

#endif //PXX_ANALYZER_H
