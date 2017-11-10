#include <set>
#include <map>
#include <string>
#include <iostream>
#include <cmath>
#include <cstdlib>

using namespace std;

float zero()
{
  return 0;
}

template<typename K, typename V>
map<K,V> zero()
{
  static map<K,V> z;
  return z;
}

float oplus (const float & a, const float & b)
{
  return a + b;
}

int oplus (const int & a, const int & b)
{
  return a + b;
}

template<typename K, typename V>
map<K,V> oplus (const map<K,V> & a, const map<K,V> & b)
{
  map<K,V> r;
  typename map<K,V>::const_iterator ita;
  typename map<K,V>::const_iterator itb;
  ita=a.begin(); itb=b.begin();
  do
  {
    if( ita != a.end() && ( itb == b.end() || ita->first < itb->first))
    {
      // entry only at "a"
      r[ita->first]=ita->second;
      ++ita;
    }
    else if ( itb != b.end() && ( ita == a.end() || itb->first < ita->first))
    {
      // entry only at "b"
      r[itb->first]=itb->second;
      ++itb;
    }
    else if ( ita != a.end() &&  itb != b.end() )
    {
      // entry at both "a" and "b"
      r[ita->first]=oplus(ita->second,itb->second);
      ++ita; ++itb;
    }
  }  while (ita != a.end() || itb != b.end() );



  return r;
}


float needs (const float & a, const float & b)
{
  return (b-a+abs(b-a))/4;
}

int needs (const int & a, const int & b)
{
  return (b-a+abs(b-a))/4;
}

template<typename K, typename V>
map<K,V> needs (const map<K,V> & a, const map<K,V> & b)
{
  map<K,V> r;
  typename map<K,V>::const_iterator ita;
  typename map<K,V>::const_iterator itb;
  ita=a.begin(); itb=b.begin();
  do
  {
    if( ita != a.end() && ( itb == b.end() || ita->first < itb->first))
    {
      // entry only at "a"
      r[ita->first]=zero();
      ++ita;
    }
    else if ( itb != b.end() && ( ita == a.end() || itb->first < ita->first))
    {
      // entry only at "b"
      r[itb->first]=needs(zero(),itb->second);
      ++itb;
    }
    else if ( ita != a.end() &&  itb != b.end() )
    {
      // entry at both "a" and "b"
      r[ita->first]=needs(ita->second,itb->second);
      ++ita; ++itb;
    }
  }  while (ita != a.end() || itb != b.end() );

  return r;
}


pair<float, float> split(const float & x, const float & h)
{
  pair<float, float> r;
  r.first=(x-h+abs(x-h))/2;
  r.second=(x+h-abs(x-h))/2;
//  cout << "split(" << x << "," << h << ")=(" 
//    << r.first << "," << r.second << ")" << endl;
  return r;
}

pair<int, int> split(const int & x, const int & h)
{
  pair<int, int> r;
  r.first=(x-h+abs(x-h))/2;
  r.second=(x+h-abs(x-h))/2;
//  cout << "split(" << x << "," << h << ")=(" 
//    << r.first << "," << r.second << ")" << endl;
  return r;
}

template<typename K, typename V>
pair<map<K,V>,map<K,V> > split (const map<K,V> & x, const map<K,V> & h)
{
  pair<map<K,V>,map<K,V> > r;
  typename map<K,V>::const_iterator itx;
  typename map<K,V>::const_iterator ith;
  itx=x.begin(); ith=h.begin();
  do
  {
    if( itx != x.end() && ( ith == h.end() || itx->first < ith->first))
    {
      // entry only at "x"
      r.first[itx->first]=itx->second;
      ++itx;
    }
    else if ( ith != h.end() && ( itx == x.end() || ith->first < itx->first))
    {
      // entry only at "h"
      ++ith;
    }
    else if ( itx != x.end() &&  ith != h.end() )
    {
      // entry at both "x" and "y"
      pair<V,V> p=split(itx->second,ith->second);
      r.first[itx->first]=p.first;
      r.second[itx->first]=p.second;
      ++itx; ++ith;
    }
  }  while (itx != x.end() || ith != h.end() );

  return r;
}



template<typename T>
class handoff
{
  public:

  T val;
  int id;
  int sck;
  int dck;
  map <int,pair<pair<int,int>,T > > slots;
  map <int,pair<pair<int,int>,T > > tokens;

  handoff(int i=0, T v=zero())
  {
    val=v;
    id=i;
    sck=0;
    dck=0;
  }

  friend ostream &operator<<( ostream &output, const handoff & o)
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

