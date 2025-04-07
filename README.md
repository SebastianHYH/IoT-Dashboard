# A simple IoT based dashboard to observe sleeping patients

## Requirements:
- Mosquitto (https://mosquitto.org/)
- Python

## How to Run
- Ensure mosquitto is running by putting in the command:
```
mosquitto.exe -v
```
in a terminal where mosquitto is installed

- Set mosquitto configuration in mosquitto.conf to:
```
listener 1883
protocol mqtt

listener 9001
protocol websockets

allow_anonymous true
```

- In the project folder, run:
```
python -m http.server 8080
```
to run the server at
```
http://localhost:8080/
```

## Troubleshooting
- You can manually change the output of each sensor by typing
```
mosquitto_pub -t sensor/{name_of_sensor} -m {message}
```

in the folder where mosquitto is installed