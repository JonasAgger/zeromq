﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

using ZeroMQ;

namespace Server_Service2
{
    class Program
    {
        static void Main(string[] args)
        {
            // SENDER
            
            var client = new HwClient("tcp://18.185.114.115:5555", ZSocketType.DEALER, BindingType.Connect, "Master Unit");
            var audio = new NetworkAudioSender(0, client);
            while (client.IsRunning())
            {
                while (true)
                {
                    var input = Console.ReadLine();

                    if (input == "q")
                    {
                        client.SetRunning(false);
                        Environment.Exit(0);
                    }
                }
            }


            // PLAYER
            /*
            var client = new HwClient("tcp://18.185.114.115:5555", ZSocketType.DEALER, BindingType.Connect, "Master Unit2");
            var audio = new NetworkAudioPlayer(client);
            while (client.IsRunning())
            {
                while (true)
                {
                    var input = Console.ReadLine();

                    if (input == "q")
                    {
                        client.SetRunning(false);
                        Environment.Exit(0);
                    }
                }
            }
            */

            //SERVER
            /*
            var server = new HwServer("tcp://*:5555", ZSocketType.ROUTER, BindingType.Bind);
            while (server.IsRunning())
            {
                while (true)
                {
                    var input = Console.ReadLine();

                    if (input == "q")
                        Environment.Exit(0);
                    else
                    {
                        server.SendString("Master Unit", "str", input);
                    }
                }
            }
            */
        }
    }
}
