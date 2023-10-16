import requests
import random
import time

# Define the InfluxDB server and database details
# influxdb_url = 'http://172.30.223.25:8086/write?db=esp02'  # Adjust the URL as needed - local wsl vm
influxdb_url = 'http://34.125.194.47/influxdb/write?db=esp02'  # Adjust the URL as needed

measurement = 'data'

# Iterate 10,000 times
for _ in range(1000):
    # Generate a new random number
    random_number = random.randint(1, 100)
    
    # Create the InfluxDB Line Protocol data point
    data_point = f'{measurement} value={random_number}'

    try:
        # Send the POST request to InfluxDB
        response = requests.post(influxdb_url, data=data_point)

        # Check the response
        if response.status_code == 204:
            print(f'Successfully inserted data {random_number} into InfluxDB!')
        else:
            print(f'Failed to insert data {random_number}. Status code:', response.status_code)

    except Exception as e:
        print('An error occurred:', str(e))
    
    # Wait for 1 second before the next iteration
    time.sleep(1)
