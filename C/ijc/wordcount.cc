#include <string>
    #include <iostream>
    #include <map>

    typedef std::map<std::string,int>  map_t;
    typedef map_t::iterator            mapiter_t;

    int main() {
      std::string word;
      map_t m; // asociativn� pole - indexem je slovo

      while( std::cin >> word )  // ?ten� slova
        m[word]++;               // po?�t�n� v�skyt? slova

      int sum = 0;
      for(mapiter_t i=m.begin(); i!=m.end(); ++i)
        std::cout << i->first <<"\t"<< i->second <<"\t"<< (sum = sum + i->second) <<"\n";
      //             slovo (kl�?)         po?et (data)
    }