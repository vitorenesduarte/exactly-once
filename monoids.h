#ifndef __MONOIDS_H_INCLUDED__
#define __MONOIDS_H_INCLUDED__

#include <map>
#include <ostream>
#include <cmath>

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
  return r;
}

pair<int, int> split(const int & x, const int & h)
{
  pair<int, int> r;
  r.first=(x-h+abs(x-h))/2;
  r.second=(x+h-abs(x-h))/2;
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

#endif
