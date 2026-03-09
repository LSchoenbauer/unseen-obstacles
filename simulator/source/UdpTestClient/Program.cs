using System;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;

class UdpIntClient
{
    const string pingMsg = "PING";
    const string pingResponse = "PONG";

    const int fps = 30;
    const int heartbeatInterval = 1000; // ms

    string targetIp = "192.168.7.3";
    int targetPort = 4210;
    bool connectionEstablished = false;
    long lastHeartbeatSentMs = 0;

	readonly UdpClient udp = new();
    int dataCounter = 100;


    void WaitForHeartbeat()
    {
        byte[] msgData = Encoding.UTF8.GetBytes(pingMsg);

        do 
        {
            udp.Send(msgData, msgData.Length, targetIp, targetPort);
            lastHeartbeatSentMs = Environment.TickCount64;
            Console.WriteLine($"[{lastHeartbeatSentMs,13:#,#.}] Sent heartbeat '{pingMsg}', waiting for '{pingResponse}' ...");

            try {
                IPEndPoint remoteEP = new(IPAddress.Any, 0);
                byte[] rspData = udp.Receive(ref remoteEP);
                string response = Encoding.ASCII.GetString(rspData, 0, rspData.Length);
                Console.WriteLine($"... received '{response}' from {remoteEP.Address}:{remoteEP.Port}");
                bool isConnected = response == pingResponse;
                if (isConnected != connectionEstablished)
                {
                    Console.WriteLine(isConnected ? "--> Connection established!" : "--> Connection lost.");
                    connectionEstablished = isConnected;
                }
            }
            catch (SocketException ex) 
            {
                if (ex.SocketErrorCode == SocketError.TimedOut)
                {
                    Console.WriteLine("... no response received, retrying.");
                    if (connectionEstablished)
                    {
                        Console.WriteLine("--> Connection lost.");
                        connectionEstablished = false;
                    }
                }
                else
                {
                    Console.WriteLine($"Socket error: {ex.Message}");
                }
            }
            long retryDurationMs =  + heartbeatInterval - (Environment.TickCount64 - lastHeartbeatSentMs);
            if (!connectionEstablished && (retryDurationMs > 10)) 
            {
                Thread.Sleep((int)retryDurationMs);
            }
        }
        while (!connectionEstablished);

        lastHeartbeatSentMs = Environment.TickCount64;
    }

    void SendData()
    {
        byte[] data = new byte[8 * sizeof(UInt32)]; // 8 Integers = 32 Bytes
        for (int i = 0; i < 8; i++)
        {
            byte[] intBytes = BitConverter.GetBytes(dataCounter + i);
            Array.Copy(intBytes, 0, data, i * 4, 4);
        }
        udp.Send(data, data.Length, targetIp, targetPort);

        Console.Write($"[{Environment.TickCount64,13:#,#.}]Sent data: ");
        for (int i = 0; i < 8; i++) Console.Write($"{BitConverter.ToUInt32(data, i * 4)}, ");
        Console.WriteLine();

        dataCounter += 10;
        if (dataCounter > 2000)
        {
            dataCounter = 100;
        }
    }

    void Run()
    {
        udp.Client.ReceiveTimeout = heartbeatInterval;
        Console.WriteLine($"Started UDP Test Client -> {targetIp}:{targetPort}");

        WaitForHeartbeat();
        while (connectionEstablished)
        {
            long startMs = Environment.TickCount64;
            SendData();
            if (startMs > (lastHeartbeatSentMs + heartbeatInterval)) 
            {
                WaitForHeartbeat();
            }
            long elapsedMs = Environment.TickCount64 - startMs;
            int sleepTime = (1000 / fps) - (int)elapsedMs;
            if (sleepTime > 1)
            {
                Thread.Sleep(sleepTime);
            }
        }
    }

    static void Main()
    {
        UdpIntClient client = new();
        client.Run();
    }
}