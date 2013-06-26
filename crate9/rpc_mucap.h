#define RPC_EVENT_READY     2201
#define RPC_BUFFER_FREE     2202
#define RPC_END_OF_CYCLE    2203
#define RPC_READY_FOR_CYCLE 2204
#define RPC_REQUEST_STOP    2205

#ifdef DEFINE_RPC_LIST
RPC_LIST rpc_list_mucap[] = {

    // sent from crates to event-builder
    {RPC_EVENT_READY, "rpc_event_ready",
     {{TID_INT, RPC_IN} , // crate_number
      {TID_INT, RPC_IN} , // event_number
      {TID_INT, RPC_IN} , // pci_address
      {TID_INT, RPC_IN} , // event size in bytes
      {0} } },

    // sent from event-builder to crates
    {RPC_BUFFER_FREE, "rpc_buffer_free",
      {{TID_INT, RPC_IN} , // pci_address
       {0} } },

    // sent from "special" crate (crate 3) to "other" crates
    {RPC_END_OF_CYCLE, "rpc_end_of_cycle",
     {{TID_INT, RPC_IN} , // cycle_number
      {TID_INT, RPC_IN} , // event number
      {0} } },

    // sent from "other" crates to "special" crate (crate 3)
    {RPC_READY_FOR_CYCLE, "rpc_ready_for_cycle",
     {{TID_INT, RPC_IN} ,  // crate_number
      {TID_INT, RPC_IN} ,  // cycle_number
      {0} } },

    // sent from "other" crates to "special" crate (crate 3)
    {RPC_REQUEST_STOP, "rpc_request_stop",
     {{TID_INT, RPC_IN} ,  // crate_number
      {TID_INT, RPC_IN} ,  // event_number
      {0} } },

    {0}
};
#else
extern RPC_LIST rpc_list_mucap[];
#endif
