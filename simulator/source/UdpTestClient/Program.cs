using System;
using System.Net;
using System.Net.Sockets;
using System.Threading;

class UdpIntClient
{
    static void Main()
    {
        string targetIp = "192.168.7.3";
        int targetPort = 4210;

        using UdpClient client = new();

        Console.WriteLine($"Started UDP Test Client -> {targetIp}:{targetPort}");

        int counter = 100;

        while (true)
        {
            byte[] data = new byte[8 * 4]; // 8 Integers = 32 Bytes

            for (int i = 0; i < 8; i++)
            {
                byte[] intBytes = BitConverter.GetBytes(counter + i);

                // in Buffer kopieren
                Array.Copy(intBytes, 0, data, i * 4, 4);
            }

            client.Send(data, data.Length, targetIp, targetPort);

            Console.Write("Sent: ");
            for (int i = 0; i < 8; i++)
                Console.Write($"{counter + i} ");

            Console.WriteLine();

            counter += 10;

            Thread.Sleep(1000);
        }
    }
}