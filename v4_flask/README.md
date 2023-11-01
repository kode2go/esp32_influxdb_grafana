# Overview

Cloud:
- Nginx: Grafana, Influx, Flask

- Flask: set value in influxdb

ESP32:
- reads from influxdb and checks state each second

To Do:
Add post message as well

# Flask Set Pin

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

    location /switch {
        alias /var/www/html;
        index index.html;
    }

    location /flask {
        include proxy_params;
        proxy_pass http://127.0.0.1:5000;
    }
}
```

## flask_app.py

```
from flask import Flask, render_template, request, jsonify, redirect, url_for
import requests

from influxdb import InfluxDBClient

# Define the InfluxDB connection parameters
host = "localhost"  # Change to your InfluxDB server's host
port = 8086  # Change to your InfluxDB server's port
database = "esp02"  # Your desired database name

# Define the data point with "state" as a float
measurement = "switch"
tags = {}  # You can add tags if necessary
#fields = {"state": 40.0}

# Create a JSON data point
#data = [
#    {
#        "measurement": measurement,
#        "tags": tags,
#        "fields": fields,
#    }
#]

app = Flask(__name__)

@app.route('/flask')
def hello():
    return "Hello from Flask!"

@app.route('/flask/trigger-curl',methods=['POST'])
def trigger_curl():
    client = InfluxDBClient(host=host, port=port, database=database)

    # Check which button was pressed
    if "high" in request.form:
        fields = {"state": 10.0}
    elif "low" in request.form:
        fields = {"state": 0.0}
    else:
        return "Invalid request"

    # Create a JSON data point
    data = [
        {
            "measurement": measurement,
            "tags": tags,
            "fields": fields,
        }
    ]

    client.write_points(data)
    client.close()
    return redirect(url_for('index'))
   # client = InfluxDBClient(host=host, port=port, database=database)
   # client.write_points(data)
   # client.close()
   # return "Written to db"

@app.route('/flask/index')
def index():
    return render_template('index.html')

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)
```

## templates/index.html

```
<!DOCTYPE html>
<html>
<head>
    <title>Flask Set Pin</title>
</head>
<body>
    <h1>Set Pin</h1>
    <form action="/flask/trigger-curl" method="post">
        <input type="submit" name="high" value="High">
        <input type="submit" name="low" value="Low">
    </form>
    <a href="http://34.125.194.47/">Home</a>
</body>
</html>
```
