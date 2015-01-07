extern ANA_MODULE MUnCompressRawData_module;
extern ANA_MODULE MOctalFADCProcessRaw_module;
extern ANA_MODULE MV1724ProcessRaw_module;
extern ANA_MODULE MDT5720ProcessRaw_module;
extern ANA_MODULE MCaenCompProcessRaw_module;
extern ANA_MODULE MMuPC1AnalysisC_module;
extern ANA_MODULE MMuPC1AnalysisMQL_module;
extern ANA_MODULE MMuSCAnalysisMQL_module;
extern ANA_MODULE MCommonOnlineDisplayPlots_module;
extern ANA_MODULE MVacuumHisto_module;
extern ANA_MODULE MOctalFADCBufferOverflow_module;
extern ANA_MODULE MOctalFADCPacketLoss_module;
extern ANA_MODULE MExpectedIslands_module;
extern ANA_MODULE MMuSCTimeDifferences_module;
extern ANA_MODULE MNumberIslands_module;
extern ANA_MODULE MPulseLengths_module;
extern ANA_MODULE MTreeOutput_module;

ANA_MODULE *trigger_module[] = {
  &MUnCompressRawData_module,
  &MOctalFADCProcessRaw_module,
  &MV1724ProcessRaw_module,
  &MDT5720ProcessRaw_module,
  &MCaenCompProcessRaw_module,
  &MMuPC1AnalysisC_module,
  &MMuPC1AnalysisMQL_module,
  &MMuSCAnalysisMQL_module,
  &MCommonOnlineDisplayPlots_module,
  &MVacuumHisto_module,
  &MOctalFADCBufferOverflow_module,
  &MOctalFADCPacketLoss_module,
  &MExpectedIslands_module,
  &MMuSCTimeDifferences_module,
  &MNumberIslands_module,
  &MPulseLengths_module,
  &MTreeOutput_module,
NULL };
