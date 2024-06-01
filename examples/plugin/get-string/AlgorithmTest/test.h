//
// Created by junjie.zhu on 24-5-15.
//
//
// Created by 14114 on 2024/4/11.
//

#pragma once

#include <cstdint>
#include <cstring>
#include <algorithm>
#include <array>
#include <limits>
#include <numeric>
#include <type_traits>
#include <vector>



static size_t criticalFactorization(unsigned char *needle, size_t needle_len,
                                    size_t *period) {
  /* Index of last byte of left half, or SIZE_MAX.  */
  size_t max_suffix, max_suffix_rev;
  size_t j;           /* Index into NEEDLE for current candidate suffix.  */
  size_t k;           /* Offset into current period.  */
  size_t p;           /* Intermediate period.  */
  unsigned char a, b; /* Current comparison bytes.  */
  /* Invariants:
     0 <= j < NEEDLE_LEN - 1
     -1 <= max_suffix{,_rev} < j (treating SIZE_MAX as if it were signed)
     min(max_suffix, max_suffix_rev) < global period of NEEDLE
     1 <= p <= global period of NEEDLE
     p == global period of the substring NEEDLE[max_suffix{,_rev}+1...j]
     1 <= k <= p
  */
  /* Perform lexicographic search.  */
  max_suffix = SIZE_MAX;
  j = 0;
  k = p = 1;
  while (j + k < needle_len) {
    a = (needle[j + k]);
    b = (needle[max_suffix + k]);
    if (a < b) {
      /* Suffix is smaller, period is entire prefix so far.  */
      j += k;
      k = 1;
      p = j - max_suffix;
    } else if (a == b) {
      /* Advance through repetition of the current period.  */
      if (k != p)
        ++k;
      else {
        j += p;
        k = 1;
      }
    } else /* b < a */
    {
      /* Suffix is larger, start over from current location.  */
      max_suffix = j++;
      k = p = 1;
    }
  }
  *period = p;
  /* Perform reverse lexicographic search.  */
  max_suffix_rev = SIZE_MAX;
  j = 0;
  k = p = 1;
  while (j + k < needle_len) {
    a = (needle[j + k]);
    b = (needle[max_suffix_rev + k]);
    if (b < a) {
      /* Suffix is smaller, period is entire prefix so far.  */
      j += k;
      k = 1;
      p = j - max_suffix_rev;
    } else if (a == b) {
      /* Advance through repetition of the current period.  */
      if (k != p)
        ++k;
      else {
        j += p;
        k = 1;
      }
    } else /* a < b */
    {
      /* Suffix is larger, start over from current location.  */
      max_suffix_rev = j++;
      k = p = 1;
    }
  }
  /* Choose the longer suffix.  Return the first byte of the right
     half, rather than the last byte of the left half.  */
  if (max_suffix_rev + 1 < max_suffix + 1)
    return max_suffix + 1;
  *period = p;
  return max_suffix_rev + 1;
}

static inline char *strstr2(const unsigned char *Hs,
                            const unsigned char *Ne) {
  uint32_t H1 = (Ne[0] << 16) | Ne[1];
  uint32_t H2 = 0;
  for (int C = Hs[0]; H1 != H2 && C != 0; C = *++Hs) {
    H2 = (H2 << 16) | C;
  }
  return H1 == H2 ? (char *)Hs - 2 : NULL;
}

static inline char *strstr3(const unsigned char *Hs,
                            const unsigned char *Ne) {
  uint32_t H1 = ((uint32_t)Ne[0] << 24) | (Ne[1] << 16) | (Ne[2] << 8);
  uint32_t H2 = 0;
  for (int C = Hs[0]; H1 != H2 && C != 0; C = *++Hs) {
    H2 = (H2 | C) << 8;
  }
  return H1 == H2 ? (char *)Hs - 3 : NULL;
}

