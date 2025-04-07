const client = mqtt.connect('ws://localhost:9001');

client.on('connect', () => {
  console.log('Connected to MQTT broker!');
  document.getElementById('status').textContent = 'Connected to MQTT broker';
  client.subscribe('sensor/pressure');
  client.subscribe('sensor/posture');
  client.subscribe('sensor/spo2');
});

client.on('message', (topic, message) => {
  const value = message.toString();

  switch (topic) {
    case 'sensor/pressure':
      document.getElementById('pressure').textContent = `Pressure: ${value} hPa`;
      break;
    case 'sensor/posture':
      document.getElementById('posture').textContent = `Posture: ${value}`;
      break;
    case 'sensor/spo2':
      document.getElementById('spo2').textContent = `SpO₂: ${value} %`;
      break;
  }
});

client.on('error', (err) => {
  console.error('Connection error: ', err);
  document.getElementById('status').textContent = 'Connection error';
});
