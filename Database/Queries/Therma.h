#pragma once

#define QueryTotalNumberOfThermas "\
SELECT COUNT(*) \
FROM kernel.thermas"

#define QuerySearchForThermaByIndex "\
SELECT therma_id \
FROM kernel.thermas \
ORDER BY therma_id DESC \
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
