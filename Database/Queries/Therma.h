#pragma once

#define QueryTotalNumberOfDSSensors "\
SELECT COUNT(*) \
FROM kernel.thermas"

#define QuerySearchForDSSensorByIndex "\
SELECT therma_id \
FROM kernel.thermas \
ORDER BY list_order ASC \
OFFSET $1 LIMIT 1"

#define QuerySearchForDSSensorById "\
SELECT therma_stamp, therma_id, therma_token, servus_id, gpio_device_number, temperature_edge, title \
FROM kernel.thermas \
WHERE therma_id = $1"

#define QueryUpdateDSSensorTitle "\
UPDATE kernel.thermas \
SET title = $2 \
WHERE therma_id = $1 \
RETURNING title"

#define QueryInsertDSSensorTemperature "\
INSERT INTO journal.temperatures (temperature_stamp, therma_id, original_stamp, temperature) \
SELECT $1, therma_id, $3, $4 \
FROM kernel.thermas \
WHERE therma_token = $2"

#define QueryDSSensorLastKnownTemperature "\
SELECT temperature \
FROM journal.temperatures \
WHERE therma_id = $1 \
ORDER BY temperature_id DESC \
LIMIT 1"

#define QueryDSSensorLowestTemperature "\
SELECT MIN(temperature) \
FROM journal.temperatures \
WHERE therma_id = $1"

#define QueryDSSensorHighestTemperature "\
SELECT MAX(temperature) \
FROM journal.temperatures \
WHERE therma_id = $1"

#define QueryDSSensorDiagramAsJava "\
SELECT journal.temperatures_diagram($1)"
