#ifndef PXX_ANALYZER_SETTINGS
#define PXX_ANALYZER_SETTINGS

#include <AnalyzerSettings.h>
#include <AnalyzerTypes.h>

class PXXAnalyzerSettings : public AnalyzerSettings
{
public:
	PXXAnalyzerSettings();
	virtual ~PXXAnalyzerSettings();

	virtual bool SetSettingsFromInterfaces();
	void UpdateInterfacesFromSettings();
	virtual void LoadSettings( const char* settings );
	virtual const char* SaveSettings();

	
	Channel mInputChannel;

protected:
	std::auto_ptr< AnalyzerSettingInterfaceChannel >	mInputChannelInterface;
};

#endif //PXX_ANALYZER_SETTINGS
