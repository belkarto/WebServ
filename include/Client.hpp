#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "Multiplexer.hpp"

class Client
{
    public:
        int                                 connect_socket;
        int                                 listen_socket;
        char*                               header_buffer;
        std::string                         headers;
        time_t                              header_timeout;
        bool                                request_line_received;
        bool                                headers_all_recieved;
        bool                                request_all_processed;
        bool								error_occurred; 
        bool                                response_all_sent;
        time_t                              last_activity;
        int                                 keepalive_requests;
        std::map<std::string, std::string>  fields;

        Client();
        void								resetState();
		void								setHost(std::string &host);
        void								setContentType(std::string &content_type);
		void								setContentLength(std::string &content_length);
		void								setConnection(std::string &connection);
};
#endif
