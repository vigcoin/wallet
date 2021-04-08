VIG WALLET
===

[![Build status](https://ci.appveyor.com/api/projects/status/b86pdx2x1wyaxmwv?svg=true)](https://ci.appveyor.com/project/calidion/wallet)


## Ubuntu Environment Setup

# Currently supported version ubuntu 20.04 LTS

```
sudo sudo apt install -y build-essential curl cmake libboost-all-dev libssl-dev libsodium-dev qt5-default qttools5-dev qttools5-dev-tools lcov git mercurial
```

**1. Clone wallet sources**

```
git clone https://github.com/vigcoin/wallet.git
cd wallet
```

**2. Get submodule update**

```
git clone https://github.com/vigcoin/coin.git cryptonote
cd cryptonote
git checkout 94f622f12e0380a693430a29081271a31b493351
```

**3. Build**

```
mkdir build && cd build && cmake .. && make
```
