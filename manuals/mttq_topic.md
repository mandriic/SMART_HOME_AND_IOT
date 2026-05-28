# Topic: mttq (MQTT)

This document defines the "mttq" messaging topic and shows how to create/use it on an MQTT broker.

## Assumption
- "mttq" is an MQTT topic (likely a typo for "mqtt"). If a different system is desired (Kafka, AWS SNS, GitHub topic, etc.), tell me and I'll create that instead.

## How MQTT topics are created
MQTT topics are created implicitly when a client publishes to them. Example using mosquitto clients:

Subscribe to the topic (listen):

```bash
mosquitto_sub -h <broker-host> -t mttq -v
```

Publish a message (this will create the topic on the broker):

```bash
mosquitto_pub -h <broker-host> -t mttq -m '{"msg":"hello"}'
```

Options:
- `-q <n>` sets QoS (0,1,2)
- `-r` sets the message as retained

## Example payloads
- Simple text: `Hello, world` 
- JSON: `{"type":"event","timestamp":"2026-05-28T11:00:00Z"}`

## Notes
- Ensure an MQTT broker (e.g., mosquitto) is running and reachable.
- If you want this topic created in Kafka, AWS, or as a GitHub repo topic, specify and the appropriate steps will be created.
