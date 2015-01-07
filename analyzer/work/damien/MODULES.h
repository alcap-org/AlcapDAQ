extern ANA_MODULE MUnCompressRawData_module;
extern ANA_MODULE MOctalFADCProcessRaw_module;
extern ANA_MODULE MV1724ProcessRaw_module;
extern ANA_MODULE MDT5720ProcessRaw_module;
extern ANA_MODULE MTreeOutput_module;
extern ANA_MODULE MVacuumHisto_module;

ANA_MODULE *trigger_module[] = {
  &MUnCompressRawData_module,
  &MOctalFADCProcessRaw_module,
  &MV1724ProcessRaw_module,
  &MDT5720ProcessRaw_module,
  &MTreeOutput_module,
  &MVacuumHisto_module,
NULL };
