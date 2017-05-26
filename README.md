# Plain framework

Plain framework is base with c++ smart framework for net applications(socket game or more).

- **Author:** Viticm
- **Website:** [http://www.cnblogs.com/lianyue/](http://www.cnblogs.com/lianyue/)
- **Version:** 2.1d

[![Build Status](https://travis-ci.org/viticm/plainframework.svg)](https://travis-ci.org/viticm/plainframework)

## Simple on linux
<img src="https://github.com/viticm/plainframework/blob/master/documentation/examples/pf_unix.gif" />

## Simple on windows
<img src="https://github.com/viticm/plainframework/blob/master/documentation/examples/pf_win.gif" />

## Develop environment

To install plain framework need depends with shell script(Only on linux or unix)

```shell
cd tools/script/linux/install/ && sh install_environment.sh
```


### Build linux

Plain build on linux use cmake, you can run command `yum -y install cmake` installed.

Your can build the plain framework with this command.

```shell
cd pf/core && cmake ./ && make
```

### Build windows

Plain build on windows use visual studio(at least version vs2015), you can open the file pf_win.sln.

Only repository: [https://github.com/viticm/pf_win/](https://github.com/viticm/pf_win/)

## Documentation

Current version for pf: 1.0

The complete docs for Plain framework can be found at the docs in the `/documentation` directory.


## Copyright and License
Plain framework was written by Viticm.
Plain framework is released under the MIT License. See the LICENSE file for details.


## Recent Changelog

### 2.0
- NewAdd: All codes will with effective and c++11 in future.

### 2.1
- NewAdd: Cache module add(see the cache module[now just for database cache with share memory]).
- NewAdd: The plugin lua add the dcache module.
- NewAdd: Replace the base variable type implement from only struct to template, change the use memory in db query to heap dynamic from stack static.
- Bugfix: Share map error in multi threads.
- BugFix: Database query tool error in multi threads.
- Bugfix: Share lock can't used in multi threads.
