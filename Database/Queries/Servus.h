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
SELECT servus_stamp, servus_id, servus_token, enabled, online, running_since, authenticator, title \
FROM kernel.servuses \
WHERE servus_id = $1"

#define QueryServusConfiguration "\
SELECT kernel.servus_configuration($1)"

#define QueryInsertServus "\
INSERT INTO kernel.servuses (title) \
VALUES ($1) \
RETURNING servus_id"

#define QueryUpdateServusTitle "\
UPDATE kernel.servuses \
SET title = $2 \
WHERE servus_id = $1 \
RETURNING title"

#define QueryServusSetOnline "\
UPDATE kernel.servuses \
SET online = TRUE \
WHERE servus_id = $1"

#define QueryServusSetOffline "\
UPDATE kernel.servuses \
SET online = FALSE \
WHERE servus_id = $1"

#define QueryToggleServusEnabledFlag "\
UPDATE kernel.servuses \
SET enabled = NOT enabled \
WHERE servus_id = $1 \
RETURNING enabled"

#define QueryServusSetRunningSince "\
UPDATE kernel.servuses \
SET running_since = $2 \
WHERE servus_id = $1"

#define QueryResetAllServuses "\
UPDATE kernel.servuses \
SET online = FALSE"

#define QueryServusesAsXML "\
SELECT kernel.servuses_for_walker()"
