MINISNSF: Numbered minisnsf creation utility
============================================
[![Travis Build Status](https://travis-ci.com/loveemu/minisnsf.svg?branch=master)](https://travis-ci.com/loveemu/minisnsf) [![AppVeyor Build Status](https://ci.appveyor.com/api/projects/status/4os92k855s51kuxw/branch/master?svg=true)](https://ci.appveyor.com/project/loveemu/minisnsf/branch/master)

Numbered minisnsf creation utility, where the offset is known.

Downloads
---------

- [Latest release](https://github.com/loveemu/minisnsf/releases/latest)

Usage
-----

Syntax: `minisnsf [Base name] [Offset] [Size] [Count]`

or

Syntax: `minisnsf [Base name] [Offset] =[Hex pattern] [Count]`

### Options

`--help`
  : Show help

`--psfby`, `--snsfby` [name]
  : Set creator name of SNSF

### Examples

```bash
minisnsf mario 0xc816 1 64
```

```bash
minisnsf mario 0xc816 =NNNNDEADBEEF 64
```
