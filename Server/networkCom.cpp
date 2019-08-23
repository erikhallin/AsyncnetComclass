#include "networkCom.h"

networkCom::networkCom()
{
    m_ready=false;
}

bool networkCom::init(string status)
{
    //Init
    m_ready=false;
    m_connected_to_server=false;
    m_clients_counter=0;

    if(status=="server")
    {
        m_net_status=net_server;
        m_ServerCom=serverCom();
        m_ServerCom.init();
        m_ready=true;
    }

    if(status=="client")
    {
        m_net_status=net_client;
        m_ClientCom=clientCom();
        m_ClientCom.init();
        m_ready=true;
    }

    return m_ready;
}

bool networkCom::connect_to_server(string sIP,int port)
{
    if(m_net_status==net_client)
    {
        if(m_ClientCom.set_IP_and_connect(sIP,port)) return true;
    }

    return false;
}

bool networkCom::set_port_and_bind(int port)
{
    if(m_net_status==net_server)
    {
        if(m_ServerCom.set_port_and_bind(port)) return true;
    }

    return false;
}

bool networkCom::start_to_listen(int backLog)
{
    if(m_net_status==net_server)
    {
        if(m_ServerCom.start_to_listen(backLog)) return true;
    }

    return false;
}

bool networkCom::set_broadcast_port(int port_sending,int port_replying)
{
    if(m_net_status==net_server)
    {
        if(m_ServerCom.set_broadcast_port(port_sending, port_replying)) return true;
    }

    return false;
}
bool networkCom::set_broadcast_port(string net, int port_sending,int port_replying)
{
    if(m_net_status==net_client)
    {
        if(m_ClientCom.set_broadcast_net_and_port(net, port_sending, port_replying)) return true;
    }

    return false;
}

bool networkCom::test_connection(void)
{
    if(m_net_status==net_client)
    {
        if(m_ClientCom.test_connection())
        {
            m_connected_to_server=true;
            return true;
        }
        else return false;
    }

    return false;
}

bool networkCom::known_socket(SOCKET new_socket)
{
    if(m_net_status==net_server)
    {
        if(m_ServerCom.known_socket(new_socket)) return true;
    }

    return false;
}

bool networkCom::broadcast_my_ip(void)
{
    if(m_net_status==net_client)
    {
        if(m_ClientCom.broadcast_my_ip()) return true;
    }

    return false;
}

bool networkCom::check_for_broadcast(void)
{
    if(m_net_status==net_server)
    {
        if(m_ServerCom.check_for_broadcast()) return true;
    }

    return false;
}

bool networkCom::check_for_broadcast_reply(void)
{
    if(m_net_status==net_client)
    {
        if(m_ClientCom.check_for_broadcast_reply()) return true;
    }

    return false;
}

bool networkCom::get_server_IP_and_port(string& IP_and_port)
{
    if(m_net_status==net_client)
    {
        if(m_ClientCom.get_server_IP_and_port(IP_and_port)) return true;
    }

    return false;
}

bool networkCom::send_data(string data_string)
{
    if(m_net_status==net_server)
    {
        m_ServerCom.send_data(data_string);
        return true;
    }

    if(m_net_status==net_client)
    {
        m_ClientCom.send_data(data_string);
        return true;
    }

    return false;
}

bool networkCom::send_data(float* data_array)
{
    if(m_net_status==net_server)
    {
        m_ServerCom.send_data(data_array);
        return true;
    }

    if(m_net_status==net_client)
    {
        m_ClientCom.send_data(data_array);
        return true;
    }

    return false;
}

bool networkCom::send_data(string data_string,SOCKET SocReceiver)
{
    if(m_net_status==net_server)
    {
        if(!m_ServerCom.send_data(data_string)) return false;
        return true;
    }

    return false;
}

bool networkCom::send_data(float* data_array,SOCKET SocReceiver)
{
    if(m_net_status==net_server)
    {
        if(!m_ServerCom.send_data(data_array)) return false;
        return true;
    }

    return false;
}

bool networkCom::recv_data(string& data_string)
{
    if(m_net_status==net_client)
    {
        if(m_ClientCom.recv_data(data_string)) return true;
    }

    return false;
}

bool networkCom::recv_data(float* data_array)
{
    if(m_net_status==net_client)
    {
        if(m_ClientCom.recv_data(data_array)) return true;
    }

    return false;
}

bool networkCom::recv_data(string& data_string,SOCKET SocSender)
{
    if(m_net_status==net_server)
    {
        m_ServerCom.recv_data(data_string,SocSender);
        return true;
    }

    return false;
}

bool networkCom::recv_data(float* data_array,SOCKET SocSender)
{
    if(m_net_status==net_server)
    {
        m_ServerCom.recv_data(data_array,SocSender);
        return true;
    }

    return false;
}

bool networkCom::add_client(SOCKET new_client)
{
    if(m_net_status==net_server)
    {
        if(m_ServerCom.add_client(new_client))
        {
            m_clients_counter++;
            return true;
        }
    }

    return false;
}

bool networkCom::remove_client(SOCKET client_to_remove)
{
    if(m_net_status==net_server)
    {
        if(m_ServerCom.remove_client(client_to_remove))
        {
            m_clients_counter--;
            return true;
        }
    }

    return false;
}

SOCKET networkCom::get_server_socket(void)
{
    if(m_net_status==net_server)
    {
        return m_ServerCom.get_server_socket();
    }

    if(m_net_status==net_client)
    {
        return m_ClientCom.get_server_socket();
    }

    return SOCKET();
}

void networkCom::lost_connection(void)
{
    m_connected_to_server=false;
}

