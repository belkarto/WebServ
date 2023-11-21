#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "Multiplexer.hpp"
//# include "Response.hpp"

class Client
{
    public:
        int                                 connect_socket;
        int                                 listen_socket;
        SERVIT                              serverIt;
        char*                               header_buffer;
        std::string                         headers; 
        time_t                              header_timeout;
        bool                                request_line_received;
        bool                                headers_all_recieved;
        bool                                request_all_processed;
        bool                                response_all_sent;
        bool                                response_template_set;
        time_t                              last_activity;
        int                                 keepalive_requests;
        std::map<std::string, std::string>  fields;
        Response		      				response;                          


        Client();
        void	resetState();
        void	setProtocolVersion(std::string &protocol_version);
		void	setMethod(std::string &method);
		void	setHost(std::string &host);
        void	setContentType(std::string &content_type);
		void	setContentLength(std::string &content_length);
		void	setConnection(std::string &connection);
        void	setTransferEncoding(std::string &encoding);
};
#endif
