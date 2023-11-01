# esp32_influxdb_grafana

## v1

## v2

## v3

Get state from influxdb so I can control the pin on and off remotely

# Install & Setup Grafana

See: https://github.com/kode2go/grafana

# Install & Setup Influx DB

See: https://github.com/kode2go/influxdb/blob/main/README.md

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

## Setup Nginx

`sudo apt-get install nginx`

`sudo nano /etc/nginx/sites-available/influxdb`

`sudo ln -s /etc/nginx/sites-available/influxdb /etc/nginx/sites-enabled/`

`sudo nginx -t`

`sudo systemctl reload nginx`

Assuming grafana is installed

```
server {
    listen 80;
    server_name 34.125.194.47;

    location / {
        proxy_pass http://localhost:3000;  # Grafana is running on port 3000
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
    }

    location /influxdb/ {
        proxy_pass http://localhost:8086/;  # InfluxDB is running on port 8086
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
        proxy_set_header X-Real-Path /influxdb/;  # Add this line
    }
}
```

## Testing

On local machine:

`curl -i -XPOST 'http://localhost:8086/write?db=esp02' --data-binary 'data value=20'`

```
curl -i -XPOST 'http://localhost:8086/query?db=esp02' --data-binary "q=SELECT LAST(*) FROM data"
HTTP/1.1 200 OK
Content-Type: application/json
Request-Id: e4b47267-786d-11ee-802e-000000000000
X-Influxdb-Build: OSS
X-Influxdb-Version: 1.6.4
X-Request-Id: e4b47267-786d-11ee-802e-000000000000
Date: Wed, 01 Nov 2023 04:19:47 GMT
Transfer-Encoding: chunked

{"results":[{"statement_id":0,"series":[{"name":"data","columns":["time","last_value"],"values":[["2023-10-31T16:37:22.9208273Z",21]]}]}]}
```

```
curl -i -XPOST 'http://localhost:8086/query?db=esp02' --data-binary "q=SELECT LAST(*) FROM data" > results.txt
```

Must install jq on Ubuntu:

```
cat result.txt | jq -r '.results[0].series[0].values[0][1]'
21
```

### Create/Insere Measurement/Data

```
> INSERT switch state=1
> SELECT * FROM switch
name: switch
time                state
----                -----
1698817760089185500 1
> INSERT switch state=0
> SELECT * FROM switch
name: switch
time                state
----                -----
1698817760089185500 1
1698817777604417800 0
>
```

On external machine:

`curl -i -XPOST 'http://34.125.194.47/influxdb/write?db=esp02' --data-binary 'data value=50'`

output:

```
HTTP/1.1 204 No Content
Server: nginx/1.18.0 (Ubuntu)
Date: Mon, 16 Oct 2023 07:53:09 GMT
Content-Type: application/json
Connection: keep-alive
Request-Id: 0c3cade5-6bf9-11ee-804f-42010ab60002
X-Influxdb-Build: OSS
X-Influxdb-Version: 1.8.10
X-Request-Id: 0c3cade5-6bf9-11ee-804f-42010ab60002
```
