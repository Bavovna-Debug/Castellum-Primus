#pragma once

#define QueryTotalNumberOfActivators "\
SELECT COUNT(*) \
FROM kernel.activators"

#define QuerySearchForActivatorByIndex "\
SELECT activator_id \
FROM kernel.activators \
ORDER BY activator_id DESC \
OFFSET $1 LIMIT 1"

#define QuerySearchForActivatorById "\
SELECT activator_stamp, activator_id, activator_token, phoenix_id, activation_code, activator_description \
FROM kernel.activators \
WHERE activator_id = $1"

#define QueryInsertActivator "\
INSERT INTO kernel.activators (activation_code, activator_description) \
VALUES($1, $2) \
RETURNING activator_id"

#define QueryUpdateActivatorActivationCode "\
UPDATE kernel.activators \
SET activation_code = $2 \
WHERE activator_id = $1"

#define QueryUpdateActivatorDescription "\
UPDATE kernel.activators \
SET activator_description = $2 \
WHERE activator_id = $1"
