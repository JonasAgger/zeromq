using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Text;
using System.Threading.Tasks;
using NAudio.Wave;
using Server_Service2;

namespace VoiceChat
{
    class Program
    {
        static void Main(string[] args)
        {
            
            // Client
            
            for (var i = 0; i < WaveIn.DeviceCount; i++)
            {
                Console.WriteLine(WaveIn.GetCapabilities(i).ProductName);
            }

            var ip = new IPEndPoint(IPAddress.Parse("18.185.114.115"), 5555);
            var sender = new Sender(ip);

            var audioSender = new NetworkAudioSender(0, sender);
            var audioReceiver = new NetworkAudioPlayer(sender);



            while (true)
            {
                var input = Console.ReadLine();
                if (input == "q")
                    break;
                else if (input != null)
                    sender.Send(Encoding.UTF8.GetBytes(input));
            }
            audioReceiver.Dispose();
            audioSender.Dispose();
            sender.Dispose();
            
            
            // Server
            /*
            var server = new UDPServer(5555);

            while (true)
            {
                if (Console.ReadLine() == "q") break;
            }
            server.Dispose();
            */
        }

        public static void ReadBytes(byte[] bytes)
        {
            Console.WriteLine(Encoding.UTF8.GetString(bytes));
        }
    }
}
