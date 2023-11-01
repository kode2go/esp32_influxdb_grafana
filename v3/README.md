# About

After getting this to work locally `curl -i -XPOST 'http://localhost:8086/query?db=esp02' --data-binary "q=SELECT LAST(*) FROM data"`

Decided to try out on influxdb cloud and query the data which seemd to work.

If I can query then I can also most likely write, so in that case I would write to a new measurement called "switch" and state = 1/0
So that would allow me to control the geyser if it is on or off remotely.

Sine the esp32 would publish the power reading and now read the influxdb state value, independent of power value. State would depend on
battery SOC, time of day which is linked to power output. Eventually, would like to extract these values from sunsynk api.

esp32:
- publish power to influxdb
- read state from influxdb

cloud:
- influxdb
- grafana
- nginx
- static query page to switch on/off pin15

# HTML Query

![image](https://github.com/kode2go/esp32_influxdb_grafana/assets/29664888/42859b5d-ee9f-4577-98e6-120c7919b1a9)
