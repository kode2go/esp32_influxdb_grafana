# esp32_influxdb_grafana

# Install Grafana

## Setup

`sudo systemctl start influxdb`

```
bbarsch@ubuntu-focal-1:~$ influx
Connected to http://localhost:8086 version 1.8.10
InfluxDB shell version: 1.8.10
> CREATE DATABASE esp02
> SHOW DATABASES
name: databases
name
----
_internal
esp02
> USE esp02
Using database esp02
> INSERT data value=42
> SHOW MEASUREMENTS
name: measurements
name
----
data
> ^C
> exit
```
