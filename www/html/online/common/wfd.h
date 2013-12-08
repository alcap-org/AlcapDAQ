#ifndef wfd_h
#define wfd_h

#define WFD_STROFFSET           'b'
#define WFD_NUMBER_OF_CRATES     6
#define WFD_NUMBER_OF_SLOTS     19
#define WFD_NUMBER_OF_CHANNELS   4

unsigned char wfd_crate_number   [WFD_NUMBER_OF_CRATES  ] = {1,2,3,4,5,6};
unsigned char wfd_slot_number    [WFD_NUMBER_OF_SLOTS   ] = {2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20};
unsigned char wfd_channel_number [WFD_NUMBER_OF_CHANNELS] = {0,1,2,3};
char         *wfd_crate_name     [WFD_NUMBER_OF_CRATES]   = {"B0","R1","G1","R0","B1","G0"};


#endif /* wfd_h defined */
