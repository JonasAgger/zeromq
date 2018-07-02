using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using ZeroMQ;

namespace Server_Service2
{
    public enum BindingType{
    Bind,
    Connect
    }

    public abstract class ZeroMQBase
    {
        // Threads to handle connections
        private Thread _sender;
        private Thread _receiver;
        // Socket
        private ZSocket _socket;
        // MessageQueue to threads
        protected ConcurrentQueue<ZMessage> MsgQueue = new ConcurrentQueue<ZMessage>();
        // Boolean to handle running threads/terminate threads
        private bool _running = true;
        protected bool IsRunning() => _running;
        protected void SetRunning(bool running) => _running = running;
        protected ZeroMQBase(string addr, ZSocketType socketType, BindingType bindingType, string identity = "")
        {
            // Making socket of socketType
            _socket = new ZSocket(socketType);

            // Setting identity. Needed if not the server/router, since its needed to identify the receiver.
            // Identity needs to be set before connecting.
            if (identity != "") // Default name is Master Unit
                _socket.IdentityString = identity;

            // Either binding or connecting to given address
            if (bindingType == BindingType.Bind)
                _socket.Bind(addr); // "tcp://*:5555" for server.  "tcp://18.185.114.115:5555" for client
            else if (bindingType == BindingType.Connect)
                _socket.Connect(addr);

            // Starting threads.
            _sender = new Thread(SenderMethod);
            _receiver = new Thread(ReceiverMethod);
            _sender.Start();
            _receiver.Start();
        }


        // Sender and Receiver implementations
        private void SenderMethod()
        {
            while (IsRunning())
            {
                while (MsgQueue.IsEmpty) { }

                if (MsgQueue.TryDequeue(out var response))
                {
                    _socket.SendMessage(response);
                }

            }
        }
        private void ReceiverMethod()
        {
            while (IsRunning())
            {
                var received = _socket.ReceiveMessage();

                if (received[received.Count - 1].ToString() == "sluk")
                    SetRunning(false);
                else
                {
                    OnMessageReceivedEventHandler?.Invoke(this, new ZeroMQEventArgs(received));
                }
            }
        }

        protected virtual void SetUp(EventHandler<ZeroMQEventArgs> handler)
        {
            OnMessageReceivedEventHandler += handler;
        }

        private event EventHandler<ZeroMQEventArgs> OnMessageReceivedEventHandler;
    }

    public class ZeroMQEventArgs : EventArgs
    {
        public ZMessage ReceivedZMessage;

        public ZeroMQEventArgs(ZMessage msg) => ReceivedZMessage = msg;
    }
}
