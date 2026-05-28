# SMART_HOME_AND_IOT

Overview
--------

A collection of instructions and examples for Smart Home and IoT projects: setting up Node-RED, Grafana, and MQTT on a Raspberry Pi.

Quick overview
--------------

- Node-RED — a visual tool for wiring together hardware devices, APIs and online services.
- Grafana — visualization and dashboarding for metrics and time-series data.
- MQTT — lightweight publish/subscribe messaging protocol for IoT devices.

Installing Node-RED
-------------------

On Debian/Ubuntu-based systems (including Raspberry Pi OS) use the official installer:

```bash
bash <(curl -sL https://github.com/node-red/linux-installers/releases/latest/download/update-nodejs-and-nodered-deb)
```

Documentation: https://nodered.org/docs/getting-started/raspberrypi

Installing Grafana
------------------

Add the official repository and install Grafana:

```bash
sudo mkdir -p /etc/apt/keyrings/
wget -q -O - https://apt.grafana.com/gpg.key | gpg --dearmor | sudo tee /etc/apt/keyrings/grafana.gpg > /dev/null
echo "deb [signed-by=/etc/apt/keyrings/grafana.gpg] https://apt.grafana.com stable main" | sudo tee /etc/apt/sources.list.d/grafana.list
sudo apt-get update
sudo apt-get install -y grafana
sudo systemctl enable grafana-server
sudo systemctl start grafana-server
```

Open your browser at http://<IP_ADDRESS>:3000 and log in with the default credentials `admin` / `admin`.

Node-RED + MQTT (Raspberry Pi)
------------------------------

Useful video guide: https://www.youtube.com/watch?v=WxUTYzxIDns

Tips
----

- Run an MQTT broker (e.g. Mosquitto) to route messages between ESP devices and Node-RED.
- Store metrics in a time-series database (e.g. InfluxDB) and visualize them in Grafana.

Repository layout
-----------------

- `code/` — helper scripts and examples
- `configs/` — configuration files (e.g. `mosquitto.conf`)
- `src/` — project source code

