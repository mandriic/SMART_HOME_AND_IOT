# SMART_HOME_AND_IOT

Node-Red
bash <(curl -sL https://github.com/node-red/linux-installers/releases/latest/download/update-nodejs-and-nodered-deb)
https://nodered.org/docs/getting-started/raspberrypi

Install grafana
sudo mkdir -p /etc/apt/keyrings/
wget -q -O - https://apt.grafana.com/gpg.key | gpg --dearmor | sudo tee /etc/apt/keyrings/grafana.gpg > /dev/null
echo "deb [signed-by=/etc/apt/keyrings/grafana.gpg] https://apt.grafana.com stable main" | sudo tee /etc/apt/sources.list.d/grafana.list
sudo apt-get update
sudo apt-get install -y grafana
sudo /bin/systemctl enable grafana-server
sudo /bin/systemctl start grafana-server
Open a browser and go to http://<ip address>:3000, where the IP address is the address that you used to connect to the Raspberry Pi earlier. You’re greeted with the Grafana login page.
Log in to Grafana with the default username admin, and the default password admin.

Node Red MQTT on the Raspberry Pi
https://www.youtube.com/watch?v=WxUTYzxIDns