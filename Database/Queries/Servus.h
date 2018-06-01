#pragma once

#define QueryTotalNumberOfServuses "\
SELECT COUNT(*) \
FROM kernel.servuses"

#define QuerySearchForServusByAuthenticator "\
SELECT servus_id \
FROM kernel.servuses \
WHERE authenticator = $1"

#define QuerySearchForServusByIndex "\
SELECT servus_id \
FROM kernel.servuses \
ORDER BY list_order ASC \
OFFSET $1 LIMIT 1"

#define QuerySearchForServusById "\
SELECT servus_stamp, servus_id, servus_token, enabled, online, authenticator, servus_description \
FROM kernel.servuses \
WHERE servus_id = $1"

#define QueryServusConfiguration "\
SELECT kernel.servus_configuration($1)"

#define QueryInsertServus "\
INSERT INTO kernel.servuses (servus_description) \
VALUES ($1) \
RETURNING servus_id"

#define QueryToggleServusEnabledFlag "\
UPDATE kernel.servuses \
SET enabled = NOT enabled \
WHERE servus_id = $1 \
RETURNING enabled"

#define QueryUpdateServusDescription "\
UPDATE kernel.servuses \
SET servus_description = $2 \
WHERE servus_id = $1 \
RETURNING servus_description"

#define QueryServusSetOnline "\
UPDATE kernel.servuses \
SET online = TRUE \
WHERE servus_id = $1"

#define QueryServusSetOffline "\
UPDATE kernel.servuses \
SET online = FALSE \
WHERE servus_id = $1"

#define QueryResetAllServuses "\
UPDATE kernel.servuses \
SET online = FALSE"
