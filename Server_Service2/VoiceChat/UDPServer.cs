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

                    Console.WriteLine("{0} connecting", b.Result.RemoteEndPoint.Address);

                    if (connected.Exists(node => Equals(node.GetEndPoint().Address, b.Result.RemoteEndPoint.Address)))
                    {
                        var nodes = connected.RemoveAll(node => Equals(node.GetEndPoint().Address, b.Result.RemoteEndPoint.Address));

                        Console.WriteLine("Removed {0} entries in the list", nodes);
                    }

                    Console.WriteLine("added new from ip : {0}", b.Result.RemoteEndPoint.Address);
                    connected.Add(new UDPServerNode(++_enumport, Received, OnConnectionClosed));

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

        public void Send(string str)
        {
            Received(Encoding.UTF8.GetBytes(str), null);
        }

        protected void OnConnectionClosed(UDPServerNode node)
        {
            connected.Remove(node);
        }

        protected void Received(byte[] audioData, UDPServerNode node)
        {
            foreach (var serverNode in connected)
            {
                if (node == serverNode)
                    continue;

                serverNode.Send(audioData);
            }
        }

        public void Dispose()
        {
            foreach (var serverNode in connected)
                serverNode.Dispose();
            connected = null;
            client.Dispose();
        }
    }

    class UDPServerNode
    {
        private UdpClient client;
        private Action<byte[], UDPServerNode> _handler;
        private Action<UDPServerNode> _onConnectionClosed;
        private IPEndPoint _connectedEndPoint = null;

        public UDPServerNode(int port, Action<byte[], UDPServerNode> handler, Action<UDPServerNode> onConnectionClosed)
        {
            client = new UdpClient(new IPEndPoint(IPAddress.Any, port));
            ThreadPool.QueueUserWorkItem(ListenerThread, port);
            _handler = handler;
            _onConnectionClosed = onConnectionClosed;
        }

        private void ListenerThread(object state)
        {
            var connecting = client.ReceiveAsync();

            connecting.Wait();
            _connectedEndPoint = connecting.Result.RemoteEndPoint;

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
            _onConnectionClosed?.Invoke(this);
        }

        public void Send(byte[] audioData)
        { 
            if(_connectedEndPoint != null)
                client.Send(audioData, audioData.Length, _connectedEndPoint);
        }

        public IPEndPoint GetEndPoint()
        {
            return _connectedEndPoint;
        }

        public void Dispose()
        {
            client.Dispose();
        }
    }
}
