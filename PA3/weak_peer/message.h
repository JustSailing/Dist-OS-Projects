#ifndef MESSAGE_H
#define MESSAGE_H

#include <cstdint>
#include <uuid/uuid.h>


/*
 *    MessageID used for processing different messages from the client
 *
 *    QUERY        : Query the super peer for a file and return the list of peers who have
 *    QUERYHIT     : Query hit from super peer 
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
    QUERYHIT,
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
    uint16_t super_peer;   // not needed will be 0
    uint16_t port;         // used to connect to super peer when query hit
    char     ip[50];       // used to connect to super peer when query hit
    uint16_t peer_id;      // peer node id: 0 for unregistered, > 0 registered
    uint16_t size;         // size of message
    uint16_t mess_in_seq;  // num in sequence
    uint16_t total_mess;   // total number of messages sent
    uint16_t ttl;          // time to live for message
    uint32_t   uq_id;        // uuid
    char     body[1024];   // the message 
};



#endif