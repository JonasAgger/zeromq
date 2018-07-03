using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using ZeroMQ;

namespace Server_Service2
{
    public class HwClient : ZeroMQBase
    {
        public new bool IsRunning() => base.IsRunning();
        public new void SetRunning(bool running) => base.SetRunning(running);


        public HwClient(string address, ZSocketType socketType, BindingType bindingType, string identity) : base(address, socketType, bindingType, identity)
        {
            //SetUp();
           //ZSocketType.DEALER
           //"Master Unit"
           //"tcp://18.185.114.115:5555"
        }

        public void SetUp(EventHandler<ZeroMQEventArgs> handler = null)
        {
            Console.WriteLine("Setting Up ZeroMQ connection");
            if (handler == null)
                base.SetUp(OnMessageReceived);
            else 
                base.SetUp(handler);
        }

        public void SendData(byte[] data)
        {
            var response = new ZMessage {new ZFrame(data)};
            MsgQueue.Enqueue(response);
        }

        public void SendString(string type, string message)
        {
            MsgQueue.Enqueue(new ZMessage { new ZFrame(type), new ZFrame(message) });
        }

        private void OnMessageReceived(object obj, ZeroMQEventArgs zArgs)
        {
            var message = zArgs.ReceivedZMessage;
            Console.WriteLine(message[message.Count - 1]);
        }
    }
}
