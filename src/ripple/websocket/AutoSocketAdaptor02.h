/*
 * Copyright (c) 2011, Peter Thorson. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the WebSocket++ Project nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL PETER THORSON BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef WEBSOCKETPP_SOCKET_AUTOSOCKETADAPTOR02_HPP
#define WEBSOCKETPP_SOCKET_AUTOSOCKETADAPTOR02_HPP

#include <ripple/websocket/AutoSocket.h>
#include <beast/asio/placeholders.h>
#include <functional>

namespace ripple {
namespace websocket {

/**
   Adapt an AutoSocket to the websocketpp framework.
 */
template <class endpoint_type>
class AutoSocketAdaptor02 {
public:
    using socket_init_callback = websocketpp_02::socket::socket_init_callback;

    boost::asio::io_service& get_io_service()
    {
        return m_io_service;
    }

    static void handle_shutdown(
        websocket::AutoSocket::Ptr, boost::system::error_code const&)
    {
    }

    AutoSocket::handshake_type get_handshake_type()
    {
        auto isServer = static_cast <endpoint_type*> (this)->is_server();
        using boost::asio::ssl::stream_base;
        return isServer ? stream_base::server : stream_base::client;
    }

    class handler_interface {
    public:
    	virtual ~handler_interface() = default;

        virtual void on_tcp_init() {};
        virtual boost::asio::ssl::context& get_ssl_context () = 0;
        virtual SocketSecurity security() const = 0;
    };

    // Connection specific details
    template <typename connection_type>
    class connection {
    public:
        // should these two be public or protected. If protected, how?
        AutoSocket::lowest_layer_type& get_raw_socket()
        {
            return m_socket_ptr->lowest_layer();
        }

        AutoSocket& get_socket()
        {
            return *m_socket_ptr;
        }

        bool is_secure()
        {
            return m_socket_ptr->isSecure();
        }

        AutoSocket::lowest_layer_type& get_native_socket()
        {
            return m_socket_ptr->lowest_layer();
        }

    protected:
        connection (AutoSocketAdaptor02 <endpoint_type>& e)
                : m_endpoint(e)
                , m_connection(static_cast <connection_type&>(*this))
        {
        }

        void init()
        {
            auto& ssl_context = m_connection.get_handler()->get_ssl_context();

            m_socket_ptr = boost::make_shared <AutoSocket> (
                m_endpoint.get_io_service(),
                ssl_context,
                m_connection.get_handler()->security());
        }

        void async_init (socket_init_callback callback)
        {
            m_connection.get_handler()->on_tcp_init();

            // wait for TLS handshake
            // TODO: configurable value
            m_connection.register_timeout(
                5000, websocketpp_02::fail::status::TIMEOUT_TLS,
                "Timeout on TLS handshake");

            m_socket_ptr->async_handshake(
                m_endpoint.get_handshake_type(),
                std::bind(
                    &connection<connection_type>::handle_init,
                    this,
                    callback,
                    beast::asio::placeholders::error
                )
            );
        }

        void handle_init (socket_init_callback callback,
                          boost::system::error_code const& error)
        {
            m_connection.cancel_timeout();
            callback(error);
        }

        // note, this function for some reason shouldn't/doesn't need to be
        // called for plain HTTP connections. not sure why.
        bool shutdown()
        {
            m_socket_ptr->async_shutdown(
                // Don't block on connection shutdown DJS
                std::bind (
		            &AutoSocketAdaptor02<endpoint_type>::handle_shutdown,
                    m_socket_ptr,
                    beast::asio::placeholders::error
				)
			);

            return true;
        }

    private:
        boost::shared_ptr<boost::asio::ssl::context> m_context_ptr;
        AutoSocket::Ptr m_socket_ptr;
        AutoSocketAdaptor02 <endpoint_type>& m_endpoint;
        connection_type& m_connection;
    };

protected:
    AutoSocketAdaptor02 (boost::asio::io_service& m) : m_io_service(m)
    {
    }

private:
    boost::asio::io_service&    m_io_service;
};

} // websocket
} // ripple

#endif // WEBSOCKETPP_SOCKET_AUTOSOCKETADAPTOR02_HPP
