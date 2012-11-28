#ifndef MESSAGE_H
#define MESSAGE_H

#include <openssl/rsa.h>

#include <string>

class Message
{
	private:
		char* _sender;
		char* _receiver;
		char* _content;
		RSA* _senderPrivKey;
		RSA* _receiverPubKey;

	public:
		Message(const char* sender, const RSA* senderPrivateKey, const char* receiver, const RSA* receiverPublicKey, const char* content);
		~Message();

		static Message* retrieveMessage(const char* xmlMessage);
		const char* toXml() const;

		const char* getContent() const {return _content;}
		const char* getSender() const {return _sender;}
		const char* getReceiver() const {return _receiver;}
};

#endif //!MESSAGE_H
