#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <fcntl.h>          // Needed for AllocConsole()
#include <winsock2.h>
#include <vector>
#include <string>
#include "..\Server\networkCom.h" //use same as for Server

#define WM_WSAASYNC (WM_USER +5)

using namespace std;

networkCom g_NetCom;//Object for all network communication

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
    WNDCLASSEX wcex;
    HWND hwnd;
    MSG msg;
    BOOL bQuit = FALSE;

    /* register window class */
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_OWNDC;
    wcex.lpfnWndProc = WindowProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = "Client";
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);;


    if (!RegisterClassEx(&wcex))
        return 0;

    /* create main window */
    hwnd = CreateWindowEx(0,
                          "Client",
                          "Other Window",
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT,
                          CW_USEDEFAULT,
                          256,
                          256,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);

    ShowWindow(hwnd, nCmdShow);


    //Open a console window
    AllocConsole();
    //Connect console output
    HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);
    int hCrt          = _open_osfhandle((long) handle_out, _O_TEXT);
    FILE* hf_out      = _fdopen(hCrt, "w");
    setvbuf(hf_out, NULL, _IONBF, 1);
    *stdout = *hf_out;
    //Connect console input
    HANDLE handle_in = GetStdHandle(STD_INPUT_HANDLE);
    hCrt             = _open_osfhandle((long) handle_in, _O_TEXT);
    FILE* hf_in      = _fdopen(hCrt, "r");
    setvbuf(hf_in, NULL, _IONBF, 128);
    *stdin = *hf_in;
    //Set console title
    SetConsoleTitle("Client - Console");


    if(!g_NetCom.init("client"))
    {   //Error
        cout<<"Problem with Network\n";
    }
    else cout<<"Network initialized\n";

    WSAAsyncSelect( g_NetCom.get_server_socket() , hwnd, WM_WSAASYNC, FD_READ | FD_WRITE | FD_ACCEPT | FD_CONNECT | FD_CLOSE);

    string IP_num="192.168.0.100";
    int port=5001;
    cout<<"Hit [Enter] to connect to "<<IP_num<<endl;
    system("PAUSE");

    if(!g_NetCom.connect_to_server(IP_num,port) )
    {
        cout<<"Connection Failed\n";
    }
    else cout<<"Trying to connect to Server...\n";

    if(g_NetCom.broadcast_my_ip())
    {
        cout<<"broadcasted my IP\n";
    }
    else cout<<"error while broadcasting my IP\n";


    /* program main loop */
    while (!bQuit)
    {
        /* check for messages */
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            /* handle or dispatch messages */
            if (msg.message == WM_QUIT)
            {
                bQuit = TRUE;
            }
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
            /* OpenGL animation code goes here */
            if(g_NetCom.check_for_broadcast_reply())
            {
                //got reply from server
                string IP_and_port;
                g_NetCom.get_server_IP_and_port(IP_and_port);
                cout<<"Server could be joined at: "<<IP_and_port<<endl;
            }
            Sleep(2000);
        }
    }


    /* destroy the window explicitly */
    DestroyWindow(hwnd);

    return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_CLOSE:
            PostQuitMessage(0);
        break;

        case WM_DESTROY:
            return 0;

        case WM_KEYDOWN:
        {
            switch (wParam)
            {
                case VK_ESCAPE:
                    PostQuitMessage(0);
                break;
            }
        }
        break;

        case WM_WSAASYNC:
        {
            cout<<"WM_WSAASYNC\n";
            // what word?
            switch(WSAGETSELECTEVENT(lParam))
            {
                case FD_READ:
                {
                    cout<<"FD_READ\n";

                    //RECV either a string or a float*

                    //RECV a string
                    string buffer;
                    g_NetCom.recv_data(buffer);
                    //output
                    cout<<buffer<<endl;

                    //RECV a float* or float array
                    /*float buffer[256];
                    g_NetCom.recv_data(buffer);
                    if(buffer[0]>256)
                    {
                        cout<<"ERROR: Too big package!\n";
                        break;
                    }
                    //output
                    for(int i=0;i<buffer[0];i++) cout<<buffer[i]<<endl;*/

                } break;

                case FD_WRITE:
                {
                    cout<<"FD_WRITE\n";
                } break;

                case FD_CONNECT:// Client is now connected to server
                {
                    cout<<"FD_CONNECT\n";

                    if(g_NetCom.test_connection())
                    {
                        cout<<"You are now connected to the server\n";
                    }
                    else//not connected
                    {
                        cout<<"Could not connect to server\n";
                        break;
                    }

                    //test connection by sending a string or a float*

                    /*//SEND a string
                    g_NetCom.send_data("test");

                    //SEND a float* or float[], first element have to indicate number of element (data[0]=sizeof(data)/sizeof(data[0]))
                    float *pData=new float[4];
                    pData[0]=4; pData[1]=0.1; pData[2]=99; pData[3]=684;
                    g_NetCom.send_data(data);
                    delete[] pData;*/

                } break;

                case FD_CLOSE:
                {
                    cout<<"FD_CLOSE\n";

                    g_NetCom.lost_connection();
                    cout<<"Server Lost\n";
                } break;

            }
        }

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}