#define hash2(p) (((size_t)(p)[0] - ((size_t)(p)[-1] << 3)) % sizeof(shift))
#ifndef CHAR_BIT
#define CHAR_BIT 8
#endif
#define AVAILABLE(h, h_l, j, n_l) ((j) <= (h_l) - (n_l))

char *two_way_long_needle(unsigned char *haystack, size_t haystack_len,
                          unsigned char *needle, size_t needle_len) {

  size_t i;      /* Index into current byte of NEEDLE.  */
  size_t j;      /* Index into current window of HAYSTACK.  */
  size_t period; /* The period of the right half of needle.  */
  size_t suffix; /* The index of the right half of needle.  */
  size_t shift_table[1U << CHAR_BIT]; /* See below.  */
  /* Factor the needle into two halves, such that the left half is
     smaller than the global period, and the right half is
     periodic (with a period as large as NEEDLE_LEN - suffix).  */
  suffix = criticalFactorization(needle, needle_len, &period);
  /* Populate shift_table.  For each possible byte value c,
     shift_table[c] is the distance from the last occurrence of c to
     the end of NEEDLE, or NEEDLE_LEN if c is absent from the NEEDLE.
     shift_table[NEEDLE[NEEDLE_LEN - 1]] contains the only 0.  */
  for (i = 0; i < 1U << CHAR_BIT; i++)
    shift_table[i] = needle_len;
  for (i = 0; i < needle_len; i++)
    shift_table[(needle[i])] = needle_len - i - 1;
  /* Perform the search.  Each iteration compares the right half
     first.  */
  if (std::memcpy(needle, needle + period, suffix) == 0) {
    /* Entire needle is periodic; a mismatch can only advance by the
       period, so use memory to avoid rescanning known occurrences
       of the period.  */
    size_t memory = 0;
    size_t shift;
    j = 0;
    while (AVAILABLE(haystack, haystack_len, j, needle_len)) {
      unsigned char *pneedle;
      unsigned char *phaystack;
      /* Check the last byte first; if it does not match, then
         shift to the next possible match location.  */
      shift = shift_table[(haystack[j + needle_len - 1])];
      if (0 < shift) {
        if (memory && shift < period) {
          /* Since needle is periodic, but the last period has
             a byte out of place, there can be no match until
             after the mismatch.  */
          shift = needle_len - period;
        }
        memory = 0;
        j += shift;
        continue;
      }
      /* Scan for matches in right half.  The last byte has
         already been matched, by virtue of the shift table.  */
      i = std::max(suffix, memory);
      pneedle = &needle[i];
      phaystack = &haystack[i + j];
      while (i < needle_len - 1 && ((*pneedle++) == (*phaystack++)))
        ++i;
      if (needle_len - 1 <= i) {
        /* Scan for matches in left half.  */
        i = suffix - 1;
        pneedle = &needle[i];
        phaystack = &haystack[i + j];
        while (memory < i + 1 && ((*pneedle--) == (*phaystack--)))
          --i;
        if (i + 1 < memory + 1)
          return (char *)(haystack + j);
        /* No match, so remember how many repetitions of period
           on the right half were scanned.  */
        j += period;
        memory = needle_len - period;
      } else {
        j += i - suffix + 1;
        memory = 0;
      }
    }
  } else {
    /* The two halves of needle are distinct; no extra memory is
       required, and any mismatch results in a maximal shift.  */
    size_t shift;
    period = std::max(suffix, needle_len - suffix) + 1;
    j = 0;
    while (AVAILABLE(haystack, haystack_len, j, needle_len)) {
      const unsigned char *pneedle;
      const unsigned char *phaystack;
      /* Check the last byte first; if it does not match, then
         shift to the next possible match location.  */
      shift = shift_table[(haystack[j + needle_len - 1])];
      if (0 < shift) {
        j += shift;
        continue;
      }
      /* Scan for matches in right half.  The last byte has
         already been matched, by virtue of the shift table.  */
      i = suffix;
      pneedle = &needle[i];
      phaystack = &haystack[i + j];
      while (i < needle_len - 1 && ((*pneedle++) == (*phaystack++)))
        ++i;
      if (needle_len - 1 <= i) {
        /* Scan for matches in left half.  */
        i = suffix - 1;
        pneedle = &needle[i];
        phaystack = &haystack[i + j];
        while (i != SIZE_MAX && ((*pneedle--) == (*phaystack--)))
          --i;
        if (i == SIZE_MAX)
          return (char *)(haystack + j);
        j += period;
      } else
        j += i - suffix + 1;
    }
  }
  return NULL;
}

