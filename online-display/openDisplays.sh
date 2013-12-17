#! /bin/bash

OnlineDisplayDir="$HOME/AlcapDAQ/online-display"

"$OnlineDisplayDir"/online-display &
#"$OnlineDisplayDir"/online-display -M MODULES_time_diff_PP &
"$OnlineDisplayDir"/online-display -M MODULES_time_diff &

