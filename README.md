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
cd wallet
```

**2. Get submodule update**

```
git clone https://github.com/vigcoin/coin.git cryptonote
cd cryptonote
git checkout ac75d60b116b6a31eb689b0c62fa670671294e77
```

**3. Build**

```
mkdir build && cd build && cmake .. && make
```
