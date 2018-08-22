set terminal svg size 800, 600
set output 'temperature.svg'

plot 'log.csv' using 0:2 title 'BMP180 A Temperature', \
     'log.csv' using 0:4 title 'BMP180 B Temperature', \
     'log.csv' using 0:6 title 'BME280 A Temperature', \
     'log.csv' using 0:9 title 'BME280 B Temperature', 

set terminal svg size 800, 600
set output 'pressure.svg'

plot 'log.csv' using 0:3 title 'BMP180 A Pressure', \
     'log.csv' using 0:5 title 'BMP180 B Pressure', \
     'log.csv' using 0:7 title 'BME280 A Pressure', \
     'log.csv' using 0:10 title 'BME280 B Pressure'

set terminal svg size 800, 600
set output 'humidity.svg'

plot 'log.csv' using 0:8  title 'BME280 A Humidity', \
     'log.csv' using 0:11 title 'BME280 B Humidity'
