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

        public void SendAudio(ZMessage message)
        {
            var response = new ZMessage();

            response.Add(new ZFrame(message[0].ToString() == "Master Unit" ? "Master Unit2" : "Master Unit"));
            for (int i = 1; i < message.Count; i++)
            {
                response.Add(message[i].Duplicate());
            }

            MsgQueue.Enqueue(response);
        }

        public void SendAudio(byte[] message)
        {
            var response = new ZMessage {new ZFrame("Master Unit2"), new ZFrame(message)};        

            MsgQueue.Enqueue(response);
        }

        private void OnMessageReceived(object obj, ZeroMQEventArgs zArgs)
        {
            //SendAudio(zArgs.ReceivedZMessage);
            try
            {
                var data = zArgs.ReceivedZMessage[1].Read();
                SendAudio(data);
            }
            catch (Exception e)
            {
                Console.WriteLine(e);
            }
        }
    }
}
