# Zombies Ate My Neighbors

A SNESoIP optimised version of Zombies Ate My Neighbors.

## How to apply the patch

1. Obtain a copy of the ROM:

```
ROM: Zombies Ate My Neighbors (U) [!].sfc
MD5: 23c2af7897d9384c4791189b68c142eb
```

2. Apply patch using [asar](https://github.com/RPGHacker/asar):

```
asar "Zombies Ate My Neighbors (U) [!].asar" "Zombies Ate My Neighbors (U) [!].sfc"
```

If asar has problems patching the file, make sure the filename extension
is `sfc` and not `smc`.  The patched ROM should have the following
checksum:

```
MD5: 50c8efb1933c3e8955d6910abdb4d13f
```
