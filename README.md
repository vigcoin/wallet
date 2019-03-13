VIG WALLET
===

[![Build status](https://ci.appveyor.com/api/projects/status/b86pdx2x1wyaxmwv?svg=true)](https://ci.appveyor.com/project/calidion/wallet)


## Ubuntu Environment Setup

```
sudo apt install -y build-essential curl cmake libboost-all-dev libssl-dev libsodium-dev qt5-default qttools5-dev qttools5-dev-tools python-pip python-dev lcov git mercurial
```

**1. Clone wallet sources**

```
git clone https://github.com/vigcoin/wallet.git
```

**2. Get submodule update**

```
git submodule update --depth 1 --init --recursive
```

**3. Build**

```
mkdir build && cd build && cmake .. && make
```
