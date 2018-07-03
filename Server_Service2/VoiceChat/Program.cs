using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Text;
using System.Threading.Tasks;
using Server_Service2;

namespace VoiceChat
{
    class Program
    {
        static void Main(string[] args)
        {
            /*
            var ip = new IPEndPoint(IPAddress.Parse("18.185.114.115"), 5555);
            var sender = new Sender(ip);

            var data = Encoding.UTF8.GetBytes("hello there");

            while (true)
                if (Console.ReadLine() == "q")
                    break;

            sender.Send(data);
            data = Encoding.UTF8.GetBytes("how you doin");

            while (true)
                if (Console.ReadLine() == "q")
                    break;

            sender.Send(data);
            */
            var ip = new IPEndPoint(IPAddress.Parse("18.185.114.115"), 5555);
            var receiver = new Receiver(ip, ReadBytes);

            while (true)
                if (Console.ReadLine() == "q")
                    break;

            // Sender
            /*
            var ip = new IPEndPoint(IPAddress.Parse("18.185.114.115"), 5555);
            var sender = new NetworkAudioSender(0, new Sender(ip));
            while (true)
                if (Console.ReadLine() == "q")
                    break;
            sender.Dispose();
            */

            // Receiver
            /*
            var ip = new IPEndPoint(IPAddress.Parse("18.185.114.115"), 5555);
            var player = new NetworkAudioPlayer(new Receiver(ip));
            while (true)
                if (Console.ReadLine() == "q")
                    break;
            player.Dispose();
            */

            // Server
            /*
            var server = new UDPServer(5555);

            while (true)
            {
                if (Console.ReadLine() == "q") break;
            }
            */
        }

        public static void ReadBytes(byte[] bytes)
        {
            Console.WriteLine(Encoding.UTF8.GetString(bytes));
        }
    }
}
