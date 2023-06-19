//
// Created by william on 2/20/23.
//

#ifndef CLIENT_MESSAGE_H
#define CLIENT_MESSAGE_H
#include <cstdint>
#include <openssl/sha.h>

/*
 *    MessageID used for processing different messages from the client
 *
 *    LIST_FILES   : To list all files on the server
 *    SINGLE_DL    : For a single file download
 *    MULTI_SEQ_DL : For sequential mulitple file downloads
 *    MULTI_PARA_DL: For parallel multiple file downloads
 *    ERR_FILE     : Error. For now when the file does not exist on the server
 *    FINISH       : End of current request
 */
enum MessageID : uint16_t
{
    LIST_FILES = 0,
    SINGLE_DL,
    ADD_FILE,
    DELETE_FILE,
    APPEND_FILE,
    REPLACE_FILE,
    // MULTI_SEQ_DL,       // Didn't need this. Can be handled on client side
    // MULTI_PARA_DL,      // This option was removed considering the server would not know that the client is downloading
    ERR_FILE,             // ^ in parallel it would just use the sockets and multiple threads to download in parallel
    FINISH             // Didn't need it since the total bytes transfered is in the Message struct
};

/*
 *   Message used for passing messages in the form of packets from the client
 *
 *   MessageID message: Type of request by the client
 *   uint16_t  size   : Total size of the incoming request
 *   char body [1024] : Can be the whole message or part of a message being passed
 */
struct Message
{
    MessageID message;                      // 2 bytes
    uint16_t size;                          // 2 bytes
    char checksum[SHA256_DIGEST_LENGTH];    // 32 bytes
    char body[1024];                           // variable bytes
};
#endif //CLIENT_MESSAGE_H
