#pragma once

#define QueryBeginServusSession "\
INSERT INTO debug.servus_sessions \
(servus_ip) \
VALUES($1) \
RETURNING session_id"

#define QueryCloseServusSession "\
UPDATE debug.servus_sessions \
SET session_close = NOW() \
WHERE session_id = $1"

#define QueryCommentServusSession "\
UPDATE debug.servus_sessions \
SET session_status = $2 \
WHERE session_id = $1"

#define QueryReportServusRTSP "\
INSERT INTO debug.servus_rtsp \
(session_id, header_complete, payload_complete, request_payload, response_payload) \
VALUES ($1, $2, $3, $4, $5)"

#define QueryBeginPhoenixSession "\
INSERT INTO debug.phoenix_sessions \
(phoenix_ip) \
VALUES($1) \
RETURNING session_id"

#define QueryClosePhoenixSession "\
UPDATE debug.phoenix_sessions \
SET session_close = NOW() \
WHERE session_id = $1"

#define QueryCommentPhoenixSession "\
UPDATE debug.phoenix_sessions \
SET session_status = $2 \
WHERE session_id = $1"

#define QueryReportPhoenixRTSP "\
INSERT INTO debug.phoenix_rtsp \
(session_id, header_complete, payload_complete, request_payload, response_payload, response_status) \
VALUES ($1, $2, $3, $4, $5, $6)"
