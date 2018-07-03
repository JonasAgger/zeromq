using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace VoiceChat
{
    public interface IAudioClient
    {
        void SetReceived(Action<byte[]> onAudioReceivedAction);
        void Send(byte[] audioData);
    }

    public class Sender : IAudioClient
    {
        private Action<byte[]> handler;
        private readonly UdpClient udpSender;
        public Sender(IPEndPoint endPoint)
        {
            udpSender = new UdpClient();
            udpSender.Connect(endPoint);

            var data = Encoding.UTF8.GetBytes("hello");

            Send(data);

            var b = udpSender.ReceiveAsync();

            b.Wait();

            udpSender.Dispose();
            var port = int.Parse(Encoding.UTF8.GetString(b.Result.Buffer));
            var newEP = new IPEndPoint(endPoint.Address, port);
            udpSender = new UdpClient();
            udpSender.Connect(newEP);

            Send(data);

            ThreadPool.QueueUserWorkItem(ListenerThread, endPoint);
        }

        private void ListenerThread(object state)
        {
            do
            {
                try
                {
                    var b = udpSender.ReceiveAsync();

                    b.Wait();
                    handler?.Invoke(b.Result.Buffer);
                    //Console.WriteLine("Received: {0}", Encoding.UTF8.GetString(b.Result.Buffer));
                }
                catch (SocketException)
                {
                    // usually not a problem - just means we have disconnected
                }
            } while (true);
        }

        public void SetReceived(Action<byte[]> onAudioReceivedAction)
        {
            handler += onAudioReceivedAction;
        }

        public void Send(byte[] payload)
        {
            udpSender.Send(payload, payload.Length);
        }

        public void Dispose()
        {
            udpSender?.Close();
        }
    }

    public class Receiver : IAudioClient
    {
        private Action<byte[]> handler;
        private readonly UdpClient udpListener;
        private bool listening;

        public Receiver(IPEndPoint ip, Action<byte[]> onAudioReceivedAction)
        {
            udpListener = new UdpClient();
            udpListener.Connect(ip);

            var data = Encoding.UTF8.GetBytes("hello");

            Send(data);

            var b = udpListener.ReceiveAsync();

            b.Wait();

            udpListener.Dispose();
            var port = int.Parse(Encoding.UTF8.GetString(b.Result.Buffer));
            var newEP = new IPEndPoint(ip.Address, port);
            udpListener = new UdpClient();
            udpListener.Connect(newEP);

            handler += onAudioReceivedAction;

            ThreadPool.QueueUserWorkItem(ListenerThread, ip);
            listening = true;
        }

        private void ListenerThread(object state)
        {
            var endPoint = (IPEndPoint)state;
            try
            {
                while (listening)
                {
                    byte[] b = udpListener.Receive(ref endPoint);
                    handler?.Invoke(b);
                }
            }
            catch (SocketException)
            {
                // usually not a problem - just means we have disconnected
            }
        }

        public void Dispose()
        {
            listening = false;
            udpListener?.Close();
        }

        public void SetReceived(Action<byte[]> onAudioReceivedAction)
        {
            throw new NotImplementedException();
        }

        public void Send(byte[] audioData)
        {
            throw new NotImplementedException();
        }
    }
}
