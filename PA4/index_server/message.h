#ifndef MESSAGE_H
#define MESSAGE_H

#include <cstdint>


/*
 *    MessageID used for processing different messages from the client
 *
 *    QUERY        : Query the index server for a file and return the list of peers who have
 *    REGISTER     : Register peer with the index server
 *    UNREGISTER   : Unregister peer with the index server
 *    ADD_FILE     : Add a file to the list of files the peer has
 *    DELETE_FILE  : Error. For now when the file does not exist on the server
 *    COMPLETED    : Used to tell the peer the request has been completed
 *    UPDATE       : Used to update the index server with peers list of files
 *    ERROR        : Send errors back and forth to peer and index server
 */
enum MessageId : uint16_t
{
    QUERY = 0,
    REGISTER,
    UNREGISTER,
    ADD_FILE,
    DELETE_FILE,
    DOWNLOAD,
    COMPLETED,
    UPDATE,
    ERROR,
};
#pragma pack(2)
struct Message
{
    MessageId mess_id;     // type of message
    uint16_t peer_id;      // peer node id: 0 for unregistered, > 0 registered
    uint16_t size;         // size of message
    uint16_t mess_in_seq;  // num in sequence
    uint16_t total_mess;   // total number of messages sent
    uint16_t offset;       // offset within the file
    uint64_t filesize;     // filesize used for downloading
    char body[10000];       // the message
};
#define CHUCK_SIZE 10000
#endif