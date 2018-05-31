#pragma once

#define QueryTotalNumberOfRelays "\
SELECT COUNT(*) \
FROM kernel.relays"

#define QuerySearchForRelayByIndex "\
SELECT relay_id \
FROM kernel.relays \
ORDER BY list_order ASC \
OFFSET $1 LIMIT 1"

#define QuerySearchForRelayById "\
SELECT relay_stamp, relay_id, relay_token, servus_id, gpio_pin_number, relay_description \
FROM kernel.relays \
WHERE relay_id = $1"

#define QueryUpdateRelayDescription "\
UPDATE kernel.relays \
SET relay_description = $2 \
WHERE relay_id = $1 \
RETURNING relay_description"