  void mergein (handoff j)
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

void test1()
{
  handoff<int> o1(1);
  handoff<int> o2(2);

  o1.plus(4); o1.dck=20;
  o2.plus(8); o2.sck=10;
  cout << o1 << endl;
  cout << o2 << endl;
  o1.mergein(o2);
  cout << "--- 1 <- 2 ---" << endl;
  cout << o1 << endl;
  cout << o2 << endl;
  o2.mergein(o1);
  cout << "--- 2 <- 1 ---" << endl;
  cout << o1 << endl;
  cout << o2 << endl;
  o1.mergein(o2);
  cout << "--- 1 <- 2 ---" << endl;
  cout << o1 << endl;
  cout << o2 << endl;
  o2.mergein(o1);
  cout << "--- 2 <- 1 ---" << endl;
  cout <<  o1 << endl;
  cout <<  o2 << endl;
}

void test2()
{
  // replay resilience 
  handoff<int> ov[4];
  handoff<int> oldov[4];
  cout << "--- Start ----" << endl;
  for (int i=0; i<4; i++)
  {
    ov[i].val=i*10;
    ov[i].id=i;
    oldov[i]=ov[i];
    cout << ov[i] << endl;;
  }
  for (int i=0; i<4*100; i++)
  {
//    cout << i % 4 << "<->" << (i+1)%4 << endl;
    cout << i % 4 << "<-" << (i+1)%4 << endl;
    ov[i%4].mergein(ov[(i+1)%4]);
    cout << ov[i % 4] << endl << ov[(i+1) % 4] << endl;
    cout << i % 4 << "->" << (i+1)%4 << endl;
    ov[(i+1)%4].mergein(ov[i%4]);
    cout << ov[i % 4] << endl << ov[(i+1) % 4] << endl;
    
    // not so random atacker replay
    if (rand() % 20 == 0)
      oldov[i%4]=ov[i%4]; // store fresher old state
    if (rand() % 10 == 0)
    { 
      cout << "*replay ->*" << endl;
      ov[i%4].mergein(oldov[(i+1)%4]); // merge old state ->
    }
    if (rand() % 4 == 0)
    {
      cout << "*replay <-*" << endl;
      ov[(i+1)%4].mergein(oldov[i%4]); // merge old state  <-
    }


    cout << "--- Run ----" << endl;
    for (int i=0; i<4; i++)
    {
      cout << ov[i] << endl;;
    }
  }
  for (int i=0; i<4; i++)
  {
    cout << "--- End ----" << endl;
    cout << ov[i] << endl;;
  }
  for (int i=0; i<4; i++)
  {
    cout << "--- Old at End ----" << endl;
    cout << oldov[i] << endl;;
  }
}

void test3()
{
  // map monoid
  map<int,float> a=zero<int,float>();
  map<int,float> b=zero<int,float>();
  a[0]=5;
  a[1]=5;
  b[1]=10;
  b[2]=10;
  map<int,float> c=oplus(a,b);
  cout << c[0] << endl;
  cout << c[1] << endl;
  cout << c[2] << endl;
  map<int,float> d=needs(a,b);
  cout << d[0] << endl;
  cout << d[1] << endl;
  cout << d[2] << endl;
  pair<map<int,float>,map<int,float> > p;
  p=split(a,d);
  cout << "--------" << endl;
  cout << p.first[0] << endl;
  cout << p.first[1] << endl;
  cout << p.first[2] << endl;
  cout << "--------" << endl;
  cout << p.second[0] << endl;
  cout << p.second[1] << endl;
  cout << p.second[2] << endl;
}

void gplot1()
{
  const int nodes=1024;
  const int rounds=nodes/4;
  const int lossp=0; // message loss probability per link transmition, scale 0 to 100
  const int storep=20; // probablility of storing a state for later replay
  const int replayp=75; // probability of injecting a past message state
  const bool replay=false;
  handoff<int> ov[nodes]; 
  handoff<int> rsov[nodes]; // state at round start
  handoff<int> rov[nodes]; // state cache for replay fault injection
  cout << "# Initialize" << endl;
  cout << "# Round Slots Tokens" << endl;
  for (int i=0; i<nodes; i++)
  {
    ov[i].val=(rand() % 256);
    ov[i].id=i;
    rov[i]=ov[i];
  }
  // Ring based sync
  for (int r=0; r<rounds; r++)
  {
    // Report on state at round start
    if (r % 4 == 0) // Only output each k rounds
    {
      int totslots=0; int tottokens=0;
      for (int i=0; i<nodes; i++)
      {
        totslots+=ov[i].numslots();
        tottokens+=ov[i].numtokens();
      }
      cout << r << " " << totslots*1.0/nodes << " " << tottokens*1.0/nodes << endl;
    }
    // cout  << totslots << " " << tottokens << endl;
    // Store state at round start
    for (int i=0; i<nodes; i++)
    {
      rsov[i]=ov[i];
      // Store for replay fault injection
      if (replay)
      {
        if (rand() % 100 <= storep)
          rov[i]=ov[i];
      }
    }
    // Create state at round end
    for (int i=0; i<nodes; i++)
    {
      // DHT
      // Process Messages
//      for (int l=0; l < log2(nodes) -1; l++) // closest 1st
      for (int l=log2(nodes) -2; l>=0; l--) // furthest 1st
      {
        // Do replay fault injection
        if (replay)
        {
          if (rand() % 100 <= replayp)
            ov[i].mergein(rov[(i+((unsigned int) exp2(l)))%nodes]); // merge from "next"
          if (rand() % 100 <= replayp)
            ov[i].mergein(rov[(i-((unsigned int) exp2(l))+nodes)%nodes]); // merge from "previous"
        }
        // Receive messages, with possible losses
        if (rand() % 100 >= lossp)
          ov[i].mergein(rsov[(i+((unsigned int) exp2(l)))%nodes]); // merge from "next"
        if (rand() % 100 >= lossp) 
          ov[i].mergein(rsov[(i-((unsigned int) exp2(l))+nodes)%nodes]); // merge from "previous"
      }
      // Ring
      // ov[i].mergein(rsov[(i+1)%nodes]); // merge from next
      // ov[i].mergein(rsov[(i-1+nodes)%nodes]); // merge from previous
    }
  }
  // Comment report on final values
  for (int i=0; i<nodes; i++)
    cout << "# node " << i << " val " << ov[i].fetch() << endl;

}

int main()
{
   test1();
   test2();
   test3();
   gplot1();
}

