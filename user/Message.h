#ifndef MESSAGE_H
#define MESSAGE_H

#include <openssl/rsa.h>

#include <string>

class Message
{
	private:
		std::string _sender;
		std::string _receiver;
		std::string _content;
		RSA* _senderPrivKey;
		RSA* _receiverPubKey;

	public:
		Message(const std::string& sender, RSA* senderPrivateKey, const std::string& receiver, RSA* receiverPublicKey, const std::string& content);

		static Message* retrieveMessage(const std::string& xmlMessage, RSA* senderPublicKey, RSA* receiverPrivateKey);
		std::string toXml() const;

		std::string getContent() const {return _content;}
		std::string getSender() const {return _sender;}
		std::string getReceiver() const {return _receiver;}
};

#endif //!MESSAGE_H
