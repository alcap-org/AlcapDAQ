#ifndef AlCapConstants_h_
#define AlCapConstants_h_

const int n_SiT_channels = 4;
std::string all_SiT_channel_names[n_SiT_channels] = {"SiT-1", "SiT-2", "SiT-3", "SiT-4"};

const int n_SiL1_channels = 14; // ignoring SiL1-1 and SiL1-16 for the time being
std::string all_SiL1_channel_names[n_SiL1_channels] = {/*"SiL1-1",*/ "SiL1-2", "SiL1-3", "SiL1-4", "SiL1-5", "SiL1-6", "SiL1-7", "SiL1-8", "SiL1-9", "SiL1-10", "SiL1-11", "SiL1-12", "SiL1-13", "SiL1-14", "SiL1-15"/*, "SiL1-16"*/};

const int n_SiL3_channels = 1;
std::string all_SiL3_channel_names[n_SiL3_channels] = {"SiL3"};

const int n_SiR1_channels = 4;
std::string all_SiR1_channel_names[n_SiR1_channels] = {"SiR1-1", "SiR1-2", "SiR1-3", "SiR1-4"};

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
