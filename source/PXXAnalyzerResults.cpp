#include "PXXAnalyzerResults.h"
#include <AnalyzerHelpers.h>
#include "PXXAnalyzer.h"
#include "PXXAnalyzerSettings.h"
#include <iostream>
#include <fstream>

PXXAnalyzerResults::PXXAnalyzerResults( PXXAnalyzer* analyzer, PXXAnalyzerSettings* settings )
:	AnalyzerResults(),
	mSettings( settings ),
	mAnalyzer( analyzer )
{
}

PXXAnalyzerResults::~PXXAnalyzerResults()
{
}

void PXXAnalyzerResults::GenerateBubbleText( U64 frame_index, Channel& channel, DisplayBase display_base )
{
	ClearResultStrings();
	Frame frame = GetFrame( frame_index );

	char number_str[128];
	AnalyzerHelpers::GetNumberString( frame.mData1, display_base, 8, number_str, 128 );
  AddResultString(number_str);
  if (frame.mType == 1) strcat(number_str, " (START)");
  if (frame.mType == 2) strcat(number_str, frame.mFlags ? " (CRC ERR)" : " (END)");
  AddResultString(number_str);
  if (frame.mData2) sprintf(&number_str[strlen(number_str)], " %04X", frame.mData2);
	AddResultString( number_str );
}

void PXXAnalyzerResults::GenerateExportFile( const char* file, DisplayBase display_base, U32 export_type_user_id )
{
	std::ofstream file_stream( file, std::ios::out );

	U64 trigger_sample = mAnalyzer->GetTriggerSample();
	U32 sample_rate = mAnalyzer->GetSampleRate();

	file_stream << "Time [s],Packet ID,Value" << std::endl;

	U64 num_frames = GetNumFrames();
  U32 i=0;
  U64 current_packet = 0;
	while( i < num_frames )
	{
		Frame frame = GetFrame( i );
		
		char time_str[128];
		AnalyzerHelpers::GetTimeString( frame.mStartingSampleInclusive, trigger_sample, sample_rate, time_str, 128 );

    U64 packet_id = GetPacketContainingFrameSequential(i);
    if (packet_id != INVALID_RESULT_INDEX)
      file_stream << time_str << "," << packet_id << ",";
    else
      file_stream << time_str << ",,";  //it's ok for a frame not to be included in a packet.

		char number_str[128];
		AnalyzerHelpers::GetNumberString( frame.mData1, display_base, 8, number_str, 128 );
    file_stream << number_str;
    if (frame.mData2) {
      AnalyzerHelpers::GetNumberString(frame.mData2, display_base, 16, number_str, 128);
      file_stream << "," << number_str;
    }
    file_stream << std::endl;

		if( UpdateExportProgressAndCheckForCancel( i, num_frames ) == true )
		{
			file_stream.close();
			return;
		}
	}

	file_stream.close();
}

void PXXAnalyzerResults::GenerateFrameTabularText( U64 frame_index, DisplayBase display_base )
{
#ifdef SUPPORTS_PROTOCOL_SEARCH
	Frame frame = GetFrame( frame_index );
	ClearTabularText();

	char number_str[128];
	AnalyzerHelpers::GetNumberString( frame.mData1, display_base, 8, number_str, 128 );
	AddTabularText( number_str );
#endif
}

void PXXAnalyzerResults::GeneratePacketTabularText( U64 packet_id, DisplayBase display_base )
{
	//not supported

}

void PXXAnalyzerResults::GenerateTransactionTabularText( U64 transaction_id, DisplayBase display_base )
{
	//not supported
}