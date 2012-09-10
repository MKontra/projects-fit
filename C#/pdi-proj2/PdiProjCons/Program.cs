using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Ch.Elca.Iiop;
using Ch.Elca.Iiop.Services;
using omg.org.CosNaming;
using DistFileServer;
using DistFileServer.Server;
using DistFileServer.IIOPInterface;
using System.IO;
using System.Threading;
using System.Runtime.Remoting.Channels;
using System.Net.Sockets;

namespace PdiProjCons
{
    class Program
    {
        static void Main(string[] args)
        {
            CorbaInit orb = CorbaInit.GetInit();
            IiopClientChannel clientChannel = new IiopClientChannel();
            //ChannelServices.RegisterChannel(clientChannel, false);

            IiopChannel iiop = new IiopChannel(8807);
            ChannelServices.RegisterChannel(iiop, false);

            NamingContext nc = InitialRefLineParser.getNamingServiceForArgs(args);
            
            if (nc == null)
                Console.WriteLine("Cannot connet to Naming Service");

            Console.WriteLine(Thread.CurrentThread.ManagedThreadId);

            DFServer dfs = new DFServer(Directory.GetCurrentDirectory() );
            DFSAccess dfa = new DFSAccess(dfs, "DFSAccess", nc);
            DFSManager dfm = new DFSManager(dfs, "DFSManager", nc);
            dfs.getServerWaitHandle().WaitOne();
        }
    }
}
