#pragma once

#define QueryTotalNumberOfThermas "\
SELECT COUNT(*) \
FROM kernel.thermas"

#define QuerySearchForThermaByIndex "\
SELECT therma_id \
FROM kernel.thermas \
ORDER BY list_order ASC \
OFFSET $1 LIMIT 1"

#define QuerySearchForThermaById "\
SELECT therma_stamp, therma_id, therma_token, servus_id, gpio_device_number, therma_description \
FROM kernel.thermas \
WHERE therma_id = $1"

#define QueryUpdateThermaDescription "\
UPDATE kernel.thermas \
SET therma_description = $2 \
WHERE therma_id = $1 \
RETURNING therma_description"

#define QueryInsertTemperature "\
INSERT INTO journal.temperatures (temperature_stamp, therma_id, original_stamp, temperature) \
SELECT $1, therma_id, $3, $4 \
FROM kernel.thermas \
WHERE therma_token = $2"

#define QueryThermaLastKnownTemperature "\
SELECT temperature \
FROM journal.temperatures \
WHERE therma_id = $1 \
ORDER BY temperature_id DESC \
LIMIT 1"

#define QueryThermaLowestTemperature "\
SELECT MIN(temperature) \
FROM journal.temperatures \
WHERE therma_id = $1"

#define QueryThermaHighestTemperature "\
SELECT MAX(temperature) \
FROM journal.temperatures \
WHERE therma_id = $1"
