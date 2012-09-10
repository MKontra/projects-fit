using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Ch.Elca.Iiop.Services;
using Ch.Elca.Iiop;
using System.Runtime.Remoting.Channels;
using omg.org.CosNaming;
using System.Runtime.Remoting;

namespace DFSManagerClient
{
    class Program
    {
        static void Main(string[] args)
        {
            CorbaInit orb = CorbaInit.GetInit();
            IiopClientChannel clientChannel = new IiopClientChannel();
            ChannelServices.RegisterChannel(clientChannel,false);

            DFS.DFSmanager dfsmanag = null;

            while (true)
            {
                Console.WriteLine("Select connection method:");
                Console.WriteLine("1. Direct - enter object corbaloc name");
                Console.WriteLine("2. Direct - enter object IOR");
                Console.WriteLine("3. Using Nameservice - enter nameservice host and port");
                try{
                    string response = Console.ReadLine();
                    int means = Int32.Parse(response);

                    switch (means)
                    {
                        case 1:
                            Console.WriteLine("Enter corbaloc url:");
                            string retprom = Console.ReadLine();
                            retprom = "corbaloc:iiop:localhost:8807/DFSManager";
                            dfsmanag = RemotingServices.Connect(typeof(DFS.DFSmanager), retprom) as DFS.DFSmanager;break;
                        case 2:
                            Console.WriteLine("Enter ior:");
                            string retior = Console.ReadLine();
                            dfsmanag = RemotingServices.Connect(typeof(DFS.DFSmanager), retior) as DFS.DFSmanager;break;
                        case 3:
                            Console.WriteLine("Enter nameservice host:");
                            string namhost = Console.ReadLine();
                            Console.WriteLine("Enter nameservice port:");
                            int namport = Int32.Parse(Console.ReadLine());
                            Console.WriteLine("Enter object name:");
                            string objname = Console.ReadLine();
                            NamingContext nameService = orb.GetNameService(namhost, namport);
                            NameComponent[] name = new NameComponent[] { new NameComponent(objname) };
                            dfsmanag = (DFS.DFSmanager)nameService.resolve(name);break;
                        default: Console.WriteLine("BadChoice");break;
                    }

                } catch (Exception e)
                {
                    Console.WriteLine("Couldnt connect");
                    Console.WriteLine( e.ToString() );
                }
                if ( dfsmanag != null ) break;
            }
            

            bool done = false;
            do
            {
                Console.WriteLine("Issue Command:");
                Console.WriteLine("1. Add Node");
                Console.WriteLine("2. Remove Node");
                Console.WriteLine("3. List Nodes");
                Console.WriteLine("4. Shutdown server");
                Console.WriteLine("5. Exit");
                try
                {
                    string response = Console.ReadLine();
                    int means = Int32.Parse(response);

                    switch (means)
                    {
                        case 1:
                            Console.WriteLine("Enter Node Name:");
                            string nname = Console.ReadLine();
                            dfsmanag.addNode(nname); break;
                        case 2:
                            Console.WriteLine("Enter Node Name:");
                            string fnamer = Console.ReadLine();
                            dfsmanag.removeNode(fnamer);break;
                        case 3:
                            foreach (var item in dfsmanag.listNodes())
                            {
                                Console.WriteLine(item);
                            }break;
                        case 4:
                            Console.WriteLine("Shutting down server...");
                            dfsmanag.shutdownDFS();
                            break;
                        case 5:
                            done = true;break;
                        default: Console.WriteLine("BadChoice"); break;
                    }

                }
                catch (Exception e)
                {
                    Console.WriteLine("Couldnt connect");
                    Console.WriteLine(e.ToString());
                }

            } while (!done);

        }
    }
}
