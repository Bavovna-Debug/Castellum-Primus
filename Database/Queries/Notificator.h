#pragma once

#define QueryNumberOfPendingNotifications "\
SELECT COUNT(*) \
FROM journal.notifications \
JOIN kernel.phoenixes USING (phoenix_id) \
WHERE pending IS TRUE AND device_token IS NOT NULL"

#define QueryAllPendingNotifications "\
SELECT notification_id, device_token, payload \
FROM journal.notifications \
JOIN kernel.phoenixes USING (phoenix_id) \
WHERE pending IS TRUE AND device_token IS NOT NULL \
ORDER BY notification_id \
LIMIT 10"

#define QueryMarkNotificationAsSent "\
UPDATE journal.notifications \
SET pending = FALSE \
WHERE notification_id = $1"

#define QueryMarkNotificationAsRescheduled "\
UPDATE journal.notifications \
SET rescheduled = GREATEST(0, rescheduled) + 1 \
WHERE notification_id = $1"
