#ifndef PXX_SIMULATION_DATA_GENERATOR
#define PXX_SIMULATION_DATA_GENERATOR

#include <SimulationChannelDescriptor.h>
#include <string>
class PXXAnalyzerSettings;

class PXXSimulationDataGenerator
{
public:
	PXXSimulationDataGenerator();
	~PXXSimulationDataGenerator();

	void Initialize( U32 simulation_sample_rate, PXXAnalyzerSettings* settings );
	U32 GenerateSimulationData( U64 newest_sample_requested, U32 sample_rate, SimulationChannelDescriptor** simulation_channel );

protected:
	PXXAnalyzerSettings* mSettings;
	U32 mSimulationSampleRateHz;

protected:
	U8 CreatePXXByte(U8 ones_count);
  U8 mPXXData[20];
	U32 mStringIndex;

	SimulationChannelDescriptor mPXXSimulationData;

};
#endif //PXX_SIMULATION_DATA_GENERATOR