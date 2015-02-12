#ifndef __LineNumbers_hpp__
#define __LineNumbers_hpp__

#include <vector>

inline uint32_t to_rle(uint16_t val, uint32_t count) {
  uint32_t r = val;
  r = (r << 16) | count;
  return r;
}

inline void unpack_rle(uint32_t rle, uint16_t &val, uint32_t &count) {
  count = rle & (0xFFFF);
  val = rle >> 16;
}

inline void rle_encode_line_nos(const std::vector<uint16_t> &line_dat, std::vector<uint32_t> &rle)
{
  if (line_dat.size() > 1) {
    uint16_t prev = line_dat[0];
    int count = 1;
    for (int i=1;i<line_dat.size();i++) {
      if (line_dat[i] != prev) {
	rle.push_back(to_rle(prev,count));
	prev = line_dat[i];
	count = 1;
      } else {
	count++;
      }
    }
    rle.push_back(to_rle(prev,count));
  }
}

inline void rle_decode_line_nos(const uint32_t *p, int cnt, std::vector<uint16_t> &line_dat)
{
  int ptr = 0;
  for (int i=0;i<cnt;i++) {
    uint16_t rval;
    uint32_t rcnt;
    unpack_rle(p[i],rval,rcnt);
    for (int j=0;j<rcnt;j++)
      line_dat.push_back(rval);
  }
}

#endif