uint32_t findSubString(const char *haystack, uint32_t hsLen,
                       const char *needle, uint32_t neLen) {
  const unsigned char *hs = (const unsigned char *)haystack;
  const unsigned char *ne = (const unsigned char *)needle;

  /* Handle short needle special cases first.  */
  if (ne[0] == '\0')
    return 0;
  hs = (const unsigned char *)strchr((const char *)hs, ne[0]);
  if (hs == NULL || ne[1] == '\0')
    return hs == NULL ? -1 : hs - (const unsigned char *)haystack;
  if (ne[2] == '\0')
    return (unsigned char *)strstr2(hs, ne) - (const unsigned char *)haystack;
  if (ne[3] == '\0')
    return (unsigned char *)strstr3(hs, ne) - (const unsigned char *)haystack;

  /* Ensure haystack length is at least as long as needle length.
     Since a match may occur early on in a huge haystack, use strnlen
     and read ahead a few cachelines for improved performance.  */
  size_t ne_len = neLen;
  size_t hs_len = std::min(hsLen, neLen);
  if (hs_len < ne_len)
    return -1;

  /* Check whether we have a match.  This improves performance since we
     avoid initialization overheads.  */
  if (memcmp(hs, ne, ne_len) == 0)
    return hs - (const unsigned char *)haystack;

  /* Use Two-Way algorithm for very long needles.  */
  if ((ne_len > 256))
    return (unsigned char *)two_way_long_needle((unsigned char *)hs, hs_len,
                                                (unsigned char *)ne, ne_len)
                                        - (const unsigned char *)haystack;

  const unsigned char *end = hs + hs_len - ne_len;
  uint8_t shift[256];
  size_t tmp, shift1;
  size_t m1 = ne_len - 1;
  size_t offset = 0;

  /* Initialize bad character shift hash table.  */
  memset(shift, 0, sizeof(shift));
  for (size_t i = 1; i < m1; i++)
    shift[hash2(ne + i)] = i;
  /* Shift1 is the amount we can skip after matching the hash of the
     needle end but not the full needle.  */
  shift1 = m1 - shift[hash2(ne + m1)];
  shift[hash2(ne + m1)] = m1;

  while (1) {
    if (hs > end) {
      end += strnlen((const char *)end + m1 + 1, 2048);
      if (hs > end)
        return -1;
    }

    /* Skip past character pairs not in the needle.  */
    do {
      hs += m1;
      tmp = shift[hash2(hs)];
    } while (tmp == 0 && hs <= end);

    /* If the match is not at the end of the needle, shift to the end
       and continue until we match the hash of the needle end.  */
    hs -= tmp;
    if (tmp < m1)
      continue;

    /* Hash of the last 2 characters matches.  If the needle is long,
       try to quickly filter out mismatches.  */
    if (m1 < 15 || memcmp(hs + offset, ne + offset, 8) == 0) {
      if (memcmp(hs, ne, m1) == 0)
        return hs - (const unsigned char *)haystack;

      /* Adjust filter offset when it doesn't find the mismatch.  */
      offset = (offset >= 8 ? offset : m1) - 8;
    }

    /* Skip based on matching the hash of the needle end.  */
    hs += shift1;
  }
}
