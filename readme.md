# PXX Protocol Analyzer
A Salae Logic analyzer plugin for decoding Frsky PXX protocol data.

To use the PXX Analyzer in Windows download the [appropriate 32 or 64 bit DLL here](https://github.com/hexfet/PXXAnalyzer/tree/master/bin).  Add the file to the default Salae Logic analyzers directory (likely C:\Program Files\Saleae Inc\Analyzers).  Start Logic and PXX should appear in the analyzers list.

The analyzer decodes the PXX byte values. Sampled bits are marked with a dot, while bits ignored due to bit stuffing are marked with X.  Packets are delineated by flag bytes, so a packet ID is output in the export files.

For linux or Mac OS please build from source. See the directions at [https://github.com/saleae/AnalyzerSDK](https://github.com/saleae/AnalyzerSDK)


