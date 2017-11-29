#ifndef AlCapConstants_h_
#define AlCapConstants_h_

const int n_SiT_channels = 4;
std::string all_SiT_channel_names[n_SiT_channels] = {"SiT_1", "SiT_2", "SiT_3", "SiT_4"};

const int n_SiL1_channels = 14; // ignoring SiL1_1 and SiL1_16 for the time being
std::string all_SiL1_channel_names[n_SiL1_channels] = {/*"SiL1_1",*/ "SiL1_2", "SiL1_3", "SiL1_4", "SiL1_5", "SiL1_6", "SiL1_7", "SiL1_8", "SiL1_9", "SiL1_10", "SiL1_11", "SiL1_12", "SiL1_13", "SiL1_14", "SiL1_15"/*, "SiL1_16"*/};

const int n_SiL3_channels = 1;
std::string all_SiL3_channel_names[n_SiL3_channels] = {"SiL3"};

const int n_SiR1_channels = 4;
std::string all_SiR1_channel_names[n_SiR1_channels] = {"SiR1_1", "SiR1_2", "SiR1_3", "SiR1_4"};

const int n_SiR2_channels = 1;
std::string all_SiR2_channel_names[n_SiR2_channels] = {"SiR2"};

const int n_SiR3_channels = 1;
std::string all_SiR3_channel_names[n_SiR3_channels] = {"SiR3"};

const int n_GeLoGain_channels = 1;
std::string all_GeLoGain_channel_names[n_GeLoGain_channels] = {"GeLoGain"};

const int n_GeHiGain_channels = 1;
std::string all_GeHiGain_channel_names[n_GeHiGain_channels] = {"GeHiGain"};

const int n_all_channels = n_SiT_channels + n_SiL1_channels + n_SiL3_channels + n_SiR1_channels + n_SiR2_channels + n_SiR3_channels + n_GeLoGain_channels + n_GeHiGain_channels;
std::string all_channel_names[n_all_channels];

#endif
