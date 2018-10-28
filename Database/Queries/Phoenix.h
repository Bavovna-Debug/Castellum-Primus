#pragma once

#define QueryTotalNumberOfPhoenixes "\
SELECT COUNT(*) \
FROM kernel.phoenixes"

#define QuerySearchForPhoenixByIndex "\
SELECT phoenix_id \
FROM kernel.phoenixes \
ORDER BY phoenix_id DESC \
OFFSET $1 LIMIT 1"

#define QuerySearchForPhoenixByToken "\
SELECT phoenix_id \
FROM kernel.phoenixes \
WHERE phoenix_token = $1"

#define QuerySearchForPhoenixById "\
SELECT phoenix_stamp, phoenix_id, phoenix_token, device_name, device_model, software_version, title \
FROM kernel.phoenixes \
WHERE phoenix_id = $1"

#define QuerySearchForActivatorByCode "\
SELECT activator_id \
FROM kernel.activators \
WHERE activation_code LIKE $1 and phoenix_id IS NULL"

#define QuaraAssociateActivatorWithPhoenix "\
UPDATE kernel.activators \
SET phoenix_id = $2 \
WHERE activator_id = $1"

#define QueryInsertPhoenix "\
INSERT INTO kernel.phoenixes (vendor_token, device_name, device_model, software_version, title) \
VALUES($1, $2, $3, $4, $5) \
RETURNING phoenix_id"

#define QueryUpdatePhoenixDeviceToken "\
UPDATE kernel.phoenixes \
SET device_token = $2 \
WHERE phoenix_id = $1"

#define QueryUpdatePhoenixTitle "\
UPDATE kernel.phoenixes \
SET title = $2 \
WHERE phoenix_id = $1 \
RETURNING title"

#define QueryUpdatePhoenixSoftwareVersion "\
UPDATE kernel.phoenixes \
SET software_version = $2 \
WHERE phoenix_id = $1 \
RETURNING software_version"

#define QueryRemovePhoenixById "\
DELETE FROM kernel.phoenixes \
WHERE phoenix_id = $1"

#define QueryGetNumberOfNotificationsForPhoenix "\
SELECT COUNT(*) \
FROM journal.notifications \
WHERE phoenix_id = $1"
