#include <iostream>
#include <sstream>
#include <cstring>
#include <cstdio>

#include <libxml/tree.h>
#include <libxml/xpath.h>

#include <openssl/pem.h>
#include <openssl/sha.h>
#include <openssl/blowfish.h>

#include "../common/Key.h"
#include "Message.h"
#include "hex_code.h"

Message::Message(const std::string& sender, RSA* senderPrivateKey, const std::string& receiver, RSA* receiverPublicKey, const std::string& content) {
	_sender = sender;
	_senderPrivKey = senderPrivateKey;
	_receiver = receiver;
	_receiverPubKey = receiverPublicKey;
	_content = _content;
}

Message* Message::retrieveMessage(const std::string& xmlMessage, RSA* senderPublicKey, RSA* receiverPrivateKey) {
	xmlDoc *doc;
	xmlXPathContext *xpathCtx;
	xmlXPathObject *xpathObj;

  	doc = xmlNewDoc((const xmlChar*) xmlMessage.c_str());
  	xpathCtx = xmlXPathNewContext (doc);

  	xpathObj = xmlXPathEvalExpression ((xmlChar *) "/EnvXml/From", xpathCtx);
	std::string sender((char*) xmlNodeListGetString(doc, xpathObj->nodesetval->nodeTab[0]->xmlChildrenNode, 1));

	xpathObj = xmlXPathEvalExpression ((xmlChar *) "/EnvXml/To", xpathCtx);
	std::string receiver((char*) xmlNodeListGetString(doc, xpathObj->nodesetval->nodeTab[0]->xmlChildrenNode, 1));

  	xpathObj = xmlXPathEvalExpression ((xmlChar *) "/EnvXml/Sign", xpathCtx);
	char * signature = (char*) xmlNodeListGetString(doc, xpathObj->nodesetval->nodeTab[0]->xmlChildrenNode, 1);

	xpathObj = xmlXPathEvalExpression ((xmlChar *) "/EnvXml/MsgXml", xpathCtx);
	const char * msgXml = (char*) xmlNodeListGetString(doc, xpathObj->nodesetval->nodeTab[0]->xmlChildrenNode, 1);

	xpathObj = xmlXPathEvalExpression ((xmlChar *) "/EnvXml/MsgCrypt", xpathCtx);
	const char * content = (const char*) xmlNodeListGetString(doc, xpathObj->nodesetval->nodeTab[0]->xmlChildrenNode, 1);

	xpathObj = xmlXPathEvalExpression ((xmlChar *) "/EnvXml/Token", xpathCtx);
	char * token = (char*) xmlNodeListGetString(doc, xpathObj->nodesetval->nodeTab[0]->xmlChildrenNode, 1);

	// ------ checking hash
	unsigned char contentHash[20];
	SHA1(reinterpret_cast<const unsigned char*>(msgXml), strlen(msgXml), contentHash);

	unsigned char hash[20];
	RSA_public_decrypt(128, reinterpret_cast<unsigned char*>(signature), hash, senderPublicKey, RSA_PKCS1_PADDING);

	if(!strncmp((char*)hash, (char*)contentHash, 20)) {
		std::cerr << "Bad signature!" << std::endl;
		throw std::exception();
	}

	

	return new Message(sender, NULL, receiver, NULL, "");
}


std::string Message::toXml() const {
	const char* content = _content.c_str();
	int messageSize = strlen(content);

	// -------- encrypt content with blowfish key
	// generate blowfish key to encrypt the message
	unsigned char* key = Key(16).getKey();
	unsigned char encryptedMessage[messageSize];
	
	BF_KEY *blowKey = new BF_KEY;
	BF_set_key(blowKey, 16, key);
	unsigned char ivec[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	int num = 0;
	BF_cfb64_encrypt(reinterpret_cast<const unsigned char*>(content), encryptedMessage, messageSize, blowKey, ivec, &num, BF_ENCRYPT);

	// -------- encrypt blowfish key with receiver public key
	//encrypting blowfish key
	int pbSize = RSA_size(_receiverPubKey);
	unsigned char encryptedbfk[pbSize];
	RSA_public_encrypt(pbSize, key, encryptedbfk, _receiverPubKey, RSA_PKCS1_OAEP_PADDING);
	
	//getting hash for
	unsigned char hash[20];
	std::stringstream ss;
	ss << "<Token>" << hex_encode(encryptedbfk, pbSize) << "</Token>" << "<MsgCrypt>" << hex_encode(encryptedMessage, messageSize) << "</MsgCrypt>";
	std::string toHash = ss.str(); 
	SHA1(reinterpret_cast<const unsigned char*>(toHash.c_str()), toHash.size(), hash);

	// creating encrypting hash with sender private key -> signature
	unsigned char signature[128];
	RSA_private_encrypt(20, hash, signature, _senderPrivateKey, RSA_PKCS1_PADDING);

	delete key;

	ss.str("");
	ss << "<EnvXml>";
		ss << "<From>" << _sender << "</From>";
		ss << "<To>" << _receiver << "</To>";
		ss << "<Sign>" << hex_encode(signature, 128) << "</Sign>";
		ss << "<MsgXml>";
			ss << "<Token>" << hex_encode(encryptedbfk, pbSize) << "</Token>";
			ss << "<MsgCrypt>" << hex_encode(encryptedMessage, messageSize) << "</MsgCrypt>";
		ss << "</MsgXml>";
	ss << "</EnvXml>";

	return ss.str();
}
