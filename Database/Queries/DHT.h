#pragma once

#define QueryTotalNumberOfDHTSensors "\
SELECT COUNT(*) \
FROM kernel.dhts"

#define QuerySearchForDHTSensorByIndex "\
SELECT dht_id \
FROM kernel.dhts \
ORDER BY list_order ASC \
OFFSET $1 LIMIT 1"

#define QuerySearchForDHTSensorById "\
SELECT dht_stamp, dht_id, dht_token, servus_id, gpio_pin_number, humidity_edge, temperature_edge, title \
FROM kernel.dhts \
WHERE dht_id = $1"

#define QueryInsertDHTSensorHumidity "\
INSERT INTO journal.dht_humidities (humidity_stamp, dht_id, original_stamp, humidity) \
SELECT $1, dht_id, $3, $4 \
FROM kernel.dhts \
WHERE dht_token = $2"

#define QueryInsertDHTSensorTemperature "\
INSERT INTO journal.dht_temperatures (temperature_stamp, dht_id, original_stamp, temperature) \
SELECT $1, dht_id, $3, $4 \
FROM kernel.dhts \
WHERE dht_token = $2"

#define QueryDHTSensorLastKnownHumidity "\
SELECT humidity \
FROM journal.dht_humidities \
WHERE dht_id = $1 \
ORDER BY humidity_id DESC \
LIMIT 1"

#define QueryDHTSensorLowestHumidity "\
SELECT MIN(humidity) \
FROM journal.dht_humidities \
WHERE dht_id = $1"

#define QueryDHTSensorHighestHumidity "\
SELECT MAX(humidity) \
FROM journal.dht_humidities \
WHERE dht_id = $1"

#define QueryDHTSensorLastKnownTemperature "\
SELECT temperature \
FROM journal.dht_temperatures \
WHERE dht_id = $1 \
ORDER BY temperature_id DESC \
LIMIT 1"

#define QueryDHTSensorLowestTemperature "\
SELECT MIN(temperature) \
FROM journal.dht_temperatures \
WHERE dht_id = $1"

#define QueryDHTSensorHighestTemperature "\
SELECT MAX(temperature) \
FROM journal.dht_temperatures \
WHERE dht_id = $1"
