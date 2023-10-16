# esp32_influxdb_grafana

# Install & Setup Grafana

# Install & Setup Influx DB

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
