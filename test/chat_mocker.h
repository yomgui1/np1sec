/**
   This is a mock/simulator of a multi party chat protocol runing on the
   same machine for the purpose of testing np1sec 

   Authors: Vmon, 2015-01: initial version
 */

#include <map>
#include <vector>

#ifndef TEST_CHAT_MOCKER_H
#define CHAT_MOCKER_H
/**
   This class store the information about different participants
   including their receive and send functions 
 */

class MockParticipant {
 public:
  std::string nick;
  void* aux_data;  // the data to send to receive_handler
  void (*receive_handler)(std::string room_name,
                          std::string message,
                          void* aux_data);
};

class MockRoom {
 protected:
  uint64_t global_message_id = 0;
  std::string name;
  std::map<std::string, MockParticipant> _participant_list;

  void broadcast(std::string message) {
    for (std::map<std::string, MockParticipant>::iterator
       cur_participant = _participant_list.begin();
       cur_participant != _participant_list.end(); cur_participant++)
      (*(cur_participant->second).receive_handler)(name, message,
                             (cur_participant->second).aux_data);
  }

 public:
  void join(std::string nick,
            void (*receive_handler)(std::string room_name,
                                    std::string message,
                                    void* user_data)) {
      _participant_list[nick].nick = nick;
      _participant_list[nick].receive_handler = receive_handler;
      // _participant_list[nick].aux_data = user_data;
      broadcast(":o?JOIN:o?"+nick);
    }

  std::vector<std::string>  participant_list()  {
    std::vector<std::string> participant_nicks;
      for (std::map<std::string, MockParticipant>::iterator
          cur_participant = _participant_list.begin();
          cur_participant != _participant_list.end(); cur_participant++)
          participant_nicks.push_back((cur_participant->second).nick);

      return participant_nicks;
  }

  void leave(std::string nick) {
      _participant_list.erase(nick);
      broadcast(":o?LEAVE:o?" + nick);
    }

  void send(std::string sender_nick, std::string message) {
      broadcast(":o?SEND:o?"+
               std::to_string(global_message_id)+
               ":o?"+sender_nick+":o?"+message);
    }
};

/**
   This client simulate both client and server.
   as everythnig happens locally
 */
class ChatMocker {
 protected:
  std::map<std::string, MockRoom> rooms;
  std::map<std::string, MockParticipant> signed_in_participant;

 public:
  /**
   * Add the participant to the singed in list and keep track of their
   * receive handler 
   */
  void sign_in(std::string nick,
               void (*receive_handler)(std::string room_name,
                                       std::string message,
                                       void* user_data),
               void* user_data) {
    signed_in_participant[nick].nick = nick;
    signed_in_participant[nick].receive_handler = receive_handler;
    signed_in_participant[nick].aux_data = user_data;
  }

  /**
   * join the room by adding the name of the participant to the room list
   */
  void join(std::string room, std::string nick) {
    rooms[room].join(nick, signed_in_participant[nick].receive_handler);
  }

  /**
     return the list of participant in the room
     it makes the base for the list of participant in the room
   */
  std::vector<std::string> participant_list(std::string room) {
    return rooms[room].participant_list();
  }

  /**
   * drop the participant from the room
   */
  void leave(std::string room, std::string nick) {
    rooms[room].leave(nick);
  }

  /**
   * drop the participant from the room
   */
  void send(std::string room, std::string nick, std::string message) {
    // global_message_id++;
    rooms[room].send(nick, message);
  }
};

#endif  // TEST_CHAT_MICKER_H_