#ifndef __HANDOFF_H_INCLUDED__
#define __HANDOFF_H_INCLUDED__

#include <map>
#include <iostream>

using namespace std;

template<typename T>
class Handoff
{
public:
  T val;
  int id;
  int sck;
  int dck;
  map <int,pair<pair<int,int>,T > > slots;
  map <int,pair<pair<int,int>,T > > tokens;

  Handoff(int i=0, T v=zero())
  {
    val=v;
    id=i;
    sck=0;
    dck=0;
  }

  friend ostream &operator<<( ostream &output, const Handoff & o)
  { 
    output << "id: " << o.id << " val: " << o.val << 
      " sck: " << o.sck <<" dck: " << o.dck << endl;
    typename map<int,pair<pair<int,int>,T> >::const_iterator it;
    for (it = o.slots.begin(); it != o.slots.end(); it++)
    {
      output << "slot: " << it->first << "->([sck:" << it->second.first.first <<
        ",dck:" << it->second.first.second << "]," << it->second.second << ")" << endl;
    }
    for (it = o.tokens.begin(); it != o.tokens.end(); it++)
    {
      output << "token: " << it->first << "->([sck:" << it->second.first.first <<
        ",dck:" << it->second.first.second << "]," << it->second.second << ")" << endl;
    }
    return output;            
  }

  unsigned int numtokens()
  {
    return tokens.size();
  }

  unsigned int numslots()
  {
    return slots.size();
  }

  T fetch()
  {
    return val;
  }

  void plus(T v)
  {
    val = oplus(val, v);
  }

  T minus(T h)
  {
    pair<T, T> p= split(val,h);
    val=p.first;
    return p.second;
  }

  void mergein (Handoff j)
  {
    typename map<int,pair<pair<int,int>,T > >::const_iterator its;
    typename map<int,pair<pair<int,int>,T > >::const_iterator itt;
    pair<pair<int,int>,T> token;
    pair<pair<int,int>,T> slot;

    // fill slots
    its=slots.find(j.id);
    if ( its != slots.end() )
    {
      // j \in dom(slots)
      // cout << "*slot for j*" << endl;
      slot=its->second;
      itt=j.tokens.find(id);
      if ( itt != j.tokens.end() )
      {
        // i \in dom(tokens)
        // cout << "*token for i*" << endl;
        token=itt->second;
        if ( token.first == slot.first ) // ck match
        {
          // cout << "*ck match*" << endl;
          val = oplus(val, token.second); // incorporate payload
          slots.erase(its); // slot filled
        }
        //else cout << "*no ck match*" << endl;
      }
    }

    // discard slot
    its=slots.find(j.id); // needed since could have been erase in previous code 
    if ( its != slots.end() )
    {
      // j \in dom(slots)
      slot=its->second;
      if (j.sck > slot.first.first)
       slots.erase(its);
    }

    // create slot
    its=slots.find(j.id); // still needed  
    if ( its == slots.end() )
    {
      // j \not \in dom(slots)
      T hint = needs(val,j.val); // get needs
      if (hint != zero()) // only make slots if really needed
      {
        slot.first.first=j.sck;
        slot.first.second=dck;
        slot.second=hint;
        slots[j.id]=slot;
        dck=dck+1;
      }
    }

    // discard token
    itt=tokens.find(j.id);
    if ( itt != tokens.end() )
    {
      // j \in dom(i.tokens)
      token=itt->second;
      its=j.slots.find(id);
      if ( its != j.slots.end() )
      {
        // i \in dom(j.slots)
        slot=its->second;
        if ( token.first.second < slot.first.second )
        {
          tokens.erase(itt);
//          cout << "ACTIVATED!!" << endl;
//          exit(1);
        }
      }
      else
      {
        // i \not \in dom(j.slots)
        if (token.first.second < j.dck )
          tokens.erase(itt);
      }
    }

    // create token
    its=j.slots.find(id);  
    if ( its != j.slots.end() )
    {
      // i \in dom(j.slots)
      slot=its->second;
      if ( slot.first.first == sck )
      {
        pair<T, T> p = split(val,slot.second);
        val=p.first;
        token.first=slot.first;
        token.second=p.second;
        tokens[j.id]=token;
        sck=sck+1;
      }
    }

  }

};

#endif
