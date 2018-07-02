using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.Remoting.Messaging;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using ZeroMQ;

namespace Server_Service2
{
    public class HwServer : ZeroMQBase
    {
        public new bool IsRunning() => base.IsRunning();
        public new void SetRunning(bool running) => base.SetRunning(running);

        public HwServer(string address, ZSocketType socketType, BindingType bindingType) : base(address, socketType,
            bindingType)
        {
            SetUp();
        }

        private void SetUp()
        {
            Console.WriteLine();
            Console.WriteLine("Starting: ./{0} HWServer", AppDomain.CurrentDomain.FriendlyName);
            Console.WriteLine();
            Console.WriteLine("Name is set to Server");
            Console.WriteLine();
            Console.WriteLine("Setting up ZeroMQ socket");
            Console.WriteLine();

            base.SetUp(OnMessageReceived);
        }

        public void SendString(string receiver, string type, string message)
        {
            MsgQueue.Enqueue(new ZMessage { new ZFrame(receiver), new ZFrame(type), new ZFrame(message) });
        }

        private void OnMessageReceived(object obj, ZeroMQEventArgs zArgs)
        {
            var message = zArgs.ReceivedZMessage;

            foreach(var mess in message)
                Console.WriteLine(mess);

        }
    }
}
