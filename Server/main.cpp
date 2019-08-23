#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <fcntl.h>          // Needed for AllocConsole()
#include <winsock2.h> //included in windows.h?
#include <vector>
#include <string>
#include "networkCom.h"

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
    wcex.lpszClassName = "Server";
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);;


    if (!RegisterClassEx(&wcex))
        return 0;

    /* create main window */
    hwnd = CreateWindowEx(0,
                          "Server",
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
    SetConsoleTitle("Server - Console");


    //if(!init_network(hwnd))
    if(!g_NetCom.init("server"))
    {   //Error
        cout<<"Problem with Network\n";
    }
    else cout<<"Network initialized\n";

    if(!g_NetCom.set_port_and_bind(5001))
    {   //Error
        cout<<"Problem with Socket Binding\n";
    }
    else cout<<"Server Port set\n";

    WSAAsyncSelect( g_NetCom.get_server_socket() , hwnd, WM_WSAASYNC, FD_READ | FD_WRITE | FD_ACCEPT | FD_CONNECT | FD_CLOSE);

    if(!g_NetCom.start_to_listen(10))
    {   //Error
        cout<<"Problem with Socket Listening\n";
    }
    else cout<<"Server Socket is now Listening for Clients\n";


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
            g_NetCom.check_for_broadcast();
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

                    /*//RECV a string
                    string buffer;
                    g_NetCom.recv_data(buffer,wParam);
                    //output
                    cout<<buffer<<endl;

                    //RECV a float* or float array
                    float buffer[256];
                    g_NetCom.recv_data(buffer,wParam);
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

                case FD_CONNECT:// not used in Server
                {
                    ;
                } break;

                case FD_ACCEPT:// Client wants to join
                {
                    cout<<"FD_ACCEPT\n";

                    if(g_NetCom.add_client(wParam)) cout<<"New Client Joined\n";
                    else cout<<"Bad Client tried to join\n";

                    //test connection by sending a string or a float* to all clients

                    //SEND a string
                    g_NetCom.send_data("test");

                    //SEND a float* or float[], first element have to indicate number of element (data[0]=sizeof(data)/sizeof(data[0]))
                    /*float *pData=new float[4];
                    pData[0]=4; pData[1]=0.1; pData[2]=99; pData[3]=684;
                    g_NetCom.send_data(pData);
                    delete[] pData;*/

                    return(0);
                } break;

                case FD_CLOSE:
                {
                    cout<<"FD_CLOSE\n";

                    if(g_NetCom.remove_client(wParam))
                    {
                        cout<<"Client Removed\n";
                    }

                } break;

            }
        }


        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}
