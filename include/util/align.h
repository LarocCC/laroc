#ifndef LAROC_UTIL_ALIGN_H
#define LAROC_UTIL_ALIGN_H

/// Align \p n to \p align.
///
/// Examples:
/// alignTo(1, 4) == 4;
/// alignTo(13, 8) == 16;
int alignTo(int n, int align);

#endif
