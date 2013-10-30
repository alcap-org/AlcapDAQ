while ((1))
do
#nc -l -p 10000 >> /dev/null && echo -e "*RALL*\nHVP 190\nVAC 6.7E-6\nOK" | nc localhost 10001
#nc -l -p 10000 && echo -e "*RALL*\nHVP 190\nVAC 6.7E-6\nOVC" | nc localhost 10001
echo -e "*RALL*\nHVP 190 5.2E-6\nVAC 6.7E-6\nOVC" | nc localhost 10001

done
