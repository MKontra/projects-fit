using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Ch.Elca.Iiop.Services;
using Ch.Elca.Iiop;
using System.Runtime.Remoting.Channels;
using omg.org.CosNaming;
using System.Runtime.Remoting;

namespace DFSConsoleClient
{
    class Program
    {
        static void Main(string[] args)
        {
            CorbaInit orb = CorbaInit.GetInit();
            IiopClientChannel clientChannel = new IiopClientChannel();
            ChannelServices.RegisterChannel(clientChannel,false);

            DFS.DFSaccess dfsclient = null;

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
                            retprom = "corbaloc:iiop:localhost:8807/DFSAccess";
                            dfsclient = RemotingServices.Connect(typeof(DFS.DFSaccess), retprom) as DFS.DFSaccess;break;
                        case 2:
                            Console.WriteLine("Enter ior:");
                            string retior = Console.ReadLine();
                            dfsclient = RemotingServices.Connect(typeof(DFS.DFSaccess), retior) as DFS.DFSaccess;break;
                        case 3:
                            Console.WriteLine("Enter nameservice host:");
                            string namhost = Console.ReadLine();
                            Console.WriteLine("Enter nameservice port:");
                            int namport = Int32.Parse(Console.ReadLine());
                            Console.WriteLine("Enter object name:");
                            string objname = Console.ReadLine();
                            NamingContext nameService = orb.GetNameService(namhost, namport);
                            Console.ReadLine();
                            Console.WriteLine("Aquired NAMING SERVICE");
                            NameComponent[] name = new NameComponent[] { new NameComponent(objname) };
                            dfsclient = (DFS.DFSaccess)nameService.resolve(name);break;
                        default: Console.WriteLine("BadChoice");break;
                    }

                } catch (Exception e)
                {
                    Console.WriteLine("Couldnt connect");
                    Console.WriteLine( e.ToString() );
                }
                if ( dfsclient != null ) break;
            }
            

            bool done = false;
            do
            {
                Console.WriteLine("Issue Command:");
                Console.WriteLine("1. Add File");
                Console.WriteLine("2. RemoveFile");
                Console.WriteLine("3. Get File List");
                Console.WriteLine("4. Get File");
                Console.WriteLine("5. Exit");
                try
                {
                    string response = Console.ReadLine();
                    int means = Int32.Parse(response);

                    switch (means)
                    {
                        case 1:
                            Console.WriteLine("Enter File Name:");
                            string fname = Console.ReadLine();
                            Console.WriteLine("Enter File Contents:");
                            byte[] cnt = System.Text.Encoding.UTF8.GetBytes(Console.ReadLine());
                            dfsclient.addFile(fname, cnt); break;
                        case 2:
                            Console.WriteLine("Enter File Name:");
                            string fnamer = Console.ReadLine();
                            dfsclient.removeFile(fnamer);break;
                        case 3:
                            foreach (var item in dfsclient.getFileList())
                            {
                                Console.WriteLine(item);
                            }break;
                        case 4:
                            Console.WriteLine("Enter File Name:");
                            string fnamed = Console.ReadLine();
                            Console.WriteLine(System.Text.Encoding.UTF8.GetString(dfsclient.getFile(fnamed)));break;
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
