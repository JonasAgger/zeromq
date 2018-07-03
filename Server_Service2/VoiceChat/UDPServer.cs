using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Reflection;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using NAudio.CoreAudioApi;

namespace VoiceChat
{
    class UDPServer
    {
        private UdpClient client;
        private List<UDPServerNode> connected = new List<UDPServerNode>();
        private int _port;
        private int _enumport;
        public UDPServer(int port)
        {
            this._port = port;
            _enumport = port;
            ThreadPool.QueueUserWorkItem(ListenerThread, _port);
        }

        private void ListenerThread(object state)
        {
            while (true)
            {
                client = new UdpClient(new IPEndPoint(IPAddress.Any, _port));
                try
                {
                    var b = Task.Run<UdpReceiveResult>(async () => await client.ReceiveAsync() );

                    b.Wait();

                    Console.WriteLine("added new from ip : {0}", b.Result.RemoteEndPoint.Address);
                    connected.Add(new UDPServerNode(++_enumport, Received));

                    var data = Encoding.UTF8.GetBytes(_enumport.ToString());

                    client.Send(data, data.Length, b.Result.RemoteEndPoint);
                    client.Close();
                }
                catch (SocketException)
                {
                    // usually not a problem - just means we have disconnected
                }
            }
        }

        protected void Received(byte[] audioData, UDPServerNode node)
        {
            foreach (var serverNode in connected)
            {
                if (node == serverNode) continue;
                serverNode.Send(audioData);
            }
        }
    }

    class UDPServerNode
    {
        private UdpClient client;
        private Action<byte[], UDPServerNode> _handler;

        public UDPServerNode(int port, Action<byte[], UDPServerNode> handler)
        {
            client = new UdpClient(new IPEndPoint(IPAddress.Any, port));
            ThreadPool.QueueUserWorkItem(ListenerThread, port);
            _handler = handler;
        }

        private void ListenerThread(object state)
        {
            do
            {
                try
                {
                    var b = client.ReceiveAsync();

                    b.Wait();
                    _handler?.Invoke(b.Result.Buffer, this);
                }
                catch (SocketException)
                {
                    // usually not a problem - just means we have disconnected
                }
            } while (client.Client.Connected);
        }

        public void Send(byte[] audioData)
        {
            client.Send(audioData, audioData.Length);
        }
    }
}
