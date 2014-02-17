#include "message.h"

Message::Message(uint8_t* m) {
	//std::cout << "Entering Message" << std::endl;
	msg = m;
	type = FindMsgType();
	size = FindMsgSize();

	encMsg = NULL;
	uint8_t *ptr = EncMsgOffset();
	if (ptr != NULL) {
		encMsg = new Message(ptr);
	}
	//std::cout << "Exiting Message" << std::endl;
}

int Message::FindMsgType() {
	return ((BaseMessage*)msg)->type;
}

int Message::FindMsgSize() {
	return ((BaseMessage*)msg)->size + 0;	
}

void Message::ChangeBaseMessage(int field, char* value) {
	//std::cout << "Entering ChangeBaseMessage" << std::endl;
	BaseMessage *ptr = (BaseMessage*)msg;

	if (field == 0) {
		uint16_t *cur = &(ptr->src);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 1) {
		uint16_t *cur = &(ptr->dest);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 2) {
		uint32_t *cur = &(ptr->seq);
		uint32_t tmp=ntohl(*cur);
		if (value[0] == '=') {
			tmp = (uint32_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint32_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint32_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint32_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint32_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint32_t)rand();
		}
		 *cur=htonl(tmp);
	}
	if (field == 3) {
		uint32_t *cur = &(ptr->ack);
		uint32_t tmp=ntohl(*cur);
		if (value[0] == '=') {
			tmp = (uint32_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint32_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint32_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint32_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint32_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint32_t)rand();
		}
		 *cur=htonl(tmp);
	}
	if (field == 4) {
		if (value[0] == '=') {
			ptr->res = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->res += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->res -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->res *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->res /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->res = (uint16_t)rand();
		}
	}
	if (field == 5) {
		if (value[0] == '=') {
			ptr->size = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->size += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->size -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->size *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->size /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->size = (uint16_t)rand();
		}
	}
	if (field == 6) {
		if (value[0] == '=') {
			ptr->type = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->type += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->type -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->type *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->type /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->type = (uint16_t)rand();
		}
	}
	if (field == 7) {
		if (value[0] == '=') {
			ptr->urg = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->urg += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->urg -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->urg *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->urg /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->urg = (uint16_t)rand();
		}
	}
	if (field == 8) {
		if (value[0] == '=') {
			ptr->ece = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->ece += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->ece -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->ece *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->ece /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->ece = (uint16_t)rand();
		}
	}
	if (field == 9) {
		if (value[0] == '=') {
			ptr->cwr = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->cwr += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->cwr -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->cwr *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->cwr /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->cwr = (uint16_t)rand();
		}
	}
	if (field == 10) {
		uint16_t *cur = &(ptr->window);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 11) {
		uint16_t *cur = &(ptr->chk);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 12) {
		uint16_t *cur = &(ptr->urgptr);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	//std::cout << "Exiting ChangeBaseMessage" << std::endl;
}

void Message::ChangeFin(int field, char* value) {
	//std::cout << "Entering ChangeFin" << std::endl;
	Fin *ptr = (Fin*)msg;

	if (field == 0) {
		uint16_t *cur = &(ptr->src);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 1) {
		uint16_t *cur = &(ptr->dest);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 2) {
		uint32_t *cur = &(ptr->seq);
		uint32_t tmp=ntohl(*cur);
		if (value[0] == '=') {
			tmp = (uint32_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint32_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint32_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint32_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint32_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint32_t)rand();
		}
		 *cur=htonl(tmp);
	}
	if (field == 3) {
		uint32_t *cur = &(ptr->ack);
		uint32_t tmp=ntohl(*cur);
		if (value[0] == '=') {
			tmp = (uint32_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint32_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint32_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint32_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint32_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint32_t)rand();
		}
		 *cur=htonl(tmp);
	}
	if (field == 4) {
		if (value[0] == '=') {
			ptr->res = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->res += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->res -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->res *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->res /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->res = (uint16_t)rand();
		}
	}
	if (field == 5) {
		if (value[0] == '=') {
			ptr->size = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->size += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->size -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->size *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->size /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->size = (uint16_t)rand();
		}
	}
	if (field == 6) {
		if (value[0] == '=') {
			ptr->type = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->type += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->type -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->type *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->type /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->type = (uint16_t)rand();
		}
	}
	if (field == 7) {
		if (value[0] == '=') {
			ptr->urg = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->urg += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->urg -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->urg *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->urg /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->urg = (uint16_t)rand();
		}
	}
	if (field == 8) {
		if (value[0] == '=') {
			ptr->ece = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->ece += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->ece -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->ece *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->ece /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->ece = (uint16_t)rand();
		}
	}
	if (field == 9) {
		if (value[0] == '=') {
			ptr->cwr = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->cwr += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->cwr -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->cwr *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->cwr /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->cwr = (uint16_t)rand();
		}
	}
	if (field == 10) {
		uint16_t *cur = &(ptr->window);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 11) {
		uint16_t *cur = &(ptr->chk);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 12) {
		uint16_t *cur = &(ptr->urgptr);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	//std::cout << "Exiting ChangeFin" << std::endl;
}

void Message::ChangeSyn(int field, char* value) {
	//std::cout << "Entering ChangeSyn" << std::endl;
	Syn *ptr = (Syn*)msg;

	if (field == 0) {
		uint16_t *cur = &(ptr->src);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 1) {
		uint16_t *cur = &(ptr->dest);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 2) {
		uint32_t *cur = &(ptr->seq);
		uint32_t tmp=ntohl(*cur);
		if (value[0] == '=') {
			tmp = (uint32_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint32_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint32_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint32_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint32_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint32_t)rand();
		}
		 *cur=htonl(tmp);
	}
	if (field == 3) {
		uint32_t *cur = &(ptr->ack);
		uint32_t tmp=ntohl(*cur);
		if (value[0] == '=') {
			tmp = (uint32_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint32_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint32_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint32_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint32_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint32_t)rand();
		}
		 *cur=htonl(tmp);
	}
	if (field == 4) {
		if (value[0] == '=') {
			ptr->res = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->res += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->res -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->res *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->res /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->res = (uint16_t)rand();
		}
	}
	if (field == 5) {
		if (value[0] == '=') {
			ptr->size = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->size += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->size -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->size *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->size /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->size = (uint16_t)rand();
		}
	}
	if (field == 6) {
		if (value[0] == '=') {
			ptr->type = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->type += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->type -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->type *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->type /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->type = (uint16_t)rand();
		}
	}
	if (field == 7) {
		if (value[0] == '=') {
			ptr->urg = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->urg += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->urg -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->urg *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->urg /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->urg = (uint16_t)rand();
		}
	}
	if (field == 8) {
		if (value[0] == '=') {
			ptr->ece = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->ece += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->ece -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->ece *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->ece /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->ece = (uint16_t)rand();
		}
	}
	if (field == 9) {
		if (value[0] == '=') {
			ptr->cwr = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->cwr += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->cwr -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->cwr *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->cwr /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->cwr = (uint16_t)rand();
		}
	}
	if (field == 10) {
		uint16_t *cur = &(ptr->window);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 11) {
		uint16_t *cur = &(ptr->chk);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 12) {
		uint16_t *cur = &(ptr->urgptr);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	//std::cout << "Exiting ChangeSyn" << std::endl;
}

void Message::ChangeFinSyn(int field, char* value) {
	//std::cout << "Entering ChangeFinSyn" << std::endl;
	FinSyn *ptr = (FinSyn*)msg;

	if (field == 0) {
		uint16_t *cur = &(ptr->src);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 1) {
		uint16_t *cur = &(ptr->dest);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 2) {
		uint32_t *cur = &(ptr->seq);
		uint32_t tmp=ntohl(*cur);
		if (value[0] == '=') {
			tmp = (uint32_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint32_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint32_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint32_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint32_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint32_t)rand();
		}
		 *cur=htonl(tmp);
	}
	if (field == 3) {
		uint32_t *cur = &(ptr->ack);
		uint32_t tmp=ntohl(*cur);
		if (value[0] == '=') {
			tmp = (uint32_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint32_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint32_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint32_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint32_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint32_t)rand();
		}
		 *cur=htonl(tmp);
	}
	if (field == 4) {
		if (value[0] == '=') {
			ptr->res = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->res += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->res -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->res *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->res /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->res = (uint16_t)rand();
		}
	}
	if (field == 5) {
		if (value[0] == '=') {
			ptr->size = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->size += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->size -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->size *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->size /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->size = (uint16_t)rand();
		}
	}
	if (field == 6) {
		if (value[0] == '=') {
			ptr->type = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->type += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->type -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->type *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->type /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->type = (uint16_t)rand();
		}
	}
	if (field == 7) {
		if (value[0] == '=') {
			ptr->urg = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->urg += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->urg -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->urg *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->urg /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->urg = (uint16_t)rand();
		}
	}
	if (field == 8) {
		if (value[0] == '=') {
			ptr->ece = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->ece += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->ece -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->ece *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->ece /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->ece = (uint16_t)rand();
		}
	}
	if (field == 9) {
		if (value[0] == '=') {
			ptr->cwr = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->cwr += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->cwr -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->cwr *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->cwr /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->cwr = (uint16_t)rand();
		}
	}
	if (field == 10) {
		uint16_t *cur = &(ptr->window);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 11) {
		uint16_t *cur = &(ptr->chk);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 12) {
		uint16_t *cur = &(ptr->urgptr);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	//std::cout << "Exiting ChangeFinSyn" << std::endl;
}

void Message::ChangeRst(int field, char* value) {
	//std::cout << "Entering ChangeRst" << std::endl;
	Rst *ptr = (Rst*)msg;

	if (field == 0) {
		uint16_t *cur = &(ptr->src);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 1) {
		uint16_t *cur = &(ptr->dest);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 2) {
		uint32_t *cur = &(ptr->seq);
		uint32_t tmp=ntohl(*cur);
		if (value[0] == '=') {
			tmp = (uint32_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint32_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint32_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint32_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint32_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint32_t)rand();
		}
		 *cur=htonl(tmp);
	}
	if (field == 3) {
		uint32_t *cur = &(ptr->ack);
		uint32_t tmp=ntohl(*cur);
		if (value[0] == '=') {
			tmp = (uint32_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint32_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint32_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint32_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint32_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint32_t)rand();
		}
		 *cur=htonl(tmp);
	}
	if (field == 4) {
		if (value[0] == '=') {
			ptr->res = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->res += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->res -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->res *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->res /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->res = (uint16_t)rand();
		}
	}
	if (field == 5) {
		if (value[0] == '=') {
			ptr->size = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->size += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->size -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->size *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->size /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->size = (uint16_t)rand();
		}
	}
	if (field == 6) {
		if (value[0] == '=') {
			ptr->type = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->type += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->type -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->type *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->type /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->type = (uint16_t)rand();
		}
	}
	if (field == 7) {
		if (value[0] == '=') {
			ptr->urg = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->urg += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->urg -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->urg *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->urg /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->urg = (uint16_t)rand();
		}
	}
	if (field == 8) {
		if (value[0] == '=') {
			ptr->ece = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->ece += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->ece -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->ece *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->ece /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->ece = (uint16_t)rand();
		}
	}
	if (field == 9) {
		if (value[0] == '=') {
			ptr->cwr = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->cwr += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->cwr -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->cwr *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->cwr /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->cwr = (uint16_t)rand();
		}
	}
	if (field == 10) {
		uint16_t *cur = &(ptr->window);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 11) {
		uint16_t *cur = &(ptr->chk);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 12) {
		uint16_t *cur = &(ptr->urgptr);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	//std::cout << "Exiting ChangeRst" << std::endl;
}

void Message::ChangeFinRst(int field, char* value) {
	//std::cout << "Entering ChangeFinRst" << std::endl;
	FinRst *ptr = (FinRst*)msg;

	if (field == 0) {
		uint16_t *cur = &(ptr->src);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 1) {
		uint16_t *cur = &(ptr->dest);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 2) {
		uint32_t *cur = &(ptr->seq);
		uint32_t tmp=ntohl(*cur);
		if (value[0] == '=') {
			tmp = (uint32_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint32_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint32_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint32_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint32_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint32_t)rand();
		}
		 *cur=htonl(tmp);
	}
	if (field == 3) {
		uint32_t *cur = &(ptr->ack);
		uint32_t tmp=ntohl(*cur);
		if (value[0] == '=') {
			tmp = (uint32_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint32_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint32_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint32_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint32_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint32_t)rand();
		}
		 *cur=htonl(tmp);
	}
	if (field == 4) {
		if (value[0] == '=') {
			ptr->res = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->res += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->res -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->res *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->res /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->res = (uint16_t)rand();
		}
	}
	if (field == 5) {
		if (value[0] == '=') {
			ptr->size = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->size += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->size -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->size *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->size /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->size = (uint16_t)rand();
		}
	}
	if (field == 6) {
		if (value[0] == '=') {
			ptr->type = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->type += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->type -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->type *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->type /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->type = (uint16_t)rand();
		}
	}
	if (field == 7) {
		if (value[0] == '=') {
			ptr->urg = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->urg += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->urg -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->urg *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->urg /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->urg = (uint16_t)rand();
		}
	}
	if (field == 8) {
		if (value[0] == '=') {
			ptr->ece = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->ece += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->ece -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->ece *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->ece /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->ece = (uint16_t)rand();
		}
	}
	if (field == 9) {
		if (value[0] == '=') {
			ptr->cwr = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->cwr += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->cwr -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->cwr *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->cwr /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->cwr = (uint16_t)rand();
		}
	}
	if (field == 10) {
		uint16_t *cur = &(ptr->window);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 11) {
		uint16_t *cur = &(ptr->chk);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 12) {
		uint16_t *cur = &(ptr->urgptr);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	//std::cout << "Exiting ChangeFinRst" << std::endl;
}

void Message::ChangeSynRst(int field, char* value) {
	//std::cout << "Entering ChangeSynRst" << std::endl;
	SynRst *ptr = (SynRst*)msg;

	if (field == 0) {
		uint16_t *cur = &(ptr->src);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 1) {
		uint16_t *cur = &(ptr->dest);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 2) {
		uint32_t *cur = &(ptr->seq);
		uint32_t tmp=ntohl(*cur);
		if (value[0] == '=') {
			tmp = (uint32_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint32_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint32_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint32_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint32_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint32_t)rand();
		}
		 *cur=htonl(tmp);
	}
	if (field == 3) {
		uint32_t *cur = &(ptr->ack);
		uint32_t tmp=ntohl(*cur);
		if (value[0] == '=') {
			tmp = (uint32_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint32_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint32_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint32_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint32_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint32_t)rand();
		}
		 *cur=htonl(tmp);
	}
	if (field == 4) {
		if (value[0] == '=') {
			ptr->res = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->res += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->res -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->res *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->res /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->res = (uint16_t)rand();
		}
	}
	if (field == 5) {
		if (value[0] == '=') {
			ptr->size = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->size += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->size -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->size *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->size /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->size = (uint16_t)rand();
		}
	}
	if (field == 6) {
		if (value[0] == '=') {
			ptr->type = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->type += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->type -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->type *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->type /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->type = (uint16_t)rand();
		}
	}
	if (field == 7) {
		if (value[0] == '=') {
			ptr->urg = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->urg += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->urg -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->urg *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->urg /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->urg = (uint16_t)rand();
		}
	}
	if (field == 8) {
		if (value[0] == '=') {
			ptr->ece = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->ece += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->ece -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->ece *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->ece /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->ece = (uint16_t)rand();
		}
	}
	if (field == 9) {
		if (value[0] == '=') {
			ptr->cwr = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->cwr += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->cwr -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->cwr *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->cwr /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->cwr = (uint16_t)rand();
		}
	}
	if (field == 10) {
		uint16_t *cur = &(ptr->window);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 11) {
		uint16_t *cur = &(ptr->chk);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 12) {
		uint16_t *cur = &(ptr->urgptr);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	//std::cout << "Exiting ChangeSynRst" << std::endl;
}

void Message::ChangeFinSynRst(int field, char* value) {
	//std::cout << "Entering ChangeFinSynRst" << std::endl;
	FinSynRst *ptr = (FinSynRst*)msg;

	if (field == 0) {
		uint16_t *cur = &(ptr->src);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 1) {
		uint16_t *cur = &(ptr->dest);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 2) {
		uint32_t *cur = &(ptr->seq);
		uint32_t tmp=ntohl(*cur);
		if (value[0] == '=') {
			tmp = (uint32_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint32_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint32_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint32_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint32_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint32_t)rand();
		}
		 *cur=htonl(tmp);
	}
	if (field == 3) {
		uint32_t *cur = &(ptr->ack);
		uint32_t tmp=ntohl(*cur);
		if (value[0] == '=') {
			tmp = (uint32_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint32_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint32_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint32_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint32_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint32_t)rand();
		}
		 *cur=htonl(tmp);
	}
	if (field == 4) {
		if (value[0] == '=') {
			ptr->res = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->res += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->res -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->res *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->res /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->res = (uint16_t)rand();
		}
	}
	if (field == 5) {
		if (value[0] == '=') {
			ptr->size = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->size += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->size -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->size *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->size /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->size = (uint16_t)rand();
		}
	}
	if (field == 6) {
		if (value[0] == '=') {
			ptr->type = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->type += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->type -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->type *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->type /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->type = (uint16_t)rand();
		}
	}
	if (field == 7) {
		if (value[0] == '=') {
			ptr->urg = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->urg += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->urg -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->urg *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->urg /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->urg = (uint16_t)rand();
		}
	}
	if (field == 8) {
		if (value[0] == '=') {
			ptr->ece = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->ece += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->ece -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->ece *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->ece /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->ece = (uint16_t)rand();
		}
	}
	if (field == 9) {
		if (value[0] == '=') {
			ptr->cwr = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->cwr += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->cwr -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->cwr *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->cwr /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->cwr = (uint16_t)rand();
		}
	}
	if (field == 10) {
		uint16_t *cur = &(ptr->window);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 11) {
		uint16_t *cur = &(ptr->chk);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 12) {
		uint16_t *cur = &(ptr->urgptr);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	//std::cout << "Exiting ChangeFinSynRst" << std::endl;
}

void Message::ChangePsh(int field, char* value) {
	//std::cout << "Entering ChangePsh" << std::endl;
	Psh *ptr = (Psh*)msg;

	if (field == 0) {
		uint16_t *cur = &(ptr->src);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 1) {
		uint16_t *cur = &(ptr->dest);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 2) {
		uint32_t *cur = &(ptr->seq);
		uint32_t tmp=ntohl(*cur);
		if (value[0] == '=') {
			tmp = (uint32_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint32_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint32_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint32_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint32_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint32_t)rand();
		}
		 *cur=htonl(tmp);
	}
	if (field == 3) {
		uint32_t *cur = &(ptr->ack);
		uint32_t tmp=ntohl(*cur);
		if (value[0] == '=') {
			tmp = (uint32_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint32_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint32_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint32_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint32_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint32_t)rand();
		}
		 *cur=htonl(tmp);
	}
	if (field == 4) {
		if (value[0] == '=') {
			ptr->res = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->res += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->res -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->res *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->res /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->res = (uint16_t)rand();
		}
	}
	if (field == 5) {
		if (value[0] == '=') {
			ptr->size = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->size += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->size -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->size *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->size /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->size = (uint16_t)rand();
		}
	}
	if (field == 6) {
		if (value[0] == '=') {
			ptr->type = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->type += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->type -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->type *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->type /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->type = (uint16_t)rand();
		}
	}
	if (field == 7) {
		if (value[0] == '=') {
			ptr->urg = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->urg += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->urg -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->urg *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->urg /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->urg = (uint16_t)rand();
		}
	}
	if (field == 8) {
		if (value[0] == '=') {
			ptr->ece = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->ece += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->ece -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->ece *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->ece /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->ece = (uint16_t)rand();
		}
	}
	if (field == 9) {
		if (value[0] == '=') {
			ptr->cwr = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->cwr += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->cwr -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->cwr *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->cwr /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->cwr = (uint16_t)rand();
		}
	}
	if (field == 10) {
		uint16_t *cur = &(ptr->window);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 11) {
		uint16_t *cur = &(ptr->chk);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 12) {
		uint16_t *cur = &(ptr->urgptr);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	//std::cout << "Exiting ChangePsh" << std::endl;
}

void Message::ChangeFinPsh(int field, char* value) {
	//std::cout << "Entering ChangeFinPsh" << std::endl;
	FinPsh *ptr = (FinPsh*)msg;

	if (field == 0) {
		uint16_t *cur = &(ptr->src);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 1) {
		uint16_t *cur = &(ptr->dest);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 2) {
		uint32_t *cur = &(ptr->seq);
		uint32_t tmp=ntohl(*cur);
		if (value[0] == '=') {
			tmp = (uint32_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint32_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint32_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint32_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint32_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint32_t)rand();
		}
		 *cur=htonl(tmp);
	}
	if (field == 3) {
		uint32_t *cur = &(ptr->ack);
		uint32_t tmp=ntohl(*cur);
		if (value[0] == '=') {
			tmp = (uint32_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint32_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint32_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint32_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint32_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint32_t)rand();
		}
		 *cur=htonl(tmp);
	}
	if (field == 4) {
		if (value[0] == '=') {
			ptr->res = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->res += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->res -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->res *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->res /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->res = (uint16_t)rand();
		}
	}
	if (field == 5) {
		if (value[0] == '=') {
			ptr->size = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->size += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->size -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->size *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->size /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->size = (uint16_t)rand();
		}
	}
	if (field == 6) {
		if (value[0] == '=') {
			ptr->type = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->type += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->type -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->type *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->type /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->type = (uint16_t)rand();
		}
	}
	if (field == 7) {
		if (value[0] == '=') {
			ptr->urg = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->urg += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->urg -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->urg *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->urg /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->urg = (uint16_t)rand();
		}
	}
	if (field == 8) {
		if (value[0] == '=') {
			ptr->ece = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->ece += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->ece -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->ece *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->ece /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->ece = (uint16_t)rand();
		}
	}
	if (field == 9) {
		if (value[0] == '=') {
			ptr->cwr = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->cwr += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->cwr -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->cwr *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->cwr /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->cwr = (uint16_t)rand();
		}
	}
	if (field == 10) {
		uint16_t *cur = &(ptr->window);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 11) {
		uint16_t *cur = &(ptr->chk);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 12) {
		uint16_t *cur = &(ptr->urgptr);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	//std::cout << "Exiting ChangeFinPsh" << std::endl;
}

void Message::ChangeSynPsh(int field, char* value) {
	//std::cout << "Entering ChangeSynPsh" << std::endl;
	SynPsh *ptr = (SynPsh*)msg;

	if (field == 0) {
		uint16_t *cur = &(ptr->src);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 1) {
		uint16_t *cur = &(ptr->dest);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 2) {
		uint32_t *cur = &(ptr->seq);
		uint32_t tmp=ntohl(*cur);
		if (value[0] == '=') {
			tmp = (uint32_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint32_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint32_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint32_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint32_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint32_t)rand();
		}
		 *cur=htonl(tmp);
	}
	if (field == 3) {
		uint32_t *cur = &(ptr->ack);
		uint32_t tmp=ntohl(*cur);
		if (value[0] == '=') {
			tmp = (uint32_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint32_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint32_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint32_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint32_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint32_t)rand();
		}
		 *cur=htonl(tmp);
	}
	if (field == 4) {
		if (value[0] == '=') {
			ptr->res = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->res += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->res -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->res *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->res /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->res = (uint16_t)rand();
		}
	}
	if (field == 5) {
		if (value[0] == '=') {
			ptr->size = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->size += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->size -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->size *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->size /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->size = (uint16_t)rand();
		}
	}
	if (field == 6) {
		if (value[0] == '=') {
			ptr->type = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->type += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->type -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->type *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->type /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->type = (uint16_t)rand();
		}
	}
	if (field == 7) {
		if (value[0] == '=') {
			ptr->urg = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->urg += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->urg -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->urg *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->urg /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->urg = (uint16_t)rand();
		}
	}
	if (field == 8) {
		if (value[0] == '=') {
			ptr->ece = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->ece += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->ece -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->ece *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->ece /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->ece = (uint16_t)rand();
		}
	}
	if (field == 9) {
		if (value[0] == '=') {
			ptr->cwr = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->cwr += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->cwr -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->cwr *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->cwr /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->cwr = (uint16_t)rand();
		}
	}
	if (field == 10) {
		uint16_t *cur = &(ptr->window);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 11) {
		uint16_t *cur = &(ptr->chk);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 12) {
		uint16_t *cur = &(ptr->urgptr);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	//std::cout << "Exiting ChangeSynPsh" << std::endl;
}

void Message::ChangeFinSynPsh(int field, char* value) {
	//std::cout << "Entering ChangeFinSynPsh" << std::endl;
	FinSynPsh *ptr = (FinSynPsh*)msg;

	if (field == 0) {
		uint16_t *cur = &(ptr->src);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 1) {
		uint16_t *cur = &(ptr->dest);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 2) {
		uint32_t *cur = &(ptr->seq);
		uint32_t tmp=ntohl(*cur);
		if (value[0] == '=') {
			tmp = (uint32_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint32_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint32_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint32_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint32_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint32_t)rand();
		}
		 *cur=htonl(tmp);
	}
	if (field == 3) {
		uint32_t *cur = &(ptr->ack);
		uint32_t tmp=ntohl(*cur);
		if (value[0] == '=') {
			tmp = (uint32_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint32_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint32_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint32_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint32_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint32_t)rand();
		}
		 *cur=htonl(tmp);
	}
	if (field == 4) {
		if (value[0] == '=') {
			ptr->res = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->res += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->res -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->res *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->res /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->res = (uint16_t)rand();
		}
	}
	if (field == 5) {
		if (value[0] == '=') {
			ptr->size = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->size += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->size -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->size *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->size /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->size = (uint16_t)rand();
		}
	}
	if (field == 6) {
		if (value[0] == '=') {
			ptr->type = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->type += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->type -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->type *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->type /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->type = (uint16_t)rand();
		}
	}
	if (field == 7) {
		if (value[0] == '=') {
			ptr->urg = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->urg += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->urg -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->urg *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->urg /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->urg = (uint16_t)rand();
		}
	}
	if (field == 8) {
		if (value[0] == '=') {
			ptr->ece = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->ece += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->ece -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->ece *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->ece /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->ece = (uint16_t)rand();
		}
	}
	if (field == 9) {
		if (value[0] == '=') {
			ptr->cwr = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->cwr += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->cwr -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->cwr *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->cwr /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->cwr = (uint16_t)rand();
		}
	}
	if (field == 10) {
		uint16_t *cur = &(ptr->window);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 11) {
		uint16_t *cur = &(ptr->chk);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 12) {
		uint16_t *cur = &(ptr->urgptr);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	//std::cout << "Exiting ChangeFinSynPsh" << std::endl;
}

void Message::ChangeRstPsh(int field, char* value) {
	//std::cout << "Entering ChangeRstPsh" << std::endl;
	RstPsh *ptr = (RstPsh*)msg;

	if (field == 0) {
		uint16_t *cur = &(ptr->src);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 1) {
		uint16_t *cur = &(ptr->dest);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 2) {
		uint32_t *cur = &(ptr->seq);
		uint32_t tmp=ntohl(*cur);
		if (value[0] == '=') {
			tmp = (uint32_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint32_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint32_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint32_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint32_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint32_t)rand();
		}
		 *cur=htonl(tmp);
	}
	if (field == 3) {
		uint32_t *cur = &(ptr->ack);
		uint32_t tmp=ntohl(*cur);
		if (value[0] == '=') {
			tmp = (uint32_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint32_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint32_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint32_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint32_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint32_t)rand();
		}
		 *cur=htonl(tmp);
	}
	if (field == 4) {
		if (value[0] == '=') {
			ptr->res = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->res += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->res -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->res *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->res /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->res = (uint16_t)rand();
		}
	}
	if (field == 5) {
		if (value[0] == '=') {
			ptr->size = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->size += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->size -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->size *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->size /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->size = (uint16_t)rand();
		}
	}
	if (field == 6) {
		if (value[0] == '=') {
			ptr->type = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->type += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->type -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->type *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->type /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->type = (uint16_t)rand();
		}
	}
	if (field == 7) {
		if (value[0] == '=') {
			ptr->urg = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->urg += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->urg -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->urg *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->urg /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->urg = (uint16_t)rand();
		}
	}
	if (field == 8) {
		if (value[0] == '=') {
			ptr->ece = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->ece += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->ece -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->ece *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->ece /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->ece = (uint16_t)rand();
		}
	}
	if (field == 9) {
		if (value[0] == '=') {
			ptr->cwr = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->cwr += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->cwr -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->cwr *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->cwr /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->cwr = (uint16_t)rand();
		}
	}
	if (field == 10) {
		uint16_t *cur = &(ptr->window);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 11) {
		uint16_t *cur = &(ptr->chk);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 12) {
		uint16_t *cur = &(ptr->urgptr);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	//std::cout << "Exiting ChangeRstPsh" << std::endl;
}

void Message::ChangeFinRstPsh(int field, char* value) {
	//std::cout << "Entering ChangeFinRstPsh" << std::endl;
	FinRstPsh *ptr = (FinRstPsh*)msg;

	if (field == 0) {
		uint16_t *cur = &(ptr->src);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 1) {
		uint16_t *cur = &(ptr->dest);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 2) {
		uint32_t *cur = &(ptr->seq);
		uint32_t tmp=ntohl(*cur);
		if (value[0] == '=') {
			tmp = (uint32_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint32_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint32_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint32_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint32_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint32_t)rand();
		}
		 *cur=htonl(tmp);
	}
	if (field == 3) {
		uint32_t *cur = &(ptr->ack);
		uint32_t tmp=ntohl(*cur);
		if (value[0] == '=') {
			tmp = (uint32_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint32_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint32_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint32_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint32_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint32_t)rand();
		}
		 *cur=htonl(tmp);
	}
	if (field == 4) {
		if (value[0] == '=') {
			ptr->res = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->res += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->res -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->res *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->res /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->res = (uint16_t)rand();
		}
	}
	if (field == 5) {
		if (value[0] == '=') {
			ptr->size = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->size += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->size -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->size *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->size /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->size = (uint16_t)rand();
		}
	}
	if (field == 6) {
		if (value[0] == '=') {
			ptr->type = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->type += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->type -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->type *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->type /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->type = (uint16_t)rand();
		}
	}
	if (field == 7) {
		if (value[0] == '=') {
			ptr->urg = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->urg += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->urg -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->urg *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->urg /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->urg = (uint16_t)rand();
		}
	}
	if (field == 8) {
		if (value[0] == '=') {
			ptr->ece = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->ece += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->ece -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->ece *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->ece /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->ece = (uint16_t)rand();
		}
	}
	if (field == 9) {
		if (value[0] == '=') {
			ptr->cwr = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->cwr += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->cwr -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->cwr *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->cwr /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->cwr = (uint16_t)rand();
		}
	}
	if (field == 10) {
		uint16_t *cur = &(ptr->window);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 11) {
		uint16_t *cur = &(ptr->chk);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 12) {
		uint16_t *cur = &(ptr->urgptr);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	//std::cout << "Exiting ChangeFinRstPsh" << std::endl;
}

void Message::ChangeSynRstPsh(int field, char* value) {
	//std::cout << "Entering ChangeSynRstPsh" << std::endl;
	SynRstPsh *ptr = (SynRstPsh*)msg;

	if (field == 0) {
		uint16_t *cur = &(ptr->src);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 1) {
		uint16_t *cur = &(ptr->dest);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 2) {
		uint32_t *cur = &(ptr->seq);
		uint32_t tmp=ntohl(*cur);
		if (value[0] == '=') {
			tmp = (uint32_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint32_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint32_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint32_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint32_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint32_t)rand();
		}
		 *cur=htonl(tmp);
	}
	if (field == 3) {
		uint32_t *cur = &(ptr->ack);
		uint32_t tmp=ntohl(*cur);
		if (value[0] == '=') {
			tmp = (uint32_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint32_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint32_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint32_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint32_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint32_t)rand();
		}
		 *cur=htonl(tmp);
	}
	if (field == 4) {
		if (value[0] == '=') {
			ptr->res = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->res += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->res -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->res *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->res /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->res = (uint16_t)rand();
		}
	}
	if (field == 5) {
		if (value[0] == '=') {
			ptr->size = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->size += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->size -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->size *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->size /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->size = (uint16_t)rand();
		}
	}
	if (field == 6) {
		if (value[0] == '=') {
			ptr->type = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->type += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->type -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->type *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->type /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->type = (uint16_t)rand();
		}
	}
	if (field == 7) {
		if (value[0] == '=') {
			ptr->urg = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->urg += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->urg -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->urg *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->urg /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->urg = (uint16_t)rand();
		}
	}
	if (field == 8) {
		if (value[0] == '=') {
			ptr->ece = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->ece += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->ece -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->ece *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->ece /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->ece = (uint16_t)rand();
		}
	}
	if (field == 9) {
		if (value[0] == '=') {
			ptr->cwr = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->cwr += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->cwr -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->cwr *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->cwr /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->cwr = (uint16_t)rand();
		}
	}
	if (field == 10) {
		uint16_t *cur = &(ptr->window);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 11) {
		uint16_t *cur = &(ptr->chk);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 12) {
		uint16_t *cur = &(ptr->urgptr);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	//std::cout << "Exiting ChangeSynRstPsh" << std::endl;
}

void Message::ChangeFinSynRstPsh(int field, char* value) {
	//std::cout << "Entering ChangeFinSynRstPsh" << std::endl;
	FinSynRstPsh *ptr = (FinSynRstPsh*)msg;

	if (field == 0) {
		uint16_t *cur = &(ptr->src);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 1) {
		uint16_t *cur = &(ptr->dest);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 2) {
		uint32_t *cur = &(ptr->seq);
		uint32_t tmp=ntohl(*cur);
		if (value[0] == '=') {
			tmp = (uint32_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint32_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint32_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint32_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint32_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint32_t)rand();
		}
		 *cur=htonl(tmp);
	}
	if (field == 3) {
		uint32_t *cur = &(ptr->ack);
		uint32_t tmp=ntohl(*cur);
		if (value[0] == '=') {
			tmp = (uint32_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint32_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint32_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint32_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint32_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint32_t)rand();
		}
		 *cur=htonl(tmp);
	}
	if (field == 4) {
		if (value[0] == '=') {
			ptr->res = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->res += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->res -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->res *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->res /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->res = (uint16_t)rand();
		}
	}
	if (field == 5) {
		if (value[0] == '=') {
			ptr->size = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->size += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->size -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->size *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->size /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->size = (uint16_t)rand();
		}
	}
	if (field == 6) {
		if (value[0] == '=') {
			ptr->type = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->type += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->type -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->type *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->type /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->type = (uint16_t)rand();
		}
	}
	if (field == 7) {
		if (value[0] == '=') {
			ptr->urg = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->urg += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->urg -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->urg *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->urg /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->urg = (uint16_t)rand();
		}
	}
	if (field == 8) {
		if (value[0] == '=') {
			ptr->ece = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->ece += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->ece -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->ece *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->ece /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->ece = (uint16_t)rand();
		}
	}
	if (field == 9) {
		if (value[0] == '=') {
			ptr->cwr = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->cwr += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->cwr -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->cwr *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->cwr /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->cwr = (uint16_t)rand();
		}
	}
	if (field == 10) {
		uint16_t *cur = &(ptr->window);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 11) {
		uint16_t *cur = &(ptr->chk);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 12) {
		uint16_t *cur = &(ptr->urgptr);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	//std::cout << "Exiting ChangeFinSynRstPsh" << std::endl;
}

void Message::ChangeAck(int field, char* value) {
	//std::cout << "Entering ChangeAck" << std::endl;
	Ack *ptr = (Ack*)msg;

	if (field == 0) {
		uint16_t *cur = &(ptr->src);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 1) {
		uint16_t *cur = &(ptr->dest);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 2) {
		uint32_t *cur = &(ptr->seq);
		uint32_t tmp=ntohl(*cur);
		if (value[0] == '=') {
			tmp = (uint32_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint32_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint32_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint32_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint32_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint32_t)rand();
		}
		 *cur=htonl(tmp);
	}
	if (field == 3) {
		uint32_t *cur = &(ptr->ack);
		uint32_t tmp=ntohl(*cur);
		if (value[0] == '=') {
			tmp = (uint32_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint32_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint32_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint32_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint32_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint32_t)rand();
		}
		 *cur=htonl(tmp);
	}
	if (field == 4) {
		if (value[0] == '=') {
			ptr->res = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->res += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->res -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->res *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->res /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->res = (uint16_t)rand();
		}
	}
	if (field == 5) {
		if (value[0] == '=') {
			ptr->size = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->size += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->size -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->size *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->size /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->size = (uint16_t)rand();
		}
	}
	if (field == 6) {
		if (value[0] == '=') {
			ptr->type = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->type += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->type -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->type *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->type /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->type = (uint16_t)rand();
		}
	}
	if (field == 7) {
		if (value[0] == '=') {
			ptr->urg = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->urg += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->urg -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->urg *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->urg /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->urg = (uint16_t)rand();
		}
	}
	if (field == 8) {
		if (value[0] == '=') {
			ptr->ece = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->ece += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->ece -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->ece *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->ece /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->ece = (uint16_t)rand();
		}
	}
	if (field == 9) {
		if (value[0] == '=') {
			ptr->cwr = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->cwr += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->cwr -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->cwr *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->cwr /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->cwr = (uint16_t)rand();
		}
	}
	if (field == 10) {
		uint16_t *cur = &(ptr->window);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 11) {
		uint16_t *cur = &(ptr->chk);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 12) {
		uint16_t *cur = &(ptr->urgptr);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	//std::cout << "Exiting ChangeAck" << std::endl;
}

void Message::ChangeFinAck(int field, char* value) {
	//std::cout << "Entering ChangeFinAck" << std::endl;
	FinAck *ptr = (FinAck*)msg;

	if (field == 0) {
		uint16_t *cur = &(ptr->src);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 1) {
		uint16_t *cur = &(ptr->dest);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 2) {
		uint32_t *cur = &(ptr->seq);
		uint32_t tmp=ntohl(*cur);
		if (value[0] == '=') {
			tmp = (uint32_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint32_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint32_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint32_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint32_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint32_t)rand();
		}
		 *cur=htonl(tmp);
	}
	if (field == 3) {
		uint32_t *cur = &(ptr->ack);
		uint32_t tmp=ntohl(*cur);
		if (value[0] == '=') {
			tmp = (uint32_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint32_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint32_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint32_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint32_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint32_t)rand();
		}
		 *cur=htonl(tmp);
	}
	if (field == 4) {
		if (value[0] == '=') {
			ptr->res = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->res += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->res -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->res *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->res /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->res = (uint16_t)rand();
		}
	}
	if (field == 5) {
		if (value[0] == '=') {
			ptr->size = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->size += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->size -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->size *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->size /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->size = (uint16_t)rand();
		}
	}
	if (field == 6) {
		if (value[0] == '=') {
			ptr->type = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->type += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->type -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->type *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->type /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->type = (uint16_t)rand();
		}
	}
	if (field == 7) {
		if (value[0] == '=') {
			ptr->urg = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->urg += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->urg -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->urg *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->urg /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->urg = (uint16_t)rand();
		}
	}
	if (field == 8) {
		if (value[0] == '=') {
			ptr->ece = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->ece += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->ece -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->ece *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->ece /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->ece = (uint16_t)rand();
		}
	}
	if (field == 9) {
		if (value[0] == '=') {
			ptr->cwr = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->cwr += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->cwr -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->cwr *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->cwr /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->cwr = (uint16_t)rand();
		}
	}
	if (field == 10) {
		uint16_t *cur = &(ptr->window);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 11) {
		uint16_t *cur = &(ptr->chk);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 12) {
		uint16_t *cur = &(ptr->urgptr);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	//std::cout << "Exiting ChangeFinAck" << std::endl;
}

void Message::ChangeSynAck(int field, char* value) {
	//std::cout << "Entering ChangeSynAck" << std::endl;
	SynAck *ptr = (SynAck*)msg;

	if (field == 0) {
		uint16_t *cur = &(ptr->src);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 1) {
		uint16_t *cur = &(ptr->dest);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 2) {
		uint32_t *cur = &(ptr->seq);
		uint32_t tmp=ntohl(*cur);
		if (value[0] == '=') {
			tmp = (uint32_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint32_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint32_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint32_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint32_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint32_t)rand();
		}
		 *cur=htonl(tmp);
	}
	if (field == 3) {
		uint32_t *cur = &(ptr->ack);
		uint32_t tmp=ntohl(*cur);
		if (value[0] == '=') {
			tmp = (uint32_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint32_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint32_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint32_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint32_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint32_t)rand();
		}
		 *cur=htonl(tmp);
	}
	if (field == 4) {
		if (value[0] == '=') {
			ptr->res = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->res += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->res -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->res *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->res /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->res = (uint16_t)rand();
		}
	}
	if (field == 5) {
		if (value[0] == '=') {
			ptr->size = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->size += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->size -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->size *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->size /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->size = (uint16_t)rand();
		}
	}
	if (field == 6) {
		if (value[0] == '=') {
			ptr->type = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->type += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->type -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->type *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->type /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->type = (uint16_t)rand();
		}
	}
	if (field == 7) {
		if (value[0] == '=') {
			ptr->urg = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->urg += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->urg -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->urg *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->urg /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->urg = (uint16_t)rand();
		}
	}
	if (field == 8) {
		if (value[0] == '=') {
			ptr->ece = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->ece += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->ece -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->ece *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->ece /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->ece = (uint16_t)rand();
		}
	}
	if (field == 9) {
		if (value[0] == '=') {
			ptr->cwr = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->cwr += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->cwr -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->cwr *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->cwr /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->cwr = (uint16_t)rand();
		}
	}
	if (field == 10) {
		uint16_t *cur = &(ptr->window);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 11) {
		uint16_t *cur = &(ptr->chk);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 12) {
		uint16_t *cur = &(ptr->urgptr);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	//std::cout << "Exiting ChangeSynAck" << std::endl;
}

void Message::ChangeFinSynAck(int field, char* value) {
	//std::cout << "Entering ChangeFinSynAck" << std::endl;
	FinSynAck *ptr = (FinSynAck*)msg;

	if (field == 0) {
		uint16_t *cur = &(ptr->src);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 1) {
		uint16_t *cur = &(ptr->dest);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 2) {
		uint32_t *cur = &(ptr->seq);
		uint32_t tmp=ntohl(*cur);
		if (value[0] == '=') {
			tmp = (uint32_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint32_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint32_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint32_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint32_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint32_t)rand();
		}
		 *cur=htonl(tmp);
	}
	if (field == 3) {
		uint32_t *cur = &(ptr->ack);
		uint32_t tmp=ntohl(*cur);
		if (value[0] == '=') {
			tmp = (uint32_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint32_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint32_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint32_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint32_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint32_t)rand();
		}
		 *cur=htonl(tmp);
	}
	if (field == 4) {
		if (value[0] == '=') {
			ptr->res = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->res += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->res -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->res *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->res /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->res = (uint16_t)rand();
		}
	}
	if (field == 5) {
		if (value[0] == '=') {
			ptr->size = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->size += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->size -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->size *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->size /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->size = (uint16_t)rand();
		}
	}
	if (field == 6) {
		if (value[0] == '=') {
			ptr->type = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->type += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->type -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->type *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->type /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->type = (uint16_t)rand();
		}
	}
	if (field == 7) {
		if (value[0] == '=') {
			ptr->urg = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->urg += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->urg -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->urg *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->urg /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->urg = (uint16_t)rand();
		}
	}
	if (field == 8) {
		if (value[0] == '=') {
			ptr->ece = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->ece += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->ece -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->ece *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->ece /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->ece = (uint16_t)rand();
		}
	}
	if (field == 9) {
		if (value[0] == '=') {
			ptr->cwr = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->cwr += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->cwr -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->cwr *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->cwr /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->cwr = (uint16_t)rand();
		}
	}
	if (field == 10) {
		uint16_t *cur = &(ptr->window);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 11) {
		uint16_t *cur = &(ptr->chk);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 12) {
		uint16_t *cur = &(ptr->urgptr);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	//std::cout << "Exiting ChangeFinSynAck" << std::endl;
}

void Message::ChangeRstAck(int field, char* value) {
	//std::cout << "Entering ChangeRstAck" << std::endl;
	RstAck *ptr = (RstAck*)msg;

	if (field == 0) {
		uint16_t *cur = &(ptr->src);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 1) {
		uint16_t *cur = &(ptr->dest);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 2) {
		uint32_t *cur = &(ptr->seq);
		uint32_t tmp=ntohl(*cur);
		if (value[0] == '=') {
			tmp = (uint32_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint32_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint32_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint32_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint32_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint32_t)rand();
		}
		 *cur=htonl(tmp);
	}
	if (field == 3) {
		uint32_t *cur = &(ptr->ack);
		uint32_t tmp=ntohl(*cur);
		if (value[0] == '=') {
			tmp = (uint32_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint32_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint32_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint32_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint32_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint32_t)rand();
		}
		 *cur=htonl(tmp);
	}
	if (field == 4) {
		if (value[0] == '=') {
			ptr->res = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->res += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->res -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->res *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->res /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->res = (uint16_t)rand();
		}
	}
	if (field == 5) {
		if (value[0] == '=') {
			ptr->size = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->size += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->size -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->size *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->size /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->size = (uint16_t)rand();
		}
	}
	if (field == 6) {
		if (value[0] == '=') {
			ptr->type = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->type += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->type -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->type *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->type /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->type = (uint16_t)rand();
		}
	}
	if (field == 7) {
		if (value[0] == '=') {
			ptr->urg = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->urg += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->urg -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->urg *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->urg /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->urg = (uint16_t)rand();
		}
	}
	if (field == 8) {
		if (value[0] == '=') {
			ptr->ece = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->ece += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->ece -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->ece *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->ece /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->ece = (uint16_t)rand();
		}
	}
	if (field == 9) {
		if (value[0] == '=') {
			ptr->cwr = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->cwr += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->cwr -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->cwr *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->cwr /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->cwr = (uint16_t)rand();
		}
	}
	if (field == 10) {
		uint16_t *cur = &(ptr->window);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 11) {
		uint16_t *cur = &(ptr->chk);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 12) {
		uint16_t *cur = &(ptr->urgptr);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	//std::cout << "Exiting ChangeRstAck" << std::endl;
}

void Message::ChangeFinRstAck(int field, char* value) {
	//std::cout << "Entering ChangeFinRstAck" << std::endl;
	FinRstAck *ptr = (FinRstAck*)msg;

	if (field == 0) {
		uint16_t *cur = &(ptr->src);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 1) {
		uint16_t *cur = &(ptr->dest);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 2) {
		uint32_t *cur = &(ptr->seq);
		uint32_t tmp=ntohl(*cur);
		if (value[0] == '=') {
			tmp = (uint32_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint32_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint32_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint32_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint32_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint32_t)rand();
		}
		 *cur=htonl(tmp);
	}
	if (field == 3) {
		uint32_t *cur = &(ptr->ack);
		uint32_t tmp=ntohl(*cur);
		if (value[0] == '=') {
			tmp = (uint32_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint32_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint32_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint32_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint32_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint32_t)rand();
		}
		 *cur=htonl(tmp);
	}
	if (field == 4) {
		if (value[0] == '=') {
			ptr->res = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->res += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->res -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->res *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->res /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->res = (uint16_t)rand();
		}
	}
	if (field == 5) {
		if (value[0] == '=') {
			ptr->size = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->size += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->size -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->size *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->size /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->size = (uint16_t)rand();
		}
	}
	if (field == 6) {
		if (value[0] == '=') {
			ptr->type = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->type += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->type -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->type *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->type /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->type = (uint16_t)rand();
		}
	}
	if (field == 7) {
		if (value[0] == '=') {
			ptr->urg = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->urg += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->urg -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->urg *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->urg /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->urg = (uint16_t)rand();
		}
	}
	if (field == 8) {
		if (value[0] == '=') {
			ptr->ece = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->ece += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->ece -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->ece *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->ece /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->ece = (uint16_t)rand();
		}
	}
	if (field == 9) {
		if (value[0] == '=') {
			ptr->cwr = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->cwr += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->cwr -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->cwr *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->cwr /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->cwr = (uint16_t)rand();
		}
	}
	if (field == 10) {
		uint16_t *cur = &(ptr->window);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 11) {
		uint16_t *cur = &(ptr->chk);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 12) {
		uint16_t *cur = &(ptr->urgptr);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	//std::cout << "Exiting ChangeFinRstAck" << std::endl;
}

void Message::ChangeSynRstAck(int field, char* value) {
	//std::cout << "Entering ChangeSynRstAck" << std::endl;
	SynRstAck *ptr = (SynRstAck*)msg;

	if (field == 0) {
		uint16_t *cur = &(ptr->src);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 1) {
		uint16_t *cur = &(ptr->dest);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 2) {
		uint32_t *cur = &(ptr->seq);
		uint32_t tmp=ntohl(*cur);
		if (value[0] == '=') {
			tmp = (uint32_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint32_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint32_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint32_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint32_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint32_t)rand();
		}
		 *cur=htonl(tmp);
	}
	if (field == 3) {
		uint32_t *cur = &(ptr->ack);
		uint32_t tmp=ntohl(*cur);
		if (value[0] == '=') {
			tmp = (uint32_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint32_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint32_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint32_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint32_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint32_t)rand();
		}
		 *cur=htonl(tmp);
	}
	if (field == 4) {
		if (value[0] == '=') {
			ptr->res = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->res += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->res -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->res *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->res /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->res = (uint16_t)rand();
		}
	}
	if (field == 5) {
		if (value[0] == '=') {
			ptr->size = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->size += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->size -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->size *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->size /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->size = (uint16_t)rand();
		}
	}
	if (field == 6) {
		if (value[0] == '=') {
			ptr->type = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->type += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->type -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->type *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->type /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->type = (uint16_t)rand();
		}
	}
	if (field == 7) {
		if (value[0] == '=') {
			ptr->urg = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->urg += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->urg -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->urg *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->urg /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->urg = (uint16_t)rand();
		}
	}
	if (field == 8) {
		if (value[0] == '=') {
			ptr->ece = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->ece += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->ece -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->ece *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->ece /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->ece = (uint16_t)rand();
		}
	}
	if (field == 9) {
		if (value[0] == '=') {
			ptr->cwr = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->cwr += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->cwr -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->cwr *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->cwr /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->cwr = (uint16_t)rand();
		}
	}
	if (field == 10) {
		uint16_t *cur = &(ptr->window);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 11) {
		uint16_t *cur = &(ptr->chk);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 12) {
		uint16_t *cur = &(ptr->urgptr);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	//std::cout << "Exiting ChangeSynRstAck" << std::endl;
}

void Message::ChangeFinSynRstAck(int field, char* value) {
	//std::cout << "Entering ChangeFinSynRstAck" << std::endl;
	FinSynRstAck *ptr = (FinSynRstAck*)msg;

	if (field == 0) {
		uint16_t *cur = &(ptr->src);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 1) {
		uint16_t *cur = &(ptr->dest);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 2) {
		uint32_t *cur = &(ptr->seq);
		uint32_t tmp=ntohl(*cur);
		if (value[0] == '=') {
			tmp = (uint32_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint32_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint32_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint32_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint32_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint32_t)rand();
		}
		 *cur=htonl(tmp);
	}
	if (field == 3) {
		uint32_t *cur = &(ptr->ack);
		uint32_t tmp=ntohl(*cur);
		if (value[0] == '=') {
			tmp = (uint32_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint32_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint32_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint32_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint32_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint32_t)rand();
		}
		 *cur=htonl(tmp);
	}
	if (field == 4) {
		if (value[0] == '=') {
			ptr->res = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->res += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->res -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->res *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->res /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->res = (uint16_t)rand();
		}
	}
	if (field == 5) {
		if (value[0] == '=') {
			ptr->size = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->size += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->size -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->size *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->size /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->size = (uint16_t)rand();
		}
	}
	if (field == 6) {
		if (value[0] == '=') {
			ptr->type = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->type += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->type -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->type *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->type /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->type = (uint16_t)rand();
		}
	}
	if (field == 7) {
		if (value[0] == '=') {
			ptr->urg = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->urg += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->urg -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->urg *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->urg /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->urg = (uint16_t)rand();
		}
	}
	if (field == 8) {
		if (value[0] == '=') {
			ptr->ece = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->ece += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->ece -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->ece *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->ece /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->ece = (uint16_t)rand();
		}
	}
	if (field == 9) {
		if (value[0] == '=') {
			ptr->cwr = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->cwr += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->cwr -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->cwr *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->cwr /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->cwr = (uint16_t)rand();
		}
	}
	if (field == 10) {
		uint16_t *cur = &(ptr->window);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 11) {
		uint16_t *cur = &(ptr->chk);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 12) {
		uint16_t *cur = &(ptr->urgptr);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	//std::cout << "Exiting ChangeFinSynRstAck" << std::endl;
}

void Message::ChangePshAck(int field, char* value) {
	//std::cout << "Entering ChangePshAck" << std::endl;
	PshAck *ptr = (PshAck*)msg;

	if (field == 0) {
		uint16_t *cur = &(ptr->src);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 1) {
		uint16_t *cur = &(ptr->dest);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 2) {
		uint32_t *cur = &(ptr->seq);
		uint32_t tmp=ntohl(*cur);
		if (value[0] == '=') {
			tmp = (uint32_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint32_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint32_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint32_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint32_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint32_t)rand();
		}
		 *cur=htonl(tmp);
	}
	if (field == 3) {
		uint32_t *cur = &(ptr->ack);
		uint32_t tmp=ntohl(*cur);
		if (value[0] == '=') {
			tmp = (uint32_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint32_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint32_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint32_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint32_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint32_t)rand();
		}
		 *cur=htonl(tmp);
	}
	if (field == 4) {
		if (value[0] == '=') {
			ptr->res = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->res += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->res -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->res *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->res /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->res = (uint16_t)rand();
		}
	}
	if (field == 5) {
		if (value[0] == '=') {
			ptr->size = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->size += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->size -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->size *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->size /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->size = (uint16_t)rand();
		}
	}
	if (field == 6) {
		if (value[0] == '=') {
			ptr->type = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->type += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->type -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->type *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->type /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->type = (uint16_t)rand();
		}
	}
	if (field == 7) {
		if (value[0] == '=') {
			ptr->urg = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->urg += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->urg -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->urg *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->urg /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->urg = (uint16_t)rand();
		}
	}
	if (field == 8) {
		if (value[0] == '=') {
			ptr->ece = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->ece += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->ece -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->ece *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->ece /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->ece = (uint16_t)rand();
		}
	}
	if (field == 9) {
		if (value[0] == '=') {
			ptr->cwr = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->cwr += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->cwr -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->cwr *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->cwr /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->cwr = (uint16_t)rand();
		}
	}
	if (field == 10) {
		uint16_t *cur = &(ptr->window);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 11) {
		uint16_t *cur = &(ptr->chk);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 12) {
		uint16_t *cur = &(ptr->urgptr);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	//std::cout << "Exiting ChangePshAck" << std::endl;
}

void Message::ChangeFinPshAck(int field, char* value) {
	//std::cout << "Entering ChangeFinPshAck" << std::endl;
	FinPshAck *ptr = (FinPshAck*)msg;

	if (field == 0) {
		uint16_t *cur = &(ptr->src);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 1) {
		uint16_t *cur = &(ptr->dest);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 2) {
		uint32_t *cur = &(ptr->seq);
		uint32_t tmp=ntohl(*cur);
		if (value[0] == '=') {
			tmp = (uint32_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint32_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint32_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint32_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint32_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint32_t)rand();
		}
		 *cur=htonl(tmp);
	}
	if (field == 3) {
		uint32_t *cur = &(ptr->ack);
		uint32_t tmp=ntohl(*cur);
		if (value[0] == '=') {
			tmp = (uint32_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint32_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint32_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint32_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint32_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint32_t)rand();
		}
		 *cur=htonl(tmp);
	}
	if (field == 4) {
		if (value[0] == '=') {
			ptr->res = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->res += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->res -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->res *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->res /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->res = (uint16_t)rand();
		}
	}
	if (field == 5) {
		if (value[0] == '=') {
			ptr->size = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->size += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->size -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->size *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->size /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->size = (uint16_t)rand();
		}
	}
	if (field == 6) {
		if (value[0] == '=') {
			ptr->type = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->type += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->type -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->type *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->type /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->type = (uint16_t)rand();
		}
	}
	if (field == 7) {
		if (value[0] == '=') {
			ptr->urg = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->urg += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->urg -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->urg *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->urg /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->urg = (uint16_t)rand();
		}
	}
	if (field == 8) {
		if (value[0] == '=') {
			ptr->ece = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->ece += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->ece -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->ece *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->ece /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->ece = (uint16_t)rand();
		}
	}
	if (field == 9) {
		if (value[0] == '=') {
			ptr->cwr = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->cwr += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->cwr -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->cwr *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->cwr /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->cwr = (uint16_t)rand();
		}
	}
	if (field == 10) {
		uint16_t *cur = &(ptr->window);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 11) {
		uint16_t *cur = &(ptr->chk);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 12) {
		uint16_t *cur = &(ptr->urgptr);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	//std::cout << "Exiting ChangeFinPshAck" << std::endl;
}

void Message::ChangeSynPshAck(int field, char* value) {
	//std::cout << "Entering ChangeSynPshAck" << std::endl;
	SynPshAck *ptr = (SynPshAck*)msg;

	if (field == 0) {
		uint16_t *cur = &(ptr->src);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 1) {
		uint16_t *cur = &(ptr->dest);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 2) {
		uint32_t *cur = &(ptr->seq);
		uint32_t tmp=ntohl(*cur);
		if (value[0] == '=') {
			tmp = (uint32_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint32_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint32_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint32_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint32_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint32_t)rand();
		}
		 *cur=htonl(tmp);
	}
	if (field == 3) {
		uint32_t *cur = &(ptr->ack);
		uint32_t tmp=ntohl(*cur);
		if (value[0] == '=') {
			tmp = (uint32_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint32_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint32_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint32_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint32_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint32_t)rand();
		}
		 *cur=htonl(tmp);
	}
	if (field == 4) {
		if (value[0] == '=') {
			ptr->res = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->res += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->res -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->res *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->res /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->res = (uint16_t)rand();
		}
	}
	if (field == 5) {
		if (value[0] == '=') {
			ptr->size = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->size += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->size -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->size *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->size /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->size = (uint16_t)rand();
		}
	}
	if (field == 6) {
		if (value[0] == '=') {
			ptr->type = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->type += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->type -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->type *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->type /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->type = (uint16_t)rand();
		}
	}
	if (field == 7) {
		if (value[0] == '=') {
			ptr->urg = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->urg += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->urg -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->urg *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->urg /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->urg = (uint16_t)rand();
		}
	}
	if (field == 8) {
		if (value[0] == '=') {
			ptr->ece = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->ece += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->ece -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->ece *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->ece /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->ece = (uint16_t)rand();
		}
	}
	if (field == 9) {
		if (value[0] == '=') {
			ptr->cwr = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->cwr += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->cwr -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->cwr *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->cwr /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->cwr = (uint16_t)rand();
		}
	}
	if (field == 10) {
		uint16_t *cur = &(ptr->window);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 11) {
		uint16_t *cur = &(ptr->chk);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 12) {
		uint16_t *cur = &(ptr->urgptr);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	//std::cout << "Exiting ChangeSynPshAck" << std::endl;
}

void Message::ChangeFinSynPshAck(int field, char* value) {
	//std::cout << "Entering ChangeFinSynPshAck" << std::endl;
	FinSynPshAck *ptr = (FinSynPshAck*)msg;

	if (field == 0) {
		uint16_t *cur = &(ptr->src);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 1) {
		uint16_t *cur = &(ptr->dest);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 2) {
		uint32_t *cur = &(ptr->seq);
		uint32_t tmp=ntohl(*cur);
		if (value[0] == '=') {
			tmp = (uint32_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint32_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint32_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint32_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint32_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint32_t)rand();
		}
		 *cur=htonl(tmp);
	}
	if (field == 3) {
		uint32_t *cur = &(ptr->ack);
		uint32_t tmp=ntohl(*cur);
		if (value[0] == '=') {
			tmp = (uint32_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint32_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint32_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint32_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint32_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint32_t)rand();
		}
		 *cur=htonl(tmp);
	}
	if (field == 4) {
		if (value[0] == '=') {
			ptr->res = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->res += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->res -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->res *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->res /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->res = (uint16_t)rand();
		}
	}
	if (field == 5) {
		if (value[0] == '=') {
			ptr->size = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->size += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->size -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->size *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->size /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->size = (uint16_t)rand();
		}
	}
	if (field == 6) {
		if (value[0] == '=') {
			ptr->type = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->type += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->type -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->type *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->type /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->type = (uint16_t)rand();
		}
	}
	if (field == 7) {
		if (value[0] == '=') {
			ptr->urg = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->urg += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->urg -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->urg *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->urg /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->urg = (uint16_t)rand();
		}
	}
	if (field == 8) {
		if (value[0] == '=') {
			ptr->ece = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->ece += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->ece -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->ece *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->ece /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->ece = (uint16_t)rand();
		}
	}
	if (field == 9) {
		if (value[0] == '=') {
			ptr->cwr = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->cwr += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->cwr -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->cwr *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->cwr /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->cwr = (uint16_t)rand();
		}
	}
	if (field == 10) {
		uint16_t *cur = &(ptr->window);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 11) {
		uint16_t *cur = &(ptr->chk);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 12) {
		uint16_t *cur = &(ptr->urgptr);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	//std::cout << "Exiting ChangeFinSynPshAck" << std::endl;
}

void Message::ChangeRstPshAck(int field, char* value) {
	//std::cout << "Entering ChangeRstPshAck" << std::endl;
	RstPshAck *ptr = (RstPshAck*)msg;

	if (field == 0) {
		uint16_t *cur = &(ptr->src);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 1) {
		uint16_t *cur = &(ptr->dest);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 2) {
		uint32_t *cur = &(ptr->seq);
		uint32_t tmp=ntohl(*cur);
		if (value[0] == '=') {
			tmp = (uint32_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint32_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint32_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint32_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint32_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint32_t)rand();
		}
		 *cur=htonl(tmp);
	}
	if (field == 3) {
		uint32_t *cur = &(ptr->ack);
		uint32_t tmp=ntohl(*cur);
		if (value[0] == '=') {
			tmp = (uint32_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint32_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint32_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint32_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint32_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint32_t)rand();
		}
		 *cur=htonl(tmp);
	}
	if (field == 4) {
		if (value[0] == '=') {
			ptr->res = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->res += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->res -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->res *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->res /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->res = (uint16_t)rand();
		}
	}
	if (field == 5) {
		if (value[0] == '=') {
			ptr->size = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->size += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->size -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->size *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->size /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->size = (uint16_t)rand();
		}
	}
	if (field == 6) {
		if (value[0] == '=') {
			ptr->type = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->type += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->type -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->type *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->type /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->type = (uint16_t)rand();
		}
	}
	if (field == 7) {
		if (value[0] == '=') {
			ptr->urg = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->urg += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->urg -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->urg *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->urg /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->urg = (uint16_t)rand();
		}
	}
	if (field == 8) {
		if (value[0] == '=') {
			ptr->ece = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->ece += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->ece -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->ece *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->ece /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->ece = (uint16_t)rand();
		}
	}
	if (field == 9) {
		if (value[0] == '=') {
			ptr->cwr = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->cwr += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->cwr -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->cwr *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->cwr /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->cwr = (uint16_t)rand();
		}
	}
	if (field == 10) {
		uint16_t *cur = &(ptr->window);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 11) {
		uint16_t *cur = &(ptr->chk);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 12) {
		uint16_t *cur = &(ptr->urgptr);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	//std::cout << "Exiting ChangeRstPshAck" << std::endl;
}

void Message::ChangeFinRstPshAck(int field, char* value) {
	//std::cout << "Entering ChangeFinRstPshAck" << std::endl;
	FinRstPshAck *ptr = (FinRstPshAck*)msg;

	if (field == 0) {
		uint16_t *cur = &(ptr->src);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 1) {
		uint16_t *cur = &(ptr->dest);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 2) {
		uint32_t *cur = &(ptr->seq);
		uint32_t tmp=ntohl(*cur);
		if (value[0] == '=') {
			tmp = (uint32_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint32_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint32_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint32_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint32_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint32_t)rand();
		}
		 *cur=htonl(tmp);
	}
	if (field == 3) {
		uint32_t *cur = &(ptr->ack);
		uint32_t tmp=ntohl(*cur);
		if (value[0] == '=') {
			tmp = (uint32_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint32_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint32_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint32_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint32_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint32_t)rand();
		}
		 *cur=htonl(tmp);
	}
	if (field == 4) {
		if (value[0] == '=') {
			ptr->res = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->res += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->res -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->res *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->res /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->res = (uint16_t)rand();
		}
	}
	if (field == 5) {
		if (value[0] == '=') {
			ptr->size = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->size += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->size -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->size *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->size /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->size = (uint16_t)rand();
		}
	}
	if (field == 6) {
		if (value[0] == '=') {
			ptr->type = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->type += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->type -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->type *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->type /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->type = (uint16_t)rand();
		}
	}
	if (field == 7) {
		if (value[0] == '=') {
			ptr->urg = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->urg += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->urg -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->urg *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->urg /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->urg = (uint16_t)rand();
		}
	}
	if (field == 8) {
		if (value[0] == '=') {
			ptr->ece = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->ece += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->ece -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->ece *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->ece /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->ece = (uint16_t)rand();
		}
	}
	if (field == 9) {
		if (value[0] == '=') {
			ptr->cwr = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->cwr += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->cwr -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->cwr *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->cwr /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->cwr = (uint16_t)rand();
		}
	}
	if (field == 10) {
		uint16_t *cur = &(ptr->window);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 11) {
		uint16_t *cur = &(ptr->chk);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 12) {
		uint16_t *cur = &(ptr->urgptr);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	//std::cout << "Exiting ChangeFinRstPshAck" << std::endl;
}

void Message::ChangeSynRstPshAck(int field, char* value) {
	//std::cout << "Entering ChangeSynRstPshAck" << std::endl;
	SynRstPshAck *ptr = (SynRstPshAck*)msg;

	if (field == 0) {
		uint16_t *cur = &(ptr->src);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 1) {
		uint16_t *cur = &(ptr->dest);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 2) {
		uint32_t *cur = &(ptr->seq);
		uint32_t tmp=ntohl(*cur);
		if (value[0] == '=') {
			tmp = (uint32_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint32_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint32_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint32_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint32_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint32_t)rand();
		}
		 *cur=htonl(tmp);
	}
	if (field == 3) {
		uint32_t *cur = &(ptr->ack);
		uint32_t tmp=ntohl(*cur);
		if (value[0] == '=') {
			tmp = (uint32_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint32_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint32_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint32_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint32_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint32_t)rand();
		}
		 *cur=htonl(tmp);
	}
	if (field == 4) {
		if (value[0] == '=') {
			ptr->res = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->res += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->res -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->res *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->res /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->res = (uint16_t)rand();
		}
	}
	if (field == 5) {
		if (value[0] == '=') {
			ptr->size = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->size += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->size -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->size *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->size /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->size = (uint16_t)rand();
		}
	}
	if (field == 6) {
		if (value[0] == '=') {
			ptr->type = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->type += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->type -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->type *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->type /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->type = (uint16_t)rand();
		}
	}
	if (field == 7) {
		if (value[0] == '=') {
			ptr->urg = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->urg += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->urg -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->urg *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->urg /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->urg = (uint16_t)rand();
		}
	}
	if (field == 8) {
		if (value[0] == '=') {
			ptr->ece = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->ece += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->ece -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->ece *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->ece /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->ece = (uint16_t)rand();
		}
	}
	if (field == 9) {
		if (value[0] == '=') {
			ptr->cwr = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->cwr += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->cwr -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->cwr *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->cwr /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->cwr = (uint16_t)rand();
		}
	}
	if (field == 10) {
		uint16_t *cur = &(ptr->window);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 11) {
		uint16_t *cur = &(ptr->chk);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 12) {
		uint16_t *cur = &(ptr->urgptr);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	//std::cout << "Exiting ChangeSynRstPshAck" << std::endl;
}

void Message::ChangeFinSynRstPshAck(int field, char* value) {
	//std::cout << "Entering ChangeFinSynRstPshAck" << std::endl;
	FinSynRstPshAck *ptr = (FinSynRstPshAck*)msg;

	if (field == 0) {
		uint16_t *cur = &(ptr->src);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 1) {
		uint16_t *cur = &(ptr->dest);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 2) {
		uint32_t *cur = &(ptr->seq);
		uint32_t tmp=ntohl(*cur);
		if (value[0] == '=') {
			tmp = (uint32_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint32_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint32_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint32_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint32_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint32_t)rand();
		}
		 *cur=htonl(tmp);
	}
	if (field == 3) {
		uint32_t *cur = &(ptr->ack);
		uint32_t tmp=ntohl(*cur);
		if (value[0] == '=') {
			tmp = (uint32_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint32_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint32_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint32_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint32_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint32_t)rand();
		}
		 *cur=htonl(tmp);
	}
	if (field == 4) {
		if (value[0] == '=') {
			ptr->res = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->res += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->res -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->res *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->res /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->res = (uint16_t)rand();
		}
	}
	if (field == 5) {
		if (value[0] == '=') {
			ptr->size = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->size += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->size -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->size *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->size /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->size = (uint16_t)rand();
		}
	}
	if (field == 6) {
		if (value[0] == '=') {
			ptr->type = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->type += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->type -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->type *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->type /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->type = (uint16_t)rand();
		}
	}
	if (field == 7) {
		if (value[0] == '=') {
			ptr->urg = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->urg += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->urg -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->urg *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->urg /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->urg = (uint16_t)rand();
		}
	}
	if (field == 8) {
		if (value[0] == '=') {
			ptr->ece = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->ece += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->ece -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->ece *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->ece /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->ece = (uint16_t)rand();
		}
	}
	if (field == 9) {
		if (value[0] == '=') {
			ptr->cwr = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			ptr->cwr += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			ptr->cwr -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			ptr->cwr *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			ptr->cwr /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			ptr->cwr = (uint16_t)rand();
		}
	}
	if (field == 10) {
		uint16_t *cur = &(ptr->window);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 11) {
		uint16_t *cur = &(ptr->chk);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	if (field == 12) {
		uint16_t *cur = &(ptr->urgptr);
		uint16_t tmp=ntohs(*cur);
		if (value[0] == '=') {
			tmp = (uint16_t)atoi(value+1);
		} else if (value[0] == '+') {
			tmp += (uint16_t)atoi(value+1);
		} else if (value[0] == '-') {
			tmp -= (uint16_t)atoi(value+1);
		} else if (value[0] == '*') {
			tmp *= (uint16_t)atoi(value+1);
		} else if (value[0] == '/') {
			tmp /= (uint16_t)atoi(value+1);
		} else if (value[0] == 'r') {
			tmp = (uint16_t)rand();
		}
		 *cur=htons(tmp);
	}
	//std::cout << "Exiting ChangeFinSynRstPshAck" << std::endl;
}

void Message::ChangeValue(int field, char* value) {
	//std::cout << "Entering ChangeValue " << type << " " << field << " " << value << std::endl;
	switch (type) {
		case BASEMESSAGE:
			ChangeBaseMessage(field, value);
			break;
		case FIN:
			ChangeFin(field, value);
			break;
		case SYN:
			ChangeSyn(field, value);
			break;
		case FINSYN:
			ChangeFinSyn(field, value);
			break;
		case RST:
			ChangeRst(field, value);
			break;
		case FINRST:
			ChangeFinRst(field, value);
			break;
		case SYNRST:
			ChangeSynRst(field, value);
			break;
		case FINSYNRST:
			ChangeFinSynRst(field, value);
			break;
		case PSH:
			ChangePsh(field, value);
			break;
		case FINPSH:
			ChangeFinPsh(field, value);
			break;
		case SYNPSH:
			ChangeSynPsh(field, value);
			break;
		case FINSYNPSH:
			ChangeFinSynPsh(field, value);
			break;
		case RSTPSH:
			ChangeRstPsh(field, value);
			break;
		case FINRSTPSH:
			ChangeFinRstPsh(field, value);
			break;
		case SYNRSTPSH:
			ChangeSynRstPsh(field, value);
			break;
		case FINSYNRSTPSH:
			ChangeFinSynRstPsh(field, value);
			break;
		case ACK:
			ChangeAck(field, value);
			break;
		case FINACK:
			ChangeFinAck(field, value);
			break;
		case SYNACK:
			ChangeSynAck(field, value);
			break;
		case FINSYNACK:
			ChangeFinSynAck(field, value);
			break;
		case RSTACK:
			ChangeRstAck(field, value);
			break;
		case FINRSTACK:
			ChangeFinRstAck(field, value);
			break;
		case SYNRSTACK:
			ChangeSynRstAck(field, value);
			break;
		case FINSYNRSTACK:
			ChangeFinSynRstAck(field, value);
			break;
		case PSHACK:
			ChangePshAck(field, value);
			break;
		case FINPSHACK:
			ChangeFinPshAck(field, value);
			break;
		case SYNPSHACK:
			ChangeSynPshAck(field, value);
			break;
		case FINSYNPSHACK:
			ChangeFinSynPshAck(field, value);
			break;
		case RSTPSHACK:
			ChangeRstPshAck(field, value);
			break;
		case FINRSTPSHACK:
			ChangeFinRstPshAck(field, value);
			break;
		case SYNRSTPSHACK:
			ChangeSynRstPshAck(field, value);
			break;
		case FINSYNRSTPSHACK:
			ChangeFinSynRstPshAck(field, value);
			break;
	}
	//std::cout << "Exiting ChangeValue" << std::endl;
}

int Message::StrToType(const char *str) {
	//std::cout << "Entering StrToType" << std::endl;
	if (strcmp(str, "BaseMessage") == 0) {
		return BASEMESSAGE;
	}
	if (strcmp(str, "Fin") == 0) {
		return FIN;
	}
	if (strcmp(str, "Syn") == 0) {
		return SYN;
	}
	if (strcmp(str, "FinSyn") == 0) {
		return FINSYN;
	}
	if (strcmp(str, "Rst") == 0) {
		return RST;
	}
	if (strcmp(str, "FinRst") == 0) {
		return FINRST;
	}
	if (strcmp(str, "SynRst") == 0) {
		return SYNRST;
	}
	if (strcmp(str, "FinSynRst") == 0) {
		return FINSYNRST;
	}
	if (strcmp(str, "Psh") == 0) {
		return PSH;
	}
	if (strcmp(str, "FinPsh") == 0) {
		return FINPSH;
	}
	if (strcmp(str, "SynPsh") == 0) {
		return SYNPSH;
	}
	if (strcmp(str, "FinSynPsh") == 0) {
		return FINSYNPSH;
	}
	if (strcmp(str, "RstPsh") == 0) {
		return RSTPSH;
	}
	if (strcmp(str, "FinRstPsh") == 0) {
		return FINRSTPSH;
	}
	if (strcmp(str, "SynRstPsh") == 0) {
		return SYNRSTPSH;
	}
	if (strcmp(str, "FinSynRstPsh") == 0) {
		return FINSYNRSTPSH;
	}
	if (strcmp(str, "Ack") == 0) {
		return ACK;
	}
	if (strcmp(str, "FinAck") == 0) {
		return FINACK;
	}
	if (strcmp(str, "SynAck") == 0) {
		return SYNACK;
	}
	if (strcmp(str, "FinSynAck") == 0) {
		return FINSYNACK;
	}
	if (strcmp(str, "RstAck") == 0) {
		return RSTACK;
	}
	if (strcmp(str, "FinRstAck") == 0) {
		return FINRSTACK;
	}
	if (strcmp(str, "SynRstAck") == 0) {
		return SYNRSTACK;
	}
	if (strcmp(str, "FinSynRstAck") == 0) {
		return FINSYNRSTACK;
	}
	if (strcmp(str, "PshAck") == 0) {
		return PSHACK;
	}
	if (strcmp(str, "FinPshAck") == 0) {
		return FINPSHACK;
	}
	if (strcmp(str, "SynPshAck") == 0) {
		return SYNPSHACK;
	}
	if (strcmp(str, "FinSynPshAck") == 0) {
		return FINSYNPSHACK;
	}
	if (strcmp(str, "RstPshAck") == 0) {
		return RSTPSHACK;
	}
	if (strcmp(str, "FinRstPshAck") == 0) {
		return FINRSTPSHACK;
	}
	if (strcmp(str, "SynRstPshAck") == 0) {
		return SYNRSTPSHACK;
	}
	if (strcmp(str, "FinSynRstPshAck") == 0) {
		return FINSYNRSTPSHACK;
	}
	//std::cout << "Exiting StrToType" << std::endl;
	return -1;
}

std::string Message::TypeToStr(int type){
	//std::cout << "Entering TypeToStr" << std::endl;
	if (type == BASEMESSAGE ) {
		return "BaseMessage";
	}
	if (type == FIN ) {
		return "Fin";
	}
	if (type == SYN ) {
		return "Syn";
	}
	if (type == FINSYN ) {
		return "FinSyn";
	}
	if (type == RST ) {
		return "Rst";
	}
	if (type == FINRST ) {
		return "FinRst";
	}
	if (type == SYNRST ) {
		return "SynRst";
	}
	if (type == FINSYNRST ) {
		return "FinSynRst";
	}
	if (type == PSH ) {
		return "Psh";
	}
	if (type == FINPSH ) {
		return "FinPsh";
	}
	if (type == SYNPSH ) {
		return "SynPsh";
	}
	if (type == FINSYNPSH ) {
		return "FinSynPsh";
	}
	if (type == RSTPSH ) {
		return "RstPsh";
	}
	if (type == FINRSTPSH ) {
		return "FinRstPsh";
	}
	if (type == SYNRSTPSH ) {
		return "SynRstPsh";
	}
	if (type == FINSYNRSTPSH ) {
		return "FinSynRstPsh";
	}
	if (type == ACK ) {
		return "Ack";
	}
	if (type == FINACK ) {
		return "FinAck";
	}
	if (type == SYNACK ) {
		return "SynAck";
	}
	if (type == FINSYNACK ) {
		return "FinSynAck";
	}
	if (type == RSTACK ) {
		return "RstAck";
	}
	if (type == FINRSTACK ) {
		return "FinRstAck";
	}
	if (type == SYNRSTACK ) {
		return "SynRstAck";
	}
	if (type == FINSYNRSTACK ) {
		return "FinSynRstAck";
	}
	if (type == PSHACK ) {
		return "PshAck";
	}
	if (type == FINPSHACK ) {
		return "FinPshAck";
	}
	if (type == SYNPSHACK ) {
		return "SynPshAck";
	}
	if (type == FINSYNPSHACK ) {
		return "FinSynPshAck";
	}
	if (type == RSTPSHACK ) {
		return "RstPshAck";
	}
	if (type == FINRSTPSHACK ) {
		return "FinRstPshAck";
	}
	if (type == SYNRSTPSHACK ) {
		return "SynRstPshAck";
	}
	if (type == FINSYNRSTPSHACK ) {
		return "FinSynRstPshAck";
	}
	//std::cout << "Exiting TypeToStr" << std::endl;
	return "Invalid";
}

uint8_t* Message::EncMsgOffset() {
	//std::cout << "Entering EncMsgOffset" << std::endl;
	if (type == BASEMESSAGE)
			return NULL;
	if (type == FIN)
			return NULL;
	if (type == SYN)
			return NULL;
	if (type == FINSYN)
			return NULL;
	if (type == RST)
			return NULL;
	if (type == FINRST)
			return NULL;
	if (type == SYNRST)
			return NULL;
	if (type == FINSYNRST)
			return NULL;
	if (type == PSH)
			return NULL;
	if (type == FINPSH)
			return NULL;
	if (type == SYNPSH)
			return NULL;
	if (type == FINSYNPSH)
			return NULL;
	if (type == RSTPSH)
			return NULL;
	if (type == FINRSTPSH)
			return NULL;
	if (type == SYNRSTPSH)
			return NULL;
	if (type == FINSYNRSTPSH)
			return NULL;
	if (type == ACK)
			return NULL;
	if (type == FINACK)
			return NULL;
	if (type == SYNACK)
			return NULL;
	if (type == FINSYNACK)
			return NULL;
	if (type == RSTACK)
			return NULL;
	if (type == FINRSTACK)
			return NULL;
	if (type == SYNRSTACK)
			return NULL;
	if (type == FINSYNRSTACK)
			return NULL;
	if (type == PSHACK)
			return NULL;
	if (type == FINPSHACK)
			return NULL;
	if (type == SYNPSHACK)
			return NULL;
	if (type == FINSYNPSHACK)
			return NULL;
	if (type == RSTPSHACK)
			return NULL;
	if (type == FINRSTPSHACK)
			return NULL;
	if (type == SYNRSTPSHACK)
			return NULL;
	if (type == FINSYNRSTPSHACK)
			return NULL;
	//std::cout << "Exiting EncMsgOffset" << std::endl;
	return NULL;
}

