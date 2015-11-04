#!/bin/bash
# Generate the HTML for the webpage we use for editing the bank map

cat R15b_BankMapHeader.txt

N_CHN=100;
i=0

while [ $i -lt $N_CHN ]; do

    echo "<tr>"
    echo "<td colspan=\"2\" align=\"center\"><odb src=\"/Analyzer/WireMap/DetectorName[$i]\" edit=1></td>"
    echo "<td colspan=\"2\" align=\"center\"><odb src=\"/Analyzer/WireMap/BankName[$i]\" edit=0></td>"
    echo "<td colspan=\"2\" align=\"center\"><odb src=\"/Analyzer/WireMap/SamplingFrequency[$i]\" edit=0></td>"
    echo "<td colspan=\"2\" align=\"center\"><odb src=\"/Analyzer/WireMap/TimeShift[$i]\" edit=1></td>"
    echo "<td colspan=\"2\" align=\"center\"><odb src=\"/Analyzer/WireMap/TriggerPolarity[$i]\" edit=1></td>"
    echo "<td colspan=\"2\" align=\"center\"><odb src=\"/Analyzer/WireMap/Pedestal[$i]\" edit=1></td>"
    echo "<td colspan=\"2\" align=\"center\"><odb src=\"/Analyzer/WireMap/Enabled[$i]\" edit=1></td>"
    echo "</tr>"

    let i=$i+1
done

echo "</table>"
echo "</form>"
echo "</body>"
echo "</html>"
