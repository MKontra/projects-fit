using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.IO;

namespace DistFileServer.Server
{
    public class DFServer
    {
        private EventWaitHandle serverrunning;
        Object thisLock = new Object();
        private String rootDir;

        private List<IDisposable> disposableListeners = new List<IDisposable>();


        private Dictionary<String, List<String>> dirlist;

        public DFServer(string rootDir)
        {
            lock (thisLock)
            {
                this.rootDir = rootDir;
                serverrunning = new EventWaitHandle(false, EventResetMode.ManualReset);
                dirlist = new Dictionary<string, List<string>>(Directory.GetDirectories(rootDir).ToDictionary(dirs => dirs, dirs => new List<string>(Directory.GetFiles(dirs).Select(inp => Path.GetFileName(inp)))));

                foreach (var item in dirlist)
                {
                    Console.WriteLine(item.Key);
                    foreach (var itemn in item.Value)
                    {
                        Console.WriteLine(itemn);
                    }
                }

                rebalalancestore(); 
            }

        }

        internal void addFile(string filename, byte[] content)
        {
            lock (thisLock)
            {
                if (dirlist.Count == 0)
                {
                    throw new DFS.DFSaccess_package.AddFileException();
                }
                if (dirlist.SelectMany(pair => pair.Value).Contains(filename))
                {
                    throw new DFS.DFSaccess_package.AddFileException();
                }
                var addto = dirlist.Aggregate((agg, next) => next.Value.Count < agg.Value.Count ? next : agg);
                try
                {
                    FileStream newfs = File.Open(addto.Key + Path.DirectorySeparatorChar + filename, FileMode.CreateNew);
                    newfs.Write(content, 0, content.Length);
                    newfs.Close();
                    dirlist[addto.Key].Add(filename);
                }
                catch (Exception e)
                {
                    Console.WriteLine(e.ToString());
                    throw new DFS.DFSaccess_package.AddFileException();
                }

            }
        }

        internal byte[] getFile(string filename)
        {
            lock (thisLock)
            {
                try
                {
                    string dir = dirlist.Where(pair => pair.Value.Contains(filename)).Single().Key;
                    FileStream newfs = File.Open(dir + Path.DirectorySeparatorChar + filename, FileMode.Open);
                    byte[] retval = new byte[newfs.Length];
                    newfs.Read(retval, 0, (int)newfs.Length);
                    newfs.Close();
                    return retval;
                }
                catch (Exception e)
                {
                    Console.WriteLine(e.ToString());
                    throw new DFS.DFSaccess_package.GetFileException();
                }
            }
        }

        internal string[] getFileList()
        {
            lock (thisLock)
            {
                return dirlist.SelectMany(pair => pair.Value).ToArray(); 
            }
        }

        internal void removeFile(string filename)
        {
            lock (thisLock)
            {
                try
                {
                    string dir = dirlist.Where(pair => pair.Value.Contains(filename)).Single().Key;
                    File.Delete(dir + Path.DirectorySeparatorChar + filename);
                    dirlist[dir].Remove(filename);
                    rebalalancestore();
                }
                catch (Exception e)
                {
                    Console.WriteLine(e.ToString());
                    throw new DFS.DFSaccess_package.GetFileException();
                }
            }
        }

        private void rebalalancestore()
        {
            //lock (thisLock)
            //{
                int maxcnt = dirlist.SelectMany(pair => pair.Value).Count() / dirlist.Count;
                IEnumerable<KeyValuePair<string, List<string>>> overs;
                while ((overs = dirlist.Where(pair => pair.Value.Count < maxcnt)).Count() > 0)
                {
                    //Console.WriteLine("Rebalancing iteration...");
                    //Console.WriteLine(maxcnt);
                    //overs.Select( x => x.Key ).ToList().ForEach(x=>Console.WriteLine(x) );

                    //Console.ReadLine();

                    var srcpair = dirlist.Where(pair => pair.Value.Count == dirlist.Select(pairi => pairi.Value.Count).Max()).First(); 
                    var srcfile = srcpair.Value.First();
                    var what = srcpair.Key + Path.DirectorySeparatorChar + srcfile;

                    var wheredir = overs.Select(p => p.Key).First();
                    var where = wheredir + Path.DirectorySeparatorChar + srcfile;

                    File.Move(what, where);
                    srcpair.Value.Remove(srcfile);
                    dirlist[wheredir].Add(srcfile);
                } 
            //}
        }

        internal void addNode(string partnodename)
        {
            lock (thisLock)
            {
                var nodename = rootDir + Path.DirectorySeparatorChar + partnodename;

                if (dirlist.Keys.Contains(nodename))
                    throw new DFS.DFSmanager_package.AddNodeException();
                Directory.CreateDirectory(nodename);
                dirlist.Add(nodename, new List<string>());
                rebalalancestore();
            }
        }

        internal string[] listNodes()
        {
            lock (thisLock)
            {
                return dirlist.Keys.Select(fuln => Path.GetFileName(fuln)).ToArray();
            }
        }

        internal void removeNode(string partnodename)
        {
            lock (thisLock)
            {
                var nodename = rootDir + Path.DirectorySeparatorChar + partnodename;

                if (dirlist.Count == 1)
                    throw new DFS.DFSmanager_package.RemoveNodeException();
                if (!dirlist.Keys.Contains(nodename))
                    throw new DFS.DFSmanager_package.RemoveNodeException();

                var addto = dirlist.Where(pair => pair.Key != nodename).First().Key;
                dirlist[nodename].ForEach(fname => File.Move(nodename + Path.DirectorySeparatorChar + fname, addto + Path.DirectorySeparatorChar + fname));
                dirlist[addto].AddRange(dirlist[nodename]);
                dirlist.Remove(nodename);
                Directory.Delete(nodename);
                rebalalancestore();
            }
        }

        internal void shutdownDFS()
        {
            foreach (var di in disposableListeners)
            {
                di.Dispose();
            }
            serverrunning.Set();
        }

        internal void registerInterface(IDisposable iod)
        {
            lock (thisLock)
            {
                disposableListeners.Add(iod); 
            }
        }

        public WaitHandle getServerWaitHandle()
        {
            return serverrunning;
        }

    }
}
