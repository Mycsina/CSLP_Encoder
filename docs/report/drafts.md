### Noteworthy optimizations
* BitStream:
  * Instead of writing to the file every byte, we use a buffer and write them in bulk in the end.
* Golomb:
  * Encoding negative numbers has a very high entropy compared to positive numbers, so we instead encode only the absolute value (preceded by a bit for the sign).
  * According to the profiler, around 10-20% of encoding time is spent calculating m-based constants (as pow() and log() are expensive operations), so we changed it to calculate only when m is changed, instead of once every encode/decode.
  * We have developed a method to estimate the ideal m for a certain reference frame, allowing for more storage efficiency in exchange for time efficiency
* DCTEncoder:
  * As the encoded values are mostly 0, we use zigzag scan (pre-defined to simplify) to group the 0s and RLE to write them efficiently.