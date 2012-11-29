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
#include "Messenger.h"

using namespace std;

Message::Message(const std::string& sender, RSA* senderPrivateKey, const std::string& receiver, RSA* receiverPublicKey, const std::string& content) {
	_sender = sender;
	_senderPrivKey = senderPrivateKey;
	_receiver = receiver;
	_receiverPubKey = receiverPublicKey;
	_content = content;
}

Message* Message::retrieveMessage(const std::string& xmlMessage, Messenger* messenger) {

    //std::cout << "XML message received : \n" << xmlMessage << std::endl;

	xmlDoc *doc;
	xmlXPathContext *xpathCtx;
    xmlXPathObject *xpathObj;

  	doc = xmlParseDoc((const xmlChar*) xmlMessage.c_str());
  	xpathCtx = xmlXPathNewContext (doc);

  	xpathObj = xmlXPathEvalExpression ((xmlChar *) "/EnvXml/From", xpathCtx);
	std::string sender((char*) xmlNodeListGetString(doc, xpathObj->nodesetval->nodeTab[0]->xmlChildrenNode, 1));

	xpathObj = xmlXPathEvalExpression ((xmlChar *) "/EnvXml/To", xpathCtx);
	std::string receiver((char*) xmlNodeListGetString(doc, xpathObj->nodesetval->nodeTab[0]->xmlChildrenNode, 1));

  	xpathObj = xmlXPathEvalExpression ((xmlChar *) "/EnvXml/Sign", xpathCtx);
	char * signature = (char*) xmlNodeListGetString(doc, xpathObj->nodesetval->nodeTab[0]->xmlChildrenNode, 1);

//    xpathObj = xmlXPathEvalExpression ((xmlChar *) "/EnvXml/MsgXml/text()", xpathCtx);
//	const char * msgXml = (char*) xmlNodeListGetString(doc, xpathObj->nodesetval->nodeTab[0]->xmlChildrenNode, 1);

    xpathObj = xmlXPathEvalExpression ((xmlChar *) "/EnvXml/MsgXml/MsgCrypt", xpathCtx);
    const char * contentHex = (const char*) xmlNodeListGetString(doc, xpathObj->nodesetval->nodeTab[0]->xmlChildrenNode, 1);

    xpathObj = xmlXPathEvalExpression ((xmlChar *) "/EnvXml/MsgXml/Token", xpathCtx);
    char * tokenHex = (char*) xmlNodeListGetString(doc, xpathObj->nodesetval->nodeTab[0]->xmlChildrenNode, 1);

	// ------ checking hash
	unsigned char contentHash[20];
    stringstream ss;
    ss << tokenHex << contentHex;
    SHA1(reinterpret_cast<const unsigned char*>(ss.str().c_str()), strlen(ss.str().c_str()), contentHash);

	unsigned char hash[20];
    Certificate cert = messenger->findUser(sender);
    RSA* senderPublicKey = cert.getPublicKey();
    RSA_public_decrypt(128, reinterpret_cast<unsigned char*>(signature), hash, senderPublicKey, RSA_PKCS1_PADDING);

	if(!strncmp((char*)hash, (char*)contentHash, 20)) {
		std::cerr << "Bad signature!" << std::endl;
		throw std::exception();
	}

    RSA* receiverPrivateKey = messenger->getLocalUser().getPrivateKey();
    char token[128];
    char bfKey[16];
    int msgSize = strlen(contentHex);
    char* content = new char[msgSize/2];
    char* decryptedContent = new char[msgSize/2];
    hex_decode((unsigned char*)tokenHex, 256, (unsigned char*)token);
    hex_decode((unsigned char*)contentHex, msgSize, (unsigned char*)content);
    RSA_private_decrypt(128, (unsigned char*)token, (unsigned char*)bfKey, receiverPrivateKey, RSA_PKCS1_OAEP_PADDING);

    BF_KEY *blowKey = new BF_KEY;
    BF_set_key(blowKey, 16, (unsigned char*)bfKey);

    unsigned char ivec[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    int num = 0;
    BF_cfb64_encrypt(reinterpret_cast<const unsigned char*>(content), (unsigned char*)decryptedContent, msgSize/2, blowKey, ivec, &num, BF_DECRYPT);



    return new Message(receiver, receiverPrivateKey , sender, receiverPrivateKey, decryptedContent);
}


std::string Message::toXml() const {
    int messageSize = _content.size()+1;
    if(messageSize%4 != 0) messageSize += 4 - messageSize%4;
    char* content = new char[messageSize];
    memset(content, 0, messageSize);
    strncpy(content, _content.c_str(), _content.size());

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
    RSA_public_encrypt(16, key, encryptedbfk, _receiverPubKey, RSA_PKCS1_OAEP_PADDING);
	
	//getting hash for
	unsigned char hash[20];
	std::stringstream ss;
    ss << hex_encode((char*)encryptedbfk, pbSize) << hex_encode((char*)encryptedMessage, messageSize);
	std::string toHash = ss.str(); 
	SHA1(reinterpret_cast<const unsigned char*>(toHash.c_str()), toHash.size(), hash);

	// creating encrypting hash with sender private key -> signature
	unsigned char signature[128];
    RSA_private_encrypt(20, hash, signature, this->_senderPrivKey, RSA_PKCS1_PADDING);

	delete key;

	ss.str("");
	ss << "<EnvXml>";
		ss << "<From>" << _sender << "</From>";
		ss << "<To>" << _receiver << "</To>";
        ss << "<Sign>" << hex_encode((char*)signature, 128) << "</Sign>";
		ss << "<MsgXml>";
            ss << "<Token>" << hex_encode((char*)encryptedbfk, pbSize) << "</Token>";
            ss << "<MsgCrypt>" << hex_encode((char*)encryptedMessage, messageSize) << "</MsgCrypt>";
		ss << "</MsgXml>";
	ss << "</EnvXml>";

    return ss.str();

}
