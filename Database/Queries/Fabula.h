#pragma once

#define QueryTotalNumberOfFabulas "\
SELECT COUNT(*) \
FROM journal.fabulas"

#define QuerySearchForFabulaByIndex "\
SELECT fabula_id \
FROM journal.fabulas \
ORDER BY fabula_id \
OFFSET $1 LIMIT 1"

#define QuerySearchForFabulaById "\
SELECT fabula_id, fabula_token, origin_stamp, fabulas.servus_id, originator_name, severity, message \
FROM journal.fabulas \
JOIN journal.originators USING (originator_id) \
WHERE fabula_id = $1"

#define QueryCreateFabula "\
INSERT INTO journal.fabulas (origin_stamp, servus_id, originator_id, severity, message) \
VALUES ($1, $2, journal.touch_originator($2, $3), $4, $5) \
RETURNING fabula_token"

#define QueryFabulasAsXML "\
SELECT journal.fabulas_for_walker($1, $2)"
