/*
* Copyright (C) 2009 Mamadou Diop.
*
* Contact: Mamadou Diop <diopmamadou(at)doubango.org>
*	
* This file is part of Open Source Doubango Framework.
*
* DOUBANGO is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*	
* DOUBANGO is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*	
* You should have received a copy of the GNU General Public License
* along with DOUBANGO.
*
*/
#include "message.h"

extern ctx_t* ctx;

int message_handle_event(const tsip_event_t *sipevent)
{
	const tsip_message_event_t* msg_event = TSIP_MESSAGE_EVENT(sipevent);
	const session_t* session;
	tsip_ssession_id_t sid;

	/* Find associated session */
	sid = tsip_ssession_get_id(sipevent->ss);
	if(!(session = session_get_by_sid(ctx->sessions, sid))){
		TSK_DEBUG_WARN("Failed to match session event.");
		return -1;
	}
	

	switch(msg_event->type){
		
		/* Informational */
		case tsip_o_message: /* Outgoing MESSAGE */
			{	/* Request successfully sent (you cannot suppose that the remote peer has received the request) ==> Informational */
				TSK_DEBUG_INFO("Transport layer successfully sent MESSAGE request");
				break;
			}

		case tsip_ao_message: /* Answer to outgoing MESSAGE */
			{
				TSK_DEBUG_INFO("Event: Answer to outgoing MESSAGE. Code=%d", TSIP_RESPONSE_CODE(sipevent->sipmessage));
				if(TSIP_RESPONSE_IS_2XX(sipevent->sipmessage)){
					TSK_DEBUG_INFO("Message successfully sent.");
				}
				break;
			}

		case tsip_i_message: /* Incoming MESSAGE */
			{
				TSK_DEBUG_INFO("Event: Incoming MESSAGE.");
				if(TSIP_MESSAGE_HAS_CONTENT(sipevent->sipmessage)){
					const tsk_buffer_t* content = TSIP_MESSAGE_CONTENT(sipevent->sipmessage);
					TSK_DEBUG_INFO("MESSAGE Content-Type: %s", TSIP_MESSAGE_CONTENT_TYPE(sipevent->sipmessage));
					TSK_DEBUG_INFO("MESSAGE Content: %s", content->data);
				}
				break;
			}

		/* Server events (For whose dev. Server Side IMS Services) */
		case tsip_ai_message: /* Answer to Incoming MESSAGE */
			{	
				TSK_DEBUG_WARN("Event not support by Client Framework.");
				break;
			}

		default:
			{	/* Any other event */
				TSK_DEBUG_WARN("%d not a valid SIP Registration event.", msg_event->type);
				break;
			}
	}

	return 0;
}

tsip_ssession_id_t message_handle_cmd(cmd_type_t cmd, const opts_L_t* opts)
{
	const session_t* session = tsk_null;
	const opt_t* opt;
	tsip_ssession_id_t id = TSIP_SSESSION_INVALID_ID;

	if(!(session = session_handle_cmd(cmd, opts))){
		return -1;
	}
	else{
		id = tsip_ssession_get_id(session->handle);
	}
	
	switch(cmd){
		case cmd_message:
			{	/* Send SIP MESSAGE */
				/* Payload */
				if((opt = opt_get_by_type(opts, opt_payload))){
					tsip_action_MESSAGE(session->handle,
						TSIP_ACTION_SET_PAYLOAD(opt->value, tsk_strlen(opt->value)),
						TSIP_ACTION_SET_NULL());
				}
				else{
					tsip_action_MESSAGE(session->handle,
						TSIP_ACTION_SET_NULL());
				}
				break;
			}
		default:
			/* already handled by session_handle_cmd() */
			break;
	}
	
	return id;
}