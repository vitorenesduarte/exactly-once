#include <assert.h>
#include "monoids.h"
#include "handoff.h"

using namespace std;

void test1()
{
  cout << "Test 1..." << endl;

  Handoff<int> o1(1);
  Handoff<int> o2(2);

  // init
  o1.plus(4); o1.dck=20;
  o2.plus(8); o2.sck=10;

  // merge 1 <- 2
  o1.mergein(o2);
  assert (o1.val == 4 && o1.sck == 0 && o1.dck == 21);
  assert (o2.val == 8 && o2.sck == 10 && o2.dck == 0);

  // merge 1 -> 2
  o2.mergein(o1);
  assert (o1.val == 4 && o1.sck == 0 && o1.dck == 21);
  assert (o2.val == 6 && o2.sck == 11 && o2.dck == 0);

  // merge 1 <- 2
  o1.mergein(o2);
  assert (o1.val == 6 && o1.sck == 0 && o1.dck == 21);
  assert (o2.val == 6 && o2.sck == 11 && o2.dck == 0);

  // merge 1 -> 2
  o2.mergein(o1);
  assert (o1.val == 6 && o1.sck == 0 && o1.dck == 21);
  assert (o2.val == 6 && o2.sck == 11 && o2.dck == 0);
}

void test2()
{
  cout << "Test 2..." << endl;

  // replay resilience 
  Handoff<int> ov[4];
  Handoff<int> oldov[4];

  // init
  for (int i=0; i<4; i++)
  {
    ov[i].val=i*10;
    ov[i].id=i;
    oldov[i]=ov[i];
  }
  for (int i=0; i<4*100; i++)
  {
    // merge i % 4 <- (i + 1) % 4
    ov[i%4].mergein(ov[(i+1)%4]);
    
    // merge i % 4 -> (i + 1) % 4
    ov[(i+1)%4].mergein(ov[i%4]);
    
    // not so random atacker replay
    if (rand() % 2 == 0)
    {
      oldov[i%4]=ov[i%4]; // store fresher old state
    }
    
    if (rand() % 2 == 0)
    { 
      ov[i%4].mergein(oldov[(i+1)%4]); // merge old state ->
    }

    if (rand() % 2 == 0)
    {
      ov[(i+1)%4].mergein(oldov[i%4]); // merge old state  <-
    }
  }
  for (int i=0; i<4; i++)
  {
    assert (ov[i].val == 15);
  }
}

void test3()
{
  cout << "Test 3..." << endl;

  // map monoid
  map<int,float> a=zero<int,float>();
  map<int,float> b=zero<int,float>();
  a[0]=5;
  a[1]=5;
  b[1]=10;
  b[2]=10;

  map<int,float> c=oplus(a,b);
  assert(c[0] == 5 && c[1] == 15 && c[2] == 10);

  map<int,float> d=needs(a,b);
  assert(d[0] == 0 && d[1] == 2.5 && d[2] == 5);

  pair<map<int,float>,map<int,float> > p;
  p=split(a,d);
  assert(p.first[0] == 5 && p.first[1] == 2.5 && p.first[2] == 0);
  assert(p.second[0] == 0 && p.second[1] == 2.5 && p.second[2] == 0);
}

void gplot1()
{
  const int nodes=1024;
  const int rounds=nodes/4;
  const int lossp=0; // message loss probability per link transmition, scale 0 to 100
  const int storep=20; // probablility of storing a state for later replay
  const int replayp=75; // probability of injecting a past message state
  const bool replay=false;
  Handoff<int> ov[nodes]; 
  Handoff<int> rsov[nodes]; // state at round start
  Handoff<int> rov[nodes]; // state cache for replay fault injection
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
//   gplot1();
}

