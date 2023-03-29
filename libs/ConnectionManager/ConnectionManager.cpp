#include "ConnectionManager.h"


ConnectionManager::ConnectionManager() {
	this->reconnection_attempts = 0;
}

void ConnectionManager::resetReconnectionAttempts() {
	this->reconnection_attempts = 0;
}

void ConnectionManager::reconnect() {
	static unsigned long lmillis = millis();

	if (this->connected()) {
		this->resetReconnectionAttempts();
		return;
	}

	if (this->reconnection_attempts >= RECONNECT_ATTEMPTS) {
		return this->retryReconnection();
	}
	
	if ((millis() - lmillis) >= WAITING_TO_RETRY_RECONNECT_IN_MS) {
		this->connect();
		this->reconnection_attempts++;
		lmillis = millis();
	}
}

void ConnectionManager::retryReconnection() {
	static unsigned long lmillis = millis();

	if ((millis() - lmillis) >= RETRY_RECONNECT_IN_MS) {
		this->resetReconnectionAttempts();
		lmillis = millis();
	}
}